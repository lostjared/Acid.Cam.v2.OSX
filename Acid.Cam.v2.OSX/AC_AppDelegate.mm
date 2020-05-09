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

#import "AC_AppDelegate.h"
#import "AC_Controller.h"
#import <GameController/GameController.h>


@implementation AC_AppDelegate {
    SyphonClient* syClient;
    IBOutlet NSArrayController *availableServersController;
    NSArray *selectedServerDescriptions;
//    NSTimeInterval fpsStart;
//    NSUInteger fpsCount;
}

@synthesize window;
@synthesize glView;
@synthesize FPS;
@synthesize center;
@synthesize image_view;

+ (NSSet *)keyPathsForValuesAffectingStatus
{
    return [NSSet setWithObjects:@"frameWidth", @"frameHeight", @"FPS", @"selectedServerDescriptions", @"view.error", nil];
}

- (NSString *)status
{
    if (self.image_view.error)
    {
        return self.image_view.error.localizedDescription;
    }
    else if (self.frameWidth && self.frameHeight)
    {
        return [NSString stringWithFormat:@"%lu x %lu : %lu FPS", (unsigned long)self.frameWidth, (unsigned long)self.frameHeight, (unsigned long)self.FPS];
    }
    else
    {
        return @"--";
    }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
}

- (IBAction) listen_for:(id)sender {
    
    [self startProgram];
    
}

- (void) startProgram {
    [availableServersController bind:@"contentArray" toObject:[SyphonServerDirectory sharedDirectory] withKeyPath:@"servers" options:nil];
    
    [self bind:@"selectedServerDescriptions" toObject:availableServersController withKeyPath:@"selectedObjects" options:nil];
    
    [[self.image_view window] setContentMinSize:(NSSize){400.0,300.0}];
    [[self.image_view window] setDelegate:self];
    [[self.image_view window] orderFront:self];
}

- (NSArray *)selectedServerDescriptions
{
    return selectedServerDescriptions;
}

- (void)setSelectedServerDescriptions:(NSArray *)descriptions
{
    if (![descriptions isEqualToArray:selectedServerDescriptions])
    {
        NSString *currentUUID = [selectedServerDescriptions lastObject][SyphonServerDescriptionUUIDKey];
        NSString *newUUID = [descriptions lastObject][SyphonServerDescriptionUUIDKey];
        BOOL uuidChange = newUUID && ![currentUUID isEqualToString:newUUID];
        selectedServerDescriptions = descriptions;
        
        if (!newUUID || !currentUUID || uuidChange)
        {
            // Stop our current client
            [syClient stop];
            // Reset our terrible FPS display
            fpsStart = [NSDate timeIntervalSinceReferenceDate];
            fpsCount = 0;
            self.FPS = 0;
            syClient = [[SyphonClient alloc] initWithServerDescription:[descriptions lastObject]
                                                               context:[[self.image_view openGLContext] CGLContextObj]
                                                               options:nil newFrameHandler:^(SyphonClient *client) {
                                                                   // This gets called whenever the client receives a new frame.
                                                                   
                                                                   // The new-frame handler could be called from any thread, but because we update our UI we have
                                                                   // to do this on the main thread.
                                                                   
                                                                   [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                                                                       // First we track our framerate...
                                                                       self->fpsCount++;
                                                                       float elapsed = [NSDate timeIntervalSinceReferenceDate] - self->fpsStart;
                                                                       if (elapsed > 1.0)
                                                                       {
                                                                           self.FPS = ceilf(self->fpsCount / elapsed);
                                                                           self->fpsStart = [NSDate timeIntervalSinceReferenceDate];
                                                                           self->fpsCount = 0;
                                                                       }
                                                                       // ...then we check to see if our dimensions display or window shape needs to be updated
                                                                       SyphonImage *frame = [client newFrameImage];
                                                                       
                                                                       NSSize imageSize = frame.textureSize;
                                                                       
                                                                       BOOL changed = NO;
                                                                       if (self.frameWidth != imageSize.width)
                                                                       {
                                                                           changed = YES;
                                                                           self.frameWidth = imageSize.width;
                                                                       }
                                                                       if (self.frameHeight != imageSize.height)
                                                                       {
                                                                           changed = YES;
                                                                           self.frameHeight = imageSize.height;
                                                                       }
                                                                       if (changed)
                                                                       {
                                                                           [[self.image_view window] setContentAspectRatio:imageSize];
                                                                           [self resizeWindowForCurrentVideo];
                                                                       }
                                                                       // ...then update the view and mark it as needing display
                                                                       self.image_view.image = frame;
                                                                       [self.image_view setNeedsDisplay:YES];
                                                                       cv::Mat img;
                                                                       img.create(imageSize.height,imageSize.width,CV_8UC4);
                                                                       glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3) ? 1 : 4);
                                                                       glPixelStorei(GL_PACK_ROW_LENGTH, (GLint)img.step/img.elemSize());
                                                                       glReadPixels(0, 0, img.cols, img.rows, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
                                                                       cv::Mat flipped;
                                                                       cv::flip(img, flipped, 0);
                                                                       cv::cvtColor(flipped, img, CV_RGBA2BGR);
                                                                       ac::setNewSyphonImage(img);
                                                                   }];
                                                               }];
            
            // If we have a client we do nothing - wait until it outputs a frame
            
            // Otherwise clear the view
            if (syClient == nil)
            {
                self.image_view.image = nil;
                
                self.frameWidth = 0;
                self.frameHeight = 0;
                
                [self.view setNeedsDisplay:YES];
            }
        }
    }
}

- (void)applicationWillTerminate:(NSNotification *)aNotification
{
    [syClient stop];
    syClient = nil;
}

#pragma mark Window Sizing

