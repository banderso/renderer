//
//  BAROpenGLView.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

@interface BAROpenGLView : NSOpenGLView {
    CVDisplayLinkRef displayLink;
}

@end
