/*
 ToolbarDelegate.m
 Syphon (SDK)
 
 Copyright 2011 bangnoise (Tom Butterworth) & vade (Anton Marini).
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

#import "ToolbarDelegate.h"

@implementation ToolbarDelegate  {
    IBOutlet NSPopUpButton  *availableServersMenu;
    IBOutlet NSBox *statusBox;
}

- (NSArray *)toolbarDefaultItemIdentifiers:(NSToolbar *)toolbar
{
    return [NSArray arrayWithObjects:@"ServersMenuItemIdentifier",
            NSToolbarFlexibleSpaceItemIdentifier, @"StatusItemIdentifier",
            NSToolbarFlexibleSpaceItemIdentifier, @"FixedWidthItemIdentifier", nil];
}

- (NSArray *)toolbarAllowedItemIdentifiers:(NSToolbar *)toolbar
{
    return [NSArray arrayWithObjects:@"ServersMenuItemIdentifier", NSToolbarFlexibleSpaceItemIdentifier, @"StatusItemIdentifier", @"FixedWidthItemIdentifier", nil];
}

- (NSToolbarItem *)toolbar:(NSToolbar *)toolbar itemForItemIdentifier:(NSString *)itemIdentifier willBeInsertedIntoToolbar:(BOOL)flag
{
    NSToolbarItem *item = [[NSToolbarItem alloc] initWithItemIdentifier:itemIdentifier];
    if ([itemIdentifier isEqualToString:@"ServersMenuItemIdentifier"])
    {
        [item setLabel:@"Source"];
        [item setPaletteLabel:@"Source"];
        [item setToolTip:@"Select a Syphon Server"];
        [item setView:availableServersMenu];
        [item setMinSize:(NSSize){[availableServersMenu frame].size.width / 3.0, [availableServersMenu frame].size.height}];
        [item setMaxSize:[availableServersMenu frame].size];
        NSMenuItem *menuForm = [[NSMenuItem alloc] init];
        [menuForm setMenu:[availableServersMenu menu]];
        [item setMenuFormRepresentation:menuForm];
    }
    else if ([itemIdentifier isEqualToString:@"StatusItemIdentifier"])
    {
        [item setLabel:@"Status"];
        [item setPaletteLabel:@"Status"];
        [item setToolTip:@"Status"];
        [statusBox setCornerRadius:4.0];
        [item setView:statusBox];
    }
    else if ([itemIdentifier isEqualToString:@"FixedWidthItemIdentifier"])
    {
        // This is an invisible item with the same sizing behaviour as the menu, to keep the status centered
        NSView *empty = [[NSView alloc] initWithFrame:[availableServersMenu frame]];
        [item setView:empty];
        [item setMinSize:(NSSize){[empty frame].size.width / 3.0, [empty frame].size.height}];
        [item setMaxSize:[empty frame].size];
    }
    else
    {
        NSLog(@"Unexpect toolbar item %@", itemIdentifier);
        item = nil;
    }
    return item;
}
@end
