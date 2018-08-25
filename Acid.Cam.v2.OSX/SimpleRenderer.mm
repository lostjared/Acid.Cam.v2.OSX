/*
    SimpleRenderer.m
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

#import "SimpleRenderer.h"
#import <OpenGL/CGLMacro.h>

SimpleRenderer *render_;

@implementation SimpleRenderer

- (id)initWithComposition:(NSURL *)url context:(NSOpenGLContext *)context pixelFormat:(NSOpenGLPixelFormat *)format
{
	if (self = [super init])
	{
		cgl_ctx = CGLRetainContext([context CGLContextObj]);
		if ([[NSFileManager defaultManager] fileExistsAtPath:[url path]])
		{
			_renderer = [[QCRenderer alloc] initWithOpenGLContext:context
                                                      pixelFormat:format
                                                             file:[url path]];
		}
		_start = [NSDate timeIntervalSinceReferenceDate];
	}
    render_ = self;
	return self;
}

- (void) updateTexture: (cv::Mat *)fval {
    cv::flip(*fval, frame, 0);
}

- (void)destroyResources
{
	CGLReleaseContext(cgl_ctx);
}

- (void)dealloc
{
	[self destroyResources];
	[_renderer release];
	[super dealloc];
}

- (BOOL)hasNewFrame
{
    NSTimeInterval time = [NSDate timeIntervalSinceReferenceDate] - _start;
    if ([_renderer renderingTimeForTime:time arguments:nil] <= time)
    {
        return YES;
    }
    else
    {
        return NO;
    }
}

- (void)render:(NSSize)dimensions
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    if(!frame.empty()) {
        cv::Mat outval;
        outval = resizeKeepAspectRatio(frame, cv::Size(dimensions.width, dimensions.height), cv::Scalar(0, 0, 0));
        frame = outval;
        glDrawPixels(frame.cols, frame.rows, GL_BGR, GL_UNSIGNED_BYTE, (unsigned char *)frame.ptr());
    }
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

@end
