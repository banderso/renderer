//
//  Renderer.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include <iostream>
#include "Renderer.h"
#include "matrix.h"
#include "glutil.h"

bar::Renderer::Renderer(RendererContext &context,
                        RendererContext &postProcessingContext) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDepthFunc(GL_LEQUAL);
  
  this->viewWidth = context.width;
  this->viewHeight = context.height;
  if (context.renderableContexts != nullptr) {
    const RenderableContext *contexts = context.renderableContexts;
    this->initializeRenderables(context.renderableContextCount, contexts);
  }
  
  if (postProcessingContext.renderableContexts != nullptr) {
    const uint32_t index = std::numeric_limits<uint32_t>::max();
    const bar::RenderableContext * const renderable = postProcessingContext.renderableContexts;
    this->buffer = new Framebuffer(postProcessingContext.width,
                                   postProcessingContext.height,
                                   new Mesh(index,
                                            renderable->vertices,
                                            renderable->normals,
                                            nullptr,
                                            renderable->elements,
                                            new Material(index, renderable->shader)));
  }
  
  this->pixels = nullptr;
}

bar::Renderer::~Renderer() {
  this->clearRenderables();
}

void bar::Renderer::resize(float width, float height) {
  fprintf(stdout, "(%f, %f) %f\n", width, height, width / height);
  this->viewWidth = width;
  this->viewHeight = height;
  
  this->buffer->resize(width, height);
  
  if (pixels != nullptr) {
    delete [] this->pixels;
  }
  
  const uint32_t widthui = static_cast<uint32_t>(width);
  const uint32_t heightui = static_cast<uint32_t>(height);
  const uint32_t size = 3 * widthui * heightui;
  this->pixels = new GLubyte[size];
  
  for (int i = 0; i < size; ++i) {
    this->pixels[i] = 0;
  }
}

void bar::Renderer::draw(float delta) const {
  
  
  this->clear();
  LogGLError(__LINE__, __FILE__);
  
  this->buffer->bind();
  LogGLError(__LINE__, __FILE__);
  if (!this->buffer->isComplete()) {
    fprintf(stdout, "framebuffer isn't complete after binding\n");
  }
  
  this->drawObjects(delta);
  LogGLError(__LINE__, __FILE__);
  
  
  /*
  const uint32_t width = static_cast<uint32_t>(this->viewWidth);
  const uint32_t height = static_cast<uint32_t>(this->viewHeight);
  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, this->pixels);
  
  int location = 0;
  unsigned int color = 0;
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      location = i * width + j;
      color = this->pixels[location] << 16 |
              this->pixels[location + 1] << 8 |
              this->pixels[location + 2];
      
      fprintf(stdout, "0x%06x ", color);
    }
    fprintf(stdout, "\n");
  }
  fprintf(stdout, "\n\n");
  //*/
  
  this->buffer->unbind();
  LogGLError(__LINE__, __FILE__);
  
  this->clear();
  this->buffer->draw(delta);
  
  LogGLError(__LINE__, __FILE__);
  
  glFlush();
//  delete [] pixels;
}

void bar::Renderer::clear() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0,
             static_cast<GLsizei>(viewWidth),
             static_cast<GLsizei>(viewHeight));
}

void bar::Renderer::drawObjects(float delta) const {
  //GLfloat modelView[16];
  GLfloat projection[16];
    
  mtxLoadPerspective(projection, 90, viewWidth / viewHeight, 1.0f, 100.0f);
    
  const Renderable * const renderables = this->renderables;
  const uint32_t count = this->renderableCount;
    
  for (uint32_t i = 0; i < count; i++) {
    const Renderable &renderable = renderables[i];
    Mesh *mesh = renderable.mesh;
    mesh->activate();
    mesh->update(delta);

    mesh->bindProjection(projection);
    mesh->bindModelView();

    glDrawElements(GL_TRIANGLES,
                   mesh->getElementCount(),
                   mesh->getElementType(),
                   nullptr);
    LogGLError(__LINE__, __FILE__);
  }
}

void bar::Renderer::initializeRenderables(uint32_t count,
                                          const RenderableContext *contexts) {
  Renderable * const renderables = new Renderable[count];
  for (uint32_t i = 0; i < count; i++) {
    Renderable &renderable = renderables[i];
    const RenderableContext &context = contexts[i];
     
    renderable.mesh = new Mesh(i,
                               context.vertices,
                               context.normals,
                               nullptr,
                               context.elements,
                               new Material(i, context.shader));
  }
    
  bar::Vec3 location = {-1.5f, 0.0f, -3.0f};
//  bar::Vec3 location = {0.0f, 0.0f, -2.0f};
  bar::Vec3 axis = {0.0f, 1.0f, 0.0f};
    
  renderables[0].mesh->setLocation(location);
  renderables[0].mesh->setAxis(axis);
//  renderables[0].mesh->setRotation(45.0f);
  
  location.x = 1.5f;
  renderables[1].mesh->setLocation(location);

  axis.y = -axis.y;
  renderables[1].mesh->setAxis(axis);
//  renderables[1].mesh->setRotation(-45.0f);
  
  this->renderables = renderables;
  this->renderableCount = count;
}

void bar::Renderer::clearRenderables() {
  Renderable * renderables = this->renderables;
    
  delete[] renderables;
  this->renderableCount = 0;
  this->renderables = NULL;
}

