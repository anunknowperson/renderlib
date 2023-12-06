#include "scene/ParentComponent.h"
#include "core/logging.h"

struct ParentSystem {
    flecs::query<Parent> ParentQuery;
    flecs::query<Child> ChildQuery;
    flecs::query<PreviousParent> PreviousParentQuery;

    int FindChildIndex(const Child &children, const flecs::entity &entity)
    {
        for (size_t i = 0; i != children.Child.size(); i++) {
            if (children.Child[i] == entity) {
                return i;
            }
        }
        LOGE("Could not find child index");
        return -1;
    }

    void RemoveChildFromParent(flecs::world& world, flecs::entity parentEntity, flecs::entity childEntity) {
        if (!parentEntity.has<Child>()) {
            LOGW("Trying to remove a child from a parent that does not have a Child component");
            return;
        }
        auto children = parentEntity.get_mut<Child>();
        int childIndex = -1;
        for (size_t i = 0; i != children->Child.size(); ++i) {
            if (children->Child[i] == childEntity) {
                childIndex = i;
                break;
            }
        }
        if (childIndex != -1) {
            children->Child.erase(children->Child.begin() + childIndex);
        }
        if (children->Child.size() == 0) {
            parentEntity.remove<Child>();
        }
    }
};

struct GatherChangedParents {

};
