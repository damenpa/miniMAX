#include "render.hpp"
#include "minimax.hpp"
#include "tablero.hpp"

namespace {

enum class Pantalla {
    Inicio,   // elegir quien empieza
    Jugando,  // partida en curso
    Fin       // resultado + Enter para reiniciar
};

bool tableroLleno(const Tablero& t) {
    for (int i = 0; i < Tablero::size; i++) {
        for (int j = 0; j < Tablero::size; j++) {
            if (t.en(i, j) == ' ') return false;
        }
    }
    return true;
}

// eval() devuelve +10 si gana O, -10 si gana X y 0 si nadie gano todavia.
bool partidaTerminada(const Tablero& t) {
    return eval(t.mapa) != 0 || tableroLleno(t);
}

} // namespace

int main() {
    Layout L;
    render::iniciar(L, "miniMAX");

    Pantalla pantalla = Pantalla::Inicio;
    Tablero tablero;
    bool turnoHumano = true;
    int resultado = 0;          // -10 gana X, +10 gana O, 0 empate

    const float cx = L.anchoVentana() / 2.0f;
    const Rectangle botonHumano  = { cx - 230.0f, 400.0f, 460.0f, 78.0f };
    const Rectangle botonMaquina = { cx - 230.0f, 505.0f, 460.0f, 78.0f };

    while (!render::debeCerrar()) {

        // ==================== PANTALLA DE INICIO ====================
        if (pantalla == Pantalla::Inicio) {
            render::frameInicio();
                render::dibujarTextoCentrado("miniMAX", 170, 80, render::COLOR_TEXTO);
                render::dibujarTextoCentrado("Tres en raya contra minimax", 275, 28,
                                             render::COLOR_LINEA);
                render::dibujarTextoCentrado("Quien empieza?", 335, 26,
                                             render::COLOR_TEXTO);

                bool empiezoYo      = render::boton(botonHumano,  "Empiezo yo  (X)");
                bool empiezaMaquina = render::boton(botonMaquina, "Empieza la maquina  (O)");

                render::dibujarTextoCentrado("Esc para salir", L.altoVentana() - 70, 20,
                                             render::COLOR_LINEA);
            render::frameFin();

            if (empiezoYo) {
                tablero = Tablero();
                turnoHumano = true;
                resultado = 0;
                pantalla = Pantalla::Jugando;
            } else if (empiezaMaquina) {
                tablero = Tablero();
                turnoHumano = false;
                resultado = 0;
                pantalla = Pantalla::Jugando;
            }

            continue;
        }

        // ==================== PARTIDA ====================
        if (pantalla == Pantalla::Jugando) {

            // Turno del humano: solo avanza si la celda estaba libre.
            if (turnoHumano && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                auto [fila, col] = render::celdaDesdeMouse(L);
                if (fila >= 0 && tablero.marcarPunto(fila, col, 'X') == 0) {
                    turnoHumano = false;
                }
            }

            // Turno de la maquina.
            if (!turnoHumano && !partidaTerminada(tablero)) {
                auto [x, y] = mejorMovimiento(tablero.mapa);
                if (x >= 0) {
                    tablero.marcarPunto(x, y, 'O');
                    turnoHumano = true;
                }
            }

            bool terminado = partidaTerminada(tablero);
            if (terminado) {
                resultado = eval(tablero.mapa);
                pantalla = Pantalla::Fin;
            }

            render::frameInicio();
                render::dibujarTablero(tablero, L);
                if (!terminado) render::dibujarHover(tablero, L);

                if (turnoHumano) {
                    render::dibujarMensaje(L, "Tu turno (X)", render::COLOR_X);
                } else {
                    render::dibujarMensaje(L, "Turno de la maquina (O)", render::COLOR_O);
                }
                render::dibujarAyuda(L, "Click para jugar   -   Esc para salir");
            render::frameFin();

            continue;
        }

        // ==================== FIN DE PARTIDA ====================
        const char* titulo = "EMPATE";
        Color color = render::COLOR_LINEA;

        if (resultado < 0) {
            titulo = "GANASTE";
            color = render::COLOR_X;
        } else if (resultado > 0) {
            titulo = "GANO LA MAQUINA";
            color = render::COLOR_O;
        }

        if (IsKeyPressed(KEY_ENTER)) {
            tablero = Tablero();
            turnoHumano = true;
            resultado = 0;
            pantalla = Pantalla::Inicio;
            continue;
        }

        render::frameInicio();
            render::dibujarTablero(tablero, L);
            render::overlayResultado(L, titulo, color,
                                     "Presiona Enter para volver a jugar");
        render::frameFin();
    }

    render::cerrar();
    return 0;
}
