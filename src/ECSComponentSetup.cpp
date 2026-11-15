#include "ECSRegistry.hpp"

#include "ComponentsDef.hpp"

void EcsRegistry::init()
{
    static ComponentStorage<Transform> gTransforms;
    static ComponentStorage<Sprite> gSprites;
    static ComponentStorage<Movement> gMovements;

    add(&gTransforms);
    add(&gSprites);
    add(&gMovements);

    mInitialized = true;
}
