#include "Node.h"

Node::Node(int row, int col, int width, int total_rows)
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

SDL_Point Node::get_pos()
{
    return { row, col };
}

bool Node::is_modifiable() {
    return this->modifiable;
}    
bool Node::is_open() {
    return this->color == GREEN;
}    
bool Node::is_closed() const {
    return this->color == RED;
}

bool Node::is_barrier() const {
    return this->color == BLACK;
}

bool Node::is_start() const {
    return this->color == ORANGE;
}

bool Node::is_end() const {
    return this->color == PURPLE;
}
void Node::make_unmodifiable() {
    this->modifiable = false;
}

void Node::reset() {
    this->color = WHITE;
}


void Node::make_start() {
    this->color = ORANGE;
}

void Node::make_closed() {
    this->color = RED;
}

void Node::make_open() {
    this->color = GREEN;
}

void Node::make_barrier() {
    this->color = BLACK;
}

void Node::make_end() {
    this->color = PURPLE;
}

void Node::make_path() {
    this->color = TURQUOISE;
}

void Node::draw(SDL_Renderer* renderer) const {
    SDL_Rect rect = { x, y, this->width, this->width };
    SDL_SetRenderDrawColor(renderer, this->color.r, this->color.g, this->color.b, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void Node::update_neighbors(Grid<Node> grid) {
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


std::vector<Node*>& Node::get_neighbors() {
    return this->neighbors;
}

bool Node::operator<(const Node& other) const {
    return false;
}