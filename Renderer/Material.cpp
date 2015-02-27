//
//  Material.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/29/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include "Material.h"

bar::Material::Material()
    : key_(0)
    , shader_(nullptr) {}

bar::Material::Material(uint32_t key, const Shader *shader)
    : key_(key)
    , shader_(shader) {}

bar::Material::~Material() {
  delete shader_;
}

void bar::Material::use() const {
  this->shader_->activate();
}

void bar::Material::bindUniform(const char *name, GLfloat data) const {
  glUniform1f(shader_->getUniformLocation(name), data);
}

void bar::Material::bindUniform(const char *name, GLfloat *data) const {
  glUniformMatrix4fv(shader_->getUniformLocation(name), 1, GL_FALSE, data);
}



