#include <stdio.h>

int main(int argc, char **argv) {
    char* textoTemperatura;
    int temperatura;
    textoTemperatura=malloc(8);
    FILE *aTemp;
    aTemp=fopen("/sys/class/hwmon/hwmon0/temp1_input","r");
    fgets(textoTemperatura,3,aTemp);
    fclose(aTemp);
    temperatura=atoi(textoTemperatura);
    if(temperatura>97)system("cpufreq-set -f 800");
    else system("cpufreq-set -g ondemand");
    free(textoTemperatura);
    return 0;
}
