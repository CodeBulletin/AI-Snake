#pragma once
#include "BaseApp.hpp"
#include "hamiltonian.hpp"
#include "file_writer.hpp"

class HamiltonianApp : public BaseApp {
public:
	PrimsAlgorithm* PrimsGenrator = nullptr;
	std::vector<Node>* Nodes = {};
	std::unordered_map<int, std::pair<int, int>> Directions = {};
	std::vector<sf::Vertex> lineArray = {};
	bool drawPrimsMaze = false;
	bool drawgarph = false;
	bool takeShortcut = false;
	int scale = 0;
	float mainframes = 0;
	int speed_mult = 1;

	std::list<Cell*> Path = {};

	int dirMap[4][2] = {
		{  0, -1 },
		{  0,  1 },
		{ -1,  0 },
		{  1,  0 },
	};

	bool hasPath = false;

	FileWriter<2> File;

	HamiltonianApp(int cell_size, float frames,
		sf::Color& Body, sf::Color& Food, sf::Color& Eye,
		bool Fullscreen, bool dontScale, bool primsMaze, bool ShortestPath,
		int Width = 0, int Height = 0) :
		BaseApp(cell_size, frames, Body, Food, Eye, Fullscreen, dontScale, Width, Height)
	{
		mainframes = frames;
		drawPrimsMaze = primsMaze;
		takeShortcut = ShortestPath;

		std::string s1 = "Snake Game - ";
		std::string s2 = (primsMaze ? "Prim's Maze" : "Hamiltonian");
		std::string s3;
		if (!primsMaze) {
			if (takeShortcut) {
				s3 = " + Shortest Path";
				File.AddFile("Scores", "hamiltonian+bfs", ".txt", "h_score");
				File.AddFile("Scores", "hamiltonian+bfs_steps", ".txt", "h_steps");
			}
			else {
				File.AddFile("Scores", "hamiltonian", ".txt", "h_score");
				File.AddFile("Scores", "hamiltonian_steps", ".txt", "h_steps");
			}
		}
		window.setTitle((s1 + s2 + s3));
	}

	~HamiltonianApp() {
		delete snake;
		delete PrimsGenrator;
		snake = nullptr;
	}

	void setup() {
		BaseSetup();
		showScore = false;
		snake = new Snake(width, height, cellSize);
		scale = 2 * cellSize;
		PrimsGenrator = new PrimsAlgorithm(width / scale, height / scale);
		Nodes = PrimsGenrator->calculate();
		Directions = generateHamiltonianCycle(Nodes, snake->cellX(), snake->cellY());
		createLines();
	}

	void createLines() {
		lineArray.clear();
		for (int i = 0; i < snake->cellX(); i++) {
			for (int j = 0; j < snake->cellY(); j++) {
				int idx = i * snake->cellY() + j;
				int dir = Directions[idx].first;
				float x1 = (i + 0.5f) * cellSize;
				float y1 = (j + 0.5f) * cellSize;

				int i2 = i + dirMap[dir][0];
				int j2 = j + dirMap[dir][1];
				float x2 = (i2 + 0.5f) * cellSize;
				float y2 = (j2 + 0.5f) * cellSize;

				drawLine(x1, y1, x2, y2, sf::Color::White, lineArray);
			}
		}
	}

	void loop() override {
		if (drawPrimsMaze) {
			window.clear({ 30, 30, 30, 255 });
			drawPrim();
		}
		else {
			if (time >= 1 / frames) {
				for(int x = 0; x < 1 * speed_mult; x++) {
					if (!snake->isDead() && !snake->hasWon()) {
						int dir = chooseDirection();
						snake->setDir(dir);
						snake->update();
					}
					else {
						if (snake->getScore() != 0) {
							File.ChangeFile("h_score");
							File.WriteLine(std::to_string(snake->getScore()));
							File.ChangeFile("h_steps");
							File.WriteLine(std::to_string(snake->getSteps()));
						}
						snake->reset();
						Nodes = PrimsGenrator->calculate();
						Directions = generateHamiltonianCycle(Nodes, snake->cellX(),
							snake->cellY());
						createLines();
						break;
					}
				}
				window.clear({ 30, 30, 30, 30 });
				std::vector<sf::Vertex> drawArray;
				snake->draw(
					drawArray,
					Body, Food, Eye,
					cx, cy
				);
				window.draw(rect);
				window.draw(drawArray.data(), drawArray.size(), sf::Quads);

				if (drawgarph) {
					window.draw(lineArray.data(), lineArray.size(), sf::Lines);
				}

				if (showScore) {
					setScore();
					window.draw(text);
				}
				time = 0;
			}
			time += 1.0f / frameRate;
		}
		window.display();
	}

	void drawPrim() {
		for (int i = 0; i < PrimsGenrator->X; i++) {
			for (int j = 0; j < PrimsGenrator->Y; j++) {
				int idx = i * PrimsGenrator->Y + j;

				if ((*Nodes)[idx].rightwall == true) {
					drawRect(i + .9f, j, scale * .1f, scale, scale,
						sf::Color::Green);
				}
				if ((*Nodes)[idx].leftwall == true) {
					drawRect(i, j, scale * .1f, scale, scale,
						sf::Color::Green);
				}

				if ((*Nodes)[idx].upwall == true) {
					drawRect(i, j, scale, scale * .1f, scale,
						sf::Color::Green);
				}
				if ((*Nodes)[idx].downwall == true) {
					drawRect(i, j + 0.9f, scale, scale * .1f, scale,
						sf::Color::Green);
				}
			}
		}
	}

