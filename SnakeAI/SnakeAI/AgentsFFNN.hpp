#pragma once
#include "Snake.hpp"

class FFNN_Agent : Snake {
public:
	FFNN_Agent(size_t Width, size_t Height, int CellSize) :
		Snake(Width, Height, CellSize) {
		
	}
};