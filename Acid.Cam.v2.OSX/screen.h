#ifndef __SCREEN_H__
#define __SCREEN_H__
#ifdef __APPLE__
#include"ac.h"
#import <AppKit/AppKit.h>
@interface NSImage (NSImage_OpenCV) {
}
@property(nonatomic, readonly) cv::Mat CVMat;
@end
#endif
#endif
