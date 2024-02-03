#include <iostream>

#include "core/Logging.h"
#include "scene/ParentSystem.h"
#include "scene/MeshSystem.h"
#include "scene/TransformSystem.h"
#include "graphics/Graphics.h"

#include "graphics/vulkan/vk_engine.h"


int main()
{

	VulkanEngine engine;

	engine.init();

	engine.run();

	engine.cleanup();

	return 0;

}