#pragma once

namespace engine::graphics {

class graphics {

public:

    static graphics* get_instance();

    //uint64_t create_mesh_instance();

    //void set_mesh_instance_transform(uint64_t rid, glm::mat4 matrix);

    //void free_mesh_instance();

    graphics();
    ~graphics();

private:

    static graphics* singleton;


};

}