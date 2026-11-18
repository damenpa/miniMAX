#pragma once
#include <array>
#include <utility>

int eval(const std::array<std::array<char,3>,3> &tablero);
int minimax(std::array<std::array<char,3>,3> &tablero, int profundidad, bool esMax);
std::pair<int,int> mejorMovimiento(std::array<std::array<char,3>,3> &tablero);
