#include <gtest/gtest.h>

#include "flecs.h"
#include "scene/ParentSystem.h"

// Tests whether a child can be correctly assigned to a parent and later removed
TEST(ParentSystemTest, SetAndRemoveRelation) {
    flecs::world world;
    ParentSystem(world);  // Initialize the parent system in the ECS world

    flecs::entity parent = world.entity("Parent");
    flecs::entity child = world.entity("Child");

    // Initially, the child shouldn't have a parent, and the parent shouldn't
    // have children
    ASSERT_FALSE(child.has<Parent>());
    ASSERT_FALSE(parent.has<Child>());

    // Set the parent-child relationship
    setRelation(child, parent);

    // Now the child should have a parent, and the parent should have children
    ASSERT_TRUE(child.has<Parent>());
    ASSERT_TRUE(parent.has<Child>());
    EXPECT_EQ(child.get<Parent>()->parent, parent);

    // Remove the relationship and check if it's gone
    removeRelation(child);
    ASSERT_FALSE(child.has<Parent>());
    ASSERT_TRUE(parent.get<Child>()->children.empty());
}

// Tests if a specific child can be removed from a parent while keeping other
// children intact
TEST(ParentSystemTest, RemoveChildFromParent) {
    flecs::world world;
    ParentSystem(world);

    flecs::entity parent = world.entity("Parent");
    flecs::entity child1 = world.entity("Child1");
    flecs::entity child2 = world.entity("Child2");

    setRelation(child1, parent);
    setRelation(child2, parent);

    // Ensure both children are correctly assigned
    ASSERT_TRUE(parent.has<Child>());
    ASSERT_EQ(parent.get<Child>()->children.size(), 2);

    // Remove only child1 from the parent
    removeRelation(child1, parent);

    // child1 should no longer have a parent
    ASSERT_FALSE(child1.has<Parent>());

    // child2 should still be a child of the parent
    ASSERT_TRUE(child2.has<Parent>());
    ASSERT_EQ(parent.get<Child>()->children.size(), 1);
}

// Tests if changing a child's parent updates the previous parent's child list
// correctly
TEST(ParentSystemTest, ChangeParent) {
    flecs::world world;
    ParentSystem(world);

    flecs::entity oldParent = world.entity("OldParent");
    flecs::entity newParent = world.entity("NewParent");
    flecs::entity child = world.entity("Child");

    setRelation(child, oldParent);
    ASSERT_EQ(child.get<Parent>()->parent, oldParent);

    // Assign the child to a new parent
    setRelation(child, newParent);

    // The new parent should now be assigned
    ASSERT_EQ(child.get<Parent>()->parent, newParent);

    // The child should have been removed from the old parent's child list
    ASSERT_TRUE(oldParent.get<Child>()->children.empty());

    // The new parent should now have one child
    ASSERT_EQ(newParent.get<Child>()->children.size(), 1);
}

// Tests whether deleting a parent removes all its children
TEST(ParentSystemTest, DeleteParentRemovesChildren) {
    flecs::world world;
    ParentSystem(world);

    flecs::entity parent = world.entity("Parent");
    flecs::entity child = world.entity("Child");

    setRelation(child, parent);
    ASSERT_TRUE(child.has<Parent>());

    // Destroy the parent entity
    parent.destruct();

    // Progress the world to allow the system to process entity removal
    world.progress();

    // The child should also be deleted
    ASSERT_FALSE(child.is_alive());
}
