#include <SDL.h>
#include <iostream>
#include <SDL_image.h>


//размер окна
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

struct Player {
	SDL_Rect rect; //Позиция и размер игрока
	int velx, vely;
	bool onGround;
	SDL_Texture* texture;
	int frame;
	int direction; //-1 в лево, 1 в право
};

struct Platform {
	SDL_Rect rect; //Позиция и размер платформы
	SDL_Texture* texture;
};

SDL_Texture* loadTexture(const std::string& file, SDL_Renderer* renderer) {
	SDL_Texture* nexTexture = nullptr;
	SDL_Surface* loadedSurface = IMG_Load(file.c_str());

	if (loadedSurface == nullptr) {
		std::cerr << "Ne udalos zagrusit izobrajenie: " << IMG_GetError() << std::endl;
		return nullptr;
	}

	nexTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
	SDL_FreeSurface(loadedSurface);

	if (nexTexture == nullptr) {
		std::cerr << "Ne udalos sozdat' texture: " << SDL_GetError() << std::endl;
	}

	return nexTexture;
}

void initSDL(SDL_Window** window, SDL_Renderer** renderer) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "Ошибка инициализации SDL: " << SDL_GetError() << std::endl;
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
//Функция обработки ввода
void handleInput(Player& player, const Uint8* keystate) {
	player.velx = 0;

	if (keystate[SDL_SCANCODE_LEFT]) {
		player.velx = -5;
		player.direction = -1;
		player.frame = (SDL_GetTicks() / 100) % 6;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		player.velx = 5;
		player.direction = 1;
		player.frame = (SDL_GetTicks() / 100) % 6;
	}
	if (keystate[SDL_SCANCODE_SPACE] && player.onGround) {
		player.vely = -15;
		player.onGround = false;
	}
}

// Создание функций для обновления состояния игрока
void updatePlayer(Player& player, Platform& platform) {
	// Гравитация
	player.vely += 1;

	// Обновление позиции по X
	player.rect.x += player.velx;

	if (player.rect.x < 0 || (player.rect.x + player.rect.w) > SCREEN_WIDTH) {
		player.rect.x -= player.velx;
	}

	// Обновление позиции по Y
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
// рендер объектов
void render(SDL_Renderer* renderer, Player& player, Platform& platform) {
	//ochistka ecrana
	SDL_SetRenderDrawColor(renderer, 155, 200, 235, 255); //цвет фона
	SDL_RenderClear(renderer);

	//platforma

	SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
	SDL_RenderFillRect(renderer, &platform.rect);

	//player
	int frameWigth = player.rect.w;
	int frameHeight = player.rect.h;

	SDL_Rect sourceRect = { player.frame * frameWigth, 0, frameWigth, frameHeight };
	SDL_RenderCopyEx(renderer, player.texture, &sourceRect, &player.rect, 0, nullptr, player.direction == 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);

	//obnovlenie ecrana

	SDL_RenderPresent(renderer);
}

void closeSDL(SDL_Window* window, SDL_Renderer* renderer) {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "RUS");
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	initSDL(&window, &renderer);

	// Задаем начальные параметры игрока и платформы
	Player player = { {100, 100, 52, 37}, 0, 0, false, nullptr, 0, 1}; // Прямоугольник: {x, y, w, h}, скорость, на земле
	player.texture = loadTexture("\\assets\\adventurer-run3-sword-Sheet.png", renderer);

	Platform platform = { {0, 500, SCREEN_WIDTH, 50}, nullptr };  // Платформа шириной во весь экран

	bool quit = false;
	SDL_Event e;

	// Основной игровой цикл
	while (!quit) {
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		// Обработка событий
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
				SDL_DestroyTexture(player.texture);
				SDL_DestroyTexture(platform.texture);
			}
		}

		// Обработка ввода
		handleInput(player, keystate);

		// Обновление состояния игрока
		updatePlayer(player, platform);

		// Рендеринг сцены
		render(renderer, player, platform);

		// Задержка для ограничения FPS
		SDL_Delay(16); // Примерно 60 FPS
	}

	closeSDL(window, renderer);

	return 0;
}
