#include <epplet.h>
#include <stdio.h>

Epplet_gadget botonCerrar;
Epplet_gadget visorNumerico;
Epplet_gadget visorTemperatura;
Epplet_gadget barra;
int temperatura;
const int REFRESCO = 4;
char* texto;
char* textoTemperatura;

void temporizador(void *datos){
  FILE *aTemp;
  FILE *aFreq;
  aTemp=fopen("/sys/class/hwmon/hwmon0/temp1_input","r");
  fgets(textoTemperatura,3,aTemp);
  fclose(aTemp);
  aFreq=popen("cpufreq-info -f","r");
  fgets(texto,5,aFreq);
  if(texto[0]=='8')texto[3]='\0';
  pclose(aFreq);
  
  temperatura=atoi(textoTemperatura);
  Epplet_gadget_data_changed(barra);
  sprintf(textoTemperatura,"%s C",textoTemperatura);
  Epplet_change_label(visorNumerico,texto);
  Epplet_change_label(visorTemperatura,textoTemperatura);
  Esync();
  Epplet_timer(temporizador, NULL, REFRESCO, "TIMER");
  //datos = NULL;
}
void cerrar(void *datos){
  Epplet_unremember();
  Esync();
  //datos = NULL;
  exit(0);
}
static void enFoco(void *data, Window w){
  if (w == Epplet_get_main_window())
     {
	Epplet_gadget_show(botonCerrar);
     }
  return;
  //data = NULL;
}
static void fueraFoco(void *data, Window w){
  if (w == Epplet_get_main_window())
     {
	Epplet_gadget_hide(botonCerrar);
     }
  return;
  //data = NULL;
}
int main (int argc, char **argv)
{
  texto=malloc(8);
  textoTemperatura=malloc(8);
  atexit(Epplet_cleanup);
  Epplet_Init("E-cpuStat", "0.1", "Enlightenment cpu temp/freq",
	       3, 3, argc, argv, 0);
  Epplet_timer(temporizador, NULL, 0, "TIMER");
  botonCerrar=Epplet_create_button(NULL, NULL,
					   2, 2, 0, 0, "CLOSE", 0, NULL,
					   cerrar, NULL);
  Epplet_register_focus_in_handler(enFoco,NULL);
  Epplet_register_focus_out_handler(fueraFoco,NULL);
  barra = Epplet_create_vbar(32, 3, 12, 41,
				     1, &temperatura);
  Epplet_gadget_show(barra);
  visorNumerico=Epplet_create_label(3,35,texto,0);
  visorTemperatura=Epplet_create_label(6,18,textoTemperatura,0);
  Epplet_gadget_show(visorTemperatura);
  Epplet_gadget_show(visorNumerico);
  Epplet_show();
  Epplet_Loop();
  return 0;
}