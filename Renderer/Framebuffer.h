//  -*- mode: C++; -*-
//  
//  Framebuffer.h
//  Renderer
//
//  Created by Benjamin Anderson on 3/5/15.
//  Copyright (c) 2015 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__Framebuffer__
#define __Renderer__Framebuffer__

#include <OpenGL/gl3.h>

#include "Shader.h"
#include "Mesh.h"

namespace bar {
  
class Framebuffer {
 public:
  Framebuffer(float width, float height, Mesh *mesh);
  virtual ~Framebuffer();
  
  void bind();
  void unbind();
  
  void activate() const;
  void draw(float delta) const;
  void resize(float width, float height);
  
  bool isComplete() const;
  
 private:
  void initColorBuffer();
  void initRenderBuffer();
  
  void clear();
  void clearColorBuffer();
  void clearRenderBuffer();
  
  bool isBound;
  GLuint width;
  GLuint height;
  
  GLuint fbo;
  GLuint colorBuffer;
  GLuint depthStencilBuffer;
  
  Mesh *mesh;
};
  
}; // namespace bar

#endif /* defined(__Renderer__Framebuffer__) */
