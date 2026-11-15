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

    void resetForTesting()
    {
        for (auto* list : mStorageList)
        {
            if (list != nullptr)
                list->clear();
        }
        mStorageList = {};
        mEntityManager = EntityManager{};
        mInitialized = false;
    }

    // move this into user defined file
    void init();

    template <class T>
    void add(ComponentStorage<T>* store)
    {
        mStorageList.at(T::TYPE) = store;
    }

    template <class T>
    ComponentStorage<T>& getStorage()
    {
        assert(mInitialized == true && "Component Storage not initialized");
        return *static_cast<ComponentStorage<T>*>(mStorageList.at(T::TYPE));
    }

    Entity createEntity()
    {
        assert(mInitialized == true && "Component Storage not initialized");
        auto e = mEntityManager.createEntity();
        return e;
    }

    void removeEntity(Entity e)
    {
        assert(mInitialized == true && "Component Storage not initialized");
        mEntityManager.removeEntity(e);
        for (auto* list : mStorageList)
        {
            if (list != nullptr)
                list->remove(e);
        }
    }

    template <class T>
    void removeComponent(Entity e)
    {
        assert(mInitialized == true && "Component Storage not initialized");
        auto& c_store = getStorage<T>();
        c_store.remove(e);
    }

    template <class T, typename... Args>
    T& addComponent(Entity e, Args&&... args)
    {
        assert(mInitialized == true && "Component Storage not initialized");
        auto& c_store = getStorage<T>();
        auto& c = c_store.add(e, T{ std::forward<Args>(args)... });
        mEntityManager.setEntity(e, T::TYPE);
        return c;
    }

    template <class T>
    T& getComponent(Entity e)
    {
        return getStorage<T>().get(e);
    }

    template <class T>
    bool hasComponent(Entity e) const
    {
        assert(mInitialized == true && "Component Storage not initialized");
        return mEntityManager.queryEntity(e, T::TYPE);
    }

    void printSignature(Entity e) { mEntityManager.printSignature(e); }

  private:
    EcsRegistry() = default;

    bool mInitialized{ false };
    EntityManager mEntityManager;
    std::array<ComponentBase*, Components::MAX_COMPONENTS> mStorageList{};
};

#endif
