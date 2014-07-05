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

bar::Renderer::Renderer(RendererContext &context) {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
    
  this->viewWidth = context.width;
  this->viewHeight = context.height;
  if (context.renderableContexts != nullptr) {
    const RenderableContext *contexts = context.renderableContexts;
    this->initializeRenderables(context.renderableContextCount, contexts);
  }
}

bar::Renderer::~Renderer() {
  this->clearRenderables();
}

void bar::Renderer::resize(float width, float height) {
  this->viewWidth = width;
  this->viewHeight = height;
}

void bar::Renderer::draw(float delta) const {
  this->clear();
  this->drawObjects(delta);
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
    mesh->update(delta);
        
    glGetError();

    mesh->activate();
    mesh->bindProjection(projection);
    mesh->bindModelView();

    glDrawElements(GL_TRIANGLES,
                   mesh->getElementCount(),
                   mesh->getElementType(),
                   nullptr);
  }
  glFlush();
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
    
  bar::Vec3 location = {-1.5f, 0.0f, -5.0f};
  bar::Vec3 axis = {0.0f, 1.0f, 0.0f};
    
  renderables[0].mesh->setLocation(location);
  renderables[0].mesh->setAxis(axis);
  //renderables[0].mesh->setRotation(45.0f);
    
  location.x = 1.5f;
  renderables[1].mesh->setLocation(location);

  axis.y = -axis.y;
  renderables[1].mesh->setAxis(axis);
  //renderables[1].mesh->setRotation(-45.0f);
    
  this->renderables = renderables;
  this->renderableCount = count;
}

void bar::Renderer::clearRenderables() {
  Renderable * renderables = this->renderables;
    
  delete[] renderables;
  this->renderableCount = 0;
  this->renderables = NULL;
}
