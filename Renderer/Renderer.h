// -*- mode: C++; -*-
//
//  Renderer.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__Renderer__
#define __Renderer__Renderer__

#include <OpenGL/gl3.h>

#include "Framebuffer.h"
#include "Shader.h"
#include "Mesh.h"

namespace bar {
    
struct RenderableContext {
  //
  MeshAttribute *vertices;
  MeshAttribute *normals;
  //MesshAttribure *textCoords;
  MeshAttribute *elements;
  //
  //char *textureFilename;
  // Full path
  Shader *shader;
};
    
struct RendererContext {
  float width;
  float height;
  uint32_t renderableContextCount;
  RenderableContext *renderableContexts;
};
    
struct Renderable {
  Mesh *mesh;
};
    
class Renderer {
 public:
  Renderer(RendererContext &context,
           RendererContext &postProcessingContext);
  virtual ~Renderer();
  void draw(float delta) const;
  void resize(float width, float height);
 private:
  void clear() const;
  void drawObjects(float delta) const;
        
  void initializeRenderables(uint32_t count, const RenderableContext *contexts);
        
  void clearRenderables();
        
  uint32_t renderableCount;
  Renderable *renderables;
  
  Framebuffer *buffer;
  GLubyte *pixels;
  
  GLfloat viewWidth;
  GLfloat viewHeight;
};

} // namespace bar

#endif /* defined(__Renderer__Renderer__) */
