

#include <string>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


class Shader {
    private:
    std::string m_FilePath;
    unsigned int m_RendererID;

    public:
    Shader (const std::string& filepath);
    ~Shader();
    void Bind();
    void UnBind();

    void SetUniform4f(const std::string& name,float v0,float v1, float v2, float v3);

    private:
    ShaderProgramSource ParseShader(const std::string& filepath)
    unsigned int CreateShader(const std::string& vertexShader,const std:: string& fragmentShader);
    unsigned int compileshader (unsigned int type, const std::string& source);
    unsigned int GetUniformLocation(const std::string& name);
};