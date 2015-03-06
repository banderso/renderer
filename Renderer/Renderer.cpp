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
    this->initializeFrambuffer(&postProcessingContext);
  }
}

bar::Renderer::~Renderer() {
  this->clearRenderables();
}

void bar::Renderer::resize(float width, float height) {
  fprintf(stdout, "(%f, %f) %f\n", width, height, width / height);
  this->viewWidth = width;
  this->viewHeight = height;
  
  this->clearFramebuffer();
  this->initializeFrambuffer(nullptr);
}

void bar::Renderer::draw(float delta) const {
  this->clear();
  this->activateFramebuffer();
  glEnable(GL_DEPTH_TEST);
  this->drawObjects(delta);
  this->deactivateFramebuffer();
  glDisable(GL_DEPTH_TEST);
  this->drawFramebuffer(delta);
  
  GLenum error = glGetError();
  while (error != GL_NO_ERROR) {
    fprintf(stdout, "Error: %s\n", GetGLErrorString(error));
    error = glGetError();
  }
  
  glFlush();
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
}

void bar::Renderer::drawFramebuffer(float delta) const {
  GLfloat projection[16];
  
//  GLfloat aspectRatio = (GLfloat)this->viewWidth / (GLfloat)this->viewHeight;
//  mtxLoadOrthographic(projection, -aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
  mtxLoadOrthographic(projection, -1.0, 1.0, -1.0f, 1.0f, -1.0f, 1.0f);
  
  Mesh *mesh = this->buffer.mesh;
  mesh->activate();
  mesh->update(delta);
  
  mesh->bindProjection(projection);
  //mesh->bindModelView();
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, this->buffer.colorBuffer);
  glUniform1i(this->buffer.framebufferId, 0);
  glDrawElements(GL_TRIANGLES, mesh->getElementCount(), mesh->getElementType(), nullptr);
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

void bar::Renderer::initializeFrambuffer(RendererContext *context) {
  if (context != nullptr) {
    this->buffer = Framebuffer();
    fprintf(stdout, "Initializing the framebuffer mesh.\n");
    uint32_t index = std::numeric_limits<uint32_t>::max();
    bar::RenderableContext *renderable = context->renderableContexts;
    this->buffer.mesh = new Mesh(index,
                                 renderable->vertices,
                                 renderable->normals,
                                 nullptr,
                                 renderable->elements,
                                 new Material(index, renderable->shader));
    GLuint framebufferId = renderable->shader->getUniformLocation("framebuffer");
    this->buffer.framebufferId = framebufferId;
  }
  
  glGenFramebuffers(1, &this->buffer.fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, this->buffer.fbo);
  
  this->initializeTexture();
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->buffer.colorBuffer, 0);
  
  this->initializeRenderBuffer();
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->buffer.depthStencilBuffer);
  
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    fprintf(stderr, "Framebuffer failed to initialize. Aborting.\n");
    exit(1);
  }
  
  GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, drawBuffers);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bar::Renderer::clearFramebuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, this->buffer.fbo);
  
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
  this->clearRenderBuffer();
  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
  this->clearTexture();
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &this->buffer.fbo);
  this->buffer.fbo = 0;
}

void bar::Renderer::initializeTexture() {
  const GLsizei width = static_cast<GLsizei>(this->viewWidth);
  const GLsizei height = static_cast<GLsizei>(this->viewHeight);
  
  glGenTextures(1, &this->buffer.colorBuffer);
  glBindTexture(GL_TEXTURE_2D, this->buffer.colorBuffer);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void bar::Renderer::clearTexture() {
  glDeleteTextures(1, &this->buffer.colorBuffer);
  this->buffer.colorBuffer = 0;
}

void bar::Renderer::initializeRenderBuffer() {
  const GLsizei width = static_cast<GLsizei>(this->viewWidth);
  const GLsizei height = static_cast<GLsizei>(this->viewHeight);
  
  glGenRenderbuffers(1, &this->buffer.depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, this->buffer.depthStencilBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
//  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->viewWidth, this->viewHeight);
}

void bar::Renderer::clearRenderBuffer() {
  glDeleteRenderbuffers(1, &this->buffer.depthStencilBuffer);
  this->buffer.depthStencilBuffer = 0;
}


