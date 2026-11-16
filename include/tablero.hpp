#pragma once
#include <vector>

class Tablero {
private:
    static constexpr int size = 3;
    std::vector<std::vector<char>> mapa{size, std::vector<char>(size, ' ')};

public:
    int marcarPunto(int x, int y, char jugador);

    // Getter de solo lectura. Lo usa la capa de render para saber que
    // dibujar en cada celda. No contiene logica de juego.
    char en(int x, int y) const;

    char checarGanador();

};