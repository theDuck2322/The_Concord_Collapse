#include <Shader.h>

namespace Az
{

    bool Shader::load(const char *vertexPath, const char *fragmentPath)
    {
        GLuint vertexShader = loadShader(vertexPath, GL_VERTEX_SHADER);
        GLuint fragmentShader = loadShader(fragmentPath, GL_FRAGMENT_SHADER);

        if (vertexShader == 0 || fragmentShader == 0)
        {
            return EXIT_FAILURE;
        }

        // Create the shader program
        programID = glCreateProgram();
        glAttachShader(programID, vertexShader);
        glAttachShader(programID, fragmentShader);
        if (linkProgram())
        {
#ifdef AZ_DEBUG
            std::cout << "Failed to link program" << std::endl;
#endif
            return EXIT_FAILURE;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return EXIT_SUCCESS;
    }

    GLuint Shader::loadShader(const char *path, GLenum shaderType)
    {
        std::ifstream shaderFile(path);
        if (!shaderFile.is_open())
        {
#ifdef AZ_DEBUG
            std::cerr << "Failed to open shader file: " << path << std::endl;
#endif
            return 0;
        }
#ifdef AZ_DEBUG
        std::cout << "Shader Loaded Successfully: " << path << std::endl;
#endif

        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        std::string shaderCode = shaderStream.str();
        const char *shaderCodeCStr = shaderCode.c_str();

        GLuint shader = glCreateShader(shaderType);
        if (compileShader(shader, shaderCodeCStr))
        {
            return EXIT_FAILURE;
        }

        return shader;
    }

    bool Shader::compileShader(GLuint shader, const char *source)
    {
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
#ifdef AZ_DEBUG
            std::cout << "Shader Compilation Failed: " << infoLog << std::endl;
#endif
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    bool Shader::linkProgram()
    {
        glLinkProgram(programID);

        GLint success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(programID, 512, nullptr, infoLog);
#ifdef AZ_DEBUG
            std::cout << "Program Linking Failed: " << infoLog << std::endl;
#endif
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    void Shader::use() const
    {
        glUseProgram(programID);
    }

    void Shader::Unbind() const
    {
        glUseProgram(0);
    }

    GLint Shader::getUniformLocation(const std::string &name) const
    {
        return glGetUniformLocation(programID, name.c_str());
    }

    void Shader::setUniform(const std::string &name, const glm::mat4 &matrix)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix)); // Use value_ptr
        }
    }

    void Shader::setUniform(const std::string &name, const glm::mat3 &matrix)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix)); // Use value_ptr
        }
    }

    void Shader::setUniform(const std::string &name, const glm::vec4 &vector)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform4fv(location, 1, glm::value_ptr(vector)); // Use value_ptr
        }
    }

    void Shader::setUniform(const std::string &name, const glm::vec3 &vector)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform3fv(location, 1, glm::value_ptr(vector)); // Use value_ptr
        }
    }

    void Shader::setUniform(const std::string &name, const glm::vec2 &vector)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform2fv(location, 1, glm::value_ptr(vector)); // Use value_ptr
        }
    }

    void Shader::setUniform(const std::string &name, float value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform1f(location, value);
        }
    }

    void Shader::setUniform(const std::string &name, int value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform1i(location, value);
        }
    }

    void Shader::setUniform(const std::string &name, bool value)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform1i(location, static_cast<int>(value)); // Convert bool to int (1 or 0)
        }
    }

    void Shader::setUniform(const std::string &name, const int *values, size_t count)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform1iv(location, static_cast<GLsizei>(count), values);
        }
    }

    void Shader::setUniform(const std::string &name, const float *values, size_t count)
    {
        GLint location = getUniformLocation(name);
        if (location != -1)
        {
            glUniform1fv(location, static_cast<GLsizei>(count), values);
        }
    }

}