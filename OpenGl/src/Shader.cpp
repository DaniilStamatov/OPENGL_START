#include "Shader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Renderer.h"




Shader::Shader(const std::string& filename) : m_Filepath(filename), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filename);
    m_RendererID = CreateShader(source.VertexSourse, source.FragmentSource);
}

Shader::~Shader()
{
    GlCall(glDeleteProgram(m_RendererID));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);//ifstream - input file stream
    std::string line;
    enum ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}
unsigned int Shader::CompileShader(const std::string& source, unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}


unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vxShader = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fsShader = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vxShader);
    glAttachShader(program, fsShader);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vxShader);
    glDeleteShader(fsShader);

    return program;
}

void Shader::Bind() const
{
    GlCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GlCall(glUseProgram(0));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
    GlCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
    GlCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) 
{
    GlCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}




int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCashe.find(name) != m_UniformLocationCashe.end())
        return m_UniformLocationCashe[name];
    GlCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
    {
        std::cout << "Warning:" << name << std::endl;
    }
   
    m_UniformLocationCashe[name] = location;
    return location;
}