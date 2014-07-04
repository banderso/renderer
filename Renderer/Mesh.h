// -*- mode: C++; -*-
//
//  Mesh.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/29/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__Mesh__
#define __Renderer__Mesh__

#include <OpenGL/gl3.h>

#include "Material.h"

namespace bar {
enum BufferType {
  VERTEX = 0,
  NORMAL,
  TEXTURE,
  ELEMENT,
        
  COUNT // MUST ALWAYS be the LAST value
};
    
enum AttribOffset {
  POS_ATTRIB = 0,
  NORMAL_ATTRIB,
  TEX_ATTRIB,
  ELEMENT_ATTRIB
};
    
union Vec3 {
  struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
  };
  GLfloat xyz[3];
};
    
struct MeshAttribute {
  GLvoid *data;
  GLenum data_type;
  GLuint data_size;
  GLsizei data_array_size;
};
    
class Mesh {
 public:
  Mesh(uint32_t key,
       MeshAttribute *positions,
       MeshAttribute *normals,
       MeshAttribute *texcoords,
       MeshAttribute *elements,
       Material *material);
  ~Mesh();
        
  void update(float delta);
        
  void activate() const;
  void bindProjection(GLfloat *projection) const;
  void bindModelView();
  //void bindModelView(GLfloat *modelView) const;
        
  GLenum getElementType() const;
  GLsizei getElementCount() const;
        
  GLfloat *getModelView();
  void setRotation(GLfloat degrees);
  void setAxis(Vec3 axis);
  void setLocation(Vec3 location);
        
        
 private:
  Mesh();
        
  uint32_t key_;
  GLuint vao_name_;
  GLuint buffers_[COUNT];
  GLenum element_type_;
  GLsizei element_count_;
  Material *material_;
        
  GLfloat modelView_[16];
  GLfloat degrees_;
  GLfloat axis_[3];
  GLfloat location_[3];
};

} // namespace bar

#endif /* defined(__Renderer__Mesh__) */
