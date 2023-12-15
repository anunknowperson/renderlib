#include "scene/ParentSystem.h"

namespace {
void updateParent(flecs::entity e, Parent& p)
{
    if (!p.parent.is_alive() || !p.parent.has<Child>()) {
        e.destruct();
    } else {
        if (p.parent.has<Child>()) {
            auto *child = p.parent.get_mut<Child>();
            child->children.push_back(e);
        }
    }
}

void updateChild(flecs::entity e, Child& c)
{
    auto& children = c.children;
    auto newEnd = std::remove_if(
            children.begin(),
            children.end(),
            [](const flecs::entity& child) { return (!child.is_alive() || !child.has<Parent>()); }
    );
    children.erase(newEnd, children.end());
}

void removeChild(flecs::entity e, Child& c)
{
    if (c.children.empty()) {
        e.remove<Child>();
    }
}

void changeParent(flecs::entity e, Parent& p, PreviousParent& pp)
{
    if (pp.parent.is_alive() and pp.parent.has<Child>()) {
        auto *child = pp.parent.get_mut<Child>();
        auto newEnd = std::remove_if(
                child->children.begin(),
                child->children.end(),
                [e](const flecs::entity& child) { return child == e; }
        );
        child->children.erase(newEnd, child->children.end());
    }
    if (p.parent.is_alive()) {
        auto *child = p.parent.get_mut<Child>();
        child->children.push_back(e);
    }
    p.parent = pp.parent;
    e.remove<PreviousParent>();
}
}

void setRelation(flecs::entity child, flecs::entity parent)
{
    if (!parent.is_alive()) {
#ifndef NDEBUG
        LOGW("Trying to set entity %s as parent, but it is not alive", parent.name().c_str());
#endif
        return;
    }
    if (!child.is_alive()) {
#ifndef NDEBUG
        LOGWF("Trying to set entity {} as child, but it is not alive", child.name().c_str());
#endif
        return;
    }
    if (child == parent) {
#ifndef NDEBUG
        LOGWF("Trying to set entity {} as its own parent", child.name().c_str());
#endif
        return;
    }

    if (child.has<Parent>())
    {
        if (child.get<Parent>()->parent == parent) {
            return;
        } else {
            child.set<PreviousParent>({child.get<Parent>()->parent});
        }
    }
    if (parent.has<Child>())
    {
        auto &children = parent.get_mut<Child>()->children;
        if (std::find(children.begin(),
                      children.end(),
                      child) != children.end()) {
            return;
        }
        children.push_back(child);
    }
    child.set<Parent>({parent});
}

// make docs for overloads
void removeRelation(flecs::entity child)
{
    if (!child.is_alive() || !child.has<Parent>()) {
#ifndef NDEBUG
        LOGWF("Trying to remove parent from entity {}, but it is not alive or has no parent", child.name().c_str());
#endif
        return;
    }
    child.remove<Parent>();
}

// make docs for overloads
void removeRelation(flecs::entity removing_child, flecs::entity parent) {
    if (!parent.is_alive() || !parent.has<Child>() || !parent.get<Child>()->children.empty()) {
#ifndef NDEBUG
        LOGWF("Trying to remove entity {} from parent {}, but parent is not alive or has no children",
              removing_child.name().c_str(), parent.name().c_str());
#endif
    }
    if (!removing_child.is_alive() || !removing_child.has<Parent>()) {
#ifndef NDEBUG
        LOGWF("Trying to remove entity {} from parent {}, but child is not alive or has no parent",
              removing_child.name().c_str(), parent.name().c_str());
#endif
    }
    auto &children = parent.get_mut<Child>()->children;
    auto newEnd = std::remove_if(
            children.begin(),
            children.end(),
            [removing_child](const flecs::entity& child) { return child == removing_child; }
    );
}

// fix definition
void HierarchySystem(flecs::world &world)
{
    world.system<Parent>()
            .kind(flecs::OnAdd)
            .each(updateParent);

    world.system<Child>()
            .kind(flecs::OnSet)
            .each(updateChild);

    world.system<Child>()
            .kind(flecs::OnRemove)
            .each(removeChild);

    world.system<Parent, PreviousParent>()
            .kind(flecs::OnAdd)
            .each(changeParent);
}