#include "PlayerMode.hpp"
#include "RandomMode.hpp"
#include "AlgorMode.hpp"
#include "HamiltonianMode.hpp"

#include <sstream>
#include <string>
#include <iostream>

#include "file_writer.hpp"

char* mode = 0;
int Agents = 1, CellSize = 40;
bool Fullscreen = false, overideScaling = true, drawPrims = false, shortest_path = false;
sf::Color Body, Food, Eye;
int width = 800, height = 800;
float speed = 15;

void printHelp() {
	std::cout << "-solver : used to select the following solvers: \n";
	std::cout << "	player(default)\n	random\n	astar\n";
	std::cout << "	dfs\n	bfs\n	hamiltonian\n";
	std::cout << "	For Example: SnakeAI -solver=random\n\n";

	std::cout << "-fs : Toggles fullscreen on\n";
	std::cout << "	For Example: SnakeAI -fs\n\n";

	std::cout << "-cs : Change Cell Size (by default 40px) (use px for pixel measurement)\n";
	std::cout << "	if px is not used the size will be scaled by screen height / 1080\n";
	std::cout << "	For Example: SnakeAI -cs=40px | SnakeAI -cs=40\n\n";

	std::cout << "-ss : Change Screen Size (by default 800x800)\n";
	std::cout << "	Format: (Width)x(Height)\n";
	std::cout << "	For Example: SnakeAI -ss=1200x600\n\n";

	std::cout << "-colF : Change food color (by default #FF0000FF)\n";
	std::cout << "	Format: #(hex code for the color with alpha in caps)\n";
	std::cout << "	For Example: SnakeAI -colF=#0000FFFF\n\n";

	std::cout << "-colB : Change body color (by default #00FF00FF)\n";
	std::cout << "	Format: #(hex code for the color with alpha in caps)\n";
	std::cout << "	For Example: SnakeAI -colB=#0000FFFF\n\n";

	std::cout << "-colE : Change eye color (by default #000000FF)\n";
	std::cout << "	Format: #(hex code for the color with alpha in caps)\n";
	std::cout << "	For Example: SnakeAI -colE=#0000FFFF\n\n";

	std::cout << "-agents (only for random solver) : Change number of snake agents (by default 1)\n";
	std::cout << "	For Example: SnakeAI -solver=random -agents=200\n\n";

	std::cout << "-maze (only for hamiltonian solver) : Shows the maze that is used to generate cycle\n";
	std::cout << "	For Example: SnakeAI -solver=hamiltonian -maze\n\n";

	std::cout << "-reduce_path (only for hamiltonian solver) : Reduces the path by using shortest path algorithm(BFS)\n";
	std::cout << "	For Example: SnakeAI -solver=hamiltonian -reduce_path\n\n";

	std::cout << "-help | -h : Prints this page (how are u here anyway?)\n";
	std::cout << "	For Example: SnakeAI -help | SnakeAI -h\n";
}

void decode(int argc, char* argv[]) {
	Body = sf::Color::Green;
	Food = sf::Color::Red;
	Eye = sf::Color::Black;

	FileWriter<14> File;
	File.AddFile("Scores", "Player", ".txt", "p");
	File.AddFile("Scores", "Player_steps", ".txt", "ps");
	File.AddFile("Scores", "astar", ".txt", "a");
	File.AddFile("Scores", "astar_steps", ".txt", "as");
	File.AddFile("Scores", "bfs", ".txt", "b");
	File.AddFile("Scores", "bfs_steps", ".txt", "bs");
	File.AddFile("Scores", "dfs", ".txt", "d");
	File.AddFile("Scores", "dfs_steps", ".txt", "ds");
	File.AddFile("Scores", "random", ".txt", "r");
	File.AddFile("Scores", "random_steps", ".txt", "rs");
	File.AddFile("Scores", "hamiltonian", ".txt", "h");
	File.AddFile("Scores", "hamiltonian_steps", ".txt", "hs");
	File.AddFile("Scores", "hamiltonian+bfs", ".txt", "hb");
	File.AddFile("Scores", "hamiltonian+bfs_steps", ".txt", "hbs");

	for (int i = 1; i < argc; i++) {
		if (strncmp(argv[i], "-solver=", 7) == 0) {
			mode = argv[i] + 8;
		}
		else if (strncmp(argv[i], "-agents=", 7) == 0) {
			Agents = atoi(argv[i] + 8);
		}
		else if (strncmp(argv[i], "-cs=", 3) == 0) {
			char* px = std::strstr(argv[i] + 4, "px");
			if (px != nullptr) {
				*px = '\0';
				*(++px) = '\0';
				overideScaling = true;
			}
			else {
				overideScaling = false;
			}
			CellSize = atoi(argv[i] + 4);
		}
		else if (strncmp(argv[i], "-colB=#", 6) == 0) {
			std::string color(argv[i] + 7);
			uint32_t hex;
			std::stringstream ss;
			ss << std::hex << color;
			ss >> hex;
			Body = sfh::color::hexToRGBA(hex);
		}
		else if (strncmp(argv[i], "-colF=#", 6) == 0) {
			std::string color(argv[i] + 7);
			uint32_t hex;
			std::stringstream ss;
			ss << std::hex << color;
			ss >> hex;
			Food = sfh::color::hexToRGBA(hex);
		}
		else if (strncmp(argv[i], "-colE=#", 6) == 0) {
			std::string color(argv[i] + 7);
			uint32_t hex;
			std::stringstream ss;
			ss << std::hex << color;
			ss >> hex;
			Eye = sfh::color::hexToRGBA(hex);
		}
		else if (strncmp(argv[i], "-sp=", 3) == 0) {
			speed = atof(argv[i] + 4);
		}
		else if (strncmp(argv[i], "-ss=", 3) == 0) {
			char* mid = strchr(argv[i] + 4, 'x');
			*mid = '\0';
			++mid;
			width = atoi(argv[i] + 4);
			height = atoi(mid);
		}
		else if (strcmp(argv[i], "-fs") == 0) {
			Fullscreen = true;
		}
		else if (strcmp(argv[i], "-maze") == 0) {
			drawPrims = true;
		}
		else if (strcmp(argv[i], "-reduce_path") == 0) {
			shortest_path = true;
		}
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "-help") == 0) {
			printHelp();
			std::cin.get();
			exit(0);
		}
		else if (strcmp(argv[i], "-high_score") == 0) {
			exit(0);
		}
		else if (strcmp(argv[i], "-clear_data") == 0) {
			File.ClearAllFiles();
			exit(0);
		}
	}
}




int main(int argc, char* argv[]) {
	decode(argc, argv);
	if (mode == 0 || strcmp(mode, "player") == 0) {
		PlayerApp App(CellSize, speed,
			Body, Food, Eye,
			Fullscreen, overideScaling,
			width, height);
		App.run();
	}
	else if(strcmp(mode, "random") == 0) {
		RandApp App(Agents, CellSize, speed,
			Body, Food, Eye,
			Fullscreen, overideScaling,
			width, height);
		App.run();
	}
	else if (strcmp(mode, "astar") == 0) {
		AlgoApp App(0, CellSize, speed,
			Body, Food, Eye,
			Fullscreen, overideScaling,
			width, height);
		App.run();
	}
	else if (strcmp(mode, "bfs") == 0) {
		AlgoApp App(1, CellSize, speed,
			Body, Food, Eye,
			Fullscreen, overideScaling,
			width, height);
		App.run();
	}
	else if (strcmp(mode, "dfs") == 0) {
		AlgoApp App(2, CellSize, speed,
			Body, Food, Eye,
			Fullscreen, overideScaling,
			width, height);
		App.run();
	}
	else if (strcmp(mode, "hamiltonian") == 0) {
		HamiltonianApp App(CellSize, speed,
			Body, Food, Eye,
			Fullscreen, overideScaling, drawPrims, shortest_path,
			width, height);
		App.run();
	}
	return 0;
}