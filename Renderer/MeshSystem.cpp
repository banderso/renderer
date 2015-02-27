//
//  MeshSystem.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 7/5/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include "MeshSystem.h"
#include "glutil.h"

bar::MeshSystem::MeshSystem() {
  vao_buffers_.vertices  = nullptr;
  vao_buffers_.normals   = nullptr;
  vao_buffers_.texcoords = nullptr;
  vao_buffers_.elements  = nullptr;
  vao_buffers_.element_types = nullptr;
  vao_buffers_.element_counts = nullptr;
}

bar::MeshSystem::MeshSystem(uint32_t max_mesh_count) {
  vao_buffers_.vertices  = new GLuint[max_mesh_count];
  vao_buffers_.normals   = new GLuint[max_mesh_count];
  vao_buffers_.texcoords = new GLuint[max_mesh_count];
  vao_buffers_.elements  = new GLuint[max_mesh_count];
  vao_buffers_.element_types = new GLenum[max_mesh_count];
  vao_buffers_.element_counts = new GLsizei[max_mesh_count];
}

bar::MeshSystem::~MeshSystem() {
  delete[] vao_buffers_.vertices;
  delete[] vao_buffers_.normals;
  delete[] vao_buffers_.texcoords;
  delete[] vao_buffers_.elements;
  delete[] vao_buffers_.element_types;
  delete[] vao_buffers_.element_counts;
}

void bar::MeshSystem::load_buffers(GLuint vao_name,
                                   uint32_t attribute_count,
                                   const MeshAttribute *attributes) {
  if (vao_name == 0 ||
      attribute_count == 0 ||
      attribute_count > BufferType::COUNT ||
      attributes == nullptr) {
    return;
  }

  glBindVertexArray(vao_name);

  GLenum element_type = 0;
  GLsizei element_count = 0;
  GLuint buffers[BufferType::COUNT];
  glGenBuffers(BufferType::COUNT, buffers);
  
  for (uint8_t i = 0; i < attribute_count; i++) {
    const MeshAttribute &attribute = attributes[i];
    const BufferType type = attribute.type;
    const GLsizei type_size = GetGLTypeSize(attribute.data_type);
    
    switch (type) {
      case BufferType::VERTEX:
      case BufferType::NORMAL:
      case BufferType::TEXTURE:
        glBindBuffer(GL_ARRAY_BUFFER, buffers[type]);
        glBufferData(GL_ARRAY_BUFFER,
                     attribute.data_array_size,
                     attribute.data,
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(type);
        glVertexAttribPointer(type,
                              attribute.data_size,
                              attribute.data_type,
                              GL_FALSE,
                              0,
                              0);
        break;
      case BufferType::ELEMENT:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[type]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     attribute.data_array_size,
                     attribute.data,
                     GL_STATIC_DRAW);

        element_type = attribute.data_type;
        element_count = attribute.data_array_size / type_size;
        break;
      default:
        break;
    }
  }

  vao_buffers_.vertices[vao_name]  = buffers[BufferType::VERTEX];
  vao_buffers_.normals[vao_name]   = buffers[BufferType::NORMAL];
  vao_buffers_.texcoords[vao_name] = buffers[BufferType::TEXTURE];
  
  vao_buffers_.elements[vao_name]       = buffers[BufferType::ELEMENT];
  vao_buffers_.element_types[vao_name]  = element_type;
  vao_buffers_.element_counts[vao_name] = element_count;
}

void bar::MeshSystem::unload_buffers(uint32_t name_count, GLuint *vao_names) {
  if (name_count == 0 || vao_names == nullptr) {
    return;
  }

  GLuint buffers[BufferType::COUNT] = {0};
  for (uint32_t i = 0; i < name_count; i++) {
    GLuint name = vao_names[i];
    buffers[BufferType::VERTEX]  = vao_buffers_.vertices[name];
    buffers[BufferType::NORMAL]  = vao_buffers_.normals[name];
    buffers[BufferType::TEXTURE] = vao_buffers_.texcoords[name];
    buffers[BufferType::ELEMENT] = vao_buffers_.elements[name];
    glDeleteBuffers(BufferType::COUNT, buffers);

    vao_buffers_.vertices[name] = 0;
    vao_buffers_.normals[name] = 0;
    vao_buffers_.texcoords[name] = 0;
    vao_buffers_.elements[name] = 0;
    vao_buffers_.element_types[name] = 0;
    vao_buffers_.element_counts[name] = 0;
  }
}

void bar::MeshSystem::render_elements(uint32_t name_count, GLuint *vao_names) const {
  if (name_count == 0 || vao_names == nullptr) {
    return;
  }

  GLenum *types = vao_buffers_.element_types;
  GLsizei *counts = vao_buffers_.element_counts;

  GLuint name = 0;
  for (uint32_t i = 0; i < name_count; i++) {
    name = vao_names[i];
    glDrawElements(GL_TRIANGLES,
                   types[name],
                   counts[name],
                   nullptr);
  }
}
