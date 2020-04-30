/*
 * This source file was modified from sample Syphon Examples
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/lostsidedead
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2020, Jared Bruni
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */
#import "AC_Renderer.h"
#import <OpenGL/CGLMacro.h>

AC_Renderer *render_;

@implementation AC_Renderer

- (id)initWithComposition:(NSURL *)url context:(NSOpenGLContext *)context pixelFormat:(NSOpenGLPixelFormat *)format
{
    if (self = [super init])
    {
        cgl_ctx = CGLRetainContext([context CGLContextObj]);
        _start = [NSDate timeIntervalSinceReferenceDate];
    }
    render_ = self;
    new_frame = NO;
    return self;
}

- (void) updateTexture: (cv::Mat *)fval {
    cv::flip(*fval, frame, 0);
    new_frame = YES;
}

- (void)destroyResources
{
    CGLReleaseContext(cgl_ctx);
}

- (void)dealloc
{
    [self destroyResources];
    [super dealloc];
}

- (BOOL)hasNewFrame
{
    return new_frame;
}

- (void)render:(NSSize)dimensions
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    if(!frame.empty() && new_frame == YES) {
        cv::Mat outval;
        frame =  resizeKeepAspectRatio(frame, cv::Size(dimensions.width, dimensions.height), cv::Scalar(0,0,0));
        glDrawPixels(frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, (unsigned char *)frame.ptr());
        new_frame = NO;
    }
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

@end
