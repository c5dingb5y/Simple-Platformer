#include <iostream>
#include <fstream>
#include <vector>
#include "SDL.h"
#include <string>

constexpr int characterWidth = 66;
constexpr int characterHeight = 117;

constexpr int fpsLimit = 144;

using namespace std;
 
SDL_Window* window;
SDL_Renderer* renderer;

float characterX = 0;
float characterY = 0;

float gravity = 0.0f;
bool floating = false;

int deltaTime = 0;
int fps = 0;

vector<string> loadMap(const char* mapDir) {
	vector<string> tempMapData;
	string map;

	ifstream mapData(mapDir);
	while (getline(mapData, map)) {
		tempMapData.push_back(map);
	}

	return tempMapData;
}
void drawMap(vector<string> map) {
	for (int drawY = 0; drawY < map.size(); drawY++) {
		for (int drawX = 0; drawX < map.at(drawY).length(); drawX++) {
			if (map.at(drawY).at(drawX) != '0') {
				SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

				SDL_Rect drawRect = { drawX * 80, drawY * 80, 80, 80 };
				SDL_RenderFillRect(renderer, &drawRect);
			}
		}
	}
}
bool intercept(int x, int y, int width, int height, int detectX, int detectY) {
	if (x <= detectX && x + width >= detectX) {
		if (y <= detectY && y + height >= detectY) {
			return true;
		}
	}
	return false;
}
bool detectIntercept(vector<string> map) {
	for (int drawY = 0; drawY < map.size(); drawY++) {
		for (int drawX = 0; drawX < map.at(drawY).length(); drawX++) {
			if (map.at(drawY).at(drawX) != '0') {
				if (intercept(drawX * 80, drawY * 80, 80, 80, characterX, characterY)) {
					return true;
				}
				if (intercept(drawX * 80, drawY * 80, 80, 80, characterX + characterWidth, characterY)) {
					return true;
				}
				if (intercept(drawX * 80, drawY * 80, 80, 80, characterX, characterY + characterHeight)) {
					return true;
				}
				if (intercept(drawX * 80, drawY * 80, 80, 80, characterX + characterWidth, characterY + characterHeight)) {
					return true;
				}

				if (intercept(drawX * 80, drawY * 80, 80, 80, characterX, characterY + characterHeight / 2)) {
					return true;
				}
				if (intercept(drawX * 80, drawY * 80, 80, 80, characterX + characterWidth, characterY + characterHeight / 2)) {
					return true;
				}
			}
		}
	}

	return false;
}
bool move(vector<string> map, float speed, int direction) {
	switch (direction) {
	case 1:
		characterX += speed;
		if (detectIntercept(map)) {
			characterX -= speed;
			return true;
		}
		break;
	case 2:
		characterY += speed;
		if (detectIntercept(map)) {
			characterY -= speed;
			return true;
		}
		break;
	}

	return false;
}

void setGravity(vector<string> map) {
	if (deltaTime > 0 && fps > 0) {
		//cout << (float)(gravity / (float)(fps / 120.0f)) << endl;

		if (gravity <= 40) {
			gravity += deltaTime / 20.0;
		}
		if (move(map, gravity / (fps / 120.0f), 2)) {
			if (gravity >= 0) {
				gravity = 0;
				floating = false;
			}
			if (gravity <= 0) {
				gravity = 0;
			}
		}
	}
}
void moveCharacter(vector<string> map, float speed) {
	const Uint8* state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_LEFT]) {
		move(map, -speed, 1);
	}
	if (state[SDL_SCANCODE_RIGHT]) {
		move(map, speed, 1);
	}
	if (state[SDL_SCANCODE_UP]) {
		if (floating == false && gravity == 0) {
			gravity = -20;
			floating = true;
		}	
	}
}

int main(int argc, char* args[]) {
	vector<string> map = loadMap("F:\\Scarlet\\Project\\Simple Platformer\\Simple Platformer\\Simple Platformer\\data\\testMap.t");

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_CreateWindowAndRenderer(500, 450, SDL_WINDOW_RESIZABLE, &window, &renderer);

	int prvTime = SDL_GetTicks();
	
	while (true) {
		prvTime = SDL_GetTicks();

		SDL_Event event;
		SDL_PollEvent(&event);

		setGravity(map);
		moveCharacter(map, deltaTime);

		SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
		SDL_RenderClear(renderer);

		drawMap(map);

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_Rect drawRect = { characterX, characterY, characterWidth, characterHeight };
		SDL_RenderFillRect(renderer, &drawRect);

		SDL_RenderPresent(renderer);
		
		int tempFixer = (SDL_GetTicks() - prvTime);
		if ((1000 / fpsLimit) > tempFixer) {
			SDL_Delay((1000 / fpsLimit) - tempFixer);
		}

		if ((1000 / fpsLimit) > tempFixer) {
			deltaTime = SDL_GetTicks() - prvTime;
		}
		else {
			deltaTime = 0;
		}

		if (deltaTime != 0) {
			fps = 1000 / deltaTime;
		}
	}
}