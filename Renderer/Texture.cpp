//
//  Texture.cpp
//  Renderer
//
//  Created by Ben Anderson on 3/9/15.
//  Copyright (c) 2015 Benjamin Anderson. All rights reserved.
//

#include "Texture.h"

bar::TextureParameters::TextureParameters() {
  glGenSamplers(1, &this->name);
}

bar::TextureParameters::~TextureParameters() {
  glDeleteSamplers(1, &this->name);
}

void bar::TextureParameters::bind(GLenum textureUnit) const {
  glBindSampler(textureUnit, this->name);
}

void bar::TextureParameters::unbind(GLenum textureUnit) const {
  glBindSampler(textureUnit, 0);
}

void bar::TextureParameters::setParameter(GLenum name, GLint value) const {
  glSamplerParameteri(this->name, name, value);
}

bar::Texture::Texture(GLuint width, GLuint height, const void *data) {
  glGenTextures(1, &this->name);
  glBindTexture(GL_TEXTURE_2D, this->name);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bar::Texture::~Texture() {
  glDeleteTextures(1, &this->name);
}

void bar::Texture::bind(GLuint textureUnit) const {
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_2D, this->name);
}

void bar::Texture::unbind(GLuint textureUnit) const {
  glActiveTexture(textureUnit);
  glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint bar::Texture::getName() const {
  return this->name;
}
