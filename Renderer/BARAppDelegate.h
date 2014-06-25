//
//  BARAppDelegate.h
//  Renderer
//
//  Created by Benjamin Anderson on 6/22/14.
//  Copyright (c) 2014 Benjamin Anderson. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "BAROpenGLView.h"

@interface BARAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (weak) IBOutlet BAROpenGLView *view;


@end
