#pragma once
#include "Cell.hpp"
#include "sfh_random.hpp"
#include <SFML/Graphics.hpp>
#include <list>
#include <vector>

#include "sfh_shape.hpp"

class Snake {
public:
	bool consumed = true;

	Snake(size_t Width, size_t Height, int CellSize) :
		m_cellx(Width / CellSize), m_celly(Height / CellSize), m_cellsize(CellSize)
	{
		m_cells = new Cell[m_cellx * m_celly];
		for (size_t x = 0; x < m_cellx; x++) {
			for (size_t y = 0; y < m_celly; y++) {
				size_t idx = x * m_celly + y;
				*(m_cells + idx) = { x, y, 0 };
			}
		}
		Cell* x = m_cells + ((m_cellx / 2) * m_celly + (m_celly / 2));
		m_snakebody.push_back(x);
		m_snakebody.push_back(x + m_celly);

		m_snakebody.front()->type = 1;
		m_snakebody.back()->type = 1;

		resetFood();
		m_dir = 2;
		m_decodeDir();
	}

	~Snake() {
		if (m_cells != nullptr) {
			delete[] m_cells;
			m_cells = nullptr;
		}
	}

	void reset() {
		m_won = false;
		for (auto c : m_snakebody) {
			c->type = 0;
		}
		m_snakebody.clear();
		m_cells[food.x * m_celly + food.y].type = 0;

		Cell* x = m_cells + ((m_cellx / 2) * m_celly + (m_celly / 2));
		m_snakebody.push_back(x);
		m_snakebody.push_back(x + m_celly);

		m_snakebody.front()->type = 1;
		m_snakebody.back()->type = 1;

		resetFood();
		m_dir = 2;
		m_decodeDir();

		m_dead = false;
		m_score = 0;
		m_steps = 0;
	}

	bool setDir(int dir) {
		if (m_dir == 0 && dir == 1 || m_dir == 1 && dir == 0 ||
			m_dir == 2 && dir == 3 || m_dir == 3 && dir == 2) return false;
		m_dir = dir;
		m_decodeDir();
		return true;
	}

	int& getScore() {
		return m_score;
	}

	int& getSteps() {
		return m_steps;
	}

	bool& isDead() {
		return m_dead;
	}

	bool& hasWon() {
		return m_won;
	}

	Cell* foodLocation() {
		return &m_cells[food.x * m_celly + food.y];
	}

	Cell* head() {
		return m_snakebody.front();
	}

	Cell* tail() {
		return m_snakebody.back();
	}

	Cell* cells() {
		return m_cells;
	}

	size_t getMaxSize() {
		return m_cellx * m_celly;
	}

	size_t cellX() { return m_cellx; }
	size_t cellY() { return m_celly; }

	void resetFood() {
		if ((m_cellx * m_celly != (size_t)m_score + 2) && !m_won) {
			while (true) {
				size_t idx = sfh::random::random1d<size_t>(0, m_cellx - 1);
				size_t idy = sfh::random::random1d<size_t>(0, m_celly - 1);
				size_t id = idx * m_celly + idy;

				if (m_cells[id].type == 0) {
					m_cells[id].type = 2;
					food = { idx, idy };
					m_foodOver = false;
					break;
				}
			}
		}
		else {
			m_won = true;
		}
	}

	void update() {
		Cell* Head = m_snakebody.front();
		size_t xf = Head->x + m_xinc;
		size_t yf = Head->y + m_yinc;
		if (xf + 1 == 0 || xf >= m_cellx || yf + 1 == 0 || yf >= m_celly) {
			m_dead = true;
			return;
		}
		size_t id = xf * m_celly + yf;
		Cell* Next = (m_cells + id);
		if (Next->type == 2) {
			m_foodOver = true;
			m_score++;
			consumed = true;
		}
		else {
			m_snakebody.back()->type = 0;
			m_snakebody.pop_back();
		}
		if (Next->type == 1) {
			m_dead = true;
			return;
		}
		m_steps++;
		Next->type = 1;
		m_snakebody.push_front(Next);
		if (m_foodOver) {
			resetFood();
		}
	}

	void draw(std::vector<sf::Vertex>& drawArray, const sf::Color& bodyCol,
		const sf::Color& foodCol, const sf::Color& eyeCol,
		float shiftX, float shiftY, float cellp = 0.4f) {
		std::vector<sf::Vertex> vertArray(m_snakebody.size() * 4 + 20);
		size_t i = 0;
		float x, y, nx, ny, diffx, diffy;
		float scale = 0.5f - cellp / 2.0f + cellp / 5.0f;
		for (auto Cell = m_snakebody.begin(); Cell != m_snakebody.end(); Cell++) {
			if (*Cell != m_snakebody.back()) {
				x = (*Cell)->x * m_cellsize + m_cellsize / 2.0f;
				y = (*Cell)->y * m_cellsize + m_cellsize / 2.0f;
				++Cell;
				nx = (*Cell)->x * m_cellsize + m_cellsize / 2.0f;
				ny = (*Cell)->y * m_cellsize + m_cellsize / 2.0f;

				diffx = x - nx;
				diffy = y - ny;
				auto vec = sfh::shape::thickLine(
					{x + diffx * scale + shiftX, y + diffy * scale + shiftY},
					{nx - diffx * scale + shiftX, ny - diffy * scale + shiftY},
					0.8f * m_cellsize, bodyCol
				);
				for (int j = 0; j < 4; j++) {
					vertArray[i + j] = vec[j];
				}
				--Cell;
			}
			i+=4;
		}
		if (!m_won) {
			x = food.x;
			y = food.y;
			m_drawSq(
				x * m_cellsize + 0.1f * m_cellsize + shiftX,
				y * m_cellsize + 0.1f * m_cellsize + shiftY,
				m_cellsize - 0.2f * m_cellsize,
				i, vertArray,
				foodCol
			);
		}
		i += 4;

		x = m_snakebody.front()->x;
		y = m_snakebody.front()->y;

		float x_c = (m_dir > 1) ? (0.6f * m_cellsize) * - (m_dir - 3) : 0;
		float y_c = m_dir == 1 ? -0.6f * m_cellsize : 0;

		m_drawSq(
			x * m_cellsize + x_c - y_c + shiftX,
			y * m_cellsize + 0.6f * m_cellsize + y_c + shiftY,
			m_cellsize - 0.6f * m_cellsize,
			i, vertArray,
			bodyCol
		);
		i += 4;

		m_drawSq(
			x * m_cellsize + 0.1f * m_cellsize + x_c - y_c + shiftX,
			y * m_cellsize + 0.7f * m_cellsize + y_c + shiftY,
			m_cellsize - 0.8f * m_cellsize,
			i, vertArray,
			eyeCol
		);
		i += 4;

		x_c = (m_dir > 1) ? (0.6f * m_cellsize) * - (m_dir - 3) : 0;
		y_c = m_dir == 0 ? 0.6f * m_cellsize : 0;
		m_drawSq(
			x * m_cellsize + x_c + y_c + shiftX,
			y * m_cellsize + y_c + shiftY,
			m_cellsize - 0.6f * m_cellsize,
			i, vertArray,
			bodyCol
		);
		i += 4;

		m_drawSq(
			x * m_cellsize + 0.1f * m_cellsize + x_c + y_c + shiftX,
			y * m_cellsize + 0.1f * m_cellsize + y_c + shiftY,
			m_cellsize - 0.8f * m_cellsize,
			i, vertArray,
			eyeCol
		);
		i += 4;

		drawArray.insert(drawArray.end(), vertArray.begin(), vertArray.end());
	}
protected:
	size_t m_cellx = 0, m_celly = 0;
	int m_cellsize = 20;

	Cell* m_cells = nullptr;
	std::list<Cell*> m_snakebody;

	int m_score = 0, m_steps = 0, m_dir = 0, m_xinc = 0, m_yinc = 0;
	sf::Vector2<size_t> food{};
	bool m_dead = false, m_foodOver = false, m_won;

	void m_decodeDir() {
		if (m_dir <= 1) {
			m_yinc = m_dir * 2 - 1;
			m_xinc = 0;
		}
		else {
			m_xinc = m_dir * 2 - 5;
			m_yinc = 0;
		}
	}

	void m_drawSq(float x0, float y0, float size, size_t idx,
		std::vector<sf::Vertex>& vertArray, const sf::Color& col) {
		vertArray[idx + 0].position = {
			x0, y0
		};
		vertArray[idx + 1].position = {
			x0, y0 + size
		};
		vertArray[idx + 2].position = {
			x0 + size, y0 + size
		};
		vertArray[idx + 3].position = {
			x0 + size, y0
		};

		vertArray[idx + 0].color = col;
		vertArray[idx + 1].color = col;
		vertArray[idx + 2].color = col;
		vertArray[idx + 3].color = col;
	}
};