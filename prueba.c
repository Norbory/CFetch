#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include "cpuinfo_x86.h"

// Enum para los nombres de los archivos de batería
enum BatteryFiles {
  "status",
  "capacity",
  "energy_full",
  "energy_full_design"
};

// Abre y obtiene el contenido de un archivo de batería
char *read_battery_file(const char *file_path) {
  // Arma la ruta
  char full_path[256];
  snprintf(full_path, sizeof(full_path), "/sys/class/power_supply/BAT1/%s", file_path);

  // Verifica el contenido del archivo
  FILE *file = fopen(full_path, "r");
  if (!file) {
      return NULL;
  }
  // Registra el valor del archivo
  char buffer[32] = {0};
  char *output = NULL;
  if (fgets(buffer, sizeof(buffer), file)) {
      buffer[strcspn(buffer, "\n")] = 0;
      output = strdup(buffer);
  } else {
      output = strdup("No disponible");
  }
  // Cierra el archivo y retorna el valor
  fclose(file);
  return output;
}

char *get_battery_status() {
  return read_battery_file("status");
}

char *get_battery_capacity() {
  return read_battery_file("capacity");
}

char *get_battery_percentage() {
  char *energy_full = read_battery_file("energy_full");
  char *energy_full_design = read_battery_file("energy_full_design");

  int full_capacity = atoi(energy_full);
  int design_capacity = atoi(energy_full_design);

  if (design_capacity > 0) {
      int percentage = (full_capacity * 100) / design_capacity;
      char *result = malloc(16);
      snprintf(result, 16, "%d%%", percentage);
      return result;
  } else {
      return strdup("No disponible");
  }
}

void label_gtk(char *(*f)(), GtkWidget *box) {
    char *feature = f();
    
    if (feature != NULL) {        
        GtkWidget *label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), feature);
        
        // Asegurar que el label sea visible y tenga alineación
        gtk_widget_set_halign(label, GTK_ALIGN_START); 
        gtk_label_set_xalign(GTK_LABEL(label), 0.0);
        
        gtk_container_add(GTK_CONTAINER(box), label);
        
        // Liberar memoria
        free(feature);
    } else {
        printf("Error: La función devolvió NULL\n");
        // Opcional: Añadir un label de error también en rojo
        GtkWidget *label = gtk_label_new("<span foreground='red'>Error: Sin datos</span>");
        gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
        gtk_container_add(GTK_CONTAINER(box), label);
    }
}

static void window_content (GtkWidget *box) {
  // Make a subtitle label
  GtkWidget *subtitle = gtk_label_new("Battery Features");
  gtk_container_add(GTK_CONTAINER(box), subtitle);

  label_gtk(get_battery_status, box);
  label_gtk(get_battery_capacity, box);
  label_gtk(get_battery_percentage, box);
}

static void on_activate (GtkApplication *app) {
  // Create a new window
  GtkWidget *window = gtk_application_window_new (app);
  gtk_window_set_title(GTK_WINDOW(window), "Primera aplicacion con GTK");
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 400);

  // Crear un contenedor (caja vertical)
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  // Create a new button
  GtkWidget *button = gtk_button_new_with_label ("Hello, World!");

  // Muestra el contenido de la ventana
  window_content(box);

  gtk_container_add(GTK_CONTAINER(box), button);
  gtk_container_add (GTK_CONTAINER (window), box);

  // When the button is clicked, close the window passed as an argument
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_close), window);
  gtk_window_present (GTK_WINDOW (window));
  gtk_widget_show_all(GTK_WIDGET(box));
}

int main (int argc, char *argv[]) {
  // Create a new application
  GtkApplication *app = gtk_application_new ("com.example.GtkApplication",
                                             G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
