#include "minimax.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <random>
#include <utility>

namespace {

// Probabilidad (en %) de jugar una celda libre al azar en vez de la mejor jugada.
// Subir estos numeros hace al nivel mas flojo; 0 = nunca falla.
constexpr int AZAR_FACIL = 55;
constexpr int AZAR_MEDIO = 20;

std::mt19937 &dado() {
    static std::mt19937 motor(std::random_device{}());
    return motor;
}

bool lleno(const std::array<std::array<char,3>,3> &tablero) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[i][j] == ' ') return false;
        }
    }
    return true;
}

int puntuarLinea(char c1, char c2, char c3) {
    int fichasIA = (c1 == 'O') + (c2 == 'O') + (c3 == 'O');
    int fichasHumano = (c1 == 'X') + (c2 == 'X') + (c3 == 'X');

    // Linea bloqueada por ambos jugadores
    if (fichasIA > 0 && fichasHumano > 0) return 0;

    if (fichasIA == 2) return 3;
    if (fichasIA == 1) return 1;
    if (fichasHumano == 2) return -3;
    if (fichasHumano == 1) return -1;

    return 0;
}

int evalHeuristica(const std::array<std::array<char,3>,3> &tablero) {
    int puntaje = 0;

    for (int i = 0; i < 3; i++) {
        puntaje += puntuarLinea(tablero[i][0], tablero[i][1], tablero[i][2]); // Filas
        puntaje += puntuarLinea(tablero[0][i], tablero[1][i], tablero[2][i]); // Columnas
    }
    puntaje += puntuarLinea(tablero[0][0], tablero[1][1], tablero[2][2]);     // Diagonal principal
    puntaje += puntuarLinea(tablero[0][2], tablero[1][1], tablero[2][0]);     // Diagonal secundaria

    // Bono leve por controlar el centro
    if (tablero[1][1] == 'O') puntaje += 1;
    else if (tablero[1][1] == 'X') puntaje -= 1;

    return puntaje;
}

} // namespace

int eval(const std::array<std::array<char,3>,3> &tablero) {
    for (int i = 0; i < 3; i++) {
        // filas
        if (tablero[i][0] != ' ' && tablero[i][0] == tablero[i][1] && tablero[i][1] == tablero[i][2]) {
            return (tablero[i][0] == 'O') ? 100 : -100;
        }
        // columnas
        if (tablero[0][i] != ' ' && tablero[0][i] == tablero[1][i] && tablero[1][i] == tablero[2][i]) {
            return (tablero[0][i] == 'O') ? 100 : -100;
        }
    }
    if (tablero[0][0] != ' ' && tablero[0][0] == tablero[1][1] && tablero[1][1] == tablero[2][2]) {
        return (tablero[0][0] == 'O') ? 100 : -100;
    }
    if (tablero[0][2] != ' ' && tablero[0][2] == tablero[1][1] && tablero[1][1] == tablero[2][0]) {
        return (tablero[0][2] == 'O') ? 100 : -100;
    }
    return 0;
}

int minimax(std::array<std::array<char,3>,3> &tablero, int profundidad, int maxProfundidad, bool esMax) {
    int puntaje = eval(tablero);

    if (puntaje == 100) return puntaje - profundidad;
    if (puntaje == -100) return puntaje + profundidad;
    if (lleno(tablero)) return 0;

    // Corte por limite de profundidad
    if (profundidad >= maxProfundidad) {
        return evalHeuristica(tablero);
    }

    if (esMax) {
        int mejor = std::numeric_limits<int>::min();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (tablero[i][j] == ' ') {
                    tablero[i][j] = 'O';
                    mejor = std::max(mejor, minimax(tablero, profundidad + 1, maxProfundidad, false));
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
                    mejor = std::min(mejor, minimax(tablero, profundidad + 1, maxProfundidad, true));
                    tablero[i][j] = ' '; // Backtracking
                }
            }
        }
        return mejor;
    }
}

std::pair<int, int> mejorMovimiento(std::array<std::array<char,3>,3> &tablero, int dificultad) {
    int maxProfundidad = 9; // 3 = Dificil
    int azar = 0;           // % de jugar al azar

    if (dificultad == 1) {
        maxProfundidad = 1; // 1 = Facil
        azar = AZAR_FACIL;
    } else if (dificultad == 2) {
        maxProfundidad = 2; // 2 = Medio
        azar = AZAR_MEDIO;
    }

    // Celdas libres del tablero.
    std::pair<int, int> libres[9];
    int cuantas = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[i][j] == ' ') libres[cuantas++] = {i, j};
        }
    }
    if (cuantas == 0) return {-1, -1};

    // Jugada al azar: asi los niveles faciles dejan pasar bloqueos y remates.
    if (azar > 0 && static_cast<int>(dado()() % 100) < azar) {
        int indice = static_cast<int>(dado()() % cuantas);
        return libres[indice];
    }

    int mejorValor = std::numeric_limits<int>::min();
    std::pair<int, int> mejorJugada = {-1, -1};

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[i][j] == ' ') {
                tablero[i][j] = 'O';
                int valorMovimiento = minimax(tablero, 1, maxProfundidad, false);
                tablero[i][j] = ' ';

                if (valorMovimiento > mejorValor) {
                    mejorJugada = {i, j};
                    mejorValor = valorMovimiento;
                }
            }
        }
    }
    return mejorJugada;
}
