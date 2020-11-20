#include <epplet.h>
#include <stdio.h>
#include <glibtop/mem.h>
#include <glibtop/swap.h>

const int REFRESCO = 5;
int carga[2];
Epplet_gadget botonCerrar;
Epplet_gadget barras[2];
Epplet_gadget visorNumerico;
Epplet_gadget visorTemperatura;
char texto[64];
char textoTemperatura[6];
int temperatura=0;

static void enFoco(void *data, Window w){
  if (w == Epplet_get_main_window())
     {
		Epplet_gadget_show(botonCerrar);
     }
  return;
  data = NULL;
}

static void fueraFoco(void *data, Window w){
  if (w == Epplet_get_main_window())
     {
		Epplet_gadget_hide(botonCerrar);
     }
  return;
  data = NULL;
}

void temporizador(void *datos){
  FILE *aTemp;
  if((aTemp=fopen("/sys/class/thermal/thermal_zone0/temp","r")) ||
	(aTemp=fopen("/sys/class/hwmon/hwmon0/temp1_input","r")))
	{
	  if(fgets(textoTemperatura,3,aTemp)!=0)
			temperatura=atoi(textoTemperatura);
	  fclose(aTemp);
	  carga[1]=temperatura;
	  strcat(textoTemperatura," C");
    } else
	  sprintf(textoTemperatura,"N/D");
  
  glibtop_mem memoria;
  /*glibtop_swap intercambio;*/
  glibtop_get_mem(&memoria);
  /*glibtop_get_swap(&intercambio);*/
  carga[0]=((float)memoria.used*100)/(float)memoria.total;
  
  sprintf(texto,"%d%% %4.f / %4.f",carga[0],(float)memoria.used/1000000,(float)memoria.total/1000000);
  Epplet_gadget_data_changed(barras[0]);
  Epplet_gadget_data_changed(barras[1]);
  Epplet_change_label(visorNumerico,texto);
  
  Epplet_change_label(visorTemperatura,textoTemperatura);
  Esync();
  Epplet_timer(temporizador, NULL, REFRESCO, "TIMER");
  datos = NULL;
}

void cerrar(void *datos){
  Epplet_unremember();
  Esync();
  datos = NULL;
  exit(0);
}

int main (int argc, char **argv)
{
  atexit(Epplet_cleanup);
  Epplet_Init("E-cpuMem", "0.1", "Enlightenment memory usage",
	       5, 3, argc, argv, 0);
  Epplet_timer(temporizador, NULL, 0, "TIMER");
  botonCerrar=Epplet_create_button(NULL, NULL,
					   2, 2, 0, 0, "CLOSE", 0, NULL,
					   cerrar, NULL);
  Epplet_register_focus_in_handler(enFoco,NULL);
  Epplet_register_focus_out_handler(fueraFoco,NULL);
  barras[0] = Epplet_create_hbar(12, 3 + (0 * 16), 62, 12,
				     0, &(carga[0]));
  Epplet_gadget_show(barras[0]);
  barras[1] = Epplet_create_hbar(24, 3 + (1 * 16), 50, 12,
				     0, &(carga[1]));
  Epplet_gadget_show(barras[1]);
  Epplet_gadget_show(Epplet_create_label(3,3,"M",0));
  visorTemperatura=Epplet_create_label(3,19,textoTemperatura,0);
  Epplet_gadget_show(visorTemperatura);
  visorNumerico=Epplet_create_label(3,35,texto,0);
  Epplet_gadget_show(visorNumerico);
  Epplet_show();
  Epplet_Loop();

  return 0;
}
