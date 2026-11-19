#pragma once
#include <array>
#include <utility>

int eval(const std::array<std::array<char,3>,3> &tablero);
int minimax(std::array<std::array<char,3>,3> &tablero, int profundidad, int maxProfundidad, bool esMax);

// dificultad: 1 = Facil, 2 = Medio, 3 = Dificil
std::pair<int,int> mejorMovimiento(std::array<std::array<char,3>,3> &tablero, int dificultad = 3);
