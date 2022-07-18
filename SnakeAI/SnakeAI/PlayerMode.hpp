#pragma once
#include "BaseApp.hpp"
#include "file_writer.hpp"

class PlayerApp : public BaseApp {
public:
	FileWriter<2> File;

	PlayerApp(int cell_size, float frames,
		sf::Color& Body, sf::Color& Food, sf::Color& Eye,
		bool Fullscreen, bool dontScale, int Width = 0, int Height = 0) :
		BaseApp(cell_size, frames, Body, Food, Eye, Fullscreen, dontScale, Width, Height)
	{
		File.AddFile("Scores", "Player", ".txt", "MainScore");
		File.AddFile("Scores", "Player_steps", ".txt", "Steps");
	}

	~PlayerApp() {
		delete snake;
		snake = nullptr;
	}

	void setup() {
		BaseSetup();
		snake = new Snake(width, height, cellSize);
	}

	void loop() override {
		if (time >= 1 / frames) {
			window.clear({30, 30, 30, 255});
			std::vector<sf::Vertex> drawArray;
			if (!snake->isDead() && !snake->hasWon()) {
				snake->update();
			}
			else if (!File.haswritten) {
				File.haswritten = true;
				File.ChangeFile("MainScore");
				File.WriteLine(std::to_string(snake->getScore()));
				File.ChangeFile("Steps");
				File.WriteLine(std::to_string(snake->getSteps()));
			}
			time = 0;
			snake->draw(
				drawArray,
				Body, Food, Eye,
				cx, cy
			);
			setScore();
			window.draw(rect);
			window.draw(drawArray.data(), drawArray.size(), sf::Quads);
			if (showScore) {
				window.draw(text);
			}
		}

		time += frameTime;
		window.display();
	}

	void KeyPressedEvent() override {
		auto key = event.key.code;
		if (!snake->isDead()) {
			if (key == sf::Keyboard::Up || key == sf::Keyboard::W) {
				snake->setDir(0);
			}
			else if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
				snake->setDir(1);
			}
			else if (key == sf::Keyboard::Left || key == sf::Keyboard::A) {
				snake->setDir(2);
			}
			else if (key == sf::Keyboard::Right || key == sf::Keyboard::D) {
				snake->setDir(3);
			}
		}
		if (key == sf::Keyboard::Space) {
			snake->reset();
			File.haswritten = false;
		}
		if (key == sf::Keyboard::Tab) {
			showScore = !showScore;
		}
	}
};