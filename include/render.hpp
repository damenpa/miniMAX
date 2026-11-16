#pragma once
#include "raylib.h"
#include "tablero.hpp"

// Geometria de la ventana. Todo el dibujo sale de aqui, asi que cambiar
// el tamano es tocar solo esta estructura.
struct Layout {
    int   margen = 40;
    int   lado   = 480;      // lado del tablero en pixeles
    float celda  = 160.0f;   // debe valer lado / 3
    int   alto   = 600;      // alto total de la ventana

    int anchoVentana() const { return margen * 2 + lado; }
};

namespace render {

// ---------------------------------------------------------------------------
// Paleta. Usala para los mensajes y para lo que agregues, asi todo combina.
// ---------------------------------------------------------------------------
inline constexpr Color COLOR_FONDO    = { 24, 24, 34, 255 };
inline constexpr Color COLOR_X        = { 230, 90, 90, 255 };
inline constexpr Color COLOR_O        = { 90, 170, 240, 255 };
inline constexpr Color COLOR_LINEA    = { 200, 200, 210, 255 };
inline constexpr Color COLOR_TEXTO    = { 235, 235, 245, 255 };
inline constexpr Color COLOR_VICTORIA = { 250, 210, 90, 255 };

// ---------------------------------------------------------------------------
// Ciclo de vida de la ventana
// ---------------------------------------------------------------------------
void iniciar(const Layout& L, const char* titulo);
void cerrar();

// true cuando el usuario cierra la ventana o pulsa Esc.
bool debeCerrar();

// Envoltorio de BeginDrawing + ClearBackground y de EndDrawing. Llamalos
// una vez por frame, en ese orden.
void frameInicio();
void frameFin();

// ---------------------------------------------------------------------------
// Dibujo del tablero (solo lee el Tablero, nunca lo modifica)
// ---------------------------------------------------------------------------
void dibujarTablero(const Tablero& t, const Layout& L);

// Resalta en tenue la celda libre bajo el mouse. Llamalo solo si la
// partida sigue en curso (esa decision es de tu logica, no del render).
void dibujarHover(const Tablero& t, const Layout& L);

// Dibuja una linea sobre tres celdas, para marcar la jugada ganadora.
// c0, c1 y c2 son indices planos 0..8 (fila * 3 + col).
void resaltarLinea(const Layout& L, int c0, int c1, int c2);

// Texto de estado en la franja superior de la ventana.
void dibujarMensaje(const Layout& L, const char* texto, Color color);

// Texto de ayuda en la franja inferior.
void dibujarAyuda(const Layout& L, const char* texto);

// ---------------------------------------------------------------------------
// Entrada
// ---------------------------------------------------------------------------

// Celda bajo el mouse como indice plano 0..8, o -1 si el puntero esta
// fuera del tablero.
int celdaDesdeMouse(const Layout& L);

} // namespace render
