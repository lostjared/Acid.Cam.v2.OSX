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
 
 Copyright (c) 2019, Jared Bruni
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

@implementation AC_AppDelegate

@synthesize window;
@synthesize glView;
@synthesize profile;
@synthesize game_controller;
@synthesize FPS;

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication
{
    return YES;
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
    
    NSNotificationCenter * center = [NSNotificationCenter defaultCenter];
    [center addObserverForName: GCControllerDidConnectNotification object: nil queue: nil usingBlock: ^(NSNotification * note) {
        GCController *control = note.object;
        NSLog(@"Connected: %s\n", control.vendorName.UTF8String );
        [controller initControllers:self];
    }];
    [GCController startWirelessControllerDiscoveryWithCompletionHandler:^() {
        NSLog(@"End...");
    }];
}

- (void) applicationWillTerminate:(NSNotification *)notification
{	
    [syServer stop];
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
