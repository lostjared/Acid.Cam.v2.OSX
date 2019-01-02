/*
 * This source file was modified from sample Syphon Examples
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2018, Jared Bruni
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

#import "AC_ImageView.h"
#import <OpenGL/CGLMacro.h>

@interface AC_ImageView ()
@property (readwrite) BOOL needsReshape;
@end

@implementation AC_ImageView

@synthesize needsReshape = _needsReshape, image = _image;


- (void)awakeFromNib
{
    const GLint on = 1;
    [[self openGLContext] setValues:&on forParameter:NSOpenGLCPSwapInterval];
    
    self.needsReshape = YES;
    if ([NSView instancesRespondToSelector:@selector(setWantsBestResolutionOpenGLSurface:)])
    {
        // 10.7+
        [self setWantsBestResolutionOpenGLSurface:YES];
    }
}

- (void)dealloc
{
    [_image release];
    [super dealloc];
}

- (void)reshape
{
    self.needsReshape = YES;
    [super reshape];
}

- (NSSize)renderSize
{
    if ([NSView instancesRespondToSelector:@selector(convertRectToBacking:)])
    {
        // 10.7+
        return [self convertSizeToBacking:[self bounds].size];
    }
    else return [self bounds].size;
}

- (void)drawRect:(NSRect)dirtyRect
{
    CGLContextObj cgl_ctx = [[self openGLContext] CGLContextObj];
    
    NSSize frameSize = self.renderSize;
    
    if (self.needsReshape)
    {
        // Setup OpenGL states
        glViewport(0, 0, frameSize.width, frameSize.height);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, frameSize.width, 0.0, frameSize.height, -1, 1);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        glTranslated(frameSize.width * 0.5, frameSize.height * 0.5, 0.0);
        
        [[self openGLContext] update];
        
        self.needsReshape = NO;
    }
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    SyphonImage *image = self.image;
    if (image)
    {
        glEnable(GL_TEXTURE_RECTANGLE_EXT);
        
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, image.textureName);
        
        NSSize textureSize = image.textureSize;
        
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
        
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 0);
        glDisable(GL_TEXTURE_RECTANGLE_EXT);
    }
    [[self openGLContext] flushBuffer];
}

@end
