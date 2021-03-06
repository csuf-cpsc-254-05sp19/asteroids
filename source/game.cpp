#include "game.h"
#include <iostream>
#include <vector>

static SDL_Renderer* renderer = nullptr;


Game::Game() 
    : quit(false), counted_frames(0), fps(0.0f), window(NULL)
{
    for (int i = 0; i < 20; ++i) {
        rockList.push_back(Rock());
    }
}

Game::~Game() {
}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        quit = true;
    } else {
        window = SDL_CreateWindow(title, xpos, ypos, width, height, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
        if (window == NULL) {
            std::cerr << "Could not create window! SDL Error: " << SDL_GetError() << std::endl;
        } else {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
            if (renderer == NULL) {
                std::cerr << "Could not create renderer! SDL Error: " << SDL_GetError() << std::endl;
            } else {
                SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
                int imgFlag = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlag) & imgFlag)) {
                    std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
                }
            }
        }
    }
}

bool Game::loadMedia() {
    bool success = true;

    SDL_Texture* texture;
    if ((texture = loadFromFile("../resources/ship.png")) == nullptr) {
        std::cerr << "Unable to load ship" << std::endl;
        success = false;
    }
    assets.insert(std::make_pair("ship", texture));

    if ((texture = loadFromFile("../resources/ship_thrust.png")) == nullptr) {
        std::cerr << "Unable to load ship" << std::endl;
        success = false;
    }
    assets.insert(std::make_pair("ship_thrust", texture));

    return success;
}

SDL_Texture* Game::loadFromFile(std::string path) {
    SDL_Texture* loadedTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
    } else {
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0xFF, 0x00, 0xFF));
        loadedTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (loadedTexture == nullptr) {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(loadedSurface);
    }
    return loadedTexture;

}

void Game::handleEvents() {
    static SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            quit = true;
            return;
        }
        if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            s.handleEvents(e);
        }
    }
}

void Game::update() {

    framerate_timer.start();                                            // handle framerate
    fps = counted_frames / (framerate_timer.getTicks() / 1000.f);       //
    if (fps > 200000) {                                                 //
        fps = 0;                                                        //
    }                                                                   //
    ++counted_frames;                                                   //
    int frameTicks = framerate_timer.getTicks();                        //
    if (frameTicks < 1000 / 60) {                                       //
        SDL_Delay(1000 / 60 - frameTicks);                              //
    }                                                                   //
    s.update();
    for (int i = 0; i < rockList.size(); ) {
        rockList[i].update();
        if (SDL_HasIntersection(&(rockList[i].getRect()), &(s.getRect())) == SDL_TRUE) {
            s.lowerHp();
            std::swap(rockList[i], rockList.back());
            rockList.pop_back();
            continue;
        }
        ++i;
    }
    for (int i = 0; i < s.getBulletListSize(); ) {
        s.bulletUpdate(i);
        for (int j = 0; j < rockList.size(); ) {
            if (SDL_HasIntersection(&(s.getBulletHitbox(i)), &(rockList[j].getRect())) == SDL_TRUE) {
                s.setBulletRemoveStatus(i, true);

                std::swap(rockList[j], rockList.back());
                rockList.pop_back();
                continue;
            }
            ++j;
        }
        if (s.bulletIsRemovable(i)) {
            s.removeBullet(i);
            continue;
        }
        ++i;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);

    if (s.isMoving()) {
        s.render(renderer, (assets.find("ship_thrust"))->second);
    } else {
        s.render(renderer, (assets.find("ship"))->second);
    }
    for (int i = 0; i < s.getBulletListSize(); ++i) {
        s.bulletRender(i, renderer);
    }
    for (int i = 0; i < rockList.size(); ++i) {
        rockList[i].render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::close() {
    rockList.clear();
    IMG_Quit();
    SDL_Quit();
}
