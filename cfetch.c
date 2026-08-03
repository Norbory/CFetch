#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "cpuinfo_x86.h"

// Constantes
const int WIDTH = 256;
const int HEIGHT = 256;

// Función básica para codificar Base64 (Requerida por el protocolo Kitty)
char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length) {
    const char encoding_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    *output_length = 4 * ((input_length + 2) / 3);
    char *encoded_string = malloc(*output_length + 1);
    if (!encoded_string) return NULL;

    for (size_t i = 0, j = 0; i < input_length;) {
        uint32_t octet_a = i < input_length ? data[i++] : 0;
        uint32_t octet_b = i < input_length ? data[i++] : 0;
        uint32_t octet_c = i < input_length ? data[i++] : 0;
        uint32_t triple = (octet_a << 16) + (octet_b << 8) + octet_c;
        encoded_string[j++] = encoding_table[(triple >> 18) & 0x3F];
        encoded_string[j++] = encoding_table[(triple >> 12) & 0x3F];
        encoded_string[j++] = encoding_table[i > input_length + 1 ? '=' : (triple >> 6) & 0x3F];
        encoded_string[j++] = encoding_table[i > input_length ? '=' : triple & 0x3F];
    }
    encoded_string[*output_length] = '\0';
    return encoded_string;
}

int displayImage() {
    // 1. Inicializar GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Error al inicializar GLFW\n");
        return -1;
    }

    // Ocultar la ventana (Crucial para que no parpadee en el escritorio)
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); 

    int width = 256;
    int height = 256;
    GLFWwindow* window = glfwCreateWindow(width, height, "cfetch_buffer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 2. Renderizar algo en OpenGL (Ejemplo: Un fondo azul con un triángulo rojo)
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f); // Color de fondo
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f); glVertex2f(0.0f, 0.5f);
        glColor3f(0.0f, 1.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
        glColor3f(0.0f, 0.0f, 1.0f); glVertex2f(0.5f, -0.5f);
    glEnd();

    glFlush(); // Asegurar que la GPU termine de dibujar

    // 3. Leer los pixeles de la GPU a la memoria RAM
    size_t data_size = width * height * 3; // RGB (3 bytes por pixel)
    unsigned char* pixels = (unsigned char*)malloc(data_size);
    
    // Leemos el buffer trasero de OpenGL
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Voltear la imagen verticalmente (OpenGL empieza abajo a la izquierda, la terminal arriba)
    unsigned char* flipped_pixels = (unsigned char*)malloc(data_size);
    for (int y = 0; y < height; y++) {
        memcpy(&flipped_pixels[y * width * 3], &pixels[(height - 1 - y) * width * 3], width * 3);
    }

    // 4. Codificar los pixeles crudos a Base64
    size_t b64_len;
    char* b64_data = base64_encode(flipped_pixels, data_size, &b64_len);

    // 5. Enviar comando del protocolo Kitty Graphics a la Terminal
    // f=24 significa formato RGB crudo (sin compresión PNG)
    // s=width, v=height especifican las dimensiones de los pixeles enviados
    printf("\033_Ga=T,f=24,s=%d,v=%d;%s\033\\\n", width, height, b64_data);
    fflush(stdout);

    // Limpieza de memoria
    free(pixels);
    free(flipped_pixels);
    free(b64_data);
    glfwDestroyWindow(window);
    glfwTerminate();

    // Aquí continuaría el resto del texto tradicional de tu cfetch...
    printf("  \033[1;36mcfetch\033[0m\n");
    printf("  OS: Linux\n");
    
    return 0;
}

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
