#pragma once
#include "PrimsAlgorithm.hpp"

std::unordered_map<int, std::pair<int, int>> generateHamiltonianCycle(
	std::vector<Node>* Nodes, int X, int Y) {
	std::unordered_map<int, std::pair<int, int>> Directions;
	int i = 0, j = 0;
	int n = 0;
	do {
		int iL = i / 2;
		int jL = j / 2;

		int idx = i * Y + j;
		int idxL = iL * (Y / 2) + jL;

		Node& Cell = (*Nodes)[idxL];
		if (i % 2 == 0 && j % 2 == 0) {
			if (Cell.upwall == true) {
				Directions[idx] = { 3, n }; // Move Right
				i++;
			}
			else {
				Directions[idx] = { 0, n }; // Move UP
				j--;
			}
		}
		else if (i % 2 == 1 && j % 2 == 0) {
			if (Cell.rightwall == true) {
				Directions[idx] = { 1, n }; // Move Down
				j++;
			}
			else {
				Directions[idx] = { 3, n }; // Move Right
				i++;
			}
		}
		else if (i % 2 == 1 && j % 2 == 1) {
			if (Cell.downwall == true) {
				Directions[idx] = { 2, n }; // Move Left
				i--;
			}
			else {
				Directions[idx] = { 1, n }; // Move Down
				j++;
			}
		}
		else if (i % 2 == 0 && j % 2 == 1) {
			if (Cell.leftwall == true) {
				Directions[idx] = { 0, n }; // Move Up
				j--;
			}
			else {
				Directions[idx] = { 2, n }; // Move Left
				i--;
			}
		}
		n++;
	} while (i != 0 || j != 0);
	return Directions;
}