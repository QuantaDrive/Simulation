//
// Created by stan on 14/02/25.
//

#ifndef MESH_H
#define MESH_H
#include <vector>
#include <glm/glm.hpp>

class Mesh {
    std::vector<glm::vec3> out_vertices;
    std::vector<glm::vec2> out_uvs;
    std::vector<glm::vec3> out_normals;

public:
    Mesh() = default;

    Mesh(const std::vector<glm::vec3>& out_vertices, const std::vector<glm::vec2>& out_uvs,
         const std::vector<glm::vec3>& out_normals)
        : out_vertices(out_vertices),
          out_uvs(out_uvs),
          out_normals(out_normals)
    {}

    static Mesh loadObj(const char * filename);
};

#endif //MESH_H
