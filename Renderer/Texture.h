// -*- mode: C++; -*-
//  Texture.h
//  Renderer
//
//  Created by Ben Anderson on 3/9/15.
//  Copyright (c) 2015 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__Texture__
#define __Renderer__Texture__

#include <stdio.h>

#include <OpenGL/gl3.h>

namespace bar {
enum TextureDimension {
  ONE,
  TWO,
  THREE
};
  
enum TextureType {
  COLOR,
  DEPTH,
  DEPTH_STENCIL
};

enum TextureDataType {
  UBYTE,
  BYTE,
  UINT,
  INT,
  FLOAT
};
  
struct TextureContext {
  TextureDimension dimension;
  TextureType type;
  TextureDataType dataType;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
  void *data;
};
  
class TextureParameters {
 public:
  TextureParameters();
  virtual ~TextureParameters();
  
  void bind(GLenum textureUnit) const;
  void unbind(GLenum textureUnit) const;
  
  void setParameter(GLenum name, GLint value) const;
  
 private:
  GLuint name;
};

class Texture {
 public:
  Texture(TextureContext &context);
  virtual ~Texture();
  
  void bind(GLenum textureUnit) const;
  void unbind(GLenum textureUnit) const;
  
  GLuint getName() const;
  
 private:
  GLuint name;
  GLenum target;
};

}

#endif /* defined(__Renderer__Texture__) */
