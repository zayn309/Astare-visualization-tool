#pragma once
#include <vector>
#include <SDL.h>

#include "Grid.h"

extern SDL_Color WHITE;
extern SDL_Color RED;
extern SDL_Color GREEN;
extern SDL_Color BLUE;
extern SDL_Color YELLOW;
extern SDL_Color BLACK;
extern SDL_Color PURPLE;
extern SDL_Color ORANGE;
extern SDL_Color TURQUOISE;


extern bool operator==(const SDL_Color& a, const SDL_Color& b);

class Node
{
private:
    int row;
    int col;
    int x;
    int y;
    int width;
    SDL_Color color;
    std::vector<Node*> neighbors;
    int total_rows;
    bool modifiable;
    bool diagnol;

public:
    double G_score;
    double F_score;

    Node(int row, int col, int width, int total_rows);
    

    SDL_Point get_pos();

    bool is_modifiable();

    bool is_open();

    bool is_closed() const;

    bool is_barrier() const;

    bool is_start() const;

    bool is_end() const;

    void make_unmodifiable();

    void reset();

    void activate_diagnol_moves();

    void make_start();

    void make_closed();

    void make_open();

    void make_barrier();

    void make_end();

    void make_path();

    void draw(SDL_Renderer* renderer) const;

    void update_neighbors(Grid<Node> grid);

    std::vector<Node*>& get_neighbors();

    bool operator<(const Node& other) const;


};