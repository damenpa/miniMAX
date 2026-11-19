#include "minimax.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <random>
#include <utility>

// probabilidad de que la maquina haga algo nadaqueveriento
static constexpr int AZAR_FACIL = 55;
static constexpr int AZAR_MEDIO = 18;

// generador de dado aleatorio
static std::mt19937 &dado() {
    static std::mt19937 motor(std::random_device{}());
    return motor;
}

// revisa si aun se pueden hacer jugadas
static bool lleno(const std::array<std::array<char,3>,3> &tablero) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[i][j] == ' ') return false;
        }
    }
    return true;
}

// sistema de puntuacion de lineas basadas en el control de cada jugador
static int puntuarLinea(char c1, char c2, char c3) {
    int fichasIA = (c1 == 'O') + (c2 == 'O') + (c3 == 'O');
    int fichasHumano = (c1 == 'X') + (c2 == 'X') + (c3 == 'X');

    // Linea tiene tache y circulo
    if (fichasIA > 0 && fichasHumano > 0) return 0;

    if (fichasIA == 2) return 3;
    if (fichasIA == 1) return 1;
    if (fichasHumano == 2) return -3;
    if (fichasHumano == 1) return -1;

    return 0;
}

static int evalHeuristica(const std::array<std::array<char,3>,3> &tablero) {
    int puntaje = 0;

    for (int i = 0; i < 3; i++) {
        puntaje += puntuarLinea(tablero[i][0], tablero[i][1], tablero[i][2]); // filas
        puntaje += puntuarLinea(tablero[0][i], tablero[1][i], tablero[2][i]); // columnas
    }
    // diagonales
    puntaje += puntuarLinea(tablero[0][0], tablero[1][1], tablero[2][2]);     
    puntaje += puntuarLinea(tablero[0][2], tablero[1][1], tablero[2][0]);   

    // aporta tantito si se controla el centro
    if (tablero[1][1] == 'O') puntaje += 1;
    else if (tablero[1][1] == 'X') puntaje -= 1;

    return puntaje;
}

int evalGanador(const std::array<std::array<char,3>,3> &tablero) {
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

// funcion recursiva de minimax
int minimax(std::array<std::array<char,3>,3> &tablero, int profundidad, int maxProfundidad, bool esMax) {
    int puntaje = evalGanador(tablero);

    if (puntaje == 100) return puntaje - profundidad;
    if (puntaje == -100) return puntaje + profundidad;
    if (lleno(tablero)) return 0;

    // checar profundidad para los cambios de dificultad
    if (profundidad >= maxProfundidad) {
        return evalHeuristica(tablero);
    }

    // turno de MAX
    if (esMax) {
        int mejor = std::numeric_limits<int>::min();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (tablero[i][j] == ' ') {
                    tablero[i][j] = 'O';
                    mejor = std::max(mejor, minimax(tablero, profundidad + 1, maxProfundidad, false));
                    tablero[i][j] = ' '; // paso de backtrack
                }
            }
        }
        return mejor;
    // turno de MIN
    } else {
        int mejor = std::numeric_limits<int>::max();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (tablero[i][j] == ' ') {
                    tablero[i][j] = 'X';
                    mejor = std::min(mejor, minimax(tablero, profundidad + 1, maxProfundidad, true));
                    tablero[i][j] = ' '; // paso de backtrack
                }
            }
        }
        return mejor;
    }
}

std::pair<int, int> mejorMovimiento(std::array<std::array<char,3>,3> &tablero, int dificultad) {
    int maxProfundidad = 9; // por defecto calculando todos los niveles
    int azar = 0; // por defecto sin numeros random

    if (dificultad == 1) {
        maxProfundidad = 3; 
        azar = AZAR_FACIL;
    } else if (dificultad == 2) {
        maxProfundidad = 5;
        azar = AZAR_MEDIO;
    }

    // genera lista de celdas libres para escoger con el dado
    std::pair<int, int> libres[9];
    int cuantas = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (tablero[i][j] == ' ') libres[cuantas++] = {i, j};
        }
    }
    if (cuantas == 0) return {-1, -1};

    // paso aleatorio si el azar aplica
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
