#pragma once

#include <glad/glad.h> // Use glad for OpenGL function loading
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> // Include for glm::value_ptr

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Az
{
    class Shader
    {
    public:
        GLuint programID; // Shader program ID

        Shader() : programID(0) {}

        // Load and compile shaders, link the program
        bool load(const char *vertexPath, const char *fragmentPath);

        // Use the shader program
        void use() const;
        void Unbind() const;

        // Set uniforms for different types
        void setUniform(const std::string &name, const glm::mat4 &matrix);
        void setUniform(const std::string &name, const glm::mat3 &matrix);
        void setUniform(const std::string &name, const glm::vec4 &vector);
        void setUniform(const std::string &name, const glm::vec3 &vector);
        void setUniform(const std::string &name, const glm::vec2 &vector);
        void setUniform(const std::string &name, float value);
        void setUniform(const std::string &name, int value);
        void setUniform(const std::string &name, bool value);
        void setUniform(const std::string &name, const int *values, size_t count);   // Array of ints
        void setUniform(const std::string &name, const float *values, size_t count); // Array of floats

    private:
        GLuint loadShader(const char *path, GLenum shaderType);
        GLint getUniformLocation(const std::string &name) const;

        bool compileShader(GLuint shader, const char *source);
        bool linkProgram();
    };

}