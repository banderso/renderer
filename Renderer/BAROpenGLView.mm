//
//  BAROpenGLView.m
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#import "BAROpenGLView.h"
#include "Renderer.h"

@interface BAROpenGLView (PrivateMethods)

- (void) initGL;

@end

@implementation BAROpenGLView

BAR::Renderer *renderer;

- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime
{
    //NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    [self drawView];
    
    //[pool release];
    return kCVReturnSuccess;
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

- (void)awakeFromNib
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
}

- (void)prepareOpenGL
{
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

- (void) windowWillClose:(NSNotification *)notification
{
    CVDisplayLinkStop(displayLink);
}

- (void)initGL
{
    [[self openGLContext] makeCurrentContext];
    
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    renderer = new BAR::Renderer();
}

- (void)drawView
{
    [[self openGLContext] makeCurrentContext];
    
    CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
    CGLLockContext(cglContext);
    
    renderer->draw();
    
    CGLFlushDrawable(cglContext);
    CGLUnlockContext(cglContext);
}

- (void)drawRect:(NSRect)dirtyRect
{
    [self drawView];
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

- (void)renewGState
{
    [[self window] disableScreenUpdatesUntilFlush];
    [super renewGState];
}

- (void) dealloc
{
    CVDisplayLinkStop(displayLink);
    CVDisplayLinkRelease(displayLink);
    
    delete renderer;
}

@end
