#ifndef ECS_TEST_SUPPORT_HPP_
#define ECS_TEST_SUPPORT_HPP_

#include <gtest/gtest.h>

#include "ECSRegistry.hpp"

class InitializedRegistryTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        EcsRegistry::get().resetForTesting();
        EcsRegistry::get().init();
    }

    void TearDown() override { EcsRegistry::get().resetForTesting(); }
};

class UninitializedRegistryTest : public ::testing::Test
{
  protected:
    void SetUp() override { EcsRegistry::get().resetForTesting(); }

    void TearDown() override { EcsRegistry::get().resetForTesting(); }
};

#endif
