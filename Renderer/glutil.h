//
//  glutil.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/28/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef Renderer_glutil_h
#define Renderer_glutil_h

#include <OpenGL/gl3.h>

#ifdef __cplusplus
extern "C" {
#endif
    
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
    
const char * GetGLErrorString(GLenum error);
void LogGLError(int line, const char *file);
GLsizei GetGLTypeSize(GLenum type);
    
#ifdef __cplusplus
}
#endif

#endif
