#include "tablero.hpp"
#include <iostream>

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

char Tablero::checarGanador() {

    for(int i = 0; i < 3; i++) {
        if((mapa[i][0] == mapa[i][1] && mapa[i][1] == mapa[i][2])) {
            return mapa[i][0];
        }
        if(mapa[0][i] == mapa[1][i] && mapa[1][i] == mapa[2][i]) {
            return mapa[0][i];
        }
    }

    if(mapa[0][0] == mapa[1][1] == mapa[2][2]) {
        return mapa[0][0]; 
    }
    if(mapa[2][0] == mapa[1][1] == mapa[0][2]) {
        return mapa[2][0];
    }
}