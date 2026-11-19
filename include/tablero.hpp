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

    // Reglas de la partida.
    bool lleno() const;       // ya no queda ninguna celda libre
    char ganador() const;     // 'X', 'O' o ' ' si nadie gano
    bool terminado() const;   // hay ganador o el tablero esta lleno

};
