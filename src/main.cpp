#include "render.hpp"
#include "minimax.hpp"
#include "tablero.hpp"

enum class Pantalla {
    Inicio, 
    Jugando, 
    Fin      
};

int main() {
    Layout L;
    render::iniciar(L, "GATO");

    Pantalla pantalla = Pantalla::Inicio;
    Tablero tablero;
    bool turnoHumano = true;
    int dificultad = 2;

    while (!render::debeCerrar()) {

        // ==================== INICIO ====================
        if (pantalla == Pantalla::Inicio) {
            render::AccionInicio accion = render::pantallaInicio(L, dificultad);

            if (accion.dificultad != 0) dificultad = accion.dificultad;

            if (accion.empezoHumano || accion.empezoMaquina) {
                tablero = Tablero();
                turnoHumano = accion.empezoHumano;
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

            // Turno de la maquina: la dificultad decide lo fuerte que juega.
            if (!turnoHumano && !tablero.terminado()) {
                auto [fila, col] = mejorMovimiento(tablero.mapa, dificultad);
                if (fila >= 0) {
                    tablero.marcarPunto(fila, col, 'O');
                    turnoHumano = true;
                }
            }

            if (tablero.terminado()) pantalla = Pantalla::Fin;

            render::pantallaJuego(L, tablero, turnoHumano, dificultad);
            continue;
        }

        // ==================== FIN ====================
        render::pantallaFin(L, tablero);

        if (IsKeyPressed(KEY_ENTER)) {
            tablero = Tablero();
            turnoHumano = true;
            pantalla = Pantalla::Inicio;
        }
    }

    render::cerrar();
    return 0;
}
