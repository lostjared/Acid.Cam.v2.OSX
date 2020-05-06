#ifndef __SCREEN_H__
#define __SCREEN_H__
#ifdef __APPLE__
#import <AppKit/AppKit.h>
#include "ac.h"
@interface NSImage (NSImage_OpenCV) {    
}
@property(nonatomic, readonly) cv::Mat CVMat;
@end
#endif
#endif

