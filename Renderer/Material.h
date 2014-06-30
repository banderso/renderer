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

namespace BAR {
    class Material {
    public:
        Material(uint32_t key, const Shader *shader);
        ~Material();
        
        void use() const;
        void bindUniform(const char *name, GLfloat *data) const;
        
    private:
        Material();
        
        uint32_t key_;
        const Shader *shader_;
    };
}

#endif /* defined(__Renderer__Material__) */
