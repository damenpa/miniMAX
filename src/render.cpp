#include "render.hpp"

#include <cmath>

namespace {

// El tablero del juego es 3x3 (Tablero::size es privado, asi que el render
// trabaja con su propia constante de presentacion).
constexpr int N = 3;

constexpr Color COLOR_HOVER = { 255, 255, 255, 28 };

void dibujarX(float cx, float cy, float r) {
    DrawLineEx({ cx - r, cy - r }, { cx + r, cy + r }, 9.0f, render::COLOR_X);
    DrawLineEx({ cx - r, cy + r }, { cx + r, cy - r }, 9.0f, render::COLOR_X);
}

void dibujarO(float cx, float cy, float r) {
    DrawRing({ cx, cy }, r - 5.0f, r + 5.0f, 0.0f, 360.0f, 48, render::COLOR_O);
}

// Centro en pixeles de la celda con indice plano 0..8.
Vector2 centroDeCelda(int pos, const Layout& L) {
    int fila = pos / N;
    int col  = pos % N;
    return { L.margen + col * L.celda + L.celda / 2.0f,
             L.margen + fila * L.celda + L.celda / 2.0f };
}

} // namespace

// ---------------------------------------------------------------------------
// Ventana
// ---------------------------------------------------------------------------

void render::iniciar(const Layout& L, const char* titulo) {
    InitWindow(L.anchoVentana(), L.alto, titulo);
    SetTargetFPS(60);
}

void render::cerrar() {
    CloseWindow();
}

bool render::debeCerrar() {
    return WindowShouldClose();
}

void render::frameInicio() {
    BeginDrawing();
    ClearBackground(COLOR_FONDO);
}

void render::frameFin() {
    EndDrawing();
}

// ---------------------------------------------------------------------------
// Tablero
// ---------------------------------------------------------------------------

void render::dibujarTablero(const Tablero& t, const Layout& L) {
    // Cuadricula.
    for (int i = 1; i < N; ++i) {
        int x = L.margen + static_cast<int>(i * L.celda);
        int y = L.margen + static_cast<int>(i * L.celda);

        DrawLine(x, L.margen, x, L.margen + L.lado, COLOR_LINEA);
        DrawLine(L.margen, y, L.margen + L.lado, y, COLOR_LINEA);
    }

    // Fichas.
    float radio = L.celda * 0.28f;
    for (int fila = 0; fila < N; ++fila) {
        for (int col = 0; col < N; ++col) {
            char ficha = t.en(fila, col);
            if (ficha == ' ') continue;

            Vector2 c = centroDeCelda(fila * N + col, L);
            if (ficha == 'X') dibujarX(c.x, c.y, radio);
            else              dibujarO(c.x, c.y, radio);
        }
    }
}

void render::dibujarHover(const Tablero& t, const Layout& L) {
    auto [fila, col] = celdaDesdeMouse(L);
    if (fila < 0 || col < 0) return;
    if (t.en(fila, col) != ' ') return;

    DrawRectangle(static_cast<int>(L.margen + col * L.celda),
                  static_cast<int>(L.margen + fila * L.celda),
                  static_cast<int>(L.celda),
                  static_cast<int>(L.celda),
                  COLOR_HOVER);
}

void render::resaltarLinea(const Layout& L, int c0, int c1, int c2) {
    Vector2 a = centroDeCelda(c0, L);
    Vector2 b = centroDeCelda(c2, L);

    float dx = b.x - a.x;
    float dy = b.y - a.y;
    float largo = std::sqrt(dx * dx + dy * dy);
    if (largo < 1.0f) return;

    float ux = dx / largo;
    float uy = dy / largo;
    float extra = L.celda * 0.30f;

    DrawLineEx({ a.x - ux * extra, a.y - uy * extra },
               { b.x + ux * extra, b.y + uy * extra },
               7.0f, COLOR_VICTORIA);

    (void)c1;   // el trazo va de extremo a extremo; c1 solo documenta la linea
}

// ---------------------------------------------------------------------------
// Texto
// ---------------------------------------------------------------------------

void render::dibujarMensaje(const Layout& L, const char* texto, Color color) {
    DrawText(texto, L.margen, 12, 26, color);
}

void render::dibujarAyuda(const Layout& L, const char* texto) {
    DrawText(texto, L.margen, L.margen + L.lado + 18, 18, COLOR_LINEA);
}

// ---------------------------------------------------------------------------
// Entrada
// ---------------------------------------------------------------------------

std::pair<int, int> render::celdaDesdeMouse(const Layout& L) {
    Vector2 m = GetMousePosition();

    if (m.x < L.margen || m.y < L.margen) return { -1, -1 };
    if (m.x >= L.margen + L.lado || m.y >= L.margen + L.lado) return { -1, -1 };

    int col  = static_cast<int>((m.x - L.margen) / L.celda);
    int fila = static_cast<int>((m.y - L.margen) / L.celda);
    return { fila, col };
}
