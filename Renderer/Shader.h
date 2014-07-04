// -*- mode: C++; -*-
//
//  Shader.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/28/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__Shader__
#define __Renderer__Shader__

#include <OpenGL/gl3.h>

namespace bar {

class Shader {
 public:
  Shader(const char *vertexShaderFilename,
         const char *fragmentShaderFilename);
  virtual ~Shader();
        
  GLint getUniformLocation(const char *uniformName) const;
        
  void activate() const;
        
  void listAttributes() const;
  void listUniforms() const;
        
 private:
  Shader();
  const char *vertexShaderFilename;
  const char *fragmentShaderFilename;
        
  GLuint vertexShader;
  GLuint fragmentShader;
        
  GLuint program;
};

} // namespace bar

#endif /* defined(__Renderer__Shader__) */
