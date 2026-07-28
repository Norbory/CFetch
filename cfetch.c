#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpuinfo_x86.h"

void battery(void) {
  int percentage = 0;
  char buffer[16] = {0};
  char *output = "No disponible";

  FILE *capacity_file = fopen("/sys/class/power_supply/BAT1/capacity", "r");
  FILE *status_file = fopen("/sys/class/power_supply/BAT1/status", "r");
  // Obtener estado de la batería
  FILE *energy_full = fopen("/sys/class/power_supply/BAT1/energy_full", "r");
  FILE *energy_full_design = fopen("/sys/class/power_supply/BAT1/energy_full_design", "r");

  // Nivel batería
  printf("Nivel de batería: ");
  if (fgets(buffer, 16, capacity_file)){
    buffer[strcspn(buffer, "\n")] = 0;
    output = buffer;
  }
  printf("%s%%\n", output);
  
  // Estado de la batería
  printf("Estado de la batería: ");
  if (fgets(buffer, 16, status_file)){
    buffer[strcspn(buffer, "\n")] = 0;
    output = buffer;
  }
  printf("%s\n", output);

  // Capacidad de la batería
  CalculateBatteryPercentage();

  // Cierrar archivos
  fclose(capacity_file);
  fclose(status_file);
}

void CalculateBatteryPercentage(void) {
  FILE *energy_full = fopen("/sys/class/power_supply/BAT1/energy_full", "r");
  FILE *energy_full_design = fopen("/sys/class/power_supply/BAT1/energy_full_design", "r");

  if (energy_full && energy_full_design) {
    int full_capacity = 0;
    int design_capacity = 0;

    fscanf(energy_full, "%d", &full_capacity);
    fscanf(energy_full_design, "%d", &design_capacity);

    if (design_capacity > 0) {
      int percentage = (full_capacity * 100) / design_capacity;
      printf("Capacidad de la batería: %d%%\n", percentage);
    } else {
      printf("Capacidad de la batería: No disponible\n");
    }
  } else {
    printf("Capacidad de la batería: No disponible\n");
  }

  // Cierra archivos
  if (energy_full) fclose(energy_full);
  if (energy_full_design) fclose(energy_full_design);
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