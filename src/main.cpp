#include "render.hpp"
#include <iostream>

int main() {
    Layout L;
    render::iniciar(L, "miniMAX");

    Tablero tablero;
    bool turnoJugador = true;
    bool juegoTerminado = false;

    tablero.marcarPunto(0,0,'X');
    tablero.marcarPunto(0,1,'X');
    tablero.marcarPunto(0,2,'X');

    std::cout << tablero.checarGanador() << '\n';

    while (!render::debeCerrar()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int pos = render::celdaDesdeMouse(L);
            if (pos >= 0) {

            }
        }

        // ---- 2. DIBUJO (render) ----
        render::frameInicio();
            render::dibujarTablero(tablero, L);
            render::dibujarHover(tablero, L);
            render::dibujarAyuda(L, "Esc para salir");
        render::frameFin();
    }

    render::cerrar();
    return 0;
}