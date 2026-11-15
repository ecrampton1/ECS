#ifndef COMPONENTS_DEF_HPP_
#define COMPONENTS_DEF_HPP_
#include "ECS.hpp"
#include <sstream>

struct Transform
{
    float x;
    float y;
    float scale;
    float rotation;

    // Debug strings
    std::string toString() const
    {
        std::stringstream ss;
        ss << std::to_string(x) << "," << std::to_string(y) << "," << std::to_string(scale) << ","
           << std::to_string(rotation);
        return ss.str();
    }

    static const Components TYPE = Components::TRANSFORM;
};

struct Sprite
{
    size_t render_itr;
    size_t sprite;

    // Debug strings
    std::string toString() const
    {
        std::stringstream ss;
        ss << std::to_string(render_itr) << "," << std::to_string(sprite);
        return ss.str();
    }

    static const Components TYPE = Components::SPRITE;
};

struct Movement
{
    float velocity;
    float acceleration;

    // Debug strings
    std::string toString() const
    {
        std::stringstream ss;
        ss << std::to_string(velocity) << "," << std::to_string(acceleration);
        return ss.str();
    }

    static const Components TYPE = Components::MOVEMENT;
};

#endif