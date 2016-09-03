#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include<fstream>
#include <string>

#include "graphics_headers.h"

class Shader
{
  public:
    Shader();
    ~Shader();
    bool Initialize();
    void Enable();
    bool AddShader(GLenum ShaderType); 
    bool Finalize();
    GLint GetUniformLocation(const char* pUniformName);

  private:
    GLuint m_shaderProg;    
    std::vector<GLuint> m_shaderObjList;

    // private member functions

    bool loadShaderSource( const std::string& fileName, 
                                 std::string& shaderSrcStr );
};

#endif  /* SHADER_H */
