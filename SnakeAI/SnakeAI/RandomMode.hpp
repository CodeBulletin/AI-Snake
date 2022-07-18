#pragma once
#include "BaseApp.hpp"
#include "file_writer.hpp"

class RandApp : public BaseApp {
public:
	Snake** Snakes = nullptr;
	int snakes, mainframes;

	FileWriter<2> File;

	RandApp(int agents, int cell_size, float frames,
		sf::Color& Body, sf::Color& Food, sf::Color& Eye,
		bool Fullscreen, bool dontScale, int Width = 0, int Height = 0) :
		snakes(agents),
		BaseApp(cell_size, frames, Body, Food, Eye, Fullscreen, dontScale, Width, Height)
	{
		mainframes = frames;
		window.setTitle("Snake Game - Random");
		File.AddFile("Scores", "random", ".txt", "rand");
		File.AddFile("Scores", "random_steps", ".txt", "rand_s");
	}

	~RandApp() {
		snake = nullptr;
		for (int i = 0; i < snakes; i++) {
			delete Snakes[i];
		}
		delete[] Snakes;
		Snakes = nullptr;
	}

	void setup() {
		BaseSetup();
		showScore = false;
		Snakes = new Snake * [snakes];
		for (int i = 0; i < snakes; i++) {
			Snakes[i] = new Snake(width, height, cellSize);
		}
		snake = Snakes[0];
	}

	void loop() override {
		window.clear({ 30, 30, 30, 255 });
		while (time >= 1 / frames) {
			bool written = false;
			for (int i = 0; i < snakes; i++) {
				if (!Snakes[i]->isDead()) {
					int dir = sfh::random::random1d(0, 3);
					Snakes[i]->setDir(dir);
					Snakes[i]->update();
				}
				else {
					if (i == 0 && !written) {
						File.ChangeFile("rand");
						File.WriteLine(std::to_string(Snakes[i]->getScore()));
						File.ChangeFile("rand_s");
						File.WriteLine(std::to_string(Snakes[i]->getSteps()));
						written = true;
					}
					Snakes[i]->reset();
				}
			}
			time -= 1 / frames;
		}
		std::vector<sf::Vertex> drawArray;
		for (int i = 0; i < snakes; i++) {
			Snakes[i]->draw(
				drawArray,
				Body, Food, Eye,
				cx, cy
			);
		}
		window.draw(rect);
		window.draw(drawArray.data(), drawArray.size(), sf::Quads);
		if (showScore && snakes == 1) {
			setScore();
			window.draw(text);
		}
		time += frameTime;
		window.display();
	}

	void KeyPressedEvent() override {
		auto key = event.key.code;
		if (key == sf::Keyboard::Tab) {
			showScore = !showScore;
		}
		else if (key == sf::Keyboard::Num0) {
			frames = mainframes;
		}
		else if (key == sf::Keyboard::Num1) {
			frames = 30;
		}
		else if (key == sf::Keyboard::Num2) {
			frames = 60;
		}
		else if (key == sf::Keyboard::Num3) {
			frames = 144;
		}
		else if (key == sf::Keyboard::Num4) {
			frames = 250;
		}
	}
};