#ifndef COMPONENTS_DEF_HPP_
#define COMPONENTS_DEF_HPP_
#include "ECS.hpp"
#include <iostream>

struct Transform
{
    float x;
    float y;
    float scale;
    float rotation;

    void print()
    {
        std::cout << std::to_string(x) << "," << std::to_string(y) << "," << std::to_string(scale) << ","
                  << std::to_string(rotation) << std::endl;
    }

    static inline Components type = Components::TRANSFORM;
};

struct Sprite
{
    size_t render_itr;
    size_t sprite;

    void print()
    {
        std::cout << std::to_string(render_itr) << "," << std::to_string(sprite) << "," << std::endl;
    }

    static inline Components type = Components::SPRITE;
};

struct Movement
{
    float velocity;
    float acceleration;

    void print()
    {
        std::cout << std::to_string(velocity) << "," << std::to_string(acceleration) << "," << std::endl;
    }

    static inline Components type = Components::MOVEMENT;
};

#endif