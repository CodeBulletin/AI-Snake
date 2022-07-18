#pragma once
#include <vector>
#include <unordered_set>
#include "sfh_random.hpp"

struct Node {
	int i, j;
	bool upwall = true;
	bool downwall = true;
	bool leftwall = true;
	bool rightwall = true;
};

class PrimsAlgorithm {
public:
	const std::vector<std::pair<int, int>> directions = {
		{ 0 ,-1},
		{ 0 , 1},
		{ 1 , 0},
		{-1 , 0},
	};
	std::vector<Node> Nodes;
	int X, Y, size;

	PrimsAlgorithm() = default;
	PrimsAlgorithm(int X, int Y) : X(X), Y(Y), size(X * Y){
		Nodes = std::vector<Node>(size);
	}

	std::vector<Node>* calculate() {
		using namespace sfh::random;

		for (int i = 0; i < X; i++) {
			for (int j = 0; j < Y; j++) {
				int idx = i * Y + j;
				Nodes[idx].i = i;
				Nodes[idx].j = j;

				Nodes[idx].leftwall = true;
				Nodes[idx].rightwall = true;
				Nodes[idx].upwall = true;
				Nodes[idx].downwall = true;
			}
		}

		int x = random1d<int>(0, X - 1);
		int y = random1d<int>(0, Y - 1);

		int idx = x * Y + y;

		std::vector<std::pair<Node*, std::pair<int, int>>> wallslist;
		std::unordered_map<Node*, bool> partOfMaze;
		partOfMaze[&Nodes[idx]] = true;

		Add(wallslist, &Nodes[idx]);

		while (!wallslist.empty()) {
			int wallidx = random1d<int>(0, wallslist.size() - 1);
			auto wall = wallslist[wallidx];
			auto cell = getConnectedCell(wall);
			if (cell != nullptr) {
				if (partOfMaze.find(cell) == partOfMaze.end() ||
					partOfMaze.find(wall.first) == partOfMaze.end()) {
					partOfMaze[cell] = true;
					partOfMaze[wall.first] = true;
					Add(wallslist, cell);
					Connect(wall.first, cell);
				}
				else if (partOfMaze[wall.first] != partOfMaze[cell]) {
					partOfMaze[cell] = true;
					partOfMaze[wall.first] = true;
					Add(wallslist, cell);
					Connect(wall.first, cell);
				}
			}
			wallslist.erase(wallslist.begin() + wallidx);
		}
		return &Nodes;
	}

	Node* getConnectedCell(std::pair<Node*, std::pair<int, int>>& Point) {
		if (!(Point.first->i + Point.second.first >= 0 &&
			Point.first->i + Point.second.first < X))
			return nullptr;
		if (!(Point.first->j + Point.second.second >= 0 &&
			Point.first->j + Point.second.second < Y))
			return nullptr;
		int idx = (Point.first->i + Point.second.first) * Y +
			(Point.first->j + Point.second.second);
		return &Nodes[idx];
	}

	void Add(std::vector<std::pair<Node*, std::pair<int, int>>>& wallslist, Node* cell) {
		if(cell->upwall) wallslist.push_back({ cell, directions[0] });
		if(cell->downwall) wallslist.push_back({ cell, directions[1] });
		if(cell->rightwall) wallslist.push_back({ cell, directions[2] });
		if(cell->leftwall) wallslist.push_back({ cell, directions[3] });
	}

	void Connect(Node* A, Node* B) {
		if (A->i - B->i == 1) {
			A->leftwall = false;
			B->rightwall = false;
		}
		else if (A->i - B->i == -1) {
			A->rightwall = false;
			B->leftwall = false;
		}
		if (A->j - B->j == 1) {
			A->upwall = false;
			B->downwall = false;
		}
		else if (A->j - B->j == -1) {
			A->downwall = false;
			B->upwall = false;
		}
	}
};