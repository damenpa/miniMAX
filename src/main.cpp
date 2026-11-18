#include "render.hpp"
#include "minimax.hpp"
#include "tablero.hpp"

namespace {

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

    Tablero tablero;

    while (!render::debeCerrar()) {

        // ---- 1. TURNO DEL JUGADOR ----
        if (!partidaTerminada(tablero) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

            auto [fila, col] = render::celdaDesdeMouse(L);

            // marcarPunto devuelve 0 solo si la celda estaba libre.
            // Si devuelve -1 (ocupada o fuera), no se juega el turno.
            if (fila >= 0 && tablero.marcarPunto(fila, col, 'X') == 0) {

                // ---- 2. TURNO DE LA MAQUINA ----
                // Solo responde si la partida sigue en curso.
                if (!partidaTerminada(tablero)) {
                    auto [x, y] = mejorMovimiento(tablero.mapa);
                    if (x >= 0) tablero.marcarPunto(x, y, 'O');
                }
            }
        }

        // ---- 3. MENSAJE DE ESTADO ----
        int resultado = eval(tablero.mapa);
        bool terminado = (resultado != 0) || tableroLleno(tablero);

        const char* mensaje = "Tu turno (X)";
        Color color = render::COLOR_TEXTO;

        if (resultado < 0) {
            mensaje = "Ganaste!";
            color = render::COLOR_X;
        } else if (resultado > 0) {
            mensaje = "Gana la maquina";
            color = render::COLOR_O;
        } else if (terminado) {
            mensaje = "Empate";
            color = render::COLOR_LINEA;
        }

        // ---- 4. DIBUJO ----
        render::frameInicio();
            render::dibujarTablero(tablero, L);
            if (!terminado) render::dibujarHover(tablero, L);
            render::dibujarMensaje(L, mensaje, color);
            render::dibujarAyuda(L, "Click para jugar   -   Esc para salir");
        render::frameFin();
    }

    render::cerrar();
    return 0;
}
