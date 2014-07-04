//
//  Material.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/29/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include "Material.h"

BAR::Material::Material()
    : key_(0)
    , shader_(nullptr) {}

BAR::Material::Material(uint32_t key, const Shader *shader)
    : key_(key)
    , shader_(shader) {}

BAR::Material::~Material() {
  delete shader_;
}

void BAR::Material::use() const {
  this->shader_->activate();
}

void BAR::Material::bindUniform(const char *name, GLfloat *data) const {
  glUniformMatrix4fv(shader_->getUniformLocation(name), 1, GL_FALSE, data);
}



