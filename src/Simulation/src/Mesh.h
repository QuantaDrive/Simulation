//
// Created by stan on 14/02/25.
//

#ifndef MESH_H
#define MESH_H
#include <map>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace simulation
{
    class Mesh {
        GLuint VAO;
        std::vector<glm::vec3> vertices;
        GLuint vertex_buffer;
        std::vector<glm::vec3> normals;
        GLuint normal_buffer;
        std::vector<glm::vec3> colors;
        GLuint color_buffer;

        glm::vec3 translation_vec = glm::vec3(0.0f);
        glm::vec3 rotation_vec = glm::vec3(0.0f);
        glm::vec3 scale_vec = glm::vec3(1.0f);

        static bool loadMaterial(const std::string& materialFilename, std::map<std::string, glm::vec3>& materialMap);
        void loadObj(const std::string& meshFilename);
        void fillBuffers();

    public:
        Mesh();
        explicit Mesh(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals);
        explicit Mesh(std::string meshFilename);
        ~Mesh();

        glm::mat4 getTransformationMatrix();

        void translate(const glm::vec3& translation, bool relative = false);
        void rotate(glm::vec3 rotation, bool relative = false, bool isDegree = false);
        void scale(const glm::vec3& scale, bool relative = false);

        void render();
        void renderBare() const;
    };
} // simulation
#endif //MESH_H
