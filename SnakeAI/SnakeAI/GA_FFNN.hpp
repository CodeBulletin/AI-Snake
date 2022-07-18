#pragma once
#include "BaseApp.hpp"
#include "AgentsFFNN.hpp"

class GeneticAlgorithm : BaseApp {
	int agents = 0;
	FFNN_Agent** Snakes = nullptr;
public:
	GeneticAlgorithm(int agents, int cell_size, float frames,
		sf::Color& Body, sf::Color& Food, sf::Color& Eye,
		bool Fullscreen, bool dontScale, int Width = 0, int Height = 0):
		BaseApp(cell_size, frames, Body, Food, Eye, Fullscreen, dontScale, Width, Height) {

	}

	~GeneticAlgorithm() {
		snake = nullptr;
		for (int i = 0; i < agents; i++) {
			delete Snakes[i];
		}
		delete[] Snakes;
		Snakes = nullptr;
	}

	void setup() {
		BaseSetup();
		showScore = false;
		Snakes = new FFNN_Agent *[agents];
		for (int i = 0; i < agents; i++) {
			Snakes[i] = new FFNN_Agent(width, height, cellSize);
		}
	}
};