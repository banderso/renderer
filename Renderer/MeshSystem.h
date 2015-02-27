// -*- mode: C++; -*-
//
//  MeshSystem.h
//  Renderer
//
//  Created by Benjamin Anderson on 7/5/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__MeshSystem__
#define __Renderer__MeshSystem__

#include <OpenGL/gl3.h>

namespace bar {

enum BufferType {
  VERTEX = 0,
  NORMAL,
  TEXTURE,
  ELEMENT,
        
  COUNT // MUST ALWAYS be the LAST value
};

struct MeshAttribute {
  // The type of attribute
  BufferType type;
  // Mesh data
  GLvoid *data;
  // the data type of mesh data values
  GLenum data_type;
  // the number of components per attribute
  GLuint data_size;
  // the size in bytes of the mesh data array
  GLsizei data_array_size;
};

// Buffer names for each mesh.
// Indexed by vao_names;
struct Buffers {
  GLuint *vertices;
  GLuint *normals;
  GLuint *texcoords;
  
  GLuint *elements;        // The element buffer names
  GLenum *element_types;   // The data type of elements at buffer name
  GLsizei *element_counts; // The number of element at buffer name
};

class MeshSystem {
 public:
  explicit MeshSystem(uint32_t max_mesh_count);
  virtual ~MeshSystem();

  // Loads an array of meshes into buffers used by the renderer.
  // vao_name - The name of the VAO to group the buffers under.
  // attributes - The array of mesh attributes to load.
  void load_buffers(GLuint vao_name, uint32_t attribute_count, const MeshAttribute *attributes);

  // Unloads the buffers associated with the given array of VAO names.
  // name_count - The number of VAO names to unload.
  // vao_names - The names of VAOs to unload buffers for.
  void unload_buffers(uint32_t name_count, GLuint *vao_names);

  // Renders the elements associated with the given array of VAO names.
  // name_count - The number of VAO names to unload.
  // vao_names - The names of VAOs to unload buffers for.
  void render_elements(uint32_t name_count, GLuint *vao_names) const;
  
 private:
  MeshSystem();

  Buffers vao_buffers_;
};

}

#endif /* defined(__Renderer__MeshSystem__) */
