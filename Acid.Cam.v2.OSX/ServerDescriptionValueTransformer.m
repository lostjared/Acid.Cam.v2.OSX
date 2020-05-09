/*
 ServerDescriptionValueTransformer.m
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

#import "ServerDescriptionValueTransformer.h"
#import <Syphon/Syphon.h>

@implementation ServerDescriptionValueTransformer
+ (Class)transformedValueClass { return [NSString class]; }
+ (BOOL)allowsReverseTransformation { return NO; }

- (id)transformedValue:(id)value
{
	if ([value isKindOfClass:[NSArray class]])
	{
        NSMutableArray *transformed = [NSMutableArray arrayWithCapacity:[value count]];
        for (NSDictionary *description in value) {
            
            // These are the keys we can use in the server description dictionary.
            // Be prepared for either of the keys to be missing.
            
            NSString* name = [description objectForKey:SyphonServerDescriptionNameKey];
            NSString* appName = [description objectForKey:SyphonServerDescriptionAppNameKey];
            
            // A server may not have a name (usually if it is the only server in an application)
            
            NSString *title;
            if ([appName length] > 0)
            {
                if ([name length] > 0)
                {
                    title = [name stringByAppendingFormat:@" - %@", appName, nil];
                }
                else
                {
                    title = appName;
                }
            }
            else if ([name length] > 0)
            {
                title = name;
            }
            else
            {
                title = @"Untitled Server";
            }
            
            [transformed addObject:title];
        }
        return transformed;
	}
	return nil;
}	
@end
