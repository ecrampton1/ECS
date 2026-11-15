#include <gtest/gtest.h>

#include <vector>

#include "ECS.hpp"
#include "ComponentsDef.hpp"

TEST(ComponentStorageTest, AddsAndRetrievesComponent)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity entity = manager.createEntity();

    Movement& added = storage.add(entity, Movement{ 1.0f, 2.0f });

    EXPECT_EQ(1, storage.size());
    EXPECT_TRUE(storage.contains(entity));
    EXPECT_EQ(1.0f, added.velocity);
    EXPECT_EQ(2.0f, storage.get(entity).acceleration);
}

TEST(ComponentStorageTest, InsertsComponentByMove)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity entity = manager.createEntity();
    Movement source{ 5.0f, 6.0f };

    Movement& inserted = storage.insert(entity, source);

    EXPECT_EQ(5.0f, inserted.velocity);
    EXPECT_EQ(6.0f, storage.get(entity).acceleration);
}

TEST(ComponentStorageTest, RemoveMissingComponentIsNoOp)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity entity = manager.createEntity();

    storage.remove(entity);

    EXPECT_EQ(0, storage.size());
    EXPECT_FALSE(storage.contains(entity));
}

TEST(ComponentStorageTest, RemovingMiddleEntryPreservesOtherMappings)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity first = manager.createEntity();
    Entity second = manager.createEntity();
    Entity third = manager.createEntity();
    storage.add(first, Movement{ 1.0f, 1.0f });
    storage.add(second, Movement{ 2.0f, 2.0f });
    storage.add(third, Movement{ 3.0f, 3.0f });

    storage.remove(first);

    EXPECT_EQ(2, storage.size());
    EXPECT_FALSE(storage.contains(first));
    EXPECT_EQ(2.0f, storage.get(second).velocity);
    EXPECT_EQ(3.0f, storage.get(third).velocity);
}

TEST(ComponentStorageTest, RemovesLastEntry)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity entity = manager.createEntity();
    storage.add(entity, Movement{ 1.0f, 2.0f });

    storage.remove(entity);

    EXPECT_EQ(0, storage.size());
    EXPECT_FALSE(storage.contains(entity));
}

TEST(ComponentStorageTest, ClearRemovesAllComponents)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity first = manager.createEntity();
    Entity second = manager.createEntity();
    storage.add(first, Movement{ 1.0f, 1.0f });
    storage.add(second, Movement{ 2.0f, 2.0f });

    storage.clear();

    EXPECT_EQ(0, storage.size());
    EXPECT_FALSE(storage.contains(first));
    EXPECT_FALSE(storage.contains(second));
}

TEST(ComponentStorageTest, AddingHundredthComponentSucceeds)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;

    for (int i = 0; i < 100; ++i)
        storage.add(manager.createEntity(), Movement{ static_cast<float>(i), 0.0f });

    EXPECT_EQ(100, storage.size());
}

TEST(ComponentStorageTest, AddingPastInitialCapacityKeepsLogicalSize)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;

    for (int i = 0; i < 101; ++i)
        storage.add(manager.createEntity(), Movement{ static_cast<float>(i), 0.0f });

    EXPECT_EQ(101, storage.size());
}

TEST(ComponentStorageTest, AddingPastInitialCapacityPreservesValues)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    std::vector<Entity> entities;

    for (int i = 0; i < 101; ++i)
    {
        Entity entity = manager.createEntity();
        entities.push_back(entity);
        storage.add(entity, Movement{ static_cast<float>(i), static_cast<float>(i + 1) });
    }

    for (int i = 0; i < 101; ++i)
    {
        EXPECT_TRUE(storage.contains(entities[i]));
        EXPECT_EQ(static_cast<float>(i), storage.get(entities[i]).velocity);
    }
}

TEST(ComponentStorageTest, RemovalAfterGrowthPreservesRemainingMappings)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    std::vector<Entity> entities;

    for (int i = 0; i < 101; ++i)
    {
        Entity entity = manager.createEntity();
        entities.push_back(entity);
        storage.add(entity, Movement{ static_cast<float>(i), 0.0f });
    }

    storage.remove(entities.front());

    EXPECT_EQ(100, storage.size());
    EXPECT_FALSE(storage.contains(entities.front()));
    for (std::size_t i = 1; i < entities.size(); ++i)
    {
        EXPECT_TRUE(storage.contains(entities[i]));
        EXPECT_EQ(static_cast<float>(i), storage.get(entities[i]).velocity);
    }
}

#ifndef NDEBUG
TEST(ComponentStorageDeathTest, AddingDuplicateComponentAsserts)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity entity = manager.createEntity();
    storage.add(entity, Movement{ 1.0f, 2.0f });

    EXPECT_DEATH(storage.add(entity, Movement{ 3.0f, 4.0f }), "Component added to existing entity");
}

TEST(ComponentStorageDeathTest, GettingMissingComponentAsserts)
{
    EntityManager manager;
    ComponentStorage<Movement> storage;
    Entity entity = manager.createEntity();

    EXPECT_DEATH(storage.get(entity), "Entity does not contain component");
}
#endif
