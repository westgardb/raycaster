#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#define PI  3.1415926535
#define PI2 1.5707963267	// PI divided by 2
#define PI3 4.7123889803	// 3 PI over 2
#define TAU 6.2831853071

#define WIDTH 1280
#define HEIGHT 720

// 90 degree field of view expressed in radians
#define FOV 1.3962634016
#define WALLHEIGHT 500

typedef struct {
	int x;
	int y;
	float dx;
	float dy;
	float xvel;
	float yvel;
	float rvel;
	float speed;
	float maxSpeed;
	float rspeed;
	float angle;
	float hitDist;
} Player;

SDL_Renderer* renderer;
SDL_Window* window;

int mWidth = 8;
int mHeight = 8;
int mScale = 64;

const int map[] = {
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 1, 1, 1, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 1,
	1, 0, 0, 0, 0, 1, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

void logPlayerInfo(Player* player) {
	printf("x: %d\n", player->x);
	printf("y: %d\n", player->y);
	printf("dx: %f\n", player->dx);
	printf("dy: %f\n", player->dy);
	printf("xvel: %f\n", player->xvel);
	printf("yvel: %f\n", player->yvel);
	printf("angle: %f\n\n", player->angle);
}

void drawRay2D(Player* player, float rx, float ry) {
	float screenx = (player->x + player->dx) * mScale;
	float screeny = (player->y + player->dy) * mScale;

	//logPlayerInfo(player);
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
	SDL_RenderDrawLine(
		renderer, 
		screenx, 
		screeny,
		rx * mScale, 
		ry * mScale
	);
}

// casts rays from the player and displays them
void castRays(unsigned int numRays, Player* player) {
	float xOffset, yOffset;
	float rayX = -1;	// x,y of grid intersections
	float rayY = -1;	// initialize to default values
	float finalInterceptX, finalInterceptY;	// final coords of ray intercept
	float arctanAngle, tanAngle; 
	int mapPos;
	float perpRayDist;

	float angleIncrement = FOV/(numRays + 1);
	float rayAngle = player->angle - (FOV/2);

	int rayWidth = WIDTH/numRays;

	for (int i = 0; i < numRays; i++) {
		rayAngle += angleIncrement;
		if (rayAngle < 0) {
			rayAngle += TAU;
		} else if (rayAngle >= TAU) {
			rayAngle -= TAU;
		}

		// check horizontal grid intersections
		if (rayAngle > 0 && rayAngle < PI) {	// angle facing upward (downward on screen)
			arctanAngle = 1/tan(rayAngle);

			rayX = player->x + player->dx + (1-player->dy) * arctanAngle;
			rayY = player->y + 1;
			xOffset = arctanAngle;
			yOffset = 1;

			mapPos = ((int) rayY * mWidth ) + (int) rayX;
			while (mapPos >= 0 && (mapPos < mWidth * mHeight) && map[mapPos] == 0) {
				rayX += xOffset;
				rayY += yOffset;
				mapPos = ((int) rayY * mWidth ) + (int) rayX;
			}

		} else if (rayAngle > PI) {	// angle facing downward (upward on screen)
			arctanAngle = 1/tan(rayAngle);

			rayX = player->x + player->dx - (player->dy * arctanAngle);
			rayY = player->y;
			xOffset = -arctanAngle;
			yOffset = -1;

			mapPos = (((int) rayY - 1) * mWidth ) + (int) rayX;
			while (mapPos >= 0 && (mapPos < mWidth * mHeight) && map[mapPos] == 0) {
				rayX += xOffset;
				rayY += yOffset;
				mapPos = (((int) rayY - 1) * mWidth ) + (int) rayX;
			}
		}

		finalInterceptX = rayX;
		finalInterceptY = rayY;

		// check vertical grid intersections
		if (rayAngle < PI2 || rayAngle > PI3) {
			tanAngle = tan(rayAngle);

			rayX = player->x + 1;
			rayY = player->y + player->dy + (1 - player->dx) * tanAngle;
			xOffset = 1;
			yOffset = tanAngle;

			mapPos = ((int) rayY * mWidth ) + (int) rayX;
			while (mapPos >= 0 && (mapPos < mWidth * mHeight) && map[mapPos] == 0) {
				rayX += xOffset;
				rayY += yOffset;
				mapPos = ((int) rayY * mWidth ) + (int) rayX;
			}

		} else if (rayAngle > PI2 && rayAngle < PI3) {
			tanAngle = tan(rayAngle);

			rayX = player->x;
			rayY = player->y + player->dy - (player->dx * tanAngle);
			xOffset = -1;
			yOffset = -tanAngle;

			mapPos = ((int) rayY * mWidth ) + (int) rayX - 1;
			while (mapPos >= 0 && (mapPos < mWidth * mHeight) && map[mapPos] == 0) {
				rayX += xOffset;
				rayY += yOffset;
				mapPos = ((int) rayY * mWidth ) + (int) rayX - 1;
			}
		}
		
		// compare intersection points and draw
		float horizDistX = finalInterceptX - player->x - player->dx;
		float horizDistY = finalInterceptY - player->y - player->dy;
		float vertDistX = rayX - player->x - player->dx;
		float vertDistY = rayY - player->y - player->dy;

		if (((horizDistX * horizDistX) + (horizDistY * horizDistY))
			> ((vertDistX * vertDistX) + (vertDistY * vertDistY))) {
			finalInterceptX = rayX;
			finalInterceptY = rayY;
			perpRayDist = vertDistX * cos(player->angle) + vertDistY * sin(player->angle);
			//SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
		} else {
			perpRayDist = horizDistX * cos(player->angle) + horizDistY * sin(player->angle);
			//SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
		}

		//drawRay2D(player, finalInterceptX, finalInterceptY);

		// draw rays in 3D
		float rayHeight = WALLHEIGHT / perpRayDist;
		float rcolor = 220 - 15 * perpRayDist;
		SDL_SetRenderDrawColor(renderer, rcolor, rcolor, rcolor, 255);

		const SDL_Rect rect = {
			i * rayWidth,
			HEIGHT / 2 - rayHeight / 2,
			rayWidth,
			rayHeight
		};

		SDL_RenderDrawRect(renderer, &rect);
		SDL_RenderFillRect(renderer, &rect);

	}
}

void drawMap2D() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	for (int i = 0; i < mHeight; i++) {
		for (int j = 0; j < mWidth; j++) {
			if (map[(i * mWidth) + j] == 1) {
				
				const SDL_Rect rect = {
					j * mScale,
					i * mScale,
					mScale,
					mScale
				};

				SDL_RenderDrawRect(renderer, &rect);
				//SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
}

// move the player and check for collisions
void movePlayer(Player* player) {
	player->xvel = player->speed * cos(player->angle);
	player->yvel = player->speed * sin(player->angle);

	player->angle += player->rvel;
	if (player->angle < 0) {
		player->angle += TAU;
	} else if (player->angle >= TAU) {
		player->angle -= TAU;
	}

	// preemptively check collisions and reduce velocity
	if (player->xvel > 0) {
		if (map[((int) player->y * mWidth ) + (int) player->x + 1]) {
			if (player->dx + player->xvel > 1 - player->hitDist) {
				player->xvel -= player->dx + player->xvel - (1 - player->hitDist);
			}
		}
	} else if (player->xvel < 0) {
		if (map[((int) player->y * mWidth ) + (int) player->x - 1]) {
			if (player->dx + player->xvel < player->hitDist) {
				player->xvel -= player->dx + player->xvel - player->hitDist;
			}
		}
	}

	if (player->yvel > 0) {
		if (map[(((int) player->y + 1) * mWidth ) + (int) player->x]) {
			if (player->dy + player->yvel > 1 - player->hitDist) {
				player->yvel -= player->dy + player->yvel - (1 - player->hitDist);
			}
		}
	} else if (player->yvel < 0) {
		if (map[(((int) player->y - 1) * mWidth ) + (int) player->x]) {
			if (player->dy + player->yvel < player->hitDist) {
				player->yvel -= player->dy + player->yvel - player->hitDist;
			}
		}
	}

	player->dx += player->xvel;
	if (player->dx >= 1) {
		player->x++;
		player->dx -= 1;
	} else if (player->dx < 0) {
		player->x--;
		player->dx += 1;
	}
	
	player->dy += player->yvel;
	if (player->dy >= 1) {
		player->y++;
		player->dy -= 1;
	} else if (player->dy < 0) {
		player->y--;
		player->dy += 1;
	}
}

void drawPlayer(Player* player) {
	float screenx = (player->x + player->dx) * mScale;
	float screeny = (player->y + player->dy) * mScale;

	const SDL_Rect prect = {
		screenx - 4,
		screeny - 4,
		8,
		8
	};

	SDL_RenderDrawRect(renderer, &prect);
	SDL_RenderFillRect(renderer, &prect);

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	
	SDL_RenderDrawLine(
		renderer, 
		screenx, 
		screeny,
		screenx + (30* cos(player->angle)), 
		screeny + (30 * sin(player->angle))
	);
}

void drawSkybox() {
	const SDL_Rect skybox = {
		0,
		0,
		WIDTH,
		HEIGHT / 2
	};
	
	SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);

	SDL_RenderDrawRect(renderer, &skybox);
	SDL_RenderFillRect(renderer, &skybox);
}

int main(int argc, char* argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(
			"Raycaster",				// window title
			SDL_WINDOWPOS_CENTERED,		// initial x position
			SDL_WINDOWPOS_CENTERED,		// initial y position
			WIDTH,						// width of window
			HEIGHT,						// height of window
			0							// window flags
	);

	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	Player player;
	player.x = 1;
	player.y = 1;
	player.dx = 0.5;
	player.dy = 0.5;
	player.xvel = 0;
	player.yvel = 0;
	player.rvel = 0;
	player.speed = 0;
	player.maxSpeed = 0.02;
	player.rspeed = 0.03;
	player.angle = 0;
	player.hitDist = 0.06;


	int running = 1;

	//logPlayerInfo(&player);

	// main loop
	while (running) {

		// ----- HANDLE EVENTS ----- 
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
						case 'q':
							running = 0;
							break;
						case 'w':
							player.speed = player.maxSpeed;
							//logPlayerInfo(&player);
							break;
						case 'a':
							player.rvel = -1 * player.rspeed;
							//logPlayerInfo(&player);
							break;
						case 's':
							player.speed = -0.7 * player.maxSpeed;
							//logPlayerInfo(&player);
							break;
						case 'd':
							player.rvel = player.rspeed;
							//logPlayerInfo(&player);
							break;
					}
					break;

				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
						case 'q':
							running = 0;
							break;
						case 'w':
							player.speed = 0;
							//logPlayerInfo(&player);
							break;
						case 'a':
							player.rvel = 0;
							//logPlayerInfo(&player);
							break;
						case 's':
							player.speed = 0;
							//logPlayerInfo(&player);
							break;
						case 'd':
							player.rvel = 0;
							//logPlayerInfo(&player);
							break;
					}
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
		SDL_RenderClear(renderer);
		
		// draw player and map in white
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		movePlayer(&player);
		
		//logPlayerInfo(&player);

		//drawPlayer(&player);
		drawSkybox();
		castRays(160, &player);
		//drawMap2D();

		SDL_RenderPresent(renderer);

	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 1;
}