#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpuinfo_x86.h"

void battery(void) {
  FILE *capacity_file = fopen("/sys/class/power_supply/BAT1/capacity", "r");
  FILE *status_file = fopen("/sys/class/power_supply/BAT1/status", "r");
    
  printf("Nivel de batería: ");
  printf("%s%%\n", capacity_file ? fgets((char[4]){0}, 4, capacity_file) : "N/A");
  printf("Estado de la batería: ");
  printf("%s", status_file ? fgets((char[16]){0}, 16, status_file) : "N/A");
  fclose(capacity_file);
  fclose(status_file);
}

void main(void) {
    X86Info info = GetX86Info();
    
    char brand_string[49];
    FillX86BrandString(brand_string);

    printf("--- CFETCH CPU ---\n");
    printf("Modelo: %s\n", brand_string);
    printf("Arquitectura: %s\n", GetX86MicroarchitectureName(GetX86Microarchitecture(&info)));

    battery();
}