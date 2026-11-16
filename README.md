# miniMAX — capa de render

Interfaz gráfica en [raylib](https://www.raylib.com/) para un tres en raya (gato) en C++17.

---

## Índice

1. [Qué incluye y qué no](#1-qué-incluye-y-qué-no)
2. [Requisitos](#2-requisitos)
3. [Instalación](#3-instalación)
4. [Compilar y ver la demo](#4-compilar-y-ver-la-demo)
5. [API del render](#5-api-del-render)
6. [Cómo conectarlo a tu lógica](#6-cómo-conectarlo-a-tu-lógica)
7. [Ajustar el tamaño y los colores](#7-ajustar-el-tamaño-y-los-colores)
8. [Estructura de archivos](#8-estructura-de-archivos)
9. [Problemas comunes](#9-problemas-comunes)

---

## 1. Qué incluye y qué no

| Incluye | No incluye |
|---|---|
| Ventana, bucle de frames, cierre con Esc | Reglas del juego |
| Cuadrícula 3×3 | Quién ganó |
| Fichas `X` y `O` dibujadas geométricamente | De quién es el turno |
| Resaltado de la celda bajo el mouse | IA / minimax |
| Línea amarilla sobre la jugada ganadora | Detección de empate |
| Texto de estado y de ayuda | Reinicio de partida |
| Conversión mouse → celda (0..8) | Validación de jugadas |

La frontera es simple: **el render nunca modifica el tablero**. Todas sus funciones
reciben el tablero como `const&` y solo llaman a `Tablero::en(fila, col)`.

## 2. Requisitos

| Necesitas | Cómo comprobarlo |
|---|---|
| Linux con servidor gráfico (X11 o Wayland) | `echo $DISPLAY` |
| `g++` con C++17 | `g++ --version` |
| `curl` y `tar` | `curl --version` |
| OpenGL (Mesa) | `ls /usr/lib/*/libGL.so.1` |

Headers de desarrollo para enlazar:

```bash
# Ubuntu / Debian
sudo apt install libgl1-mesa-dev libx11-dev libxrandr-dev libxi-dev \
                 libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev

# Fedora
sudo dnf install mesa-libGL-devel libX11-devel libXrandr-devel libXi-devel \
                 libXcursor-devel libXinerama-devel wayland-devel libxkbcommon-devel

# Arch
sudo pacman -S mesa libx11 libxrandr libxi libxcursor libxinerama wayland libxkbcommon
```

raylib **no** se instala con el gestor de paquetes: se descarga precompilada en el
paso siguiente (no hace falta `cmake` ni compilar nada).

## 3. Instalación

```bash
./scripts/setup.sh
```

El script hace dos cosas:

**a) Descarga raylib 6.2 precompilada** (~2 MB, según tu arquitectura) en `vendor/`:

```bash
mkdir -p vendor && cd vendor
curl -L -o raylib.tar.gz \
  https://github.com/raysan5/raylib/releases/download/6.2/raylib-6.2_linux_amd64.tar.gz
tar xzf raylib.tar.gz && mv raylib-6.2_linux_amd64 raylib && rm raylib.tar.gz
```

Debes terminar con `vendor/raylib/include/raylib.h` y `vendor/raylib/lib/libraylib.a`.
En ARM usa `raylib-6.2_linux_arm64.tar.gz`; en 32 bits, `..._linux_i386.tar.gz`.

**b) Crea enlaces de `libGL` / X11** en `vendor/lib/`, solo si tu distro trae las
librerías de ejecución (`libGL.so.1`) sin los enlaces `libGL.so` que necesita el
linker. Si ya instalaste los paquetes `-dev` del punto 2, este paso no hace nada.

## 4. Compilar y ver la demo

```bash
make demo && ./build/miniMAX-demo
```

Abre la ventana con un tablero de ejemplo para que veas colores, hover y tipografías.
`Esc` la cierra. **La demo no juega**: solo dibuja.

| Comando | Qué hace |
|---|---|
| `make` | compila **tu** `src/main.cpp` en `build/miniMAX` |
| `make run` | ejecuta `build/miniMAX` |
| `make demo` | compila `examples/demo_render.cpp` en `build/miniMAX-demo` |
| `make debug` | `build/miniMAX-debug` sin optimizar (`-O0 -g`), para `gdb` |
| `make clean` | borra `build/` |

> Ahora mismo `src/main.cpp` está vacío, así que `make run` abre y cierra al
> instante. Es tu archivo: ahí va tu lógica.

Ventana resultante (560×600):

```
┌─────────────────────────────┐
│  Ganaste!                   │  <- dibujarMensaje()
│  ┌─────┬─────┬─────┐        │
│  │  X──┼──X──┼──X  │        │  <- resaltarLinea()
│  ├─────┼─────┼─────┤        │
│  │     │  O  │     │        │
│  ├─────┼─────┼─────┤        │
│  │ ░░░ │     │  O  │        │  <- ░ = dibujarHover()
│  └─────┴─────┴─────┘        │
│  El juego lo controla ...   │  <- dibujarAyuda()
└─────────────────────────────┘
```

## 5. API del render

Todo vive en el namespace `render`. `render.hpp` ya incluye `raylib.h`, así que
tienes disponible todo raylib (`IsMouseButtonPressed`, `GetMousePosition`, etc.)
sin includes extra.

### Ventana

| Función | Qué hace |
|---|---|
| `iniciar(const Layout&, const char* titulo)` | abre la ventana y fija 60 FPS |
| `debeCerrar()` | `true` si cierran la ventana o pulsan `Esc` |
| `frameInicio()` | `BeginDrawing()` + limpia el fondo |
| `frameFin()` | `EndDrawing()` (volcado y swap de buffers) |
| `cerrar()` | `CloseWindow()` |

### Dibujo

| Función | Qué hace |
|---|---|
| `dibujarTablero(t, L)` | cuadrícula + todas las fichas de `t` |
| `dibujarHover(t, L)` | resalta en tenue la celda libre bajo el mouse |
| `resaltarLinea(L, c0, c1, c2)` | trazo amarillo sobre las celdas `c0..c2` |
| `dibujarMensaje(L, texto, color)` | texto grande en la franja superior |
| `dibujarAyuda(L, texto)` | texto pequeño en la franja inferior |

### Entrada

| Función | Qué hace |
|---|---|
| `celdaDesdeMouse(L)` | celda bajo el mouse como `0..8`, o `-1` si está fuera |

El índice `0..8` es plano: **`pos / 3` es la fila y `pos % 3` la columna**, o sea
lo mismo que `fila * 3 + col`. Encaja directo con `Tablero::marcarPunto(fila, col, ...)`.

### Colores

```cpp
render::COLOR_FONDO     // fondo de la ventana
render::COLOR_X         // rojo de las X
render::COLOR_O         // azul de las O
render::COLOR_LINEA     // cuadrícula
render::COLOR_TEXTO     // mensajes por defecto
render::COLOR_VICTORIA  // amarillo de la línea ganadora
```

## 6. Cómo conectarlo a tu lógica

El esqueleto es este. Los comentarios marcan **dónde va lo tuyo**; todo lo demás
es una llamada al render.

```cpp
#include "render.hpp"     // ya trae raylib.h y tablero.hpp

int main() {
    Layout L;
    render::iniciar(L, "miniMAX");

    Tablero tablero;
    // ... tu estado: turno, partida terminada, etc.

    while (!render::debeCerrar()) {
        // ---- 1. ENTRADA (tuya) ----
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int pos = render::celdaDesdeMouse(L);
            if (pos >= 0) {
                // tu lógica: validar turno, marcar, llamar a minimax...
            }
        }

        // ---- 2. DIBUJO (render) ----
        render::frameInicio();
            render::dibujarTablero(tablero, L);
            render::dibujarHover(tablero, L);
            // render::resaltarLinea(L, a, b, c);
            // render::dibujarMensaje(L, "Tu turno", render::COLOR_TEXTO);
            render::dibujarAyuda(L, "Esc para salir");
        render::frameFin();
    }

    render::cerrar();
    return 0;
}
```

### El único requisito sobre tu `Tablero`

El render necesita leer las celdas. Para eso tu clase debe exponer:

```cpp
char en(int fila, int col) const;   // ' ', 'X' u 'O'
```

Es el único método que se agregó a tu `Tablero` (un getter de solo lectura, sin
lógica). Si prefieres otro nombre, cambia la llamada en `src/render.cpp:76`.

### Dos detalles que te van a ahorrar tiempo

**El mouse ya usa el mismo eje que el dibujo.** No hay que invertir `Y`: raylib
tiene el origen arriba a la izquierda, igual que `GetMousePosition()`. Ese es el
bug clásico de OpenGL y aquí no existe.

**No hace falta llamar a `dibujarHover` cuando la partida terminó.** El render no
sabe cuándo termina, así que resalta siempre que haya una celda libre. Ese `if` es
decisión tuya:

```cpp
if (!partidaTerminada) render::dibujarHover(tablero, L);
```

## 7. Ajustar el tamaño y los colores

Todo el layout sale de una sola estructura en `include/render.hpp`:

```cpp
struct Layout {
    int   margen = 40;      // espacio alrededor del tablero
    int   lado   = 480;     // lado del tablero en px
    float celda  = 160.0f;  // debe ser lado / 3
    int   alto   = 600;     // alto total de la ventana
};
```

Cambiar `lado` y `alto` es suficiente; **si cambias `lado`, actualiza `celda` a
`lado / 3`** para que la cuadrícula y el mouse sigan coincidiendo. El ancho de la
ventana se calcula solo: `margen * 2 + lado`.

Los colores son constantes en el mismo header. Cambiarlas ahí afecta a todo.

## 8. Estructura de archivos

```
miniMAX/
├── include/
│   ├── render.hpp          # API pública del render
│   └── tablero.hpp         # TU tablero (+ el getter en())
├── src/
│   ├── render.cpp          # implementación con raylib
│   ├── tablero.cpp         # TU lógica
│   └── main.cpp            # TU bucle de juego
├── examples/
│   └── demo_render.cpp     # demo que no toca tu main
├── scripts/
│   └── setup.sh            # descarga raylib
├── vendor/                 # raylib + symlinks (no se versiona)
├── build/                  # binarios (no se versiona)
└── makefile
```

Quién conoce qué:

| Archivo | Sabe de raylib | Modifica el tablero |
|---|---|---|
| `render.cpp` | ✅ | ❌ (solo `en()`) |
| `tablero.cpp` | ❌ | ✅ |
| `main.cpp` | opcional | ✅ |

`tablero.cpp` conserva tu `dibujar()` por consola: sirve para depurar sin abrir
ventana y no depende del render.

## 9. Problemas comunes

| Síntoma | Causa | Solución |
|---|---|---|
| `cannot find -lGL` / `cannot find -lX11` | Faltan enlaces `.so` de desarrollo | Instala los paquetes `-dev` del [punto 2](#2-requisitos) o corre `./scripts/setup.sh` |
| `fatal error: raylib.h: No such file` | No descargaste raylib | `./scripts/setup.sh` |
| `error: GLFW: Failed to initialize Window` | Sin servidor gráfico (SSH, contenedor) | Necesitas un `DISPLAY` real |
| La ventana se ve negra o crashea | Driver GL problemático | `LIBGL_ALWAYS_SOFTWARE=1 make run` |
| Bajo Wayland no aparece la ventana | raylib precompilada usa backend X11 | `env -u WAYLAND_DISPLAY make demo` (fuerza XWayland) |
| Los clicks caen en otra celda | Escalado HiDPI | Ajusta `Layout::celda` o añade `SetWindowState(FLAG_WINDOW_HIGHDPI)` en `main` |
| `'X'` y `'O'` no aparecen | `Tablero::en()` devuelve otra cosa | Verifica que devuelve `' ', 'X'` o `'O'` |

Comprobación rápida de tu entorno:

```bash
glxinfo -B        # GPU + versión de OpenGL
echo $DISPLAY     # no debe estar vacío
```

---

**Licencia**: uso libre. raylib se distribuye bajo zlib/libpng (ver
`vendor/raylib/LICENSE`).
