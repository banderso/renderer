//
//  Mesh.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/29/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include "Mesh.h"

#include "glutil.h"
#include "matrix.h"

#include <stdio.h>

bar::Mesh::Mesh()
    : key_(0)
    , vao_name_(0)
    , element_type_(0)
    , element_count_(0)
    , material_(nullptr)
    , degrees_(0.0f) {}

bar::Mesh::Mesh(uint32_t key,
                MeshAttribute *positions,
                MeshAttribute *normals,
                MeshAttribute *texcoords,
                MeshAttribute *elements,
                Material *material)
    : key_(key)
    , vao_name_(0)
    , element_type_(0)
    , element_count_(0)
    , material_(material)
    , degrees_(0.0f){
  glGenVertexArrays(1, &vao_name_);
  glBindVertexArray(vao_name_);
    
  glGenBuffers(BufferType::COUNT, buffers_);
    
  GLsizei type_size = 0;
  if (positions) {
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType::VERTEX]);
    glBufferData(GL_ARRAY_BUFFER,
                 positions->data_array_size,
                 positions->data,
                 GL_STATIC_DRAW);
        
        
    type_size = GetGLTypeSize(positions->data_type);
    glEnableVertexAttribArray(AttribOffset::POS_ATTRIB);
    glVertexAttribPointer(AttribOffset::POS_ATTRIB,
                          positions->data_size,
                          positions->data_type,
                          GL_FALSE,
                          0, //positions->data_size * type_size,
                          0);
  }
    
  if (normals) {
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType::NORMAL]);
    glBufferData(GL_ARRAY_BUFFER,
                 normals->data_array_size,
                 normals->data,
                 GL_STATIC_DRAW);
        
    type_size = GetGLTypeSize(normals->data_type);
    glEnableVertexAttribArray(AttribOffset::NORMAL_ATTRIB);
    glVertexAttribPointer(AttribOffset::NORMAL_ATTRIB,
                          normals->data_size,
                          normals->data_type,
                          GL_FALSE,
                          0, //normals->data_size * type_size,
                          0);
  }
    
  if (texcoords) {
    glBindBuffer(GL_ARRAY_BUFFER, buffers_[BufferType::TEXTURE]);
    glBufferData(GL_ARRAY_BUFFER,
                 texcoords->data_array_size,
                 texcoords->data,
                 GL_STATIC_DRAW);
        
    type_size = GetGLTypeSize(texcoords->data_type);
    glEnableVertexAttribArray(AttribOffset::TEX_ATTRIB);
    glVertexAttribPointer(AttribOffset::TEX_ATTRIB,
                          texcoords->data_size,
                          texcoords->data_type,
                          GL_FALSE,
                          0, //texcoords->data_size * type_size,
                          0);
  }
    
  if (elements) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_[ELEMENT]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 elements->data_array_size,
                 elements->data,
                 GL_STATIC_DRAW);
        
    element_type_ = elements->data_type;
    element_count_ = elements->data_array_size / GetGLTypeSize(element_type_);
  }
    
  axis_[0] = axis_[2] = 0.0f;
  axis_[1] = 1.0f;
    
  location_[0] = location_[1] = location_[2] = 0.0f;
}

bar::Mesh::~Mesh() {
  glDeleteBuffers(BufferType::COUNT, buffers_);
  delete material_;
}

void bar::Mesh::update(float delta) {
  const float speed = 0.00001f;
  degrees_ += (speed * delta);
}

void bar::Mesh::activate() const {
  glBindVertexArray(vao_name_);
  material_->use();
}

void bar::Mesh::bindProjection(GLfloat *projection) const {
  if (material_) {
    material_->bindUniform("projection", projection);
  }
}

void bar::Mesh::bindModelView() {
  if (material_) {
    material_->bindUniform("modelView", getModelView());
  }
}

/*
  void bar::Mesh::bindModelView(GLfloat *modelView) const {
  if (material_) {
  material_->bindUniform("modelView", modelView);
  }
  }
*/
GLenum bar::Mesh::getElementType() const {
  return element_type_;
}

GLsizei bar::Mesh::getElementCount() const {
  return element_count_;
}

GLfloat *bar::Mesh::getModelView() {
  GLfloat rotation[16];
  GLfloat brotation[16];
  GLfloat translation[16];
    
  GLfloat temp[16];
    
  mtxLoadIdentity(rotation);
  mtxLoadIdentity(brotation);
  mtxLoadIdentity(translation);
    
  mtxLoadRotate(rotation, degrees_, axis_[0], axis_[1], axis_[2]);
  mtxLoadRotate(brotation, degrees_, axis_[1], axis_[0], axis_[2]);
    
  mtxLoadTranslate(translation, location_[0], location_[1], location_[2]);
    
  mtxMultiply(temp, rotation, brotation);
  mtxMultiply(modelView_, translation, temp);
  return modelView_;
}

void bar::Mesh::setRotation(GLfloat degrees) {
  degrees_ = degrees;
}

void bar::Mesh::setAxis(Vec3 axis) {
  axis_[0] = axis.x;
  axis_[1] = axis.y;
  axis_[2] = axis.z;
}

void bar::Mesh::setLocation(Vec3 location) {
  location_[0] = location.x;
  location_[1] = location.y;
  location_[2] = location.z;
}
