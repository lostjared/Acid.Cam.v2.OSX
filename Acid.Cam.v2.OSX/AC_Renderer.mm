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

GLuint material;

@interface AC_Renderer ()
@property (readwrite) BOOL needsReshape;
@end


int row_w = 0, row_h = 0;

@implementation AC_Renderer

@synthesize needsReshape = _needsReshape;

- (id)initWithComposition:(NSURL *)url context:(NSOpenGLContext *)context pixelFormat:(NSOpenGLPixelFormat *)format
{
    if (self = [super init])
    {
        cgl_ctx = CGLRetainContext([context CGLContextObj]);
        _start = [NSDate timeIntervalSinceReferenceDate];
    }
    render_ = self;
    new_frame = NO;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &material);
    return self;
}

- (void) updateTexture: (cv::Mat *)fval {
    frame = *fval;
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
    NSSize frameSize = dimensions;
    glViewport(0, 0, frameSize.width, frameSize.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, frameSize.width, 0.0, frameSize.height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(frameSize.width * 0.5, frameSize.height * 0.5, 0.0);
   
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    cv::Mat value;
    cv::flip(frame, value, 0);
    frame = value;
    NSSize textureSize;
    textureSize.width = frame.cols;
    textureSize.height = frame.rows;
    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, material);
    
    static bool set = false;
    if(set == false) {
        glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.cols);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT,
                        GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGB, textureSize.width, textureSize.height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.ptr());
        set = true;
        row_w = textureSize.width;
        row_h = textureSize.height;
    }
    else if(new_frame) {
        cv::Mat val;
        cv::resize(frame, val, cv::Size(row_w, row_h));
        frame = val;
        textureSize.width = row_w;
        textureSize.height = row_h;
        glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.cols);
        glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT,
                        GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        glTexSubImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, 0, 0, textureSize.width, textureSize.height, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());
        new_frame = false;
    }

    glColor4f(1.0, 1.0, 1.0, 1.0);
    NSSize scaled;
    float wr = textureSize.width / frameSize.width;
    float hr = textureSize.height / frameSize.height;
    float ratio;
    ratio = (hr < wr ? wr : hr);
    scaled = NSMakeSize((textureSize.width / ratio), (textureSize.height / ratio));
    
    GLfloat tex_coords[] =
    {
        0.0,                0.0,
        (GLfloat)textureSize.width,  0.0,
        (GLfloat)textureSize.width,  (GLfloat)textureSize.height,
        0.0,                (GLfloat)textureSize.height
    };
    
    float halfw = scaled.width * 0.5;
    float halfh = scaled.height * 0.5;
    
    GLfloat verts[] =
    {
        -halfw, -halfh,
        halfw, -halfh,
        halfw, halfh,
        -halfw, halfh
    };
    
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glTexCoordPointer(2, GL_FLOAT, 0, tex_coords );
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, verts );
    glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    glDisableClientState(GL_VERTEX_ARRAY);
    
}

@end
