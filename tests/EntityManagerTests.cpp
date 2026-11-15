#include <gtest/gtest.h>

#include <cstdint>

#include "ECS.hpp"

TEST(EntityTest, ExposesIdVersionAndRawValue)
{
    EntityManager manager;
    Entity entity = manager.createEntity();

    EXPECT_EQ(0, entity.getId());
    EXPECT_EQ(0, entity.getVersion());
    EXPECT_EQ(0, entity.getValue());
}

TEST(EntityTest, EqualityIncludesVersion)
{
    EntityManager manager;
    Entity stale = manager.createEntity();
    manager.removeEntity(stale);
    Entity recycled = manager.createEntity();

    ASSERT_EQ(stale.getId(), recycled.getId());
    ASSERT_NE(stale.getVersion(), recycled.getVersion());
    EXPECT_FALSE(stale == recycled);
    EXPECT_TRUE(recycled == recycled);
}

TEST(EntityManagerTest, CreatesSequentialEntities)
{
    EntityManager manager;

    Entity first = manager.createEntity();
    Entity second = manager.createEntity();

    EXPECT_EQ(0, first.getId());
    EXPECT_EQ(1, second.getId());
    EXPECT_EQ(2, manager.getNumberOfEntities());
}

TEST(EntityManagerTest, RemoveDeactivatesEntityAndUpdatesCount)
{
    EntityManager manager;
    Entity first = manager.createEntity();
    Entity second = manager.createEntity();
    manager.setEntity(second, Components::TRANSFORM);

    manager.removeEntity(first);

    EXPECT_EQ(1, manager.getNumberOfEntities());
    EXPECT_FALSE(manager.queryEntity(first, Components::TRANSFORM));
    EXPECT_TRUE(manager.queryEntity(second, Components::TRANSFORM));
}

TEST(EntityManagerTest, RecyclesRemovedIdWithNewVersion)
{
    EntityManager manager;
    Entity original = manager.createEntity();
    manager.removeEntity(original);

    Entity recycled = manager.createEntity();

    EXPECT_EQ(original.getId(), recycled.getId());
    EXPECT_EQ(original.getVersion() + 1, recycled.getVersion());
    EXPECT_EQ(1, manager.getNumberOfEntities());
}

TEST(EntityManagerTest, RemovingUnknownEntityIsNoOp)
{
    EntityManager manager;
    Entity unknown;

    manager.removeEntity(unknown);

    EXPECT_EQ(0, manager.getNumberOfEntities());
}

TEST(EntityManagerTest, QueryRequiresActiveEntityAndComponent)
{
    EntityManager manager;
    Entity entity = manager.createEntity();

    EXPECT_FALSE(manager.queryEntity(entity, Components::TRANSFORM));
    manager.setEntity(entity, Components::TRANSFORM);
    EXPECT_TRUE(manager.queryEntity(entity, Components::TRANSFORM));
    manager.removeEntity(entity);
    EXPECT_FALSE(manager.queryEntity(entity, Components::TRANSFORM));
}

TEST(EntityManagerTest, StaleHandleDoesNotRemoveCurrentGeneration)
{
    EntityManager manager;
    Entity stale = manager.createEntity();
    manager.removeEntity(stale);
    Entity current = manager.createEntity();
    manager.setEntity(current, Components::TRANSFORM);

    ASSERT_EQ(stale.getId(), current.getId());
    ASSERT_NE(stale.getVersion(), current.getVersion());
    manager.removeEntity(stale);

    EXPECT_TRUE(manager.queryEntity(current, Components::TRANSFORM));
    EXPECT_EQ(1, manager.getNumberOfEntities());
}

TEST(EntityManagerTest, DoubleRemoveDoesNotDuplicateFreeListEntry)
{
    EntityManager manager;
    Entity removed = manager.createEntity();
    manager.removeEntity(removed);
    manager.removeEntity(removed);

    EXPECT_EQ(0, manager.getNumberOfEntities());

    Entity first = manager.createEntity();
    Entity second = manager.createEntity();

    EXPECT_NE(first.getValue(), second.getValue());
    EXPECT_EQ(2, manager.getNumberOfEntities());
}

#ifndef NDEBUG
TEST(EntityManagerDeathTest, CreatingPastCapacityAsserts)
{
    EXPECT_DEATH(
        {
            EntityManager manager;
            for (Entity::EntityType i = 0; i < Entity::MAX_ENTITIES; ++i)
                manager.createEntity();
            manager.createEntity();
        },
        "Max Entries created");
}
#endif
