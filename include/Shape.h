#ifndef SHAPE_H
#define SHAPE_H

#include "glad/glad.h"
#include <vector>
#include <fstream>

class Shape
{
    private:
        GLuint VAO, VBO, EBO;
        GLsizeiptr indexCount;
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;

        std::vector<GLfloat> readVertices(const char *verticesPath);
        std::vector<GLuint> readIndices(const char *indicesPath);
    public:
        Shape(const char *verticesPath, const char *indicesPath);
        void Draw();
        void Delete();


        std::vector<GLfloat> getVertices();
        std::vector<GLuint> getIndices();

        GLsizeiptr getVerticesSize();
        GLsizeiptr getIndicesSize();
        GLsizeiptr getVerticesSizeInBytes();
        GLsizeiptr getIndicesSizeInBytes();
        char* getName();

};
#endif