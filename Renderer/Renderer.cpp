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
  
  this->initializeFrambuffer();
}

bar::Renderer::~Renderer() {
  this->clearRenderables();
}

void bar::Renderer::resize(float width, float height) {
  this->viewWidth = width;
  this->viewHeight = height;
  
  this->clearFramebuffer();
  this->initializeFrambuffer();
}

void bar::Renderer::draw(float delta) const {
  this->clear();
  this->activateFramebuffer();
  this->drawObjects(delta);
  this->deactivateFramebuffer();

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
    
    glGetError();

    mesh->bindProjection(projection);
    mesh->bindModelView();

    glDrawElements(GL_TRIANGLES,
                   mesh->getElementCount(),
                   mesh->getElementType(),
                   nullptr);
  }
  glFlush();
}

void bar::Renderer::drawFramebuffer() const {
  
}

void bar::Renderer::activateFramebuffer() const {
  glBindFramebuffer(GL_FRAMEBUFFER, this->buffer.fbo);
}

void bar::Renderer::deactivateFramebuffer() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

void bar::Renderer::initializeFrambuffer() {
  this->buffer = Framebuffer();
  glGenFramebuffers(1, &this->buffer.fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, this->buffer.fbo);
  
  this->initializeTexture();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->buffer.colorBuffer, 0);
  
  this->initializeRenderBuffer();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->buffer.depthStencilBuffer);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
  GLfloat vertices[] = {
    -1.0f,  1.0f,  1.0f, // 0
    -1.0f,  1.0f, -1.0f, // 1
    1.0f,  1.0f, -1.0f,  // 2
    1.0f,  1.0f,  1.0f,  // 3
    
    -1.0f, -1.0f,  1.0f, // 4
    -1.0f, -1.0f, -1.0f, // 5
    1.0f, -1.0f, -1.0f,  // 6
    1.0f, -1.0f,  1.0f,  // 7
  };
  
  GLfloat normal_vals[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
  };
  
  GLuint element_vals[] = {
//    0, 1, 2,  0, 2, 3, // top
    0, 3, 4,  4, 3, 7, // front
//    7, 3, 6,  6, 3, 2, // right
//    6, 7, 5,  5, 7, 4, // bottom
//    6, 2, 5,  5, 2, 1, // back
//    1, 4, 0,  1, 5, 4  // left
  };
  
  bar::MeshAttribute positions = {
    static_cast<GLvoid *>(vertices),
    GL_FLOAT,
    3,
    sizeof(vertices)
  };
  bar::MeshAttribute normals = {
    static_cast<GLvoid *>(normal_vals),
    GL_FLOAT,
    3,
    sizeof(normal_vals)
  };
  bar::MeshAttribute elements = {
    static_cast<GLvoid *>(element_vals),
    GL_UNSIGNED_INT,
    1,
    sizeof(element_vals)
  };
  
  this->buffer.mesh = new Mesh(0, &positions, &normals, nullptr, &elements, nullptr);
}

void bar::Renderer::clearFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, this->buffer.fbo);
  
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
  this->clearRenderBuffer();
  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
  this->clearTexture();
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &this->buffer.fbo);
  this->buffer.fbo = 0;
}

void bar::Renderer::initializeTexture() {
  glGenTextures(1, &this->buffer.colorBuffer);
  glBindTexture(GL_TEXTURE_2D, this->buffer.colorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, this->viewWidth, this->viewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void bar::Renderer::clearTexture() {
  glDeleteTextures(1, &this->buffer.colorBuffer);
  this->buffer.colorBuffer = 0;
}

void bar::Renderer::initializeRenderBuffer() {
  glGenRenderbuffers(1, &this->buffer.depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, this->buffer.depthStencilBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->viewWidth, this->viewHeight);
}

void bar::Renderer::clearRenderBuffer() {
  glDeleteRenderbuffers(1, &this->buffer.depthStencilBuffer);
  this->buffer.depthStencilBuffer = 0;
}


