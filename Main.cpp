#include <iostream>
#include <vector>
#include <Windows.h>

#include <queue>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <map>
#include <set>

#include "Grid.h"
#include "Node.h"
#include "pathFinder.h"

using std::cout;
using std::endl;
using std::priority_queue;

SDL_Color WHITE     = { 255, 255, 255, 255 };
SDL_Color RED       = { 255, 0, 0, 255 };
SDL_Color GREEN     = { 0, 255, 0, 255 };
SDL_Color BLUE      = { 0, 0, 255, 255 };
SDL_Color YELLOW    = { 255, 255, 0, 255 };
SDL_Color BLACK     = { 0, 0, 0, 255 };
SDL_Color PURPLE    = { 128, 0, 128, 255 };
SDL_Color ORANGE    = { 255, 165, 0, 255 };
SDL_Color TURQUOISE = { 64, 224, 208, 255 };
SDL_Color GREY      = { 128, 128, 128, 255 };

bool operator==(const SDL_Color& a, const SDL_Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}


void drawScreen(SDL_Renderer* renderer, Grid<Node> grid) {

    SDL_SetRenderDrawColor(renderer, WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    SDL_RenderClear(renderer);
    grid.draw(renderer);
    SDL_RenderPresent(renderer);

}

SDL_Point getClickedPosOnGrid(SDL_Point pos, int rows, int width) {
    int gap = int(floor(width / rows));

    int row = int(floor(pos.x / gap));
    int col = int(floor(pos.y / gap));

    return { row,col };
}


int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    const unsigned int ROWS = 50;
    const unsigned int WIDTH = 800;


    Grid<Node> grid(ROWS, WIDTH);

    

    SDL_Window* window = SDL_CreateWindow("A* algorithm visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, WIDTH,0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    Node* start = NULL;
    Node* end = NULL;

    heuristicFunction* h = new manhattan_distance();

    PathFinder* astar = new Astar(h);


    SDL_Event event;

    bool quit = false;

    bool is_drawing = false;
    bool is_resetting = false;


    while (!quit) {

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_c){
                    grid.reset();
                    start = NULL;
                    end = NULL;
                }
                if (event.key.keysym.sym == SDLK_LSHIFT) {
                    grid.set_diagnol();
                }
                if (event.key.keysym.sym == SDLK_e)
                {
                    h = new manhattan_distance();
                    astar = new Astar(h);
                }
                if (event.key.keysym.sym == SDLK_e) {
                    h = new euclidean_distance();
                    astar = new Astar(h );
                }
                
                if (event.key.keysym.sym == SDLK_d) {
                    h = new diagonal_distance();
                    astar = new Astar(h);
                }
                if (event.key.keysym.sym == SDLK_SPACE && start && end) {
                    grid.set_neighbors();
                    astar->Run(start, end, grid, renderer, &drawScreen);
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT && !is_drawing) {
                    is_resetting = true;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    is_resetting = false;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT && !is_resetting) {
                    is_drawing = true;
                }
            }

            else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    is_drawing = false;
                }
            }

            if (is_drawing || is_resetting) {
                int x = event.motion.x;
                int y = event.motion.y;

                SDL_Point pos = getClickedPosOnGrid({ x,y }, ROWS, WIDTH);
                if (pos.x >= 0 && pos.x < ROWS && pos.y >= 0 && pos.y < ROWS) {
                    Node* node = grid[pos.x][pos.y];
                    if (!node->is_modifiable()) {
                        continue;
                    }
                    if (is_drawing) {
                        if (!start && node != end) {
                            start = node;
                            start->make_start();
                        }
                        else if (!end && node != start) {
                            end = node;
                            end->make_end();
                        }
                        else if (node != start && node != end) {
                            node->make_barrier();
                        }
                    }
                    if (is_resetting) {
                        node->reset();
                        if (node == start)
                            start = NULL;
                        else if (node == end)
                            end = NULL;

                    }

                }
            }

        }

        drawScreen(renderer, grid);

    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}