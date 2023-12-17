#pragma once

namespace engine::graphics {

class Graphics {

public:

    static Graphics* get_instance();

    //uint64_t create_mesh_instance();

    //void set_mesh_instance_transform(uint64_t rid, glm::mat4 matrix);

    //void free_mesh_instance();

    Graphics();
    ~Graphics();

private:

    static Graphics* singleton;


};

}