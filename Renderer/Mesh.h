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

namespace BAR {
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
        
        void activate() const;
        void bindProjection(GLfloat *projection) const;
        void bindModelView(GLfloat *modelView) const;
        
        GLenum getElementType() const;
        GLsizei getElementCount() const;
        
    private:
        Mesh();
        
        uint32_t key_;
        GLuint vao_name_;
        GLuint buffers_[COUNT];
        GLenum element_type_;
        GLsizei element_count_;
        Material *material_;
    };
}

#endif /* defined(__Renderer__Mesh__) */
