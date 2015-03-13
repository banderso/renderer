//
//  Framebuffer.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 3/5/15.
//  Copyright (c) 2015 Benjamin Anderson. All rights reserved.
//

#include <cstdlib>
#include <cstdio>

#include "Framebuffer.h"
#include "matrix.h"
#include "glutil.h"

bar::Framebuffer::Framebuffer(float width, float height, Mesh *mesh)
    : width(0)
    , height(0)
    , fbo(0)
    , depthStencilBuffer(0)
    , texture(nullptr)
    , texParams(nullptr)
    , mesh(mesh)
{
  fprintf(stdout, "Initializing the framebuffer mesh.\n");

  glGenFramebuffers(1, &this->fbo);
  this->resize(width, height);
}

bar::Framebuffer::~Framebuffer() {
  this->clear();
  glDeleteFramebuffers(1, &this->fbo);
}

void bar::Framebuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}

void bar::Framebuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void bar::Framebuffer::activate() const {
//  GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
//  glDrawBuffers(1, drawBuffers);
}

void bar::Framebuffer::draw(float delta) const {
  GLfloat projection[16];
  mtxLoadOrthographic(projection, -1.0, 1.0, -1.0f, 1.0f, -1.0f, 1.0f);
  
  Mesh * const mesh = this->mesh;
  mesh->activate();
  mesh->update(delta);
  mesh->bindProjection(projection);
  //mesh->bindModelView();
  
  this->texture->bind(GL_TEXTURE0);
  this->texParams->bind(0);
  
  mesh->bind("framebuffer", 0);
  
//  mesh->validate();
  glDrawElements(GL_TRIANGLES,
                 mesh->getElementCount(),
                 mesh->getElementType(),
                 nullptr);
  
  this->texParams->unbind(0);
  this->texture->unbind(GL_TEXTURE0);
  mesh->deactivate();
}

void bar::Framebuffer::resize(float width, float height) {
  this->width = static_cast<GLuint>(width);
  this->height = static_cast<GLuint>(height);
  
  this->clear();
  
  this->bind();
  this->initColorBuffer();
  this->initRenderBuffer();
  
  if (!this->isComplete()) {
    fprintf(stderr, "Framebuffer failed to initialize. Aborting.\n");
    std::exit(EXIT_FAILURE);
  }
  
  GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
  glDrawBuffers(1, drawBuffers);
  
  fprintf(stdout, "Framebuffer: Object: %d, ColorBuffer: %d\n", this->fbo, this->texture->getName());
  
  this->unbind();
}

bool bar::Framebuffer::isComplete() const {
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  return status == GL_FRAMEBUFFER_COMPLETE;
}

void bar::Framebuffer::initColorBuffer() {
  TextureContext context = {
    TextureDimension::TWO,
    TextureType::COLOR,
    TextureDataType::UBYTE,
    this->width,
    this->height,
    0,
    nullptr
  };
  this->texture = new Texture(context);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->texture->getName(), 0);
  
  this->texParams = new TextureParameters();
  this->texParams->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  this->texParams->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  this->texParams->setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  this->texParams->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  this->texParams->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void bar::Framebuffer::initRenderBuffer() {
  glGenRenderbuffers(1, &this->depthStencilBuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
  
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->width, this->height);
  //  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, this->width, this->height);
  
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthStencilBuffer);
  
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void bar::Framebuffer::clear() {
  this->bind();
  this->clearRenderBuffer();
  this->clearColorBuffer();
  this->unbind();
}

void bar::Framebuffer::clearColorBuffer() {
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
  if (this->texture != nullptr && this->texParams != nullptr) {
    delete this->texture;
    delete this->texParams;
    this->texture = nullptr;
    this->texParams = nullptr;
  }
}

void bar::Framebuffer::clearRenderBuffer() {
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
  glDeleteRenderbuffers(1, &this->depthStencilBuffer);
  this->depthStencilBuffer = 0;
}


