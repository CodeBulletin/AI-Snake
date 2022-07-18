#pragma once
#include "BaseApp.hpp"

#include <unordered_map>
#include <list>
#include <queue>

#include "file_writer.hpp"

float herusitic(Cell* cell, Cell* end) {
	return std::sqrt(std::powf(end->x - cell->x, 2) + std::powf(end->y - cell->y, 2));
}

Cell* FindLowest(std::vector<Cell*>& Set,
	std::unordered_map<Cell*, float>& fScore,
	int& location) {
	Cell* current = Set[0];
	float low = fScore[Set[0]];
	int loc = 0;
	for (int i = 1; i < Set.size(); i++) {
		if (fScore[Set[i]] < low) {
			current = Set[i];
			low = fScore[Set[i]];
			loc = i;
		}
	}
	location = loc;
	return current;
}

class AlgoApp : public BaseApp {
public:
	std::list<Cell*> Path;
	std::list<Cell*>::iterator current;

	int type = 0, mainframes;

	FileWriter<2> File;

	AlgoApp(int type, int cell_size, float frames,
		sf::Color& Body, sf::Color& Food, sf::Color& Eye,
		bool Fullscreen, bool dontScale, int Width = 0, int Height = 0) :
		type(type),
		BaseApp(cell_size, frames, Body, Food, Eye, Fullscreen, dontScale, Width, Height)
	{
		if (type == 0) {
			window.setTitle("Snake Game - A Star");
			File.AddFile("Scores", "astar", ".txt", "score");
			File.AddFile("Scores", "astar_steps", ".txt", "steps");
		}
		else if (type == 1) {
			window.setTitle("Snake Game - BFS");
			File.AddFile("Scores", "bfs", ".txt", "score");
			File.AddFile("Scores", "bfs_steps", ".txt", "steps");
		}
		else {
			window.setTitle("Snake Game - DFS");
			File.AddFile("Scores", "dfs", ".txt", "score");
			File.AddFile("Scores", "dfs_steps", ".txt", "steps");
		}
		mainframes = frames;
	}

	~AlgoApp() {
		delete snake;
		snake = nullptr;
	}

	void setup() {
		BaseSetup();
		showScore = false;
		snake = new Snake(width, height, cellSize);
	}

	void loop() override {
		while (time >= 1 / frames) {
			if (!snake->isDead() && !snake->hasWon()) {
				if (snake->consumed) {
					if (type == 0) {
						if (astar(herusitic, FindLowest)) {
							snake->consumed = false;
						}
					} 
					else if (type == 1) {
						if (BFS()) {
							snake->consumed = false;
						}
					}
					else if (type == 2) {
						if (DFS()) {
							snake->consumed = false;
						}
					}
					current = Path.begin();
				}
				if (current != Path.end() && *current != nullptr && !snake->consumed) {
					int cx1 = (*current)->x;
					int cy1 = (*current)->y;
					current++;
					int cx2 = cx1;
					int cy2 = cy1;
					if (current != Path.end()) {
						cx2 = (*current)->x;
						cy2 = (*current)->y;
					}
					current--;
					int xdiff = cx2 - cx1;
					int ydiff = cy2 - cy1;
					if (xdiff != 0) {
						snake->setDir((1 + xdiff) / 2 + 2);
					}
					else if (ydiff != 0) {
						snake->setDir((1 + ydiff) / 2);
					}
				}
				snake->update();
				if (current != Path.end()) {
					current++;
				}
			}
			else {
				File.ChangeFile("score");
				File.WriteLine(std::to_string(snake->getScore()));
				File.ChangeFile("steps");
				File.WriteLine(std::to_string(snake->getSteps()));
				snake->reset();
				snake->consumed = true;
			}
			time -= 1 / frames;
		}
		window.clear({ 30, 30, 30, 255 });
		std::vector<sf::Vertex> drawArray;
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

		time += frameTime;
		window.display();
	}

	void reconstructPath(std::unordered_map<Cell*, Cell*>& cameFrom, Cell* current) {
		Path.clear();
		Cell* curr = current;
		while (cameFrom.find(curr) != cameFrom.end()) {
			curr = cameFrom[curr];
			Path.push_front(curr);
		}
		Path.push_back(current);
	}

