// -*- mode: ObjC; -*-
//
//  BAROpenGLView.m
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#import "BAROpenGLView.h"
#include "Renderer.h"

static const float kDesiredFrameTime = 1.0f / 60.0f;

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

bar::Renderer *renderer;
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
  //    NSLog(@"awakeFromNib: called");
  [super awakeFromNib];
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
  NSLog(@"Desired frame time: %f", kDesiredFrameTime);
  
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
  
  if (renderer == nullptr) return;
  
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
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
    NSOpenGLPFAAccelerated,
    NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAMultisample,
    NSOpenGLPFASampleAlpha,
    NSOpenGLPFASampleBuffers, 1,
    NSOpenGLPFASamples, 8,
    NSOpenGLPFAColorSize, 32,
    NSOpenGLPFADepthSize, 16,
    NSOpenGLPFANoRecovery,
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
}

- (void)initGL
{
  [[self openGLContext] makeCurrentContext];
  
  GLint swapInt = 1;
  [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
  
  NSRect viewRectPoints = [self bounds];
  NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
  
  const char *basePath = [[[NSBundle mainBundle] resourcePath] UTF8String];
  
  bar::Shader *shader;
  shader = new bar::Shader(concat(basePath, "/default.vert.glsl"),
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
  
  bar::MeshAttribute positions = {
    static_cast<GLvoid *>(vertices),
    GL_FLOAT,
    3,
    sizeof(vertices)
  };
  bar::MeshAttribute normals = {
    static_cast<GLvoid *>(normal_vals),
    GL_FLOAT,
    3,
    sizeof(normal_vals)
  };
  bar::MeshAttribute elements = {
    static_cast<GLvoid *>(element_vals),
    GL_UNSIGNED_INT,
    1,
    sizeof(element_vals)
  };
  
  bar::RenderableContext renderable[] = {
    {
      &positions,
      &normals,
      nullptr,
      &elements,
      shader
    },
    {
      &positions,
      &normals,
      nullptr,
      &elements,
      shader
    }
  };
  
  bar::RendererContext context;
  context.width = viewRectPixels.size.width;
  context.height = viewRectPixels.size.height;
  context.renderableContextCount = 2;
  context.renderableContexts = renderable;
  
  GLfloat post_vertices[] = {
    -1.0f,  1.0f,  1.0f, // 0
     1.0f,  1.0f,  1.0f, // 3
    -1.0f, -1.0f,  1.0f, // 4
     1.0f, -1.0f,  1.0f, // 7
  };
  
  GLfloat post_normal_vals[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
  };
  
  GLfloat post_texture_coords[] = {
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0
  };
  
  GLuint post_element_vals[] = {
    0, 1, 2,  2, 1, 3
  };
  bar::MeshAttribute post_positions = {
    static_cast<GLvoid *>(post_vertices),
    GL_FLOAT,
    3,
    sizeof(post_vertices)
  };
  bar::MeshAttribute post_normals = {
    static_cast<GLvoid *>(post_normal_vals),
    GL_FLOAT,
    3,
    sizeof(post_normal_vals)
  };
  bar::MeshAttribute post_texcoords = {
    static_cast<GLvoid *>(post_texture_coords),
    GL_FLOAT,
    2,
    sizeof(post_texture_coords)
  };
  bar::MeshAttribute post_elements = {
    static_cast<GLvoid *>(post_element_vals),
    GL_UNSIGNED_INT,
    1,
    sizeof(post_element_vals)
  };
  
  shader = new bar::Shader(concat(basePath, "/post.vert.glsl"),
                           concat(basePath, "/post.frag.glsl"));
  
  bar::RenderableContext postProcessingRenderable[] = {
    {
      &post_positions,
      &post_normals,
      &post_texcoords,
      &post_elements,
      shader
    }
  };
  
  bar::RendererContext postProcessingContext;
  postProcessingContext.width = viewRectPixels.size.width;
  postProcessingContext.height = viewRectPixels.size.height;
  postProcessingContext.renderableContextCount = 1;
  postProcessingContext.renderableContexts = postProcessingRenderable;
  
  renderer = new bar::Renderer(context, postProcessingContext);
}

- (void)cleanUp
{
  CVDisplayLinkStop(displayLink);
  CVDisplayLinkRelease(displayLink);
  
  delete renderer;
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

int loop_counter = 0;
- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime
{
  float delta = static_cast<float>(outputTime->videoRefreshPeriod) /
                static_cast<float>(outputTime->videoTimeScale);
  
  float frameTimeDiff = kDesiredFrameTime - delta - lastDelta;
  lastDelta += delta;
  
//   NSLog(@"%d: vt: %lld, delta: %f, lastDelta: %f, timeToRender: %f, adjusted: %f",
//         loop_counter,
//         outputTime->videoTime,
//         delta,
//         lastDelta,
//         frameTimeDiff,
//         lastDelta + frameTimeDiff);
  
  if (frameTimeDiff >= 0.000001f) {
    loop_counter++;
    return kCVReturnSuccess;
  }
  loop_counter = 0;
  
  //NSLog(@"Render");
  [self drawView:lastDelta + frameTimeDiff];
  lastDelta = frameTimeDiff;
  
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
  BAROpenGLView *view = (__bridge BAROpenGLView *)displayLinkContext;
  return [view getFrameForTime:outputTime];
}
