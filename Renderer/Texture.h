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
  Texture(GLuint width, GLuint height, const void *data);
  virtual ~Texture();
  
  void bind(GLenum textureUnit) const;
  void unbind(GLenum textureUnit) const;
  
  GLuint getName() const;
  
 private:
  GLuint name;
};

}

#endif /* defined(__Renderer__Texture__) */
