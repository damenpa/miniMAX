#include "tablero.hpp"
#include <iostream>

int Tablero::marcarPunto(int x, int y, char jugador) {

    if(this->mapa[x][y] != ' ') {
        return -1;
    }

    mapa[x][y] = jugador;
    return 0;
}

void Tablero::dibujar() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            std::cout << mapa[i][j];
            if(j != 2) {
                std::cout << "|";
            }
        }
        std::cout << std::endl;
        if(i != 2) {
            for(int k = 0; k < 3; k++) {
                std::cout << "-";
            }
            std::cout << "---";
        }
        std::cout << std::endl;
    }
}