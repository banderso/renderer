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
    , color(nullptr)
    , colorParams(nullptr)
    , depth(nullptr)
    , depthParams(nullptr)
    , normal(nullptr)
    , normalParams(nullptr)
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
//  GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
//  glDrawBuffers(2, drawBuffers);
}

void bar::Framebuffer::draw(float delta) const {
  GLfloat projection[16];
  mtxLoadOrthographic(projection, -1.0, 1.0, -1.0f, 1.0f, -1.0f, 1.0f);
  
  Mesh * const mesh = this->mesh;
  mesh->activate();
  mesh->update(delta);
  mesh->bindProjection(projection);
  //mesh->bindModelView();
  
  this->color->bind(GL_TEXTURE0);
  this->colorParams->bind(0);
  
  this->depth->bind(GL_TEXTURE1);
  this->depthParams->bind(1);
  
  this->normal->bind(GL_TEXTURE2);
  this->normalParams->bind(2);
  
  mesh->bind("framebuffer", 0);
  mesh->bind("depthbuffer", 1);
  mesh->bind("normalbuffer", 2);
  
//  mesh->validate();
  glDrawElements(GL_TRIANGLES,
                 mesh->getElementCount(),
                 mesh->getElementType(),
                 nullptr);
  
  this->normalParams->unbind(2);
  this->normal->unbind(GL_TEXTURE2);
  
  this->depthParams->unbind(1);
  this->depth->unbind(GL_TEXTURE1);
  
  this->colorParams->unbind(0);
  this->color->unbind(GL_TEXTURE0);
  
  mesh->deactivate();
}

void bar::Framebuffer::resize(float width, float height) {
  this->width = static_cast<GLuint>(width);
  this->height = static_cast<GLuint>(height);
  
  this->clear();
  
  this->bind();
  this->initColorBuffer();
  this->initDepthBuffer();
  this->initNormalBuffer();
  
  if (!this->isComplete()) {
    fprintf(stderr, "Framebuffer failed to initialize. Aborting.\n");
    std::exit(EXIT_FAILURE);
  }
  
  GLenum drawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, drawBuffers);
  
//  fprintf(stdout, "Framebuffer: Object: %d, ColorBuffer: %d\n", this->fbo, this->color->getName());
  
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
  this->color = new Texture(context);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->color->getName(), 0);
  
  this->colorParams = new TextureParameters();
  this->colorParams->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  this->colorParams->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  this->colorParams->setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  this->colorParams->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  this->colorParams->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void bar::Framebuffer::initDepthBuffer() {
  TextureContext context = {
    TextureDimension::TWO,
    TextureType::DEPTH,
    TextureDataType::FLOAT,
    this->width,
    this->height,
    0,
    nullptr
  };
  this->depth = new Texture(context);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depth->getName(), 0);
  
  this->depthParams = new TextureParameters();
  this->depthParams->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  this->depthParams->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  this->depthParams->setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  this->depthParams->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  this->depthParams->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void bar::Framebuffer::initNormalBuffer() {
  TextureContext context = {
    TextureDimension::TWO,
    TextureType::COLOR,
    TextureDataType::UBYTE,
    this->width,
    this->height,
    0,
    nullptr
  };
  this->normal = new Texture(context);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->normal->getName(), 0);
  
  this->normalParams = new TextureParameters();
  this->normalParams->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  this->normalParams->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  this->normalParams->setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  this->normalParams->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  this->normalParams->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void bar::Framebuffer::clear() {
  this->bind();
  this->clearNormalBuffer();
  this->clearDepthBuffer();
  this->clearColorBuffer();
  this->unbind();
}

void bar::Framebuffer::clearColorBuffer() {
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
  if (this->color != nullptr && this->colorParams != nullptr) {
    delete this->color;
    delete this->colorParams;
    this->color = nullptr;
    this->colorParams = nullptr;
  }
}

void bar::Framebuffer::clearDepthBuffer() {
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
  if (this->depth != nullptr && this->depthParams != nullptr) {
    delete this->depth;
    delete this->depthParams;
    this->depth = nullptr;
    this->depthParams = nullptr;
  }
}

void bar::Framebuffer::clearNormalBuffer() {
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, 0, 0);
  if (this->normal != nullptr && this->normalParams != nullptr) {
    delete this->normal;
    delete this->normalParams;
    this->normal = nullptr;
    this->normalParams = nullptr;
  }
}

