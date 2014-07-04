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

bar::Shader::Shader()
    : vertexShaderFilename(nullptr)
    , fragmentShaderFilename(nullptr) {}

bar::Shader::Shader(const char *vertexShaderFilename,
                    const char *fragmentShaderFilename)
    : vertexShaderFilename(vertexShaderFilename)
    , fragmentShaderFilename(fragmentShaderFilename)
    , vertexShader(0)
    , fragmentShader(0)
    , program(0) {
  if (this->vertexShaderFilename == nullptr ||
      this->fragmentShaderFilename == nullptr) return;
    
  this->vertexShader = bar::CreateShader(GL_VERTEX_SHADER,
                                         this->vertexShaderFilename);
  this->fragmentShader = bar::CreateShader(GL_FRAGMENT_SHADER,
                                           this->fragmentShaderFilename);
  this->program = bar::CreateProgram(this->vertexShader, this->fragmentShader);
    
}

bar::Shader::~Shader() {
  if (this->vertexShaderFilename == nullptr ||
      this->fragmentShaderFilename == nullptr) return;
    
  bar::DeleteShader(this->vertexShader);
  bar::DeleteShader(this->fragmentShader);
  bar::DeleteProgram(this->program);
    
  delete [] this->vertexShaderFilename;
  delete [] this->fragmentShaderFilename;
}

GLint bar::Shader::getUniformLocation(const char *uniformName) const {
  if (this->program) {
    return glGetUniformLocation(this->program, uniformName);
  } else {
    return -1;
  }
}

void bar::Shader::activate() const {
  glUseProgram(this->program);
}

void bar::Shader::listAttributes() const {
  bar::PrintProgramAttributes(this->program);
}

void bar::Shader::listUniforms() const {
  bar::PrintProgramUniforms(this->program);
}

