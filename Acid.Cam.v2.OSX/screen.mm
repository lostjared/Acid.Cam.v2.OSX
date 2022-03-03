// code is modified from github:

#include"screen.h"
#ifdef __APPLE__

NSImage *CaptureScreenRect(int x, int y, int w, int h) {
//    NSRect screenRect = [[NSScreen mainScreen] frame];
    NSRect screenRect;
    screenRect.size.height=h;
    screenRect.size.width =w;
    screenRect.origin.x = x;
    screenRect.origin.y = y;
    CGImageRef cgImage = CGWindowListCreateImage(screenRect, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithCGImage:cgImage];
    CGImageRelease(cgImage);
    NSImage *image = [[NSImage alloc] init];
    [image addRepresentation:rep];
    return image;
}

NSImage *CaptureScreen() {
     NSRect screenRect = [[NSScreen mainScreen] frame];
    CGImageRef cgImage = CGWindowListCreateImage(screenRect, kCGWindowListOptionOnScreenOnly, kCGNullWindowID, kCGWindowImageDefault);
    NSBitmapImageRep *rep = [[NSBitmapImageRep alloc] initWithCGImage:cgImage];
    CGImageRelease(cgImage);
    NSImage *image = [[NSImage alloc] init];
    [image addRepresentation:rep];
    return image;
}

@implementation NSImage (NSImage_OpenCV)

-(CGImageRef)CGImage
{
    
    NSImage *image = self; // an image
    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    CGRect imageCGRect = CGRectMake(0, 0, image.size.width, image.size.height);
    NSRect imageRect = NSRectFromCGRect(imageCGRect);
    CGImageRef imageRef = [image CGImageForProposedRect:&imageRect context:context hints:nil];
    return imageRef;
}



-(cv::Mat)CVMat
{
    CGImageRef imageRef = [self CGImage];
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(imageRef);
    CGFloat cols = self.size.width;
    CGFloat rows = self.size.height;
    cv::Mat cvMat(rows, cols, CV_8UC4); // 8 bits per component, 4 channels
    
    CGContextRef contextRef = CGBitmapContextCreate(cvMat.data,                 // Pointer to backing data
                                                    cols,                      // Width of bitmap
                                                    rows,                     // Height of bitmap
                                                    8,                          // Bits per component
                                                    cvMat.step[0],              // Bytes per row
                                                    colorSpace,                 // Colorspace
                                                    kCGImageAlphaNoneSkipLast |
                                                    kCGBitmapByteOrderDefault); // Bitmap info flags
    
    CGContextDrawImage(contextRef, CGRectMake(0, 0, cols, rows), imageRef);
    CGContextRelease(contextRef);
    CGImageRelease(imageRef);
    return cvMat;
}

@end


void ScreenGrab(cv::Mat &frame) {
    NSImage *cap = CaptureScreen();
    frame = [cap CVMat];
    [cap release];
}

void ScreenGrabRect(int x, int y, int w, int h, cv::Mat &frame) {
    NSImage *cap = CaptureScreenRect(x,y,w,h);
    frame = [cap CVMat];
    [cap release];
}

void convertTo(NSImage *image, cv::Mat &frame) {
    frame = [image CVMat];
}

#endif
