#pragma once
#include "raylib.h"
#include "tablero.hpp"

#include <utility>

// Geometria de la ventana. Todo el dibujo sale de aqui, asi que cambiar
// el tamano es tocar solo esta estructura.
struct Layout {
    int   margen   = 60;      // margen lateral
    int   yTablero = 120;     // donde empieza el tablero (arriba)
    int   lado     = 720;     // lado del tablero en pixeles
    float celda    = 240.0f;  // debe valer lado / 3
    int   footer   = 80;      // franja inferior

    int anchoVentana() const { return margen * 2 + lado; }        // 840
    int altoVentana()  const { return yTablero + lado + footer; }  // 920
};

namespace render {

// ---------------------------------------------------------------------------
// Paleta
// ---------------------------------------------------------------------------
inline constexpr Color COLOR_FONDO       = { 24, 24, 34, 255 };
inline constexpr Color COLOR_X           = { 230, 90, 90, 255 };
inline constexpr Color COLOR_O           = { 90, 170, 240, 255 };
inline constexpr Color COLOR_LINEA       = { 200, 200, 210, 255 };
inline constexpr Color COLOR_TEXTO       = { 235, 235, 245, 255 };
inline constexpr Color COLOR_VICTORIA    = { 250, 210, 90, 255 };
inline constexpr Color COLOR_BOTON       = { 44, 46, 62, 255 };
inline constexpr Color COLOR_BOTON_HOVER = { 72, 78, 108, 255 };
inline constexpr Color COLOR_BOTON_ACTIVO = { 250, 210, 90, 255 };
inline constexpr Color COLOR_OVERLAY     = { 14, 14, 22, 230 };

// ---------------------------------------------------------------------------
// Ciclo de vida de la ventana
// ---------------------------------------------------------------------------
void iniciar(const Layout& L, const char* titulo);
void cerrar();
bool debeCerrar();

void frameInicio();
void frameFin();

// ---------------------------------------------------------------------------
// Pantallas: lo unico que main necesita para dibujar
// ---------------------------------------------------------------------------

// Lo que eligio el usuario en la pantalla de inicio.
struct AccionInicio {
    bool empezoHumano  = false;  // pulso "Empiezo yo (X)"
    bool empezoMaquina = false;  // pulso "Empieza la maquina (O)"
    int  dificultad    = 0;      // 0 = no cambio la dificultad
};

// Dibuja la pantalla de inicio y devuelve lo que se pulso.
AccionInicio pantallaInicio(const Layout& L, int dificultadActual);

// Dibuja la partida en curso: tablero, turno y dificultad.
void pantallaJuego(const Layout& L, const Tablero& t, bool turnoHumano, int dificultad);

// Dibuja el tablero final con el cartel del resultado.
void pantallaFin(const Layout& L, const Tablero& t);

// ---------------------------------------------------------------------------
// Piezas de dibujo (para armar pantallas propias)
// ---------------------------------------------------------------------------
void dibujarTablero(const Tablero& t, const Layout& L);
void dibujarHover(const Tablero& t, const Layout& L);
void resaltarLinea(const Layout& L, int c0, int c1, int c2);

void dibujarMensaje(const Layout& L, const char* texto, Color color);
void dibujarAyuda(const Layout& L, const char* texto);
void dibujarTextoCentrado(const char* texto, int y, int tam, Color color);

// Dibuja un boton con hover. `activo` lo pinta como seleccionado.
// Devuelve true si el usuario le dio click.
bool boton(Rectangle r, const char* texto, bool activo = false);

// Oscurece el tablero y muestra el resultado en grande.
void overlayResultado(const Layout& L, const char* titulo, Color color,
                      const char* subtitulo);

// ---------------------------------------------------------------------------
// Entrada
// ---------------------------------------------------------------------------

// Celda bajo el mouse, ya en coordenadas de la matriz del Tablero:
// {fila, col}. Si el puntero esta fuera del tablero devuelve {-1, -1}.
std::pair<int, int> celdaDesdeMouse(const Layout& L);

} // namespace render
