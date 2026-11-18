#include "minimax.hpp"
#include <iostream>
#include <array>
#include <algorithm>
#include <limits>
#include <utility>

namespace {

bool lleno(const std::array<std::array<char,3>,3> &tablero) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[i][j] == ' ') return false;
        }
    }
    return true;
}

} 

int eval(const std::array<std::array<char,3>,3> &tablero) {
    for (int i = 0; i < 3; i++) {
        // filas
        if (tablero[i][0] != ' ' && tablero[i][0] == tablero[i][1] && tablero[i][1] == tablero[i][2]) {
            return (tablero[i][0] == 'O') ? 10 : -10;
        }
        // columnas
        if (tablero[0][i] != ' ' && tablero[0][i] == tablero[1][i] && tablero[1][i] == tablero[2][i]) {
            return (tablero[0][i] == 'O') ? 10 : -10;
        }
    }
    if (tablero[0][0] != ' ' && tablero[0][0] == tablero[1][1] && tablero[1][1] == tablero[2][2]) {
        return (tablero[0][0] == 'O') ? 10 : -10;
    }
    if (tablero[0][2] != ' ' && tablero[0][2] == tablero[1][1] && tablero[1][1] == tablero[2][0]) {
        return (tablero[0][2] == 'O') ? 10 : -10;
    }
    return 0;
}

int minimax(std::array<std::array<char,3>,3> &tablero, int profundidad, bool esMax) {
    int puntaje = eval(tablero);

    if (puntaje == 10) return puntaje - profundidad;
    if (puntaje == -10) return puntaje + profundidad;
    if (lleno(tablero)) return 0;

    if (esMax) {
        int mejor = std::numeric_limits<int>::min();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (tablero[i][j] == ' ') {
                    tablero[i][j] = 'O';
                    mejor = std::max(mejor, minimax(tablero, profundidad + 1, false));
                    tablero[i][j] = ' '; // Backtracking
                }
            }
        }
        return mejor;
    } else {
        int mejor = std::numeric_limits<int>::max();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (tablero[i][j] == ' ') {
                    tablero[i][j] = 'X';
                    mejor = std::min(mejor, minimax(tablero, profundidad + 1, true));
                    tablero[i][j] = ' '; // Backtracking
                }
            }
        }
        return mejor;
    }
}

std::pair<int, int> mejorMovimiento(std::array<std::array<char,3>,3> &tablero) {
    int mejorValor = std::numeric_limits<int>::min();
    std::pair<int, int> mejorMovimiento = {-1, -1};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[i][j] == ' ') {
                tablero[i][j] = 'O';
                int valorMovimiento = minimax(tablero, 0, false);
                tablero[i][j] = ' ';

                if (valorMovimiento > mejorValor) {
                    mejorMovimiento = {i, j};
                    mejorValor = valorMovimiento;
                }
            }
        }
    }
    return mejorMovimiento;
}
