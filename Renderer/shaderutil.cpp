//
//  shaderutil.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/28/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include "shaderutil.h"
#include "fileutil.h"

#include <stdio.h>

GLchar *BAR::GetShaderCompileStatus(GLuint shader) {
    GLint compileStatus = GL_TRUE;
    GLint logLength = 0;
    GLchar *buffer = nullptr;
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        
        buffer = new GLchar[logLength + 1];
        glGetShaderInfoLog(shader, logLength, nullptr, buffer);
        
        return buffer;
    }
    return buffer;
}

GLuint BAR::CreateShader(GLenum shaderType, const char *filename) {
    GLchar *infoBuffer = nullptr;
    GLuint shader = 0;
    const char * shaderSrc = BAR::ReadFile(filename);
    if (shaderSrc == nullptr) return shader;
    
    shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);
    
    infoBuffer = BAR::GetShaderCompileStatus(shader);
    if (infoBuffer != nullptr) {
        fprintf(stderr, "Shader compile error: %s\n", infoBuffer);
        delete[] infoBuffer;
        shader = 0;
    }
    
    delete[] shaderSrc;
    return shader;
}

void BAR::DeleteShader(GLuint shader) {
    if (shader) {
        glDeleteShader(shader);
    }
}

GLuint BAR::CreateProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = 0;
    if (vertexShader != 0 && fragmentShader != 0) {
        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        
        GLint linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE) {
            fprintf(stderr, "Unable to link shader program.\n");
        }
    }
    return program;
}

void BAR::DeleteProgram(GLuint program) {
    if (program) {
        glDeleteProgram(program);
    }
}

void BAR::PrintProgramAttributes(GLuint program) {
    GLint count = 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &count);
    if (count == 0) return; // no active attributes
    
    GLint attrib_size = 0;
    GLenum type = 0;
    GLsizei buff_size = 0;
    GLsizei used_size = 0;
    
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &buff_size);
    if (buff_size == 0) return; // no active attributes
    
    fprintf(stdout, "Shader attributes:\n");
    GLchar *name = new GLchar[buff_size];
    for (GLuint i = 0; i < count; i++) {
        glGetActiveAttrib(program, i, buff_size, &used_size, &attrib_size, &type, name);
        
        fprintf(stdout, "%s @ %d\n", name, glGetAttribLocation(program, name));
    }
    delete[] name;
    fprintf(stdout, "\n");
}

void BAR::PrintProgramUniforms(GLuint program) {
    GLint count = 0;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);
    if (count == 0) return; // no active uniforms
    
    GLint uniform_size = 0;
    GLenum type = 0;
    GLsizei buff_size = 0;
    GLsizei used_size = 0;
    
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &buff_size);
    if (buff_size == 0) return; // not active attributes
    
    fprintf(stdout, "Shader uniforms:\n");
    GLchar *name = new GLchar[buff_size];
    for (GLuint i = 0; i < count; i++) {
        glGetActiveUniform(program, i, buff_size, &used_size, &uniform_size, &type, name);
        
        fprintf(stdout, "%s @ %d\n", name, glGetUniformLocation(program, name));
    }
    delete[] name;
    fprintf(stdout, "\n");
}


