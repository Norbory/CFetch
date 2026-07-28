# CFETCH

CFETCH es una utilidad de línea de comandos escrita en C que emula la funcionalidad de neofetch, mostrando información detallada del sistema de manera rápida y eficiente. A diferencia de las alternativas basadas en scripts, CFETCH utiliza la biblioteca `cpu-features` de Google para obtener características precisas de la CPU a nivel de hardware, junto con lecturas directas del kernel para el estado de la batería.

## Características

- **Rendimiento Superior**: Escrito en C compilado, la ejecución es casi instantánea.
- **Precisión de Hardware**: Usa `cpu-features` para detectar arquitectura, microarquitectura y banderas de la CPU.
- **Información de Energía**: Lee directamente `/sys/class/power_supply` para estado y porcentaje de batería.
- **Sin Dependencias Pesadas**: Solo requiere la biblioteca `cpu-features` y un entorno POSIX estándar.

## Vista Previa en Terminal

Así es como se ve la salida de información en tu shell:

```text
--- CFETCH CPU ---
Modelo: AMD A8-6410 APU with AMD Radeon R5 Graphics
Arquitectura: AMD_PUMA
Nivel de batería: 94%
Estado de la batería: Charging
Capacidad de la batería: 58%
```

O en un sistema Intel:
```text
--- CFETCH CPU ---
Modelo: Intel(R) Core(TM) i7-8550U CPU @ 1.80GHz
Arquitectura: KABYLAKE
Nivel de batería: 100%
Estado de la batería: Full
Capacidad de la batería: 80%
```

## Requisitos Previos

Compilador C (gcc, clang)
CMake
Biblioteca cpu-features de Google

## Instalación y Compilación

1. Clonar el repositorio e instalar dependencias
Asegúrate de tener la biblioteca cpu-features instalada en tu sistema.

```bash
# En Ubuntu/Debian (si está disponible en repos o compilar desde fuente)
sudo apt-get install libcpu-features-dev

# O compilar cpu-features manualmente:
git clone https://github.com/google/cpu_features.git
cd cpu_features
cmake -S. -Bbuild -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --build build --target install
```

2. Compilar CFETCH
```bash
git clone https://github.com/tu-usuario/cfetch.git
cd cfetch
mkdir build && cd build
cmake ..
make
```

## Uso

Simplemente ejecuta el binario generado:
```bash
./cfetch

# O si lo colocas en un folder de tu path
cfecth
```

## Cómo funciona

### CPU
El proyecto enlaza con libcpu_features. Para arquitecturas x86, utiliza GetX86Info() para obtener el nombre de la marca (brand_string) y GetX86Microarchitecture() para identificar la arquitectura específica (ej. AMD_PUMA, INTEL_SKL).

## Batería

La información de la batería se obtiene leyendo archivos del sistema en Linux:

- Nivel: /sys/class/power_supply/BAT0/capacity
- Estado: /sys/class/power_supply/BAT0/status
- Capacidad: /sys/class/power_supply/BAT0/energy_full o charge_full

---
## Licencia

Este proyecto está bajo la licencia MIT. La biblioteca cpu-features está bajo la licencia Apache 2.0.