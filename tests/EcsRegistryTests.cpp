#include <gtest/gtest.h>

#include "TestSupport.hpp"
#include "ComponentsDef.hpp"

TEST_F(InitializedRegistryTest, CreatesEntity)
{
    Entity entity = EcsRegistry::get().createEntity();

    EXPECT_EQ(0, entity.getId());
    EXPECT_EQ(0, entity.getVersion());
    EXPECT_FALSE(EcsRegistry::get().hasComponent<Transform>(entity));
}

TEST_F(InitializedRegistryTest, AddsMultipleComponentTypes)
{
    EcsRegistry& registry = EcsRegistry::get();
    Entity entity = registry.createEntity();

    registry.addComponent<Transform>(entity, Transform{ 1.0f, 2.0f, 3.0f, 4.0f });
    registry.addComponent<Movement>(entity, Movement{ 5.0f, 6.0f });

    EXPECT_EQ(2.0f, registry.getComponent<Transform>(entity).y);
    EXPECT_EQ(6.0f, registry.getComponent<Movement>(entity).acceleration);
    EXPECT_TRUE(registry.hasComponent<Transform>(entity));
    EXPECT_TRUE(registry.hasComponent<Movement>(entity));
    EXPECT_FALSE(registry.hasComponent<Sprite>(entity));
}

TEST_F(InitializedRegistryTest, RemovesEntityFromEveryStorage)
{
    EcsRegistry& registry = EcsRegistry::get();
    Entity entity = registry.createEntity();
    registry.addComponent<Transform>(entity, Transform{ 1.0f, 2.0f, 3.0f, 4.0f });
    registry.addComponent<Movement>(entity, Movement{ 5.0f, 6.0f });

    registry.removeEntity(entity);

    EXPECT_FALSE(registry.getStorage<Transform>().contains(entity));
    EXPECT_FALSE(registry.getStorage<Movement>().contains(entity));
    EXPECT_FALSE(registry.hasComponent<Transform>(entity));
    EXPECT_FALSE(registry.hasComponent<Movement>(entity));

    Entity recycled = registry.createEntity();
    EXPECT_EQ(entity.getId(), recycled.getId());
    EXPECT_EQ(entity.getVersion() + 1, recycled.getVersion());
}

TEST_F(InitializedRegistryTest, RemovesSingleComponentAndClearsSignature)
{
    EcsRegistry& registry = EcsRegistry::get();
    Entity entity = registry.createEntity();
    registry.addComponent<Transform>(entity, Transform{ 1.0f, 2.0f, 3.0f, 4.0f });
    registry.addComponent<Movement>(entity, Movement{ 5.0f, 6.0f });

    registry.removeComponent<Transform>(entity);

    EXPECT_FALSE(registry.getStorage<Transform>().contains(entity));
    EXPECT_FALSE(registry.hasComponent<Transform>(entity));
    EXPECT_TRUE(registry.hasComponent<Movement>(entity));
    EXPECT_EQ(6.0f, registry.getComponent<Movement>(entity).acceleration);
}

TEST_F(InitializedRegistryTest, ResetClearsStoragesAndEntityState)
{
    EcsRegistry& registry = EcsRegistry::get();
    Entity entity = registry.createEntity();
    registry.addComponent<Transform>(entity, Transform{ 1.0f, 2.0f, 3.0f, 4.0f });
    ASSERT_EQ(1, registry.getStorage<Transform>().size());

    registry.resetForTesting();
    registry.init();

    EXPECT_EQ(0, registry.getStorage<Transform>().size());
    Entity next = registry.createEntity();
    EXPECT_EQ(0, next.getId());
    EXPECT_EQ(0, next.getVersion());
}

#ifndef NDEBUG
TEST_F(UninitializedRegistryTest, CreateEntityAssertsBeforeInit)
{
    EXPECT_DEATH(EcsRegistry::get().createEntity(), "Component Storage not initialized");
}

TEST_F(UninitializedRegistryTest, GetStorageAssertsBeforeInit)
{
    EXPECT_DEATH((void)EcsRegistry::get().getStorage<Transform>(), "Component Storage not initialized");
}

TEST_F(UninitializedRegistryTest, AddComponentAssertsBeforeInit)
{
    EXPECT_DEATH(EcsRegistry::get().addComponent<Transform>(Entity{}, Transform{ 1.0f, 2.0f, 3.0f, 4.0f }),
                 "Component Storage not initialized");
}
#endif
