#pragma once

#include <vector>
#include "math/vector3.h"
#include "textureImage.h"

namespace graphics
{
    class Shape
    {
    public:
        friend class Renderer;

    protected:
        std::vector<float> vertices;

        std::vector<unsigned int> polygonIndices;
        std::vector<unsigned int> frameIndices;

        unsigned int polygonVAO;
        unsigned int frameVAO;
        unsigned int textureID;

    public:
        Shape() :polygonVAO{}, frameVAO{}, textureID {} {}
        virtual ~Shape() {}

        void SetupPolygonAndFrameVAOs();

        virtual void GenerateShapeVertices(float) = 0;
        virtual void GenerateShapeVertices(math::Vector3) {}

        unsigned GetTextureID() const { return textureID; }

        void SetTextureID(unsigned newTextureID) { textureID=newTextureID; }
        void SetTextureID(TextureID textureID_) { textureID = textureID_; }
    };

    class Box : public Shape
    {
    public:
        Box();
        void GenerateShapeVertices(float extents)override final {
            GenerateShapeVertices({ extents, extents, extents });
        }
        void GenerateShapeVertices(math::Vector3)override final;
    };

    class Sphere : public Shape
    {
    public:
        static constexpr int SECTOR_CNT = 72;
        static constexpr int STACK_CNT = 24;

    public:
        Sphere();
        void GenerateShapeVertices(float)override final;
        void GenerateIndices();
    };
}
