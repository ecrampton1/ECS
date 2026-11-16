#include "inc/ECSRegistry.hpp"
#include "ComponentsDef.hpp"

int main()
{
    // init();
    // EntityManager& manager = EntityManager::getManager();
    // Entity e1 = manager.createEntity();

    auto& reg = EcsRegistry::get();
    reg.init();
    auto e1 = reg.createEntity();

    std::cout << "Entity Id:" << e1.getId() << "Version:" << e1.getVersion() << "Sig" << std::endl;
    reg.printSignature(e1);

    // auto& trans = reg.getStorage<Transform>();
    // trans.add(e1, Transform{ 1.0, 1.0, 1.0, 0.0 });
    auto& trans = reg.addComponent<Transform>(e1, Transform{ 1.0, 1.0, 1.0, 0.0 });
    std::cout << trans.toString() << std::endl;

    auto& trans_dup = reg.getComponent<Transform>(e1);
    std::cout << trans_dup.toString() << std::endl;

    reg.printSignature(e1);
}
