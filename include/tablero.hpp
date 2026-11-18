#pragma once
#include <array>

class Tablero {
    

public:
    static constexpr int size = 3;
    std::array<std::array<char, size>, size> mapa{};
    Tablero();

    int marcarPunto(int x, int y, char jugador);

    // Getter de solo lectura. Lo usa la capa de render para saber que
    // dibujar en cada celda. No contiene logica de juego.
    char en(int x, int y) const;

};