- (NSSize)windowContentSizeForCurrentVideo
{
    NSSize imageSize = NSMakeSize(self.frameWidth, self.frameHeight);
    
    if (imageSize.width == 0 || imageSize.height == 0)
    {
        imageSize.width = 640;
        imageSize.height = 480;
    }
    
    return imageSize;
}

- (NSRect)frameRectForContentSize:(NSSize)contentSize
{
    // Make sure we are at least as big as the window's minimum content size
    NSSize minContentSize = [[self.image_view window] contentMinSize];
    if (contentSize.height < minContentSize.height)
    {
        float scale = minContentSize.height / contentSize.height;
        contentSize.height *= scale;
        contentSize.width *= scale;
    }
    if (contentSize.width < minContentSize.width)
    {
        float scale = minContentSize.width / contentSize.width;
        contentSize.height *= scale;
        contentSize.width *= scale;
    }
    
    NSRect contentRect = (NSRect){[[self.image_view window] frame].origin, contentSize};
    NSRect frameRect = [[self.image_view window] frameRectForContentRect:contentRect];
    
    // Move the window up (or down) so it remains rooted at the top left
    float delta = [[self.image_view window] frame].size.height - frameRect.size.height;
    frameRect.origin.y += delta;
    
    // Attempt to remain on-screen
    NSRect available = [[[self.image_view window] screen] visibleFrame];
    if ((frameRect.origin.x + frameRect.size.width) > available.size.width)
    {
        frameRect.origin.x = available.size.width - frameRect.size.width;
    }
    if ((frameRect.origin.y + frameRect.size.height) > available.size.height)
    {
        frameRect.origin.y = available.size.height - frameRect.size.height;
    }
    
    return frameRect;
}

- (NSRect)windowWillUseStandardFrame:(NSWindow *)window defaultFrame:(NSRect)newFrame
{
    // We get this when the user hits the zoom box, if we're not already zoomed
    if ([window isEqual:[self.view window]])
    {
        // Resize to the current video dimensions
        return [self frameRectForContentSize:[self windowContentSizeForCurrentVideo]];
    }
    else
    {
        return newFrame;
    }
}

- (void)resizeWindowForCurrentVideo
{
    // Resize to the correct aspect ratio, keeping as close as possible to our current dimensions
    NSSize wantedContentSize = [self windowContentSizeForCurrentVideo];
    NSSize currentSize = [[[self.image_view window] contentView] frame].size;
    float wr = wantedContentSize.width / currentSize.width;
    float hr = wantedContentSize.height / currentSize.height;
    NSUInteger widthScaledToHeight = wantedContentSize.width / hr;
    NSUInteger heightScaledToWidth = wantedContentSize.height / wr;
    if (widthScaledToHeight - currentSize.width < heightScaledToWidth - currentSize.height)
    {
        wantedContentSize.width /= hr;
        wantedContentSize.height /= hr;
    }
    else
    {
        wantedContentSize.width /= wr;
        wantedContentSize.height /= wr;
    }
    
    NSRect newFrame = [self frameRectForContentSize:wantedContentSize];
    [[self.image_view window] setFrame:newFrame display:YES animate:NO];
}


- (NSApplicationPresentationOptions)window:(NSWindow *)window willUseFullScreenPresentationOptions:(NSApplicationPresentationOptions)proposedOptions
{
    return proposedOptions | NSApplicationPresentationAutoHideToolbar;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:self];
    CGLContextObj context = [[glView openGLContext] CGLContextObj];
    NSDictionary *options = @{SyphonServerOptionDepthBufferResolution: @16};
    syServer = [[SyphonServer alloc] initWithName:nil context:context options:options];
    [self openFile:[[NSBundle mainBundle] URLForResource:@"ServerDemo" withExtension:@"qtz"]];
    fpsStart = [NSDate timeIntervalSinceReferenceDate];
    lameRenderingTimer = [NSTimer timerWithTimeInterval:1.0/60.0 target:self selector:@selector(render:) userInfo:nil repeats:YES];
    [lameRenderingTimer retain];
    [[NSRunLoop currentRunLoop] addTimer:lameRenderingTimer forMode:NSRunLoopCommonModes];
    /*
    center = [NSNotificationCenter defaultCenter];
    [center addObserverForName: GCControllerDidConnectNotification object: nil queue: nil usingBlock: ^(NSNotification * note) {
        GCController *control = note.object;
        NSLog(@"Connected: %s\n", control.vendorName.UTF8String );
        [controller initControllers:self];
    }];
    
    [center addObserverForName: GCControllerDidDisconnectNotification object:nil queue:nil usingBlock:^(NSNotification *note) {
        [controller releaseControllers:self];
    }];
    
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:^() {
        NSLog(@"End...");
    }];*/

}

-(void) render:(NSTimer*) aTimer {
    if (renderer.hasNewFrame)
    {
        NSSize frameSize = glView.renderSize;
        [syServer bindToDrawFrameOfSize:frameSize];
        [renderer render:frameSize];
        [syServer unbindAndPublish];
        SyphonImage *image = [syServer newFrameImage];
        glView.image = image;
        [image release];
        [glView setNeedsDisplay:YES];
        fpsCount++;
        float elapsed = [NSDate timeIntervalSinceReferenceDate] - fpsStart;
        if (elapsed > 0.5)
        {
            self.FPS = fpsCount / elapsed;
            fpsCount = 0;
            fpsStart = [NSDate timeIntervalSinceReferenceDate];
        }
    }
}

- (void)openFile:(NSURL *)url {
    glView.image = nil;
    [renderer release];
    renderer = [[AC_Renderer alloc] initWithComposition:url context:[glView openGLContext] pixelFormat:[glView pixelFormat]];
}

- (IBAction) open:(id)sender {
    
}

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification{
    return YES;
}

@end
