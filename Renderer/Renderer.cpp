//
//  Renderer.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include <iostream>
#include "Renderer.h"
#include "matrix.h"

BAR::Renderer::Renderer(RendererContext &context) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    this->viewWidth = context.width;
    this->viewHeight = context.height;
    if (context.renderableContexts != nullptr) {
        const RenderableContext *contexts = context.renderableContexts;
        this->initializeRenderables(context.renderableContextCount, contexts);
    }
}

BAR::Renderer::~Renderer() {
    this->clearRenderables();
}

void BAR::Renderer::resize(float width, float height) {
    this->viewWidth = width;
    this->viewHeight = height;
}

void BAR::Renderer::draw() const {
    this->clear();
    this->drawObjects();
}

void BAR::Renderer::clear() const {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0,
               static_cast<GLsizei>(viewWidth),
               static_cast<GLsizei>(viewHeight));
}

void BAR::Renderer::drawObjects() const {
    GLfloat modelView[16];
    GLfloat projection[16];
    
    mtxLoadPerspective(projection, 90, viewWidth / viewHeight, 1.0f, 100.0f);
    
    const Renderable * const renderables = this->renderables;
    const uint32_t count = this->renderableCount;
    
    for (uint32_t i = 0; i < count; i++) {
        const Renderable &renderable = renderables[i];
        const Mesh *mesh = renderable.mesh;
        
        if (i == 0) {
            mtxLoadTranslate(modelView, -1.5f, 0.0f, -2.0f);
        } else if (i == 1) {
            mtxLoadTranslate(modelView, 1.5f, 0.0f, -2.0f);
        }
        
        glGetError();

        mesh->activate();
        mesh->bindProjection(projection);
        mesh->bindModelView(modelView);

        glDrawElements(GL_TRIANGLES,
                       mesh->getElementCount(),
                       mesh->getElementType(),
                       nullptr);
    }
    glFlush();
}

void BAR::Renderer::initializeRenderables(uint32_t count, const RenderableContext *contexts) {
    Renderable * const renderables = new Renderable[count];
    for (uint32_t i = 0; i < count; i++) {
        Renderable &renderable = renderables[i];
        const RenderableContext &context = contexts[i];
     
        renderable.mesh = new Mesh(i,
                                   context.vertices,
                                   context.normals,
                                   nullptr,
                                   context.elements,
                                   new Material(i, context.shader));
        //std::cout << "Error: " << GetGLErrorString(glGetError()) << "\n" << std::flush;
        
    }
    
    this->renderables = renderables;
    this->renderableCount = count;
}

void BAR::Renderer::clearRenderables() {
    Renderable * renderables = this->renderables;
    
    delete[] renderables;
    this->renderableCount = 0;
    this->renderables = NULL;
}
