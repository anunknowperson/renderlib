#pragma once

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>

namespace engine::graphics {

class Graphics {

public:

    static Graphics* get_instance();

    uint64_t create_mesh_instance();

    void set_mesh_instance_transform(uint64_t rid, glm::mat4 matrix);

    void free_mesh_instance(uint64_t rid);

    Graphics();
    ~Graphics();

private:

    static Graphics* singleton;

    std::map<uint64_t, int> meshes;

};

}