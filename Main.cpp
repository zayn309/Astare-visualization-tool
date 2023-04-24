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

using std::cout;
using std::endl;
using std::priority_queue;


SDL_Color WHITE = { 255, 255, 255, 255 };
SDL_Color RED = { 255, 0, 0, 255 };
SDL_Color GREEN = { 0, 255, 0, 255 };
SDL_Color BLUE = { 0, 0, 255, 255 };
SDL_Color YELLOW = { 255, 255, 0, 255 };
SDL_Color BLACK = { 0, 0, 0, 255 };
SDL_Color PURPLE = { 128, 0, 128, 255 };
SDL_Color ORANGE = { 255, 165, 0, 255 };
SDL_Color GREY = { 128, 128, 128, 255 };
SDL_Color TURQUOISE = { 64, 224, 208, 255 };



template<typename T>
class Grid {
private:

    std::vector<std::vector<T*>> grid;
    int rows;
    int width;
    int gap;

    void drawGridLines(SDL_Renderer* renderer) {

        SDL_SetRenderDrawColor(renderer, GREY.r, GREY.g, GREY.b, GREY.a);
        for (int i = 0; i < rows; i++) {
            SDL_RenderDrawLine(renderer, 0, i * gap, width, i * gap);
        }

        for (int i = 0; i < rows; i++) {
            SDL_RenderDrawLine(renderer, i * gap, 0, i * gap, width);
        }

    }

    void drawGridNodes(SDL_Renderer* renderer) {

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rows; j++) {
                grid[i][j]->draw(renderer);
            }
        }
    }

public:

    Grid(int rows, int width) : rows(rows), width(width) {
        reset();
    }


    void draw(SDL_Renderer* renderer) {
        this->drawGridNodes(renderer);
        this->drawGridLines(renderer);
    }


    std::vector<T*>& operator[](int index) {
        return grid[index];
    }
    int get_rows() {
        return this->rows;
    }

    int get_width() {
        return this->width;
    }

    void set_neighbors() {
        for (auto& row : grid) {
            for (auto& node : row) {
                node->update_neighbors(*this);
            }
        }
    }

    void reset() {

        grid = std::vector<std::vector<T*>>(rows, std::vector<T*>(rows));
        gap = int(floor(width / rows));
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < rows; j++) {
                T* node = new T(i, j, gap, rows);
                if (i == 0 || j == 0 || i == rows - 1 || j == rows - 1) {
                    node->make_barrier();
                    node->make_unmodifiable();
                }

                grid[i][j] = node;
            }
        }
    }

};


bool operator==(const SDL_Color& a, const SDL_Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

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


public:
    double G_score;
    double F_score;

    Node(int row, int col, int width, int total_rows)
    {

        this->row = row;
        this->col = col;
        this->x = row * width;
        this->y = col * width;
        this->color = WHITE;
        this->neighbors.resize(0);
        this->width = width;
        this->total_rows = total_rows;
        this->G_score = INT_MAX;
        this->F_score = INT_MAX;
        this->modifiable = true;

    }

    SDL_Point get_pos()
    {
        return { row, col };
    }
    bool is_modifiable() {
        return this->modifiable;
    }
    bool is_open() {
        return this->color == GREEN;
    }
    bool is_closed() const {
        return this->color == RED;
    }

    bool is_barrier() const {
        return this->color == BLACK;
    }

    bool is_start() const {
        return this->color == ORANGE;
    }

    bool is_end() const {
        return this->color == PURPLE;
    }
    void make_unmodifiable() {
        this->modifiable = false;
    }

    void reset() {
        this->color = WHITE;
    }


    void make_start() {
        this->color = ORANGE;
    }

    void make_closed() {
        this->color = RED;
    }

    void make_open() {
        this->color = GREEN;
    }

    void make_barrier() {
        this->color = BLACK;
    }

    void make_end() {
        this->color = PURPLE;
    }

    void make_path() {
        this->color = TURQUOISE;
    }

    void draw(SDL_Renderer* renderer) const {
        SDL_Rect rect = { x, y, this->width, this->width };
        SDL_SetRenderDrawColor(renderer, this->color.r, this->color.g, this->color.b, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    void update_neighbors(Grid<Node> grid) {
        neighbors.clear();
        if (!this->is_barrier()) {

            if (!grid[row + 1][col]->is_barrier()) { // DOWN
                neighbors.push_back(grid[row + 1][col]);
            }

            if (!grid[row - 1][col]->is_barrier()) { // UP
                neighbors.push_back(grid[row - 1][col]);
            }

            if (!grid[row][col + 1]->is_barrier()) { // RIGHT
                neighbors.push_back(grid[row][col + 1]);
            }

            if (!grid[row][col - 1]->is_barrier()) { // LEFT
                neighbors.push_back(grid[row][col - 1]);
            }
        }
    }


    std::vector<Node*>& get_neighbors() {
        return this->neighbors;
    }

    bool operator<(const Node& other) const {
        return false;
    }


};


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

void reconstruct_path(std::unordered_map<Node*, Node*>& came_from, Node* current, Node* start, Grid<Node> grid, SDL_Renderer* renderer) {
    while (came_from.count(current)) {
        current = came_from[current];
        if (current != start)
            current->make_path();

        drawScreen(renderer, grid);
    }
}

int manhattan_distance(SDL_Point p1, SDL_Point p2) {
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

bool Astar(Node* start_node, Node* end_node, Grid<Node> grid, SDL_Renderer* renderer) {
    start_node->G_score = 0;
    start_node->F_score = manhattan_distance(start_node->get_pos(), end_node->get_pos());

    std::priority_queue<Node*, std::vector<Node*>, std::function<bool(Node*, Node*)>> pq([](Node* n1, Node* n2) { return n1->F_score > n2->F_score; });
    pq.push(start_node);

    std::unordered_map<Node*, Node*> parent_map;

    std::set<Node*>pq_hash;
    pq_hash.insert(start_node);

    SDL_Event event;

    while (!pq.empty()) {

        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            exit(1);
        }

        Node* curr_node = pq.top();
        pq.pop();
        pq_hash.erase(curr_node);

        if (curr_node == end_node) {
            reconstruct_path(parent_map, curr_node, start_node, grid, renderer);
            end_node->make_end();
            return true;
        }

        for (Node* neighbor : curr_node->get_neighbors()) {

            double tentative_G_score = curr_node->G_score + 1;

            if (tentative_G_score < neighbor->G_score) {
                parent_map[neighbor] = curr_node;
                neighbor->G_score = tentative_G_score;
                neighbor->F_score = tentative_G_score + manhattan_distance(neighbor->get_pos(), end_node->get_pos());

                if (!pq_hash.count(neighbor)) {
                    pq.push(neighbor);
                    pq_hash.insert(neighbor);
                    neighbor->make_open();
                }
            }
        }

        if (curr_node != start_node) {
            curr_node->make_closed();
        }

        drawScreen(renderer, grid);
        Sleep(5);
    }
    return false;

}


int main(int argc, char* argv[]) {
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    const unsigned int ROWS = 50;
    const unsigned int WIDTH = 800;


    Grid<Node> grid(ROWS, WIDTH);


    SDL_Window* window = SDL_CreateWindow("A* algorithm visualizer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, WIDTH, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    Node* start = NULL;
    Node* end = NULL;


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
                if (event.key.keysym.sym == SDLK_SPACE) {
                    grid.set_neighbors();
                    Astar(start, end, grid, renderer);
                }
                if (event.key.keysym.sym == SDLK_c) {
                    grid.reset();
                    start = NULL;
                    end = NULL;
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