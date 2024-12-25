layout(set = 0, binding = 0) uniform  SceneData{

    mat4 view;
    mat4 proj;
    mat4 viewproj;
    vec4 ambient_color;
    vec4 sunlight_direction; //w for sun power
    vec4 sunlight_color;
} scene_data;

layout(set = 1, binding = 0) uniform GLTFMaterialData{

    vec4 color_factors;
    vec4 metal_rough_factors;

} materialData;

layout(set = 1, binding = 1) uniform sampler2D colorTex;
layout(set = 1, binding = 2) uniform sampler2D metalRoughTex;