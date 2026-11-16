#pragma once
#include <vector>

class Tablero {
private:
    static constexpr int size = 3;
    std::vector<std::vector<char>> mapa{size, std::vector<char>(size, ' ')};

public:
    int marcarPunto(int x, int y, char jugador);
    void dibujar();
};