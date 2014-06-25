//
//  Renderer.cpp
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#include <iostream>
#include "Renderer.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static inline const char * GetGLErrorString(GLenum error)
{
	const char *str;
	switch( error )
	{
		case GL_NO_ERROR:
			str = "GL_NO_ERROR";
			break;
		case GL_INVALID_ENUM:
			str = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			str = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			str = "GL_INVALID_OPERATION";
			break;
#if defined __gl_h_ || defined __gl3_h_
		case GL_OUT_OF_MEMORY:
			str = "GL_OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			str = "GL_INVALID_FRAMEBUFFER_OPERATION";
			break;
#endif
#if defined __gl_h_
		case GL_STACK_OVERFLOW:
			str = "GL_STACK_OVERFLOW";
			break;
		case GL_STACK_UNDERFLOW:
			str = "GL_STACK_UNDERFLOW";
			break;
		case GL_TABLE_TOO_LARGE:
			str = "GL_TABLE_TOO_LARGE";
			break;
#endif
		default:
			str = "(ERROR: Unknown Error Enum)";
			break;
	}
	return str;
}

BAR::Renderer::Renderer() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    this->initializeRenderables();
}

BAR::Renderer::~Renderer() {
    this->clearRenderables();
}

void BAR::Renderer::resize(float width, float height) {
    
}

void BAR::Renderer::draw() {
    this->clear();
    this->drawObjects();
}

void BAR::Renderer::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void BAR::Renderer::drawObjects() {
    static const GLfloat pMatrix[] = {
        1.8106601717798212f, 0.0f, 0.0f, 0.0f,
        0.0f, 2.414213562373095f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.002002002002002f, -1.0f,
        0.0f, 0.0f, -0.20020020020020018f, 0.0f
    };
    static const GLfloat mvMatrix[] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    
    Renderable * const renderables = this->renderables;
    const uint32_t count = this->renderableCount;
    Renderable *renderable = NULL;
    
    for (uint32_t i = 0; i < count; i++) {
        glGetError();
        renderable = &renderables[i];
        
        glUseProgram(renderable->program);
        //std::cout << "glUseProgram: " << GetGLErrorString(glGetError()) << "\n" << std::flush;
        
        glUniformMatrix4fv(renderable->pUniform, 1, GL_FALSE, pMatrix);
        //std::cout << "glUniformMatrix4fv: " << GetGLErrorString(glGetError()) << "\n" << std::flush;
        
        glUniformMatrix4fv(renderable->mvUniform, 1, GL_FALSE, mvMatrix);
        //std::cout << "glUniformMatrix4fv: " << GetGLErrorString(glGetError()) << "\n" << std::flush;
        
        glBindVertexArray(renderable->vao);
        //std::cout << "glBindVertexArray: " << GetGLErrorString(glGetError()) << "\n" << std::flush;
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        //std::cout << "glDrawArrays: " << GetGLErrorString(glGetError()) << "\n" << std::flush;
    }
    glFlush();
}

void BAR::Renderer::initializeRenderables() {
    const uint32_t count = 1;
    Renderable *const renderables = new Renderable[count];
    for (uint32_t i = 0; i < count; i++) {
        Renderable &renderable = renderables[i];
        renderable.attrib = i;
        this->initializeBuffers(renderable);
        this->initializeShaders(renderable);
    }
    
    this->renderables = renderables;
    this->renderableCount = count;
}

static const GLfloat vertices[] = {
    0.0f, 0.6f, 0.0f,
    -0.2f, -0.3f, 0.0f,
    0.2f, -0.3f, 0.0f
};

static const GLfloat normals[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};

static const GLfloat colors[] = {
    1.0f, 0.85f, 0.35f, 1.0f
};

void BAR::Renderer::initializeBuffers(Renderable &renderable) {
    
    GLuint vaoName;
    
    glGenVertexArrays(1, &vaoName);
    glBindVertexArray(vaoName);
    
    glGenBuffers(3, renderable.buffers);
    
    // vertices
    glBindBuffer(GL_ARRAY_BUFFER, renderable.buffers[VERTEX]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    const GLuint pos_idx = renderable.attrib + POS_ATTRIB;
    glEnableVertexAttribArray(pos_idx);
    glVertexAttribPointer(pos_idx, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    // normals
    glBindBuffer(GL_ARRAY_BUFFER, renderable.buffers[NORMAL]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    
    const GLuint norm_idx = renderable.attrib + NORMAL_ATTRIB;
    glEnableVertexAttribArray(norm_idx);
    glVertexAttribPointer(norm_idx, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    // colors
    glBindBuffer(GL_ARRAY_BUFFER, renderable.buffers[COLOR]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    
    const GLuint color_idx = renderable.attrib + COLOR_ATTRIB;
    glEnableVertexAttribArray(color_idx);
    glVertexAttribPointer(color_idx, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    renderable.vao = vaoName;
}

static const char * const vertexShader = ""
"#version 330\n"
"in vec3 vertexPosition;\n"
"uniform mat4 uMVMatrix;\n"
"uniform mat4 uPMatrix;\n"
"void main() {\n"
"  gl_Position = vec4(vertexPosition, 1.0);\n"
//"  gl_Position = uPMatrix * uMVMatrix * vec4(vertexPosition, 1.0);\n"
"}\n";

static const char * const fragmentShader = ""
"#version 330\n"
"out vec4 fragColor;"
"void main() {\n"
"  fragColor = vec4(1.0, 0.85, 0.35, 1.0);\n"
"}\n";

void BAR::Renderer::initializeShaders(Renderable &renderable) {
    GLint compileStatus;
    GLint infoLogLength;
    GLchar *logBuffer = nullptr;
    
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertexShader, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        logBuffer = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(vertShader, infoLogLength, NULL, logBuffer);
        std::cout << "An error occurred compiling the vertex shader: " << logBuffer << std::flush;
        delete[] logBuffer;
    }
    
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragmentShader, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &infoLogLength);
        logBuffer = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(fragShader, infoLogLength, NULL, logBuffer);
        std::cout << "An error occurred compiling the fragment shader: " << logBuffer << std::flush;
        delete[] logBuffer;
    }
    
    GLint linkStatus;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        std::cout << "Unable to link shader program." << std::flush;
    }
    
    renderable.program = program;
    renderable.vertexPositionAttribute = glGetAttribLocation(program, "vertexPosition");
    renderable.pUniform = glGetUniformLocation(program, "uPMatrix");
    renderable.mvUniform = glGetUniformLocation(program, "uMVMatrix");
}

void BAR::Renderer::clearRenderables() {
    Renderable * renderables = this->renderables;
    uint32_t rcount = this->renderableCount;
    
    for (uint32_t i = 0; i < rcount; i++) {
        Renderable &renderable = renderables[i];
        
        glDisableVertexAttribArray(renderable.attrib + POS_ATTRIB);
        glDisableVertexAttribArray(renderable.attrib + NORMAL_ATTRIB);
        glDisableVertexAttribArray(renderable.attrib + COLOR_ATTRIB);
        
        glDeleteBuffers(3, renderable.buffers);
    }
    
    delete[] renderables;
    this->renderableCount = 0;
    this->renderables = NULL;
}
