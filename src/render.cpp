#include "render.hpp"

#include <cmath>
#include <cstdio>

// El tablero del juego es 3x3 (Tablero::size).
static constexpr int N = 3;

static constexpr Color COLOR_HOVER = { 255, 255, 255, 28 };

// Fuente unica para toda la interfaz. Se carga una sola vez en iniciar().
static Font fuente;
static bool fuentePropia = false;

static constexpr float ESPACIADO = 0.0f;

// Primer .ttf del sistema que exista. Si no hay ninguno se usa la de raylib.
static void cargarFuente() {
    const char* candidatas[] = {
        "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
    };

    for (const char* ruta : candidatas) {
        if (!FileExists(ruta)) continue;

        fuente = LoadFontEx(ruta, 96, nullptr, 0);
        if (fuente.texture.id != 0 && fuente.glyphCount > 0) {
            SetTextureFilter(fuente.texture, TEXTURE_FILTER_BILINEAR);
            fuentePropia = true;
            return;
        }
    }

    fuente = GetFontDefault(); // plan B: la fuente de pixeles de raylib
}

static Vector2 medir(const char* texto, float tam) {
    return MeasureTextEx(fuente, texto, tam, ESPACIADO);
}

static void escribir(const char* texto, float x, float y, float tam, Color color) {
    DrawTextEx(fuente, texto, { x, y }, tam, ESPACIADO, color);
}

static void dibujarX(float cx, float cy, float r) {
    DrawLineEx({ cx - r, cy - r }, { cx + r, cy + r }, 12.0f, render::COLOR_X);
    DrawLineEx({ cx - r, cy + r }, { cx + r, cy - r }, 12.0f, render::COLOR_X);
}

static void dibujarO(float cx, float cy, float r) {
    DrawRing({ cx, cy }, r - 7.0f, r + 7.0f, 0.0f, 360.0f, 56, render::COLOR_O);
}

// Centro en pixeles de la celda con indice plano 0..8.
static Vector2 centroDeCelda(int pos, const Layout& L) {
    int fila = pos / N;
    int col  = pos % N;
    return { L.margen + col * L.celda + L.celda / 2.0f,
             L.yTablero + fila * L.celda + L.celda / 2.0f };
}

static const char* nombreDificultad(int dificultad) {
    if (dificultad == 1) return "Facil";
    if (dificultad == 2) return "Medio";
    return "Dificil";
}

// ---------------------------------------------------------------------------
// Ventana
// ---------------------------------------------------------------------------

void render::iniciar(const Layout& L, const char* titulo) {
    InitWindow(L.anchoVentana(), L.altoVentana(), titulo);
    SetTargetFPS(60);
    cargarFuente();
}

