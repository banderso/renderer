// -*- mode: C++; -*-
//
//  MaterialSystem.h
//  Renderer
//
//  Created by Benjamin Anderson on 7/5/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__MaterialSystem__
#define __Renderer__MaterialSystem__

#include ""

namespace bar {

const MAX_VERTEX_SHADER_COUNT   = 4;
const MAX_FRAGMENT_SHADER_COUNT = 32;
const MAX_SHADER_PROGRAM_COUNT  = 256;
const MAX_TEXTURE_COUNT         = 4096;

struct MaterialBuffer {
  GLuint *programs;
  GLuint *vertex_shaders;
  GLuint *fragment_shaders;
  GLuint *textures;
};

class MaterialSystem {
 public:
  MaterialSystem();
  virtual ~MaterialSystem();

  
  
 private:
  MaterialBuffer materials_;
};

}

#endif /* defined(__Renderer__MaterialSystem__) */
