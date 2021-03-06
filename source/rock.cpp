#include "rock.h"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

Rock::Rock() {
    hitbox.x=0;
    hitbox.y=0;
    hitbox.w=50;
    hitbox.h=50;
    posX = (rand() % ::SCREEN_WIDTH) + 1;
    posY = (rand() % ::SCREEN_HEIGHT) + 1;
    velX = 1;
    velY = 1;
    subX = 0;
    subY = 0;
    std::cout << __FILE__ << ": constructor..." << std::endl;
}

Rock::~Rock() {
}

void Rock::update() {
    hitbox.x = posX;
    hitbox.y = posY;
    posX += velX;
    posY += velY;
    if (posX < 0 || posX + hitbox.w > ::SCREEN_WIDTH) {
        velX = -velX;
    }
    if (posY < 0 || posY + hitbox.h > ::SCREEN_HEIGHT) {
        velY = -velY;
    }
}

void Rock::subpixel() {
	//assume signs of movement are positive
	    int signX = copysign(1.0,velX);
	    int signY = copysign(1.0,velY);
	//move subpixel amount forward by velocity
		subX += velX;
		subY += velY;
	//change actual position whenever subpixels exceed 1
	while( abs(subX) >= 1 ) {
		posX += signX;
		subX -= signX;
	}
	while( abs(subY) >= 1 ) {
		posY += signY;
		subY -= signY;
	}
}



void Rock::render(SDL_Renderer* render) {
    SDL_SetRenderDrawColor(render, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(render, &hitbox);
}


void Rock::free() {
    delete this;
}

const SDL_Rect& Rock::getRect() const{

    return hitbox;


}
