//
//  Shader.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/28/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include "Shader.h"
#include "shaderutil.h"
#include "fileutil.h"

#include <stdio.h>

BAR::Shader::Shader()
: vertexShaderFilename(nullptr)
, fragmentShaderFilename(nullptr) {}

BAR::Shader::Shader(const char *vertexShaderFilename,
                    const char *fragmentShaderFilename)
: vertexShaderFilename(vertexShaderFilename)
, fragmentShaderFilename(fragmentShaderFilename)
, vertexShader(0)
, fragmentShader(0)
, program(0) {
    if (this->vertexShaderFilename == nullptr ||
        this->fragmentShaderFilename == nullptr) return;
    
    this->vertexShader = BAR::CreateShader(GL_VERTEX_SHADER,
                                           this->vertexShaderFilename);
    this->fragmentShader = BAR::CreateShader(GL_FRAGMENT_SHADER,
                                             this->fragmentShaderFilename);
    this->program = BAR::CreateProgram(this->vertexShader, this->fragmentShader);
    
}

BAR::Shader::~Shader() {
    if (this->vertexShaderFilename == nullptr ||
        this->fragmentShaderFilename == nullptr) return;
    
    BAR::DeleteShader(this->vertexShader);
    BAR::DeleteShader(this->fragmentShader);
    BAR::DeleteProgram(this->program);
    
    delete [] this->vertexShaderFilename;
    delete [] this->fragmentShaderFilename;
}

GLint BAR::Shader::getUniformLocation(const char *uniformName) const {
    if (this->program) {
        return glGetUniformLocation(this->program, uniformName);
    } else {
        return -1;
    }
}

void BAR::Shader::activate() const {
    glUseProgram(this->program);
}

void BAR::Shader::listAttributes() const {
    BAR::PrintProgramAttributes(this->program);
}

void BAR::Shader::listUniforms() const {
    BAR::PrintProgramUniforms(this->program);
}

