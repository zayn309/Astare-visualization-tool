#pragma once

#include <iostream>
#include <SDL.h>

using std::min;

class heuristicFunction
{
public :
	virtual int calc(SDL_Point p1, SDL_Point p2) = 0;

};

class manhattan_distance :
    public heuristicFunction
{
public:

    int calc(SDL_Point p1, SDL_Point p2) {
        return abs(p1.x - p2.x) + abs(p1.y - p2.y);
    }

};

class euclidean_distance :
    public heuristicFunction
{
public:
    int calc(SDL_Point p1, SDL_Point p2) {
        int dx = p1.x - p2.x;
        int dy = p1.y - p2.y;
        return sqrt(dx * dx + dy * dy);
    }
};


class diagonal_distance :
    public heuristicFunction
{
public:
    int calc(SDL_Point p1, SDL_Point p2) {
        int dx = abs(p1.x - p2.x);
        int dy = abs(p1.y - p2.y);
        return (dx + dy) - min(dx, dy);
    }
};