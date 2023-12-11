#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>


#define WIDTH 720
#define HEIGHT 720
#define FONT_SIZE 32
#define SIZE 16
#define BALL_SPEED 16
#define PADDLE_SPEED 9
#define PI 3.141592625358979323846


SDL_Renderer* renderer;
SDL_Window* window;
TTF_Font* font;
SDL_Color color;
bool running;
int frameCount, timerFPS, lastFrame, fps;

SDL_Rect l_paddle, r_paddle, ball, score_board;
float velX, velY;
std::string score;
int l_score, r_score;
bool turn;


void serve() {
	// Starting positions for each paddle on start and after a point is scored
	l_paddle.y = r_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
	
	// Starting position and direction with Right Paddle as the server
	if (turn) {
		ball.x = l_paddle.x + (l_paddle.w * 4);
		velX = BALL_SPEED / 2;
	}
	// Starting position and direction with Left Paddle as the server
	else {
		ball.x = r_paddle.x - (r_paddle.w * 4);
		velX = -BALL_SPEED / 2;
	}

	// Starting height and Y-velocity
	velY = 0;
	ball.y = HEIGHT / 2 - (SIZE / 2);

	// Alternate server after each point
	turn = !turn;
}



// write score to the screen
void write(std::string text, int x, int y){
	SDL_Surface *surface;
	SDL_Texture *texture;
	if (font == NULL) std::cout << "Font failure in write()" << std::endl;
	const char* t = text.c_str();
	surface = TTF_RenderText_Solid(font, t, color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);

	// Scoreboard size and position
	score_board.w = surface->w;
	score_board.h = surface->h;
	score_board.x = x - score_board.w;
	score_board.y = y - score_board.h;

	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &score_board);
	SDL_DestroyTexture(texture);

}

// Welcome screen when game is first started up
int titleScreen() {

	// Welcome text
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Welcome to Pong", color);
	SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect messageRect;
	messageRect.x = WIDTH / 4;
	messageRect.y = 0;
	messageRect.w = WIDTH/2;
	messageRect.h = 100;

	SDL_RenderCopy(renderer, message, NULL, &messageRect);

	// Author text
	surfaceMessage = TTF_RenderText_Solid(font, "Coded by Billy Wood", color);
	message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	messageRect.x = 250;
	messageRect.y = 100;
	messageRect.w = 200;
	messageRect.h = 20;
	SDL_RenderCopy(renderer, message, NULL, &messageRect);

	// Press any button to get play
	surfaceMessage = TTF_RenderText_Solid(font, "Press any button to play", color);
	message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	messageRect.x = 160;
	messageRect.y = 350;
	messageRect.w = 400;
	messageRect.h = 75;
	SDL_RenderCopy(renderer, message, NULL, &messageRect);

	// Movement instructions
	surfaceMessage = TTF_RenderText_Solid(font, "UP Arrow - Move Up", color);
	message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	messageRect.x = 225;
	messageRect.y = 550;
	messageRect.w = 275;
	messageRect.h = 50;
	SDL_RenderCopy(renderer, message, NULL, &messageRect);

	surfaceMessage = TTF_RenderText_Solid(font, "DOWN Arrow - Move Down", color);
	message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	messageRect.x = 225;
	messageRect.y = 600;
	messageRect.w = 275;
	messageRect.h = 50;
	SDL_RenderCopy(renderer, message, NULL, &messageRect);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(message); 
	SDL_RenderPresent(renderer);
	

	// Title Screen wait for input
	SDL_Event e;
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_KEYDOWN) {
			return 0;
		}
		else break;
	}

	return 1;
}


void update() {
	// interaction between paddles and the ball
	if (SDL_HasIntersection(&ball, &r_paddle)) {
		double relIntercept = (r_paddle.y + (r_paddle.h / 2)) - (ball.y + (SIZE / 2));
		double normalize = relIntercept / (r_paddle.h / 2);
		double bounce = normalize * (5 * PI / 12);
		velX = -BALL_SPEED * cos(bounce);
		velY = BALL_SPEED * -sin(bounce);
	}
	if (SDL_HasIntersection(&ball, &l_paddle)) {
		double relIntercept = (l_paddle.y + (l_paddle.h / 2)) - (ball.y + (SIZE / 2));
		double normalize = relIntercept / (l_paddle.h / 2);
		double bounce = normalize * (5 * PI / 12);
		velX = BALL_SPEED * cos(bounce);
		velY = BALL_SPEED * -sin(bounce);
	}

	// move Right Paddle in the same Y direction as the ball
	if (ball.y > r_paddle.y + (r_paddle.h / 2)) r_paddle.y += PADDLE_SPEED;
	if (ball.y < r_paddle.y + (r_paddle.h / 2)) r_paddle.y -= PADDLE_SPEED;

	// paddle interactions with top and bottom screen boundaries
	if (l_paddle.y < 0) l_paddle.y = 0;
	if (l_paddle.y + l_paddle.h > HEIGHT) l_paddle.y = HEIGHT - l_paddle.h;

	if (r_paddle.y < 0) r_paddle.y = 0;
	if (r_paddle.y + r_paddle.h > HEIGHT) r_paddle.y = HEIGHT - r_paddle.h;

	// ball interactions with left and right screen boundaries
	if (ball.x <= 0) {
		r_score++;
		serve();
	}
	if (ball.x + SIZE >= WIDTH) {
		l_score++;
		serve();
	}
	ball.x += velX;
	ball.y += velY;

	// update score board
	score = std::to_string(l_score) + "        " + std::to_string(r_score);

	// ball interaction with top and bottom screen boundaries
	if (ball.y <= 0 || (ball.y + SIZE >= HEIGHT)) velY = -velY;


}
void input() {
	SDL_Event e;
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = false;
	if (keystates[SDL_SCANCODE_ESCAPE]) running = false;
	if (keystates[SDL_SCANCODE_UP]) l_paddle.y -= PADDLE_SPEED;
	if (keystates[SDL_SCANCODE_DOWN]) l_paddle.y += PADDLE_SPEED;
}


void render() {

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
	SDL_RenderClear(renderer);


	frameCount++;
	timerFPS = SDL_GetTicks() - lastFrame;
	if (timerFPS < (1000 / 60)) {
		SDL_Delay((1000 / 60) - timerFPS);
	}

	// set draw color to white
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

	// fill color
	SDL_RenderFillRect(renderer, &l_paddle);
	SDL_RenderFillRect(renderer, &r_paddle);
	SDL_RenderFillRect(renderer, &ball);

	// display score updates to screen
	write(score, WIDTH / 2 + FONT_SIZE, FONT_SIZE * 2);

	//show everything
	SDL_RenderPresent(renderer);
}




int main( int argc, char *argv[])
{

	// initialize everything
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed at SDL_Init()" << std::endl;
	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
	TTF_Init();
	font = TTF_OpenFont("Peepo.ttf", FONT_SIZE);
	running = 1;


	static int lastTime = 0;
	
	// set color to white
	color.r = color.g = color.b = 255;
	
	l_score = r_score = 0;

	// set dimensions of paddles and starting positions
	l_paddle.x = 32; l_paddle.h = HEIGHT / 4;
	l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
	l_paddle.w = 12;

	r_paddle = l_paddle;
	r_paddle.x = WIDTH - r_paddle.w - 32;

	//set dimensions of the ball
	ball.w = ball.h = SIZE;

	while(titleScreen());

	serve();

	while (running) {
		lastFrame = SDL_GetTicks();
		if (lastFrame <= (lastTime + 1000)) {
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}

		
		update();
		input();
		render();
	}
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();


	return 0;
}