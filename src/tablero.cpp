#include "tablero.hpp"
#include <iostream>

Tablero::Tablero() {
    for(auto& fila : mapa) {
        fila.fill(' ');
    }
}

int Tablero::marcarPunto(int x, int y, char jugador) {
    if(this->mapa[x][y] != ' ') {
        return -1;
    }
    mapa[x][y] = jugador;
    return 0;
}

char Tablero::en(int x, int y) const {
    return mapa[x][y];
}

