//
//  Texture.cpp
//  Renderer
//
//  Created by Ben Anderson on 3/9/15.
//  Copyright (c) 2015 Benjamin Anderson. All rights reserved.
//

#include "Texture.h"

#include <cstdlib>

// TextureParameters
////////////////////////////////////////////////////////////////////////////////

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

// Texture
////////////////////////////////////////////////////////////////////////////////

bar::Texture::Texture(TextureContext &context) {
  GLenum target;
  GLsizei width = static_cast<GLsizei>(context.width);
  GLsizei height = static_cast<GLsizei>(context.height);
  GLsizei depth = static_cast<GLsizei>(context.depth);
  GLint internalFormat;
  GLenum format;
  GLenum type;
  
  if (TextureType::COLOR == context.type) {
    internalFormat = GL_RGBA;
    format = GL_BGRA;
  } else if (TextureType::DEPTH == context.type) {
    internalFormat = GL_DEPTH_COMPONENT;
    format = GL_DEPTH_COMPONENT;
  } else if (TextureType::DEPTH_STENCIL == context.type) {
    internalFormat = GL_DEPTH_STENCIL;
    format = GL_DEPTH_STENCIL;
  } else {
    fprintf(stderr, "Unknown texture type.\n");
    std::exit(EXIT_FAILURE);
  }
  
  if (TextureDataType::UBYTE == context.dataType) {
    type = GL_UNSIGNED_BYTE;
  } else if (TextureDataType::BYTE == context.dataType) {
    type = GL_BYTE;
  } else if (TextureDataType::UINT == context.dataType) {
    type = GL_UNSIGNED_INT_8_8_8_8_REV;
  } else if (TextureDataType::INT == context.dataType) {
    type = GL_INT;
  } else if (TextureDataType::FLOAT == context.dataType) {
    type = GL_FLOAT;
  } else {
    fprintf(stderr, "Unknown texture data type.\n");
    std::exit(EXIT_FAILURE);
  }
  
  glGenTextures(1, &this->name);
  
  if (TextureDimension::ONE == context.dimension) {
    target = GL_TEXTURE_1D;
    glBindTexture(target, this->name);
    glTexImage1D(target, 0, internalFormat, width, 0, format, type, context.data);
  } else if (TextureDimension::TWO == context.dimension) {
    target = GL_TEXTURE_2D;
    glBindTexture(target, this->name);
    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, context.data);
  } else if (TextureDimension::THREE == context.dimension) {
    target = GL_TEXTURE_3D;
    glBindTexture(target, this->name);
    glTexImage3D(target, 0, internalFormat, width, height, depth, 0, format, type, context.data);
  }
  
  glBindTexture(target, 0);
  this->target = target;
}

bar::Texture::~Texture() {
  glDeleteTextures(1, &this->name);
}

void bar::Texture::bind(GLuint textureUnit) const {
  glActiveTexture(textureUnit);
  glBindTexture(this->target, this->name);
}

void bar::Texture::unbind(GLuint textureUnit) const {
  glActiveTexture(textureUnit);
  glBindTexture(this->target, 0);
}

GLuint bar::Texture::getName() const {
  return this->name;
}
