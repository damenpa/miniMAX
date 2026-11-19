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

bool Tablero::lleno() const {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (mapa[i][j] == ' ') return false;
        }
    }
    return true;
}

char Tablero::ganador() const {
    for (int i = 0; i < size; i++) {
        // filas
        if (mapa[i][0] != ' ' && mapa[i][0] == mapa[i][1] && mapa[i][1] == mapa[i][2]) {
            return mapa[i][0];
        }
        // columnas
        if (mapa[0][i] != ' ' && mapa[0][i] == mapa[1][i] && mapa[1][i] == mapa[2][i]) {
            return mapa[0][i];
        }
    }
    // diagonales
    if (mapa[0][0] != ' ' && mapa[0][0] == mapa[1][1] && mapa[1][1] == mapa[2][2]) {
        return mapa[0][0];
    }
    if (mapa[0][2] != ' ' && mapa[0][2] == mapa[1][1] && mapa[1][1] == mapa[2][0]) {
        return mapa[0][2];
    }
    return ' ';
}

bool Tablero::terminado() const {
    return ganador() != ' ' || lleno();
}
