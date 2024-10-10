#include "graphics/vulkan/pipelines.h"



void Pipelines::init(VkDevice device, VkDescriptorSetLayout singleImageDescriptorLayout, AllocatedImage drawImage) {

	_device = device;
	_singleImageDescriptorLayout = singleImageDescriptorLayout;
	_drawImage = drawImage;


	init_mesh_pipeline();
	init_triangle_pipeline();
}


void Pipelines::init_mesh_pipeline()
{
	VkShaderModule triangleFragShader;
	if (!vkutil::load_shader_module("./shaders/tex_image.frag.spv", _device, &triangleFragShader))
	{
		fmt::println("Error when building the triangle fragment shader module");
	}
	else
	{
		fmt::println("Triangle fragment shader successfully loaded");
	}

	VkShaderModule triangleVertexShader;
	if (!vkutil::load_shader_module("./shaders/colored_triangle_mesh.vert.spv", _device, &triangleVertexShader))
	{
		fmt::println("Error when building the triangle vertex shader module");
	}
	else
	{
		fmt::println("Triangle vertex shader successfully loaded");
	}

	VkPushConstantRange bufferRange{};
	bufferRange.offset = 0;
	bufferRange.size = sizeof(GPUDrawPushConstants);
	bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
	pipeline_layout_info.pPushConstantRanges = &bufferRange;
	pipeline_layout_info.pushConstantRangeCount = 1;
	pipeline_layout_info.pSetLayouts = &_singleImageDescriptorLayout;
	pipeline_layout_info.setLayoutCount = 1;
	VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &meshPipelineLayout));

	PipelineBuilder pipelineBuilder;

	//use the triangle layout we created
	pipelineBuilder._pipelineLayout = meshPipelineLayout;
	//connecting the vertex and pixel shaders to the pipeline
	pipelineBuilder.set_shaders(triangleVertexShader, triangleFragShader);
	//it will draw triangles
	pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	//filled triangles
	pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
	//no backface culling
	pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	//no multisampling
	pipelineBuilder.set_multisampling_none();
	//no blending
	pipelineBuilder.disable_blending();


	//pipelineBuilder.disable_depthtest();
	pipelineBuilder.enable_depthtest(true, VK_COMPARE_OP_GREATER);


	//connect the image format we will draw into, from draw image
	pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
	pipelineBuilder.set_depth_format(VK_FORMAT_UNDEFINED);

	//finally build the pipeline
	meshPipeline = pipelineBuilder.build_pipeline(_device);

	//clean structures
	vkDestroyShaderModule(_device, triangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleVertexShader, nullptr);

}

void Pipelines::init_triangle_pipeline()
{

	VkShaderModule triangleFragShader;
	if (!vkutil::load_shader_module("./shaders/colored_triangle.frag.spv", _device, &triangleFragShader))
	{
		fmt::println("Error when building the triangle fragment shader module");
	}
	else
	{
		fmt::println("Triangle fragment shader successfully loaded");
	}

	VkShaderModule triangleVertexShader;
	if (!vkutil::load_shader_module("./shaders/colored_triangle.vert.spv", _device, &triangleVertexShader))
	{
		fmt::println("Error when building the triangle vertex shader module");
	}
	else
	{
		fmt::println("Triangle vertex shader successfully loaded");
	}

	//build the pipeline layout that controls the inputs/outputs of the shader
	//we are not using descriptor sets or other systems yet, so no need to use anything other than empty default
	VkPipelineLayoutCreateInfo pipeline_layout_info = vkinit::pipeline_layout_create_info();
	VK_CHECK(vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &trianglePipelineLayout));

	PipelineBuilder pipelineBuilder;

	//use the triangle layout we created
	pipelineBuilder._pipelineLayout = trianglePipelineLayout;
	//connecting the vertex and pixel shaders to the pipeline
	pipelineBuilder.set_shaders(triangleVertexShader, triangleFragShader);
	//it will draw triangles
	pipelineBuilder.set_input_topology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	//filled triangles
	pipelineBuilder.set_polygon_mode(VK_POLYGON_MODE_FILL);
	//no backface culling
	pipelineBuilder.set_cull_mode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
	//no multisampling
	pipelineBuilder.set_multisampling_none();
	//no blending
	pipelineBuilder.disable_blending();
	//no depth testing
	pipelineBuilder.disable_depthtest();

	//connect the image format we will draw into, from draw image
	pipelineBuilder.set_color_attachment_format(_drawImage.imageFormat);
	pipelineBuilder.set_depth_format(VK_FORMAT_UNDEFINED);

	//finally build the pipeline
	trianglePipeline = pipelineBuilder.build_pipeline(_device);

	//clean structures
	vkDestroyShaderModule(_device, triangleFragShader, nullptr);
	vkDestroyShaderModule(_device, triangleVertexShader, nullptr);

}

void Pipelines::destroy() {
	vkDestroyPipelineLayout(_device, trianglePipelineLayout, nullptr);
	vkDestroyPipeline(_device, trianglePipeline, nullptr);

	vkDestroyPipelineLayout(_device, meshPipelineLayout, nullptr);
	vkDestroyPipeline(_device, meshPipeline, nullptr);
}