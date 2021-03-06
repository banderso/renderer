// -*- mode: C++; -*-
//
//  Material.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/29/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__Material__
#define __Renderer__Material__

#include "Shader.h"

namespace bar {

class Material {
 public:
  Material(uint32_t key, const Shader *shader);
  ~Material();
        
  void bind() const;
  void unbind() const;
  void bindUniform(const char *name, GLint data) const;
  void bindUniform(const char *name, GLfloat data) const;
  void bindUniform(const char *name, GLfloat *data) const;
  
  void validate() const;
        
 private:
  Material();
        
  uint32_t key_;
  const Shader *shader_;
};

} // namespace bar

#endif /* defined(__Renderer__Material__) */
