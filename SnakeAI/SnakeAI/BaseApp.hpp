#pragma once

#include "App.hpp"
#include "sfhelper.hpp"
#include "Snake.hpp"

class BaseApp : public App {
public:
	Snake* snake = nullptr;
	int cellSize = 20;

	float time = 0, frames = 20;

	sf::Font font;
	sf::Text text;

	bool showScore = true, dontScale = false;
	sf::Color Body, Food, Eye;

	float cx = 0, cy = 0;

	sf::RectangleShape rect;

	BaseApp(int cell_size, float frames,
		sf::Color& Body, sf::Color& Food, sf::Color& Eye,
		bool Fullscreen, bool dontScale, int Width = 0, int Height = 0) :
		cellSize(cell_size), frames(frames),
		Body(Body), Food(Food), Eye(Eye), dontScale(dontScale)
	{
		name = "Snake Game";
		if (Width == 0) {
			create_window(Fullscreen);
		}
		else {
			create_window(Fullscreen, Width, Height);
		}
		font.loadFromFile("Data/font.ttf");
	}

	void BaseSetup() {
		if (!dontScale) {
			cellSize *= height / 1080.0f;
		}

		cx = (width - std::floorf((float)width / cellSize) * cellSize) / 2.0f;
		cy = (height - std::floorf((float)height / cellSize) * cellSize) / 2.0f;

		rect.setPosition(cx, cy);
		rect.setFillColor(sf::Color::Black);
		rect.setSize({
			width - 2.0f * cx, height - 2.0f * cy
			});

		text.setFont(font);
		text.setCharacterSize(32);
		text.setFillColor(sf::Color::White);
	}

	virtual void setScore() {
		text.setString("Score:" + std::to_string(snake->getScore()));
		sf::FloatRect textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f,
			textRect.top + textRect.height / 2.0f);
		text.setPosition(sf::Vector2f(width / 2.0f, 32));
	}

	virtual void KeyPressedEvent() override {
		auto key = event.key.code;
		if (key == sf::Keyboard::Tab) {
			showScore = !showScore;
		}
	}
};
