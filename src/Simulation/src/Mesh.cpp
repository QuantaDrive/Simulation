//
// Created by stan on 14/02/25.
//

#include "Mesh.h"

#include <cstdio>
#include <cstring>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "Init.h"
#include "Visualization.h"

using namespace simulation;

Mesh Mesh::loadObj(const char* filename)
{
    std::vector<unsigned int> vertexIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;


    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Impossible to open the mesh file!\n");
        getchar();
        return {};
    }

    while (true)
    {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if (strcmp(lineHeader, "v") == 0)
        {
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            temp_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%*d/%d %d/%*d/%d %d/%*d/%d\n", &vertexIndex[0],
                                 &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2],
                                 &normalIndex[2]);
            if (matches != 6)
            {
                fseek(file, -strlen(lineHeader) - 1, SEEK_CUR);
                matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0],
                                 &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2],
                                 &normalIndex[2]);
            }
            if (matches != 6)
            {
                printf("File can't be read by our parser.\n");
                fclose(file);
                return {};
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
        else
        {
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            fgets(stupidBuffer, 1000, file);
        }
    }

    // For each vertex of each triangle
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    for (unsigned int i = 0; i < vertexIndices.size(); i++)
    {
        // Get the indices of its attributes
        unsigned int vertexIndex = vertexIndices[i];
        unsigned int normalIndex = normalIndices[i];

        // Get the attributes thanks to the index
        const glm::vec3& vertex = temp_vertices[vertexIndex - 1];
        const glm::vec3& normal = temp_normals[normalIndex - 1];

        // Put the attributes in buffers
        vertices.push_back(vertex);
        normals.push_back(normal);
    }
    fclose(file);

    Mesh model = Mesh(vertices, normals);
    return model;
}

Mesh::Mesh()
{
    glGenVertexArrays(1, &VAO);
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices, const std::vector<glm::vec3>& normals)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    vertex_count = vertices.size();

    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
}

Mesh::~Mesh()
{
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &normal_buffer);
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
    const glm::mat4 ModelMatrix = getTransformationMatrix();
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    glUniformMatrix4fv(MvpMatrixID, 1, GL_FALSE, &MVP[0][0]);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(
        0,                  // attribute
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );

    glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    glDisableVertexAttribArray(0);
}
