#include <SDL.h>
#include <iostream>

//������ ����
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct Player {
	SDL_Rect rect; //������� � ������ ������
	int velx, vely;
	bool onGround;
};

struct Platform {
	SDL_Rect rect; //������� � ������ ���������
};

void initSDL(SDL_Window** window, SDL_Renderer** renderer) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "������ ������������� SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}
	*window = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (*window == nullptr) {
		std::cerr << "Window Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (*renderer == nullptr) {
		std::cerr << "renderer Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
}
//������� ��������� �����
void handleInput(Player& player, const Uint8* keystate) {
	player.velx = 0;

	if (keystate[SDL_SCANCODE_LEFT]) {
		player.velx = -5;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		player.velx = 5;
	}
	if (keystate[SDL_SCANCODE_SPACE] && player.onGround) {
		player.vely = -15;
		player.onGround = false;
	}
}
// �������� ������� ��� ���������� ��������� ������
void updatePlayer(Player& player, Platform& platform) {
	// ����������
	player.vely += 1;

	// ���������� ������� �� X
	player.rect.x += player.velx;

	if (player.rect.x < 0 || (player.rect.x + player.rect.w) > SCREEN_WIDTH) {
		player.rect.x -= player.velx;
	}

	// ���������� ������� �� Y
	player.rect.y += player.vely;

	if (SDL_HasIntersection(&player.rect, &platform.rect)) {
		if (player.vely > 0) {
			player.rect.y = platform.rect.y - player.rect.h;
			player.vely = 0;
			player.onGround = true;
		}
	}
	else {
		player.onGround = false;
	}

	if (player.rect.y > SCREEN_HEIGHT - player.rect.h) {
		player.rect.y = SCREEN_HEIGHT - player.rect.h;
		player.vely = 0;
		player.onGround = true;
	}
 
}
// ������ ��������
void render(SDL_Renderer* renderer, Player& player, Platform& platform) {
	SDL_SetRenderDrawColor(renderer, 155, 200, 235, 255);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
	SDL_RenderFillRect(renderer, &platform.rect);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderFillRect(renderer, &player.rect);

	SDL_RenderPresent(renderer);
}

void closeSDL(SDL_Window* window, SDL_Renderer* renderer) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	initSDL(&window, &renderer);

	// ������ ��������� ��������� ������ � ���������
	Player player = { {100, 100, 50, 50}, 0, 0, false }; // �������������: {x, y, w, h}, ��������, �� �����
	Platform platform = { {0, 500, SCREEN_WIDTH, 50} };  // ��������� ������� �� ���� �����

	bool quit = false;
	SDL_Event e;

	// �������� ������� ����
	while (!quit) {
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		// ��������� �������
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			}
		}

		// ��������� �����
		handleInput(player, keystate);

		// ���������� ��������� ������
		updatePlayer(player, platform);

		// ��������� �����
		render(renderer, player, platform);

		// �������� ��� ����������� FPS
		SDL_Delay(16); // �������� 60 FPS
	}

	closeSDL(window, renderer);

	return 0;
}
