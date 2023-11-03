#pragma once

#include "glm/gtc/type_ptr.hpp"
#include <opengl/glad/glad.h>
#include <string>
#include <vector>

namespace graphics
{
    class Shader {
    public:
        //rule of 5 <-- for 'move assignment operator'
        Shader() :m_program{} {}
        Shader(const std::vector<std::pair<std::string, GLenum>>& shaderFiles);
        Shader(Shader&& other) noexcept;             
        Shader& operator=(Shader&& other) noexcept;  
        ~Shader();

        void Bind() const;
        void Unbind() const;

		void SetBool(const std::string& name, bool value) const;
		void SetInt(const std::string& name, int value) const;
		void SetFloat(const std::string& name, float value) const;
		void SetVec3(const std::string& name, glm::vec3 value) const;
		void SetMat4(const std::string& name, glm::mat4 value) const;

    private:
        std::string GetShaderFileContents(const std::string& filename);
        GLuint CreateShader(const std::string& source, unsigned int shaderType);

        GLuint m_program;
        std::vector<GLuint> m_shaders;
    };
}