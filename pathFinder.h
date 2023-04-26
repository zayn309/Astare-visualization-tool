#pragma once
#include "Node.h"
#include "heuristicFunction.h"
#include <unordered_map>
#include <queue>
#include <functional>
#include <set>

using std::priority_queue;

class PathFinder {
private:

public:
    heuristicFunction* h = NULL;

    PathFinder(heuristicFunction* h = NULL) {
        this->h = h;
    }

    void reconstruct_path(std::unordered_map<Node*, Node*>& came_from, Node* current, Node* start, Grid<Node> grid, SDL_Renderer* renderer, void (*drawScreen)(SDL_Renderer*, Grid<Node>)) {
        while (came_from.count(current)) {
            current = came_from[current];
            if (current != start)
                current->make_path();

            (*drawScreen)(renderer, grid);
        }
    }

    virtual bool Run(Node* start_node, Node* end_node, Grid<Node>& grid, SDL_Renderer* renderer, void (*drawScreen)(SDL_Renderer*, Grid<Node>)) = 0;


};

class Astar:public PathFinder  {
public:

    Astar(heuristicFunction* h = NULL) : PathFinder(h) {}

    bool Run(Node* start_node, Node* end_node, Grid<Node>& grid, SDL_Renderer* renderer, void (*drawScreen)(SDL_Renderer*, Grid<Node>)) {
        start_node->G_score = 0;

        start_node->F_score = this->h->calc(start_node->get_pos(), end_node->get_pos());

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
                curr_node = parent_map[curr_node];
                reconstruct_path(parent_map, curr_node, start_node, grid, renderer, drawScreen);
                return true;
            }

            for (Node* neighbor : curr_node->get_neighbors()) {

                double tentative_G_score = curr_node->G_score + 1;

                if (tentative_G_score < neighbor->G_score) {
                    parent_map[neighbor] = curr_node;
                    neighbor->G_score = tentative_G_score;
                    neighbor->F_score = tentative_G_score + this->h->calc(neighbor->get_pos(), end_node->get_pos());

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

            (*drawScreen)(renderer, grid);

            SDL_Delay(5);
        }
        return false;
    }
};