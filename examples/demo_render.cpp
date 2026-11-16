// Ejemplo de uso de la capa de render.
//
// No juega: solo dibuja un tablero fijo para que puedas ver los colores,
// el hover y el texto. Tu main.cpp real es el que decide las jugadas.
//
//   make demo && ./build/miniMAX-demo
//
#include "render.hpp"
#include "tablero.hpp"

int main() {
    Layout L;
    render::iniciar(L, "miniMAX - demo de render");

    Tablero tablero;
    tablero.marcarPunto(1, 1, 'X');
    tablero.marcarPunto(0, 0, 'O');
    tablero.marcarPunto(2, 2, 'X');

    while (!render::debeCerrar()) {
        render::frameInicio();
            render::dibujarTablero(tablero, L);
            render::dibujarHover(tablero, L);
        render::frameFin();
    }

    render::cerrar();
    return 0;
}
