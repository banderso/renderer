//
//  shaderutil.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/28/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef Renderer_shaderutil_h
#define Renderer_shaderutil_h

#include <OpenGL/gl3.h>

namespace BAR {

    GLchar * GetShaderCompileStatus(GLuint shader);
    GLuint CreateShader(GLenum shaderType, const char *filename);
    void DeleteShader(GLuint shader);
    
    GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader);
    void DeleteProgram(GLuint program);
    
    void PrintProgramAttributes(GLuint program);
    void PrintProgramUniforms(GLuint program);
}


#endif
