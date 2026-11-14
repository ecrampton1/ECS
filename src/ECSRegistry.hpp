#ifndef ECS_REGISTRY_HPP_
#define ECS_REGISTRY_HPP_

#include "ECS.hpp"

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