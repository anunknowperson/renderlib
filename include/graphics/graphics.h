#pragma once

#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine::graphics
{

class Graphics
{
public:
    static Graphics *getInstance();

    std::uint64_t create_mesh_instance();

    void set_mesh_instance_transform(std::uint64_t rid, glm::mat4 matrix);

    void free_mesh_instance(std::uint64_t rid);

    ~Graphics();

private:
    /*! \brief ctor */
    Graphics();

    std::map<std::uint64_t, int> meshes;
};

}