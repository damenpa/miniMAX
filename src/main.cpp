#include "render.hpp"
#include "minimax.hpp"
#include "tablero.hpp"

#include <cstdio>

namespace {

enum class Pantalla {
    Inicio,   // elegir dificultad y quien empieza
    Jugando,  // partida en curso
    Fin       // resultado + Enter para reiniciar
};

const char* nombreDificultad(int dificultad) {
    if (dificultad == 1) return "Facil";
    if (dificultad == 2) return "Medio";
    return "Dificil";
}

bool tableroLleno(const Tablero& t) {
    for (int i = 0; i < Tablero::size; i++) {
        for (int j = 0; j < Tablero::size; j++) {
            if (t.en(i, j) == ' ') return false;
        }
    }
    return true;
}

// eval() devuelve +100 si gana O, -100 si gana X y 0 si nadie gano todavia.
bool partidaTerminada(const Tablero& t) {
    return eval(t.mapa) != 0 || tableroLleno(t);
}

} // namespace

int main() {
    Layout L;
    render::iniciar(L, "GATO");

    Pantalla pantalla = Pantalla::Inicio;
    Tablero tablero;
    bool turnoHumano = true;
    int dificultad = 2;         // 1 = Facil, 2 = Medio, 3 = Dificil
    int resultado = 0;          // -100 gana X, +100 gana O, 0 empate

    const float cx = L.anchoVentana() / 2.0f;

    // Fila de tres botones de dificultad.
    const float anchoDif = 200.0f;
    const float altoDif  = 70.0f;
    const float hueco    = 20.0f;
    const float xDif     = cx - (3 * anchoDif + 2 * hueco) / 2.0f;

    const Rectangle botonFacil   = { xDif, 372.0f, anchoDif, altoDif };
    const Rectangle botonMedio   = { xDif + anchoDif + hueco, 372.0f, anchoDif, altoDif };
    const Rectangle botonDificil = { xDif + 2 * (anchoDif + hueco), 372.0f, anchoDif, altoDif };

    // Botones de quien empieza.
    const float anchoInicio = 460.0f;
    const float altoInicio  = 76.0f;
    const Rectangle botonHumano  = { cx - anchoInicio / 2.0f, 548.0f, anchoInicio, altoInicio };
    const Rectangle botonMaquina = { cx - anchoInicio / 2.0f, 640.0f, anchoInicio, altoInicio };

    while (!render::debeCerrar()) {

        // ==================== PANTALLA DE INICIO ====================
        if (pantalla == Pantalla::Inicio) {
            render::frameInicio();
                render::dibujarTextoCentrado("GATO", 120, 96, render::COLOR_TEXTO);
                render::dibujarTextoCentrado("Tres en raya contra minimax", 252, 26,
                                             render::COLOR_LINEA);

                render::dibujarTextoCentrado("Dificultad", 332, 24, render::COLOR_LINEA);
                bool eligeFacil   = render::boton(botonFacil,   "Facil",   dificultad == 1);
                bool eligeMedio   = render::boton(botonMedio,   "Medio",   dificultad == 2);
                bool eligeDificil = render::boton(botonDificil, "Dificil", dificultad == 3);

                render::dibujarTextoCentrado("Quien empieza?", 500, 24, render::COLOR_LINEA);
                bool empiezoYo      = render::boton(botonHumano,  "Empiezo yo  (X)");
                bool empiezaMaquina = render::boton(botonMaquina, "Empieza la maquina  (O)");

                render::dibujarTextoCentrado("Esc para salir", L.altoVentana() - 60, 20,
                                             render::COLOR_LINEA);
            render::frameFin();

            if (eligeFacil)        dificultad = 1;
            else if (eligeMedio)   dificultad = 2;
            else if (eligeDificil) dificultad = 3;

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

            // Turno de la maquina: la dificultad decide la profundidad del minimax.
            if (!turnoHumano && !partidaTerminada(tablero)) {
                auto [x, y] = mejorMovimiento(tablero.mapa, dificultad);
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

            char ayuda[80];
            std::snprintf(ayuda, sizeof ayuda, "Dificultad: %s   -   Esc para salir",
                          nombreDificultad(dificultad));

            render::frameInicio();
                render::dibujarTablero(tablero, L);
                if (!terminado) render::dibujarHover(tablero, L);

                if (turnoHumano) {
                    render::dibujarMensaje(L, "Tu turno (X)", render::COLOR_X);
                } else {
                    render::dibujarMensaje(L, "Turno de la maquina (O)", render::COLOR_O);
                }
                render::dibujarAyuda(L, ayuda);
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
