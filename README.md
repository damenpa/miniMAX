# GATO — tres en raya con minimax

Interfaz gráfica en [raylib](https://www.raylib.com/) para un tres en raya (gato) en C++17.

El juego tiene tres pantallas: **inicio** (eliges quién empieza), **partida** y
**resultado** (`GANASTE` / `GANÓ LA MÁQUINA` / `EMPATE`, y `Enter` para volver al inicio).

---

## Índice

1. [Qué incluye y qué no](#1-qué-incluye-y-qué-no)
2. [Requisitos](#2-requisitos)
3. [Instalación](#3-instalación)
4. [Compilar y jugar](#4-compilar-y-jugar)
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
| Botones, textos centrados y panel de resultado | Decidir quién empieza |
| Conversión mouse → celda (`{fila, col}`) | Validación de jugadas |

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

**a) Descarga raylib 6.2 precompilada** (~2 MB, según tu arquitectura) en `lib/`:

```bash
mkdir -p lib && cd lib
curl -L -o raylib.tar.gz \
  https://github.com/raysan5/raylib/releases/download/6.2/raylib-6.2_linux_amd64.tar.gz
tar xzf raylib.tar.gz && mv raylib-6.2_linux_amd64 raylib && rm raylib.tar.gz
```

Debes terminar con `lib/raylib/include/raylib.h` y `lib/raylib/lib/libraylib.a`.
En ARM usa `raylib-6.2_linux_arm64.tar.gz`; en 32 bits, `..._linux_i386.tar.gz`.

**b) Crea enlaces de `libGL` / X11** en `lib/`, solo si tu distro trae las
librerías de ejecución (`libGL.so.1`) sin los enlaces `libGL.so` que necesita el
linker. Si ya instalaste los paquetes `-dev` del punto 2, este paso no hace nada.

## 4. Compilar y jugar

```bash
make run
```

Abre la ventana en la pantalla de inicio, donde eliges **dificultad** y **quién
empieza**. `Esc` cierra el juego.

| Comando | Qué hace |
|---|---|
| `make` | compila `src/*.cpp` en `build/miniMAX` |
| `make run` | compila y ejecuta `build/miniMAX` |
| `make debug` | `build/miniMAX-debug` sin optimizar (`-O0 -g`), para `gdb` |
| `make clean` | borra `build/` |

> `src/main.cpp` trae el juego completo: pantalla de inicio, turnos, máquina con
> minimax y pantalla de resultado. Es tu archivo, así que puedes reescribirlo.

Ventana resultante (**840×920**). Pantalla de inicio:

```
┌───────────────────────────────────────────┐
│                  GATO                     │
│        Tres en raya contra minimax        │
│                                           │
│                Dificultad                 │
│   ┌────────┐ ┌────────┐ ┌────────┐        │  <- boton(..., activo)
│   │ Facil  │ │ Medio  │ │Dificil │        │
│   └────────┘ └────────┘ └────────┘        │
│                                           │
│             Quien empieza?                │
│  ┌─────────────────────────────────────┐  │
│  │         Empiezo yo  (X)             │  │
│  └─────────────────────────────────────┘  │
│  ┌─────────────────────────────────────┐  │
│  │      Empieza la maquina  (O)        │  │
│  └─────────────────────────────────────┘  │
│               Esc para salir              │
└───────────────────────────────────────────┘
```

Partida en curso:

```
┌───────────────────────────────────────────┐
│               Tu turno (X)                │  <- dibujarMensaje()
│  ┌───────────┬───────────┬───────────┐    │
│  │     X     │           │           │    │
│  ├───────────┼───────────┼───────────┤    │
│  │     O     │     X     │           │    │
│  ├───────────┼───────────┼───────────┤    │
│  │  ░░░░░░░  │           │     O     │    │  <- ░ = dibujarHover()
│  └───────────┴───────────┴───────────┘    │
│      Click para jugar   -   Esc para salir│  <- dibujarAyuda()
└───────────────────────────────────────────┘
```

Fin de partida (el tablero queda oscurecido detrás):

```
┌───────────────────────────────────────────┐
│  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓    │
│  ▓▓▓┌───────────────────────────────┐▓▓▓    │
│  ▓▓▓│           GANASTE             │▓▓▓    │  <- overlayResultado()
│  ▓▓▓│ Presiona Enter para volver…   │▓▓▓    │
│  ▓▓▓└───────────────────────────────┘▓▓▓    │
└───────────────────────────────────────────┘
```

## 5. API del render

Todo vive en el namespace `render`. `render.hpp` ya incluye `raylib.h`, así que
tienes disponible todo raylib (`IsMouseButtonPressed`, `GetMousePosition`, etc.)
sin includes extra.

### Ventana

| Función | Qué hace |
|---|---|
| `iniciar(const Layout&, const char* titulo)` | abre la ventana, fija 60 FPS y carga la fuente |
| `debeCerrar()` | `true` si cierran la ventana o pulsan `Esc` |
| `frameInicio()` | `BeginDrawing()` + limpia el fondo |
| `frameFin()` | `EndDrawing()` (volcado y swap de buffers) |
| `cerrar()` | `CloseWindow()` |

La tipografía es una sans-serif del sistema (`Ubuntu`, `Liberation Sans`, `DejaVu
Sans` o `Noto Sans`, la primera que exista). Si no encuentra ninguna usa la fuente
de píxeles de raylib, así que el juego nunca se queda sin texto.

### Dibujo

| Función | Qué hace |
|---|---|
| `dibujarTablero(t, L)` | cuadrícula + todas las fichas de `t` |
| `dibujarHover(t, L)` | resalta en tenue la celda libre bajo el mouse |
| `resaltarLinea(L, c0, c1, c2)` | trazo amarillo sobre las celdas `c0..c2` |
| `dibujarMensaje(L, texto, color)` | texto grande, centrado, en la franja superior |
| `dibujarAyuda(L, texto)` | texto pequeño, centrado, en la franja inferior |
| `dibujarTextoCentrado(texto, y, tam, color)` | texto centrado en la ventana, a la altura `y` |

### Widgets

| Función | Qué hace |
|---|---|
| `boton(rect, texto, activo = false)` | dibuja un botón con hover, lo pinta como seleccionado si `activo`, y devuelve `true` si le dieron click |
| `overlayResultado(L, titulo, color, subtitulo)` | oscurece el tablero y pinta el panel de resultado |

`boton()` mezcla dibujo y click a propósito: se llama dentro del frame y su
resultado se guarda para actuar después de `frameFin()`.

```cpp
bool medio = render::boton({ 320, 372, 200, 70 }, "Medio", dificultad == 2);
// ... frameFin() ...
if (medio) dificultad = 2;
```

El título del panel se encoge solo si no cabe, así que `"GANÓ LA MÁQUINA"` entra
igual que `"EMPATE"`.

### Entrada

| Función | Qué hace |
|---|---|
| `celdaDesdeMouse(L)` | devuelve `{fila, col}` de tu matriz |

Devuelve un `std::pair<int, int>` con las coordenadas directas de `mapa[fila][col]`,
así que entra tal cual en `Tablero::marcarPunto(fila, col, ...)`. Si el puntero está
fuera del tablero devuelve `{-1, -1}`:

```cpp
auto [fila, col] = render::celdaDesdeMouse(L);
if (fila >= 0) {
    tablero.marcarPunto(fila, col, 'X');
}
```

### Colores

```cpp
render::COLOR_FONDO        // fondo de la ventana
render::COLOR_X            // rojo de las X
render::COLOR_O            // azul de las O
render::COLOR_LINEA        // cuadrícula y textos secundarios
render::COLOR_TEXTO        // mensajes por defecto
render::COLOR_VICTORIA     // amarillo de la línea ganadora
render::COLOR_BOTON        // relleno de los botones
render::COLOR_BOTON_HOVER  // relleno del botón bajo el mouse
render::COLOR_BOTON_ACTIVO // relleno del botón seleccionado (amarillo)
render::COLOR_OVERLAY      // velo oscuro del panel de resultado
```

## 6. Cómo conectarlo a tu lógica

Contrato mínimo entre tu lógica y el render. Los comentarios marcan **dónde va lo
tuyo**; todo lo demás es una llamada al render. El `main.cpp` real añade las tres
pantallas explicadas más abajo.

```cpp
#include "render.hpp"     // ya trae raylib.h y tablero.hpp

int main() {
    Layout L;
    render::iniciar(L, "GATO");

    Tablero tablero;
    // ... tu estado: turno, partida terminada, etc.

    while (!render::debeCerrar()) {
        // ---- 1. ENTRADA (tuya) ----
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            auto [fila, col] = render::celdaDesdeMouse(L);
            if (fila >= 0) {
                // fila y col son las coordenadas de tu matriz
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

### Las tres pantallas

`main.cpp` es una máquina de estados con tres valores:

```cpp
enum class Pantalla { Inicio, Jugando, Fin };
```

- **Inicio**: los tres `boton()` de dificultad (el elegido se pinta con `activo`) y
  los dos de quién empieza. Al pulsar uno de inicio se resetea el tablero y se fija
  `turnoHumano` (`true` = empiezas tú, `false` = empieza la máquina).
- **Jugando**: si es turno humano y hay click, se marca `X`; si no, la máquina llama
  a `mejorMovimiento(tablero.mapa, dificultad)` y marca `O`. Al final se comprueba si
  la partida terminó.
- **Fin**: `overlayResultado()` con el título del ganador; `Enter` vuelve a `Inicio`
  (la dificultad elegida se conserva).

Si se elige "empieza la máquina", `turnoHumano` arranca en `false`, así que la `O`
entra antes del primer click.

### Niveles de dificultad

`mejorMovimiento()` recibe la dificultad y la traduce a una profundidad máxima y a
una probabilidad de jugar al azar:

| Nivel | `dificultad` | `maxProfundidad` | Azar | Qué hace |
|---|---|---|---|---|
| Fácil | 1 | 1 | 55% | solo heurística, y más de la mitad de las veces juega al azar |
| Medio | 2 | 2 | 20% | mira una jugada tuya por delante |
| Difícil | 3 | 9 | 0% | minimax completo: no pierde nunca |

La heurística (`evalHeuristica`) puntúa cada fila, columna y diagonal: `+3` por dos
fichas propias, `-3` por dos rivales, `±1` con una sola ficha y `0` si la línea está
bloqueada. Además suma `+1` si la máquina controla el centro. Los estados de victoria
siguen valiendo `±100`, muy por encima de cualquier heurística, así que la máquina
siempre remata y bloquea las jugadas inmediatas.

El azar es lo que hace que los niveles flojos se puedan ganar: cuando toca jugada
aleatoria, la máquina puede dejar pasar un bloqueo o un remate. Los dos números se
ajustan arriba de todo en `src/minimax.cpp`:

```cpp
constexpr int AZAR_FACIL = 55;   // 0 = nunca falla, 100 = siempre al azar
constexpr int AZAR_MEDIO = 20;
```

Medido sobre 400 partidas por nivel:

| Nivel | Gana el humano (principiante) | Gana el humano (X perfecto) |
|---|---|---|
| Fácil | 50% | 84% |
| Medio | 18% | 42% |
| Difícil | 0% | 0% |

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
    int   margen   = 60;      // margen lateral
    int   yTablero = 120;     // altura donde empieza el tablero
    int   lado     = 720;     // lado del tablero en px
    float celda    = 240.0f;  // debe ser lado / 3
    int   footer   = 80;      // franja inferior para dibujarAyuda()

    int anchoVentana() const;  // margen * 2 + lado            -> 840
    int altoVentana()  const;  // yTablero + lado + footer     -> 920
};
```

Cambiar `lado` es suficiente; **si cambias `lado`, actualiza `celda` a `lado / 3`**
para que la cuadrícula y el mouse sigan coincidiendo. `yTablero` te da espacio arriba
para `dibujarMensaje()` y `footer` espacio abajo para `dibujarAyuda()`.

Los colores son constantes en el mismo header. Cambiarlas ahí afecta a todo.

## 8. Estructura de archivos

```
miniMAX/
├── include/
│   ├── render.hpp          # API pública del render
│   ├── minimax.hpp         # eval / minimax / mejorMovimiento
│   └── tablero.hpp         # TU tablero (+ el getter en())
├── src/
│   ├── render.cpp          # implementación con raylib
│   ├── minimax.cpp         # TU minimax + heurística por niveles
│   ├── tablero.cpp         # TU lógica
│   └── main.cpp            # TU bucle de juego
├── scripts/
│   └── setup.sh            # descarga raylib
├── lib/                    # raylib + symlinks (no se versiona)
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
| Bajo Wayland no aparece la ventana | raylib precompilada usa backend X11 | `env -u WAYLAND_DISPLAY make run` (fuerza XWayland) |
| El texto sale con la fuente de píxeles | No hay ninguna sans-serif del sistema | Instala `fonts-dejavu` o `fonts-liberation` |
| Los clicks caen en otra celda | Escalado HiDPI | Ajusta `Layout::celda` o añade `SetWindowState(FLAG_WINDOW_HIGHDPI)` en `main` |
| `'X'` y `'O'` no aparecen | `Tablero::en()` devuelve otra cosa | Verifica que devuelve `' ', 'X'` o `'O'` |

Comprobación rápida de tu entorno:

```bash
glxinfo -B        # GPU + versión de OpenGL
echo $DISPLAY     # no debe estar vacío
```

---

**Licencia**: uso libre. raylib se distribuye bajo zlib/libpng (ver
`lib/raylib/LICENSE`).
