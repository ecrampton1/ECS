#ifndef ECS_HPP_
#define ECS_HPP_

#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <array>
#include <typeinfo>

enum Components
{
    TRANSFORM,
    SPRITE,
    MOVEMENT,
    MAX_COMPONENTS,
};

template <class T>
struct GetTypeName
{

    const char* name() { return typeid(T).name(); }
    operator const char*() { return name(); }
};

class Entity
{
  public:
    friend class EntityManager;
    using EntityType = uint32_t;
    using VersionType = uint16_t;
    using Signature = std::bitset<Components::MAX_COMPONENTS + 1>; // add 1 to specify active bit
    static constexpr std::bitset<Components::MAX_COMPONENTS + 1> ENTITY_ACTIVE{
        1 << Components::MAX_COMPONENTS
    };

    static constexpr EntityType ENTITY_MASK = 0xFFFF;
    static constexpr VersionType VERSION_MASK = 0xFFFF;
    static constexpr EntityType ENTITY_SHIFT = 0;
    static constexpr VersionType VERSION_SHIFT = 16;
    static constexpr EntityType VERSION_INCREMENT = 1 << VERSION_SHIFT;

    static constexpr EntityType MAX_ENTITIES = ENTITY_MASK;

    EntityType getId() const { return (mValue >> ENTITY_SHIFT) & ENTITY_MASK; }
    VersionType getVersion() const { return (mValue >> VERSION_SHIFT) & VERSION_MASK; }
    EntityType getValue() const { return mValue; }

    // return true if both id and version match
    bool operator==(const Entity& other) const { return mValue == other.mValue; }
    operator uint32_t() const { return mValue; }

    // Entity() = default;
    // Entity(Entity& e) = default;

  private:
    EntityType mValue{ ENTITY_MASK };
    void incrementVersion() { mValue = mValue + VERSION_INCREMENT; }
};

class EntityManager
{
  public:
    EntityManager() = default;
    Entity createEntity()
    {
        Entity e{};
        if (mAvailableEntities.empty())
        {
            assert((mNumberEntries < Entity::MAX_ENTITIES) && "Max Entries created");
            e.mValue = mNumberEntries++;
        }
        else
        {
            e = mAvailableEntities.back();
            mAvailableEntities.pop_back();
            e.incrementVersion();
        }
        mSignatures[e.getId()] = Entity::ENTITY_ACTIVE;
        return e;
    }

    Entity::EntityType getNumberOfEntities() { return mNumberEntries - mAvailableEntities.size(); }

    void removeEntity(Entity e)
    {
        if (e.getId() < mSignatures.size())
        {
            mSignatures[e.getId()] = 0x00;
            mAvailableEntities.push_back(e);
        }
    }

    bool queryEntity(Entity e, Components c) const
    {
        bool ret = false;
        if (e.getId() < mSignatures.size())
            ret = mSignatures[e.getId()].test(Components::MAX_COMPONENTS) && mSignatures[e.getId()].test(c);
        return ret;
    }

    void setEntity(Entity e, Components c)
    {
        assert(e.getId() < mSignatures.size() && "Entity ID outside of bounds");
        mSignatures[e.getId()] |= (1 << c);
    }

    void printSignature(Entity e)
    {
        assert(e.getId() < mSignatures.size() && "Entity ID outside of bounds");
        std::cout << mSignatures[e.getId()] << std::endl;
    }

  private:
    Entity::EntityType mNumberEntries{ 0 };
    std::vector<Entity> mAvailableEntities;
    std::array<Entity::Signature, Entity::MAX_ENTITIES> mSignatures{};
};

using ComponentIdx = uint16_t;

class ComponentBase
{
  public:
    virtual ~ComponentBase() = default;
    virtual void remove(Entity e) = 0;
    virtual void clear() = 0;
    virtual bool contains(Entity e) = 0;
};

template <typename Component>
class ComponentStorage : public ComponentBase
{

  public:
    ComponentStorage()
    {
        mComponents.reserve(INIT_SIZE);
        mEntities.reserve(INIT_SIZE);
    }

    template <typename... Args>
    Component& add(Entity e, Args&&... args)
    {
        assert(mEntityMap.find(e) == mEntityMap.end() && "Component added to existing entity");
        assert(mComponents.size() < MAX_SIZE && "Component size exceeded");

        if (mComponents.size() == mComponents.capacity())
        {
            mComponents.resize(mComponents.size() * SIZE_MULTIPLIER);
            mEntities.resize(mComponents.size() * SIZE_MULTIPLIER);
        }
        mEntityMap[e] = mComponents.size();
        mComponents.emplace_back(std::forward<Args>(args)...);
        mEntities.emplace_back(e);

        return mComponents.back();
    }

    Component& insert(Entity e, Component& c) { return add(e, std::move(c)); }

    Component& get(Entity e)
    {
        assert(contains(e) && "Entity does not contain component");
        return mComponents[mEntityMap[e]];
    }

    void remove(Entity e) override
    {
        if (contains(e))
        {
            auto c_id = mEntityMap[e];

            mComponents[c_id] = std::move(mComponents.back());

            mEntityMap[mEntities.back()] = c_id;
            mEntities[c_id] = mEntities.back();
            mEntityMap.erase(e);
            mComponents.pop_back();
            mEntities.pop_back();
        }
    }

    bool contains(Entity e) override
    {
        auto iter = mEntityMap.find(e);
        return (iter != mEntityMap.end());
    }

    void clear() override
    {
        mEntities.clear();
        mComponents.clear();
        mEntityMap.clear();
    }

    std::size_t size() const { return mComponents.size(); }

  private:
    static constexpr uint32_t INIT_SIZE = 100;                 ///< Initialize size of the vector
    static constexpr uint32_t SIZE_MULTIPLIER = 2;             //< Double the size of the vector
    static constexpr uint32_t MAX_SIZE = Entity::MAX_ENTITIES; ///< Max size a component vector can grow too.

    // stores the component and the version, version if component and index was deleted before
    std::vector<Component> mComponents;
    std::vector<Entity> mEntities;
    std::unordered_map<Entity::EntityType, ComponentIdx> mEntityMap;
};

/*class ComponentManager
{
  public:
    // create a map of possible components
    template <class T>
    void registerComponent()
    {
        // const char* name = typeid(Component).name();
        const char* name = GetTypeName<T>();
        assert(mComponentArrays.find(name) == mComponentArrays.end() && "Registering existing component");
        mComponentArrays[name] = std::make_shared<ComponentStorage<T>>();

        std::cout << typeid(T).name() << std::endl;
    }

    template <class T>
    T& insertComponent(Entity e, T c)
    {
        return getComponentArray<T>()->insert(e, c);
    }

    template <class T, typename... Args>
    T& addComponent(Entity e, Args&&... args)
    {
        return getComponentArray<T>()->add(e, std::forward<Args>(args)...);
    }

    template <class T>
    T& getComponent(Entity e)
    {
        return getComponentArray<T>()->get(e);
    }

    template <class T>
    void removeComponent(Entity e)
    {
        getComponentArray<T>()->remove(e);
    }

  private:
    std::unordered_map<const char*, std::shared_ptr<ComponentBase>> mComponentArrays;

    template <class T>
    std::shared_ptr<ComponentStorage<T>> getComponentArray()
    {
        const char* name = GetTypeName<T>();

        assert(mComponentArrays.find(name) != mComponentArrays.end() &&
               "Component not registered before use.");

        return std::static_pointer_cast<ComponentStorage<T>>(mComponentArrays[name]);
    }
}; */

#endif
