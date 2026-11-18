#include "render.hpp"

#include <cmath>

namespace {

// El tablero del juego es 3x3 (Tablero::size).
constexpr int N = 3;

constexpr Color COLOR_HOVER = { 255, 255, 255, 28 };

void dibujarX(float cx, float cy, float r) {
    DrawLineEx({ cx - r, cy - r }, { cx + r, cy + r }, 12.0f, render::COLOR_X);
    DrawLineEx({ cx - r, cy + r }, { cx + r, cy - r }, 12.0f, render::COLOR_X);
}

void dibujarO(float cx, float cy, float r) {
    DrawRing({ cx, cy }, r - 7.0f, r + 7.0f, 0.0f, 360.0f, 56, render::COLOR_O);
}

// Centro en pixeles de la celda con indice plano 0..8.
Vector2 centroDeCelda(int pos, const Layout& L) {
    int fila = pos / N;
    int col  = pos % N;
    return { L.margen + col * L.celda + L.celda / 2.0f,
             L.yTablero + fila * L.celda + L.celda / 2.0f };
}

} // namespace

// ---------------------------------------------------------------------------
// Ventana
// ---------------------------------------------------------------------------

void render::iniciar(const Layout& L, const char* titulo) {
    InitWindow(L.anchoVentana(), L.altoVentana(), titulo);
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
        int y = L.yTablero + static_cast<int>(i * L.celda);

        DrawLine(x, L.yTablero, x, L.yTablero + L.lado, COLOR_LINEA);
        DrawLine(L.margen, y, L.margen + L.lado, y, COLOR_LINEA);
    }

    // Marco exterior.
    DrawRectangleLines(L.margen, L.yTablero, L.lado, L.lado, COLOR_LINEA);

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
                  static_cast<int>(L.yTablero + fila * L.celda),
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
               9.0f, COLOR_VICTORIA);

    (void)c1;   // el trazo va de extremo a extremo; c1 solo documenta la linea
}

// ---------------------------------------------------------------------------
// Texto
// ---------------------------------------------------------------------------

void render::dibujarMensaje(const Layout& L, const char* texto, Color color) {
    (void)L;
    dibujarTextoCentrado(texto, 40, 36, color);
}

void render::dibujarAyuda(const Layout& L, const char* texto) {
    dibujarTextoCentrado(texto, L.yTablero + L.lado + 26, 22, COLOR_LINEA);
}

void render::dibujarTextoCentrado(const char* texto, int y, int tam, Color color) {
    int ancho = MeasureText(texto, tam);
    DrawText(texto, (GetScreenWidth() - ancho) / 2, y, tam, color);
}

// ---------------------------------------------------------------------------
// Widgets
// ---------------------------------------------------------------------------

bool render::boton(Rectangle r, const char* texto) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), r);

    DrawRectangleRounded(r, 0.28f, 10, hover ? COLOR_BOTON_HOVER : COLOR_BOTON);
    DrawRectangleRoundedLinesEx(r, 0.28f, 10, 2.0f, hover ? COLOR_TEXTO : COLOR_LINEA);

    int tam = 26;
    int ancho = MeasureText(texto, tam);
    DrawText(texto,
             static_cast<int>(r.x + (r.width - ancho) / 2.0f),
             static_cast<int>(r.y + (r.height - tam) / 2.0f),
             tam, COLOR_TEXTO);

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void render::overlayResultado(const Layout& L, const char* titulo, Color color,
                              const char* subtitulo) {
    DrawRectangle(0, L.yTablero, L.anchoVentana(), L.lado, COLOR_OVERLAY);

    int yCentro = L.yTablero + L.lado / 2;

    Rectangle panel = { L.margen + 80.0f, yCentro - 140.0f,
                        L.lado - 160.0f, 280.0f };
    DrawRectangleRounded(panel, 0.10f, 8, COLOR_FONDO);
    DrawRectangleRoundedLinesEx(panel, 0.10f, 8, 3.0f, color);

    // El titulo se encoge si no cabe en el panel.
    int tam = 76;
    int ancho = MeasureText(titulo, tam);
    int maxAncho = static_cast<int>(panel.width) - 80;
    if (ancho > maxAncho) tam = tam * maxAncho / ancho;

    dibujarTextoCentrado(titulo, static_cast<int>(panel.y + 62), tam, color);
    dibujarTextoCentrado(subtitulo, static_cast<int>(panel.y + 192), 26, COLOR_TEXTO);
}

// ---------------------------------------------------------------------------
// Entrada
// ---------------------------------------------------------------------------

std::pair<int, int> render::celdaDesdeMouse(const Layout& L) {
    Vector2 m = GetMousePosition();

    if (m.x < L.margen || m.y < L.yTablero) return { -1, -1 };
    if (m.x >= L.margen + L.lado || m.y >= L.yTablero + L.lado) return { -1, -1 };

    int col  = static_cast<int>((m.x - L.margen) / L.celda);
    int fila = static_cast<int>((m.y - L.yTablero) / L.celda);
    return { fila, col };
}
