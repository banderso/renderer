//
//  BAROpenGLView.m
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#import "BAROpenGLView.h"
#include "Renderer.h"

static char * concat(const char *base, const char *str);
static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *now,
                                    const CVTimeStamp *outputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext);

@interface BAROpenGLView (PrivateMethods)

- (void) initGLContext;
- (void) initGL;
- (void) cleanUp;
- (void) drawView:(float)delta;
- (CVReturn) getFrameForTime:(const CVTimeStamp *)outputTime;

@end

@implementation BAROpenGLView

BAR::Renderer *renderer;
float frequency = 0.0f;
float lastTime = 0.0f;
float lastDelta = 0.0f;

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format
{
    self = [super initWithFrame:frameRect pixelFormat:format];
    if (self) {
        NSLog(@"intWithFrame:pixelFormat: called");
    }
    return self;
}

- (void)awakeFromNib
{
    NSLog(@"awakeFromNib: called");
    [self initGLContext];
}

- (void) dealloc
{
    [self cleanUp];
}

#pragma mark NSWindowDelegate Protocol

- (void) windowWillClose:(NSNotification *)notification
{
    CVDisplayLinkStop(displayLink);
}

#pragma mark NSOpenGLView

- (void)prepareOpenGL
{
    NSLog(@"OpenGL vendor: %s", glGetString(GL_VENDOR));
    NSLog(@"OpenGL renderer: %s", glGetString(GL_RENDERER));
    NSLog(@"OpenGL version: %s", glGetString(GL_VERSION));
    NSLog(@"OpenGL Shading Language version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    [super prepareOpenGL];
    
    [self initGL];
    
    CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, (__bridge void *)self);
    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    
    CVDisplayLinkStart(displayLink);
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(windowWillClose:)
                                                 name:NSWindowWillCloseNotification
                                               object:[self window]];
}

- (void)drawRect:(NSRect)dirtyRect
{
    [self drawView: lastDelta];
}

- (void)reshape
{
    [super reshape];
    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLLockContext(cglContext);
    
    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
    renderer->resize(viewRectPixels.size.width,
                     viewRectPixels.size.height);
    
    CGLUnlockContext(cglContext);
}

#pragma mark NSView

- (void)renewGState
{
    [[self window] disableScreenUpdatesUntilFlush];
    [super renewGState];
}

#pragma mark PrivateMethods

- (void)initGLContext
{
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
        
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        
        0
    };
    
    NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!pf) {
        NSLog(@"No OpenGL pixel format");
    }
    
    NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:pf
                                                          shareContext:nil];
    CGLEnable((CGLContextObj)[context CGLContextObj], kCGLCECrashOnRemovedFunctions);
    
    [self setPixelFormat:pf];
    [self setOpenGLContext:context];
    
    [self setWantsBestResolutionOpenGLSurface:YES];
    frequency = CVGetHostClockFrequency();
}

- (void)initGL
{
    [[self openGLContext] makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    NSRect viewRectPoints = [self bounds];
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
    const char *basePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
    
    BAR::Shader *shader;
    shader = new BAR::Shader(concat(basePath, "/default.vert.glsl"),
                             concat(basePath, "/default.frag.glsl"));
    
    /*GLfloat vertices[] = {
        -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f
    };*/
    
    GLfloat vertices[] = {
        -1.0f,  1.0f,  1.0f, // 0
        -1.0f,  1.0f, -1.0f, // 1
         1.0f,  1.0f, -1.0f, // 2
         1.0f,  1.0f,  1.0f, // 3
        
        -1.0f, -1.0f,  1.0f, // 4
        -1.0f, -1.0f, -1.0f, // 5
         1.0f, -1.0f, -1.0f, // 6
         1.0f, -1.0f,  1.0f, // 7
    };
    
    GLfloat normal_vals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };

    GLuint element_vals[] = {
        0, 1, 2,  0, 2, 3, // top
        0, 3, 4,  4, 3, 7, // front
        7, 3, 6,  6, 3, 2, // right
        6, 7, 5,  5, 7, 4, // bottom
        6, 2, 5,  5, 2, 1, // back
        1, 4, 0,  1, 5, 4 // left
    };
    
    BAR::MeshAttribute positions = {
        static_cast<GLvoid *>(vertices),
        GL_FLOAT,
        3,
        sizeof(vertices)
    };
    
    BAR::MeshAttribute normals = {
        static_cast<GLvoid *>(normal_vals),
        GL_FLOAT,
        3,
        sizeof(normal_vals)
    };
    BAR::MeshAttribute elements = {
        static_cast<GLvoid *>(element_vals),
        GL_UNSIGNED_INT,
        1,
        sizeof(element_vals)
    };
    
    BAR::RenderableContext renderable[] = {
        {
            &positions,
            &normals,
            &elements,
            shader
        },
        {
            &positions,
            &normals,
            &elements,
            shader
        }
    };
    
    BAR::RendererContext context;
    context.width = viewRectPixels.size.width;
    context.height = viewRectPixels.size.height;
    context.renderableContextCount = 2;
    context.renderableContexts = renderable;
    
    renderer = new BAR::Renderer(context);
}

- (void)cleanUp
{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    
    delete renderer;
}

- (int64_t) calcDelta:(int64_t)now
{
    int64_t newDelta = 0;
    if (lastTime) {
        newDelta = now - lastTime;
    }
    
    lastTime = now;
    lastDelta = newDelta;
    
    return newDelta;
}

- (void)drawView:(float)delta
{
    [[self openGLContext] makeCurrentContext];
    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLLockContext(cglContext);
    
    renderer->draw(delta);
    
    CGLFlushDrawable(cglContext);
    CGLUnlockContext(cglContext);
}

- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime
{
    uint64_t hostTime = outputTime->hostTime;
    float now = (float)hostTime / frequency;
    
    if (now >= lastTime + (1.0f / 60.0f)) {
        lastDelta = now - lastDelta;
        lastTime = now;
    
        [self drawView:lastDelta];
    }
    
    return kCVReturnSuccess;
}

@end

static char * concat(const char *base, const char *str) {
    if (base == nullptr || str == nullptr) return nullptr;
    
    size_t baseLength = strlen(base);
    size_t strLength = strlen(str);
    
    char * nstr = new char[baseLength + strLength + 1];
    
    strncpy(nstr, base, baseLength + 1);
    strncat(nstr, str, strLength);
    
    return nstr;
}

static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink,
                                    const CVTimeStamp *now,
                                    const CVTimeStamp *outputTime,
                                    CVOptionFlags flagsIn,
                                    CVOptionFlags *flagsOut,
                                    void *displayLinkContext) {
    CVReturn result = [(__bridge BAROpenGLView *)displayLinkContext getFrameForTime:outputTime];
    return result;
}