	int chooseDirection() {
		auto head = snake->head();
		auto tail = snake->tail();
		auto food = snake->foodLocation();
		int y = snake->cellY();
		int idx = head->x * snake->cellY() + head->y;
		int dir = Directions[idx].first;
		int capacity = snake->getMaxSize() - snake->getScore();
		capacity -= 2;
		if (takeShortcut && snake->getScore() <= capacity * 0.5) {
			bool A = BFS(snake->foodLocation());
			if (A) {
				int ndir;
				auto current = Path.begin();
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
				int xd = cx2 - cx1;
				int yd = cy2 - cy1;
				int tail_idx = Directions[tail->x * y + tail->y].second;
				int head_idx = Directions[head->x * y + head->y].second;
				int food_idx = Directions[food->x * y + food->y].second;
				int nxtx = (head->x + xd);
				int nxty = (head->y + yd);
				int next_idx = Directions[nxtx * y + nxty].second;

				if (!(Path.size() == 1) || !(std::fabsf(food_idx - tail_idx) == 1)) {
					int head_idx_r = rDist(tail_idx, head_idx, capacity);
					int next_idx_r = rDist(tail_idx, next_idx, capacity);
					int food_idx_r = rDist(tail_idx, food_idx, capacity);
					if (next_idx_r > head_idx_r && next_idx_r <= food_idx_r) {
						if (xd != 0) {
							ndir = ((1 + xd) / 2 + 2);
						}
						else if (yd != 0) {
							ndir = ((1 + yd) / 2);
						}
						dir = ndir;
					}
				}
			}
		}
		return dir;
	}

	int rDist(int a, int b, int size) {
		if (a > b) b += size;
		return b - a;
	}

	void drawRect(float i, float j, float scaleX, float scaleY, float s, sf::Color col) {
		sf::RectangleShape rs;
		rs.setPosition({
			s * i,
			s * j
		});

		rs.setSize({
			scaleX,
			scaleY
		});

		rs.setFillColor(col);

		window.draw(text);
		window.draw(rs);
	}

	void drawLine(float x1, float y1, float x2, float y2, sf::Color col,
		std::vector<sf::Vertex>& vertArray) {
		sf::Vertex v1;
		sf::Vertex v2;

		v1.color = col;
		v2.color = col;

		v1.position = {
			x1, y1
		};

		v2.position = {
			x2, y2
		};

		vertArray.push_back(v1);
		vertArray.push_back(v2);
	}

	bool reconstructPathBFS(std::unordered_map<Cell*, Cell*>& cameFrom,
		Cell* start, Cell* end)
	{
		for (Cell* i = end; i != nullptr; i = cameFrom[i]) {
			Path.push_front(i);
		}

		if (Path.front() == start) return true;
		return false;
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

	bool BFS(Cell* dest) {
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
		return reconstructPathBFS(cameFrom, snake->head(), dest);
	}

	void KeyPressedEvent() override {
		auto key = event.key.code;
		if (key == sf::Keyboard::Tab) {
			showScore = !showScore;
		}
		else if (key == sf::Keyboard::Tilde) {
			drawgarph = !drawgarph;
		}
		else if (key == sf::Keyboard::Num1) {
			speed_mult = 1;
		}
		else if (key == sf::Keyboard::Num2) {
			speed_mult = 2;
		}
		else if (key == sf::Keyboard::Num3) {
			speed_mult = 4;
		}
		else if (key == sf::Keyboard::Num4) {
			speed_mult = 8;
		}
		else if (key == sf::Keyboard::Num5) {
			speed_mult = 16;
		}
		else if (key == sf::Keyboard::Num6) {
			speed_mult = 32;
		}
		else if (key == sf::Keyboard::Num7) {
			speed_mult = 64;
		}

		else if (key == sf::Keyboard::Equal) {
			frames = 15;
		}
		else if (key == sf::Keyboard::Num8) {
			frames = 30;
		}
		else if (key == sf::Keyboard::Num9) {
			frames = 60;
		}
		else if (key == sf::Keyboard::Num0) {
			frames = 144;
		}
		else if (key == sf::Keyboard::Subtract) {
			frames = mainframes;
		}

		if (snake->hasWon() || drawPrimsMaze || takeShortcut) {
			if (key == sf::Keyboard::Space) {
				Nodes = PrimsGenrator->calculate();
				Directions = generateHamiltonianCycle(Nodes, snake->cellX(),
					snake->cellY());
				createLines();
				frames = 30;
				if (snake->getScore() != 0) {
					File.ChangeFile("h_score");
					File.WriteLine(std::to_string(snake->getScore()));
					File.ChangeFile("h_steps");
					File.WriteLine(std::to_string(snake->getSteps()));
				}
				snake->reset();
				snake->reset();
			}
		}
	}
};