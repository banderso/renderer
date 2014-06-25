//
//  Renderer.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#ifndef __Renderer__Renderer__
#define __Renderer__Renderer__

#include <OpenGL/gl3.h>

namespace BAR {
    struct Vertex {
        GLfloat position[3];
        GLfloat color[4];
    };
    
    enum RenderableBufferType {
        VERTEX = 0,
        NORMAL,
        COLOR
    };
    
    enum VertexAttribOffset {
        POS_ATTRIB = 0,
        NORMAL_ATTRIB,
        COLOR_ATTRIB
    };
    
    struct Renderable {
        GLuint vao;
        GLuint attrib;
        GLuint buffers[3];
        GLuint program;
        GLint vertexPositionAttribute;
        GLint pUniform;
        GLint mvUniform;
    };
    
    class Renderer {
    public:
        Renderer();
        virtual ~Renderer();
        void draw();
        void resize(float width, float height);
    private:
        void clear();
        void drawObjects();
        
        void initializeRenderables();
        void initializeBuffers(Renderable &renderable);
        void initializeShaders(Renderable &renderable);
        
        void clearRenderables();
        
        uint32_t renderableCount;
        Renderable *renderables;
    };
}

#endif /* defined(__Renderer__Renderer__) */
