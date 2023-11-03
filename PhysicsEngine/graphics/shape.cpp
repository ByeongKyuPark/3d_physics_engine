#include <opengl/glad/glad.h>
#include "shape.h"
#include <cmath>
#include "math/mathConstants.h"

using namespace graphics;

//1.Generates a VAO and binds it.
//2.Generates a VBO(Vertex Buffer Object) and binds it.
//3.Loads the vertex data into the VBO.
//4.Generates an EBO(Element Buffer Object) and binds it.
//5.Loads the index data into the EBO.
//6.Sets up the vertex attribute pointer.
//7.Unbinds the VAO.
//note that since the steps for setting up both the polygonand frame VAOs are the same, the function can be shared between the two classes.
void Shape::SetupPolygonAndFrameVAOs()
{
    //1.polygon
    //VAO
    glGenVertexArrays(1, &polygonVAO);
    glBindVertexArray(polygonVAO);

    //VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices.size(),
        &vertices[0],
        GL_STATIC_DRAW
    );

    //(0) position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //(1) texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //EBO
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * polygonIndices.size(),
        &polygonIndices[0],
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);

    //2.frame
    //vao
    glGenVertexArrays(1, &frameVAO);
    glBindVertexArray(frameVAO);

    //vbo
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(float) * vertices.size(),
        &vertices[0],
        GL_STATIC_DRAW
    );
    //(0) position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //(1) texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //ebo
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(unsigned int) * frameIndices.size(),
        &frameIndices[0],
        GL_STATIC_DRAW
    );

    glBindVertexArray(0);
}

Box::Box()
{
    GenerateShapeVertices({ 0.5f, 0.5f, 0.5f });
    polygonIndices = {
        // Front face
        0, 1, 2,  0, 2, 3,
        // Back face
        4, 5, 6,  4, 6, 7,
        // Top face
        8, 9, 10,  8, 10, 11,
        // Bottom face
        12, 13, 14,  12, 14, 15,
        // Right face
        16, 17, 18,  16, 18, 19,
        // Left face
        20, 21, 22,  20, 22, 23
    };    
    frameIndices = {
        0, 1, 2, 3, 0,      // front
        5, 0, 3, 6, 5,      // L
        4, 5, 6, 7, 4,      // Rear
        1, 4, 7, 2, 1,      // R
    };
    SetupPolygonAndFrameVAOs();
}

void Box::GenerateShapeVertices(math::Vector3 extents)
{    
    vertices.clear();
    vertices = {
        // Front face
        -extents.x, -extents.y, extents.z,  0.0f, 0.0f,   // (0)
        extents.x, -extents.y, extents.z,   1.0f, 0.0f,   // (1)
        extents.x, extents.y, extents.z,    1.0f, 1.0f,   // (2)
        -extents.x, extents.y, extents.z,   0.0f, 1.0f,   // (3)

        // Back face
        extents.x, -extents.y, -extents.z, 0.0f, 0.0f,    // (4)
        -extents.x, -extents.y, -extents.z, 1.0f, 0.0f,   // (5)
        -extents.x, extents.y, -extents.z,  1.0f, 1.0f,   // (6)
        extents.x, extents.y, -extents.z,   0.0f, 1.0f,   // (7)

        // Top face
        -extents.x, extents.y, extents.z,   0.0f, 0.0f,   // (8)
        extents.x, extents.y, extents.z,    1.0f, 0.0f,   // (9)
        extents.x, extents.y, -extents.z,   1.0f, 1.0f,   // (10)
        -extents.x, extents.y, -extents.z,  0.0f, 1.0f,   // (11)

        // Bottom face
        -extents.x, -extents.y, -extents.z, 0.0f, 0.0f,   // (12)
        extents.x, -extents.y, -extents.z,  1.0f, 0.0f,   // (13)
        extents.x, -extents.y, extents.z,   1.0f, 1.0f,   // (14)
        -extents.x, -extents.y, extents.z,  0.0f, 1.0f,   // (15)

        // Right face
        extents.x, -extents.y, extents.z,   0.0f, 0.0f,   // (16)
        extents.x, -extents.y, -extents.z,  1.0f, 0.0f,   // (17)
        extents.x, extents.y, -extents.z,   1.0f, 1.0f,   // (18)
        extents.x, extents.y, extents.z,    0.0f, 1.0f,   // (19)

        // Left face
        -extents.x, -extents.y, -extents.z,  0.0f, 0.0f,  // (20)
        -extents.x, -extents.y, extents.z,  1.0f, 0.0f,   // (21)
        -extents.x, extents.y, extents.z,  1.0f, 1.0f,    // (22)
        -extents.x, extents.y, -extents.z,   0.0f, 1.0f,  // (23)
    };
}

Sphere::Sphere()
{
    GenerateShapeVertices(1.0f);
    GenerateIndices();
    SetupPolygonAndFrameVAOs();
}

//http://www.songho.ca/opengl/gl_sphere.html

void Sphere::GenerateShapeVertices(float radius)
{
    float x, y, z, xy, s,t;
    float sectorStep = 2 * math::PI / SECTOR_CNT;
    float stackStep = math::PI / STACK_CNT;
    float sectorAngle, stackAngle;

    vertices.clear();
    for(int i = 0; i <= STACK_CNT; ++i)
    {
        stackAngle = math::PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and collisionNormal, but different tex coords
        for(int j = 0; j <= SECTOR_CNT; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / SECTOR_CNT;
            t = (float)i / STACK_CNT;
            vertices.push_back(s);
            vertices.push_back(t);
        }
    }
}

void Sphere::GenerateIndices()
{
    int k1, k2;
    for(int i = 0; i < STACK_CNT; ++i)
    {
        k1 = i * (SECTOR_CNT + 1);     // beginning of current stack
        k2 = k1 + SECTOR_CNT + 1;      // beginning of next stack

        for(int j = 0; j < SECTOR_CNT; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if(i != 0)
            {
                polygonIndices.push_back(k1);
                polygonIndices.push_back(k2);
                polygonIndices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (STACK_CNT-1))
            {
                polygonIndices.push_back(k1 + 1);
                polygonIndices.push_back(k2);
                polygonIndices.push_back(k2 + 1);
            }

            // store indices for lines
            // vertical lines for all stacks, k1 => k2
        }
        frameIndices.push_back(k1);
        frameIndices.push_back(k2);
    }
}
