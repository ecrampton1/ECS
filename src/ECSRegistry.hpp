#ifndef ECS_REGISTRY_HPP_
#define ECS_REGISTRY_HPP_

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

class EcsRegistry
{

  public:
    static EcsRegistry& get()
    {
        static EcsRegistry reg;
        return reg;
    }

    // move this into user defined file
    void init();

    template <class T>
    void add(ComponentStorage<T>* store)
    {
        mStorageList.at(T::type) = store;
    }

    template <class T>
    ComponentStorage<T>& getStorage()
    {
        return *static_cast<ComponentStorage<T>*>(mStorageList.at(T::type));
    }

    Entity createEntity()
    {
        auto e = mEntityManager.createEntity();
        return e;
    }

    void removeEntity(Entity e)
    {
        mEntityManager.removeEntity(e);
        for (auto* list : mStorageList)
        {
            list->remove(e);
        }
    }

    template <class T, typename... Args>
    T& addComponent(Entity e, Args&&... args)
    {
        auto& c_store = getStorage<T>();
        auto& c = c_store.add(e, T{ std::forward<Args>(args)... });
        mEntityManager.setEntity(e, T::type);
        return c;
    }

    template <class T>
    T& getComponent(Entity e)
    {
        return getStorage<T>().get(e);
    }

    void printSignature(Entity e) { mEntityManager.printSignature(e); }

  private:
    EcsRegistry() = default;

    EntityManager mEntityManager;
    std::array<ComponentBase*, Components::MAX_COMPONENTS> mStorageList;
};

#endif