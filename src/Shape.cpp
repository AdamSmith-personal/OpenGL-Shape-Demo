#include "../Header Files/Shape.h"
#include <iostream>

// Private Methods
std::vector<GLfloat> Shape::readVertices(const char *verticesPath)
{
    std::ifstream file(verticesPath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file " + std::string(verticesPath));
    }
    std::vector<GLfloat> emptyVertices;
    GLfloat value;
    while (file >> value)
    {
        emptyVertices.emplace_back(value);
    }
    file.close();
    return emptyVertices;
}

std::vector<GLuint> Shape::readIndices(const char *indicesPath)
{
    std::ifstream file(indicesPath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file " + std::string(indicesPath));
    }
    std::vector<GLuint> emptyIndices;
    GLuint value;
    while (file >> value)
    {
        emptyIndices.emplace_back(value);
    }
    file.close();
    return emptyIndices;
}

// Public Methods
Shape::Shape(const char *verticesPath, const char *indicesPath)
{
    vertices = readVertices(verticesPath);
    indices = readIndices(indicesPath);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,6 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE,6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    indexCount = indices.size();
}


void Shape::Draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Shape::Delete()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

std::vector<GLfloat> Shape::getVertices()
{
    return vertices;
}

std::vector<GLuint> Shape::getIndices()
{
    return indices;
}

GLsizeiptr Shape::getVerticesSize()
{
    return vertices.size();
}
GLsizeiptr Shape::getIndicesSize()
{
    return indices.size();
}
GLsizeiptr Shape::getVerticesSizeInBytes()
{
    return vertices.size() * sizeof(GLfloat);
}

GLsizeiptr Shape::getIndicesSizeInBytes()
{
    return indices.size() * sizeof(GLuint);
}