	void getNeighbours(Cell* cell, std::vector<Cell*>& neighbours) {
		Cell* Data = snake->cells();

		if (cell->x > 0) {
			int idx = (cell->x - 1) * snake->cellY() + cell->y;
			if (Data[idx].type != 1) {
				neighbours.push_back(Data + idx);
			}
		}
		if (cell->x < snake->cellX() - 1) {
			int idx = (cell->x + 1) * snake->cellY() + cell->y;
			if (Data[idx].type != 1) {
				neighbours.push_back(Data + idx);
			}
		}
		if (cell->y > 0) {
			int idx = cell->x * snake->cellY() + cell->y - 1;
			if (Data[idx].type != 1) {
				neighbours.push_back(Data + idx);
			}
		}
		if (cell->y < snake->cellY() - 1) {
			int idx = cell->x * snake->cellY() + cell->y + 1;
			if (Data[idx].type != 1) {
				neighbours.push_back(Data + idx);
			}
		}
	}

	bool astar(float (*h)(Cell*, Cell*),
		Cell* (*Find)(std::vector<Cell*>&, std::unordered_map<Cell*, float>&, int&)) {
		std::vector<Cell*> openSet = { snake->head() };
		std::unordered_map<Cell*, Cell*> cameFrom;
		std::unordered_map<Cell*, float> gScore;
		std::unordered_map<Cell*, float> fScore;

		for (size_t i = 0; i < snake->getMaxSize(); i++) {
			gScore[snake->cells() + i] = FLT_MAX;
			fScore[snake->cells() + i] = FLT_MAX;
		}

		Cell* goal = snake->foodLocation();

		gScore[snake->head()] = 0.0f;
		fScore[snake->head()] = h(snake->head(), goal);

		while (!openSet.empty()) {
			int loc;
			Cell* current = Find(openSet, fScore, loc);
			if (current == goal) {
				reconstructPath(cameFrom, current);
				return true;
			}

			openSet.erase(openSet.begin() + loc);
			std::vector<Cell*> neighbours;
			getNeighbours(current, neighbours);
			for (Cell* neighbour : neighbours) {
				float tentative_gScore = gScore[current] + 1;
				if (tentative_gScore < gScore[neighbour]) {
					cameFrom[neighbour] = current;
					gScore[neighbour] = tentative_gScore;
					fScore[neighbour] = tentative_gScore + h(neighbour, goal);

					auto it = std::find(openSet.begin(), openSet.end(), neighbour);
					if (it == openSet.end()) {
						openSet.push_back(neighbour);
					}
				}
			}
		}

		return false;
	}

	bool reconstructPathBFS_DFS(std::unordered_map<Cell*, Cell*>& cameFrom,
		Cell* start, Cell* end)
	{
		for (Cell* i = end; i != nullptr; i = cameFrom[i]) {
			Path.push_front(i);
		}

		if (Path.front() == start) return true;
		return false;
	}

	bool BFS() {
		Path.clear();
		std::queue<Cell*> Queue;
		Queue.push(snake->head());

		Cell* cells = snake->cells();

		std::unordered_map<Cell*, Cell*> cameFrom;
		std::unordered_map<Cell*, bool> visited;
		for (size_t i = 0; i < snake->getMaxSize(); i++) {
			cameFrom[&cells[i]] = nullptr;
			visited[&cells[i]] = false;
		}
		visited[snake->head()] = true;
		while (!Queue.empty()) {
			Cell* cell = Queue.front();
			Queue.pop();

			std::vector<Cell*> neighbours;
			getNeighbours(cell, neighbours);
			for (Cell* neighbour : neighbours) {
				if (!visited[neighbour]) {
					visited[neighbour] = true;
					Queue.push(neighbour);
					cameFrom[neighbour] = cell;
				}
			}
		}
		return reconstructPathBFS_DFS(cameFrom, snake->head(), snake->foodLocation());
	}

	bool DFS() {
		std::list<Cell*> Stack;
		Stack.push_back(snake->head());

		Cell* cells = snake->cells();

		std::unordered_map<Cell*, bool> visited;
		std::unordered_map<Cell*, Cell*> cameFrom;
		for (size_t i = 0; i < snake->getMaxSize(); i++) {
			visited[&cells[i]] = false;
			cameFrom[&cells[i]] = nullptr;
		}
		while (!Stack.empty()) {
			Cell* current = Stack.back();
			visited[current] = true;
			Stack.pop_back();

			std::vector<Cell*> neighbours;
			getNeighbours(current, neighbours);
			for (Cell* neighbour : neighbours) {
				if (!visited[neighbour]) {
					cameFrom[neighbour] = current;
					if (neighbour == snake->foodLocation()) {
						reconstructPathBFS_DFS(cameFrom,
							snake->head(), snake->foodLocation());
						return true;
					}
					Stack.push_back(neighbour);
				}
			}
		}
		return false;
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
		else if (key == sf::Keyboard::Num5) {
			frames = 500;
		}
		else if (key == sf::Keyboard::Num6) {
			frames = 1000;
		}
	}
};