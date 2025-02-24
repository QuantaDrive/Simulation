//
// Created by stan on 14/02/25.
//

#include "Mesh.h"

#include <cstdio>
#include <cstring>
#include <map>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "Init.h"
#include "Visualization.h"

using namespace simulation;

bool Mesh::loadMaterial(const std::string& materialFilename, std::map<std::string, glm::vec3>& materialMap)
{
    FILE* materialFile = fopen(materialFilename.c_str(), "r");
    if (!materialFile) {
        printf("Unable to open material file: %s\n", materialFilename.c_str());
        return false;
    }
    char materialName[128];
    char lineHeader[128];
    while (fscanf(materialFile, "%s", lineHeader) == 1) {
        if (strcmp(lineHeader, "newmtl") == 0) {
            fscanf(materialFile, "%s\n", materialName);
        } else if (strcmp(lineHeader, "Kd") == 0) {
            glm::vec3 material;
            fscanf(materialFile, "%f %f %f\n", &material.x, &material.y, &material.z);
            materialMap[materialName] = material;
        } else {
            char buffer[1000];
            fgets(buffer, 1000, materialFile);
        }
    }
    fclose(materialFile);
    return true;
}

void Mesh::loadObj(const std::string& meshFilename)
{
    FILE* meshFile = fopen(meshFilename.c_str(), "r");
    if (!meshFile) {
        printf("Unable to open mesh file: %s\n", meshFilename.c_str());
        vertices.clear();
        return;
    }

    std::map<std::string, glm::vec3> materialMap;

    std::vector<unsigned int> vertexIndices;
    std::vector<unsigned int> normalIndices;
    std::vector<glm::vec3> tempVertices;
    std::vector<glm::vec3> tempNormals;

    char materialName[128];
    char lineHeader[128];
    while (fscanf(meshFile, "%s", lineHeader) == 1) {
        if (strcmp(lineHeader, "v") == 0) {
            glm::vec3 vertex;
            fscanf(meshFile, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            tempVertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(meshFile, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            tempNormals.push_back(normal);
        } else if (strcmp(lineHeader, "mtllib") == 0) {
            std::string directory;
            if (const size_t pos = meshFilename.find_last_of('/'); pos != std::string::npos)
                directory = meshFilename.substr(0, pos + 1);
            char materialFilename[128];
            fscanf(meshFile, "%s\n", materialFilename);
            std::string materialFilePath = directory + materialFilename;
            if (!loadMaterial(materialFilePath, materialMap)) {
                fclose(meshFile);
                return;
            }
        } else if (strcmp(lineHeader, "usemtl") == 0) {
            fscanf(meshFile, "%s\n", materialName);
        } else if (strcmp(lineHeader, "f") == 0) {
            unsigned int vertexIndex[3], normalIndex[3];
            char line[256];
            fgets(line, 256, meshFile);
            int matches = sscanf(line, "%d//%d %d//%d %d//%d\n", &vertexIndex[0],
                                 &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2],
                                 &normalIndex[2]);

            if (matches != 6) {
                matches = sscanf(line, "%d/%*d/%d %d/%*d/%d %d/%*d/%d\n", &vertexIndex[0],
                                 &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2],
                                 &normalIndex[2]);
            }
            if (matches != 6) {
                fclose(meshFile);
                return;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
            colors.push_back(materialMap[materialName]);
            colors.push_back(materialMap[materialName]);
            colors.push_back(materialMap[materialName]);
        } else {
            char buffer[1000];
            fgets(buffer, 1000, meshFile);
        }
    }

    vertices.reserve(vertexIndices.size());
    normals.reserve(vertexIndices.size());
    for (unsigned int i = 0; i < vertexIndices.size(); i++) {
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalIndices[i];

        const glm::vec3& vertex = tempVertices[vertexIndex - 1];
        const glm::vec3& normal = tempNormals[normalIndex - 1];

        vertices.push_back(vertex);
        normals.push_back(normal);
    }
    fclose(meshFile);
}

void Mesh::fillBuffers()
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(glm::vec3), &this->vertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer( // Specify how the vertex data is formatted
        0,                  // Attribute location (match shader layout)
        3,                  // Number of components per vertex (x, y, z)
        GL_FLOAT,           // Data type
        GL_FALSE,           // Normalized? (usually false)
        0,                  // Stride (0 means tightly packed)
        (void*)0            // Offset into the buffer (0 means start at the beginning)
    );
    glEnableVertexAttribArray(0); // Enable the vertex attribute

    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(glm::vec3), &this->normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
        1,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(GL_ARRAY_BUFFER, this->colors.size() * sizeof(glm::vec3), &this->colors[0], GL_STATIC_DRAW);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh::Mesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &normal_buffer);
    glGenBuffers(1, &color_buffer);
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals): Mesh()
{
    this->vertices = vertices;
    this->normals = normals;
    fillBuffers();
}

Mesh::Mesh(std::string meshFilename): Mesh()
{
    loadObj(meshFilename);
    fillBuffers();
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &normal_buffer);
    glDeleteBuffers(1, &color_buffer);
    glDeleteVertexArrays(1, &VAO);
}

glm::mat4 Mesh::getTransformationMatrix()
{
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translation_vec);
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), rotation_vec.x, glm::vec3(1.0f, 0.0f, 0.0f));
    rotation = glm::rotate(rotation, rotation_vec.y, glm::vec3(0.0f, 1.0f, 0.0f));
    rotation = glm::rotate(rotation, rotation_vec.z, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale_vec);
    return translationMatrix * rotation * scaleMatrix;
}

void Mesh::translate(const glm::vec3& translation, const bool relative)
{
    if (relative)
        translation_vec += translation;
    else
        translation_vec = translation;
}

void Mesh::rotate(glm::vec3 rotation, const bool relative, const bool isDegree)
{
    if (isDegree)
        rotation = glm::radians(rotation);
    if (relative)
        rotation_vec += rotation;
    else
        rotation_vec = rotation;
}

void Mesh::scale(const glm::vec3& scale, const bool relative)
{
    if (relative)
        scale_vec *= scale;
    else
        scale_vec = scale;
}

void Mesh::render()
{
    glm::mat4 ModelMatrix = getTransformationMatrix();
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glUniformMatrix4fv(MvpMatrixID, 1, GL_FALSE, &MVP[0][0]);
    renderBare();
}

void Mesh::renderBare() const
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    glBindVertexArray(0);
}