void render::cerrar() {
    if (fuentePropia) UnloadFont(fuente);
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
// Pantallas
// ---------------------------------------------------------------------------

render::AccionInicio render::pantallaInicio(const Layout& L, int dificultadActual) {
    const float cx = L.anchoVentana() / 2.0f;

    constexpr float anchoDif = 200.0f;
    constexpr float altoDif  = 70.0f;
    constexpr float hueco     = 20.0f;
    const float xDif = cx - (3 * anchoDif + 2 * hueco) / 2.0f;

    constexpr float anchoInicio = 460.0f;
    constexpr float altoInicio  = 76.0f;

    frameInicio();
        dibujarTextoCentrado("GATO", 120, 96, COLOR_TEXTO);

        dibujarTextoCentrado("Dificultad", 332, 24, COLOR_LINEA);
        bool facil   = boton({ xDif, 372, anchoDif, altoDif }, "Facil", dificultadActual == 1);
        bool medio   = boton({ xDif + anchoDif + hueco, 372, anchoDif, altoDif },
                             "Medio", dificultadActual == 2);
        bool dificil = boton({ xDif + 2 * (anchoDif + hueco), 372, anchoDif, altoDif },
                             "Dificil", dificultadActual == 3);

        dibujarTextoCentrado("Quien empieza?", 500, 24, COLOR_LINEA);
        bool humano  = boton({ cx - anchoInicio / 2.0f, 548, anchoInicio, altoInicio },
                             "Empiezo yo  (X)");
        bool maquina = boton({ cx - anchoInicio / 2.0f, 640, anchoInicio, altoInicio },
                             "Empieza la maquina  (O)");

        dibujarTextoCentrado("Esc para salir", L.altoVentana() - 60, 20, COLOR_LINEA);
    frameFin();

    AccionInicio accion;
    if (facil)        accion.dificultad = 1;
    else if (medio)   accion.dificultad = 2;
    else if (dificil) accion.dificultad = 3;

    accion.empezoHumano  = humano;
    accion.empezoMaquina = maquina;
    return accion;
}

void render::pantallaJuego(const Layout& L, const Tablero& t, bool turnoHumano,
                           int dificultad) {
    char ayuda[80];
    std::snprintf(ayuda, sizeof ayuda, "Dificultad: %s   -   Esc para salir",
                  nombreDificultad(dificultad));

    frameInicio();
        dibujarTablero(t, L);
        if (!t.terminado()) dibujarHover(t, L);

        if (turnoHumano) dibujarMensaje(L, "Tu turno (X)", COLOR_X);
        else             dibujarMensaje(L, "Turno de la maquina (O)", COLOR_O);

        dibujarAyuda(L, ayuda);
    frameFin();
}

void render::pantallaFin(const Layout& L, const Tablero& t) {
    // En este juego el humano siempre es 'X' y la maquina 'O'.
    char ganador = t.ganador();

    const char* titulo = "EMPATE";
    Color color = COLOR_LINEA;

    if (ganador == 'X') {
        titulo = "GANASTE";
        color = COLOR_X;
    } else if (ganador == 'O') {
        titulo = "GANO LA MAQUINA";
        color = COLOR_O;
    }

    frameInicio();
        dibujarTablero(t, L);
        overlayResultado(L, titulo, color, "Presiona Enter para volver a jugar");
    frameFin();
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
    dibujarTextoCentrado(texto, 40, 34, color);
}

void render::dibujarAyuda(const Layout& L, const char* texto) {
    dibujarTextoCentrado(texto, L.yTablero + L.lado + 28, 20, COLOR_LINEA);
}

void render::dibujarTextoCentrado(const char* texto, int y, int tam, Color color) {
    float medida = medir(texto, static_cast<float>(tam)).x;
    escribir(texto, (GetScreenWidth() - medida) / 2.0f, static_cast<float>(y),
             static_cast<float>(tam), color);
}

// ---------------------------------------------------------------------------
// Widgets
// ---------------------------------------------------------------------------

bool render::boton(Rectangle r, const char* texto, bool activo) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), r);

    Color relleno = activo ? COLOR_BOTON_ACTIVO
                           : (hover ? COLOR_BOTON_HOVER : COLOR_BOTON);
    Color borde   = activo ? COLOR_BOTON_ACTIVO
                           : (hover ? COLOR_TEXTO : COLOR_LINEA);
    Color tinta   = activo ? COLOR_FONDO : COLOR_TEXTO;

    DrawRectangleRounded(r, 0.28f, 10, relleno);
    DrawRectangleRoundedLinesEx(r, 0.28f, 10, 2.0f, borde);

    float tam = 26.0f;
    Vector2 medida = medir(texto, tam);
    escribir(texto,
             r.x + (r.width - medida.x) / 2.0f,
             r.y + (r.height - medida.y) / 2.0f,
             tam, tinta);

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
    float tam = 80.0f;
    float ancho = medir(titulo, tam).x;
    float maxAncho = panel.width - 80.0f;
    if (ancho > maxAncho) tam = tam * maxAncho / ancho;

    float xTitulo = GetScreenWidth() / 2.0f - medir(titulo, tam).x / 2.0f;
    escribir(titulo, xTitulo, panel.y + 62.0f, tam, color);

    float xSub = GetScreenWidth() / 2.0f - medir(subtitulo, 26.0f).x / 2.0f;
    escribir(subtitulo, xSub, panel.y + 192.0f, 26.0f, COLOR_TEXTO);
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
