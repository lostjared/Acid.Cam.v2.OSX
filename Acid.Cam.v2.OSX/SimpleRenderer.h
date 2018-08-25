/*
    SimpleRenderer.h
	Syphon (SDK)
	
    Copyright 2010-2011 bangnoise (Tom Butterworth) & vade (Anton Marini).
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//  Renders a QC Composition into an FBO and exposes the resulting texture

#include "AC_Controller.h"
#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>
#import <OpenGL/OpenGL.h>


@interface SimpleRenderer : NSObject {
@private
	CGLContextObj cgl_ctx;
	QCRenderer *_renderer;
	NSTimeInterval _start;
    GLuint texture_1_id;
    BOOL initval;
    cv::Mat frame;
    BOOL new_frame;
}
- (id)initWithComposition:(NSURL *)url context:(NSOpenGLContext *)context pixelFormat:(NSOpenGLPixelFormat *)format;
@property (readonly) BOOL hasNewFrame;
- (void)render:(NSSize)dimensions;
- (void)updateTexture: (cv::Mat *)fval;

@end

extern SimpleRenderer *render_;
