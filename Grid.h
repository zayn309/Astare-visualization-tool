#pragma once
#include <SDL.h>
#include <vector>

extern SDL_Color GREY;


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
    void set_diagnol() {
        for (auto& row : grid) {
            for (auto& node : row) {
                node->activate_diagnol_moves();
            }
        }
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
