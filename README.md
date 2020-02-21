# Acid.Cam.v2.OSX

![ScreenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/acid.cam.img1.jpg?raw=true "screenshot1")

[VERSION: 2.63.0 (macOS)]

If you want to support the development of this software (not required) you can do so here: https://lostsidedead.biz/showlove/anyamount.html

I can confirm Acid Cam works with Elgato Camlink 4K. With this device you can use any HDMI device as as Webcam source for Acid Cam.
Combined with OBS you can live stream your self filtering video games/yourself on your camcorder in real-time with audio.


*** IMPORTANT NOTE ***

To be able to use all the filters in this app your system should have at lest 8 GB of ram for 1080p video. For 4K higher Ram is recommended, but you can set the Max Stored Frames based on the resolution you are doing and when it is reached it will release the frames.
On my system for 2160p video when using something like a Random filter I set it to 400 frames for my system with 32 GB ram. You can set it lower based on your system ram level It is in the preferences window.
Some filters require more than the default amount of allocated frames. Usually these have the name 640 or 720 in the title. If you are running with enough ram please set the require frame limit before starting your session!

When writing C++ code using libacidcam do not call a mulithreaded filter from within another mulithreaded filter from within it's loop callback. 

Acid Cam distorts video to create art. It is designed to bo used with other software (Syphon)  or on its own. There is multiple versions of the program for macOS, Linux, and Windows as well as a command line version of the program. 
The program also has a live webcam feature. If you wish to use this feature you should use a lower resolution and not stack up to many filters. The majority of the time this program is more useful in video mode where it can process multiple filters in up to 2160p 4K resolution.

This version shares some code with libacidcam, so sometimes when I update the parts that are shared show up in both projects. libacidcam uses C++/OpenCV and Autotools, and this macOS program uses Objective-C++/Cocoa/OpenCV and Xcode

This project was created in hopes of providing some entertainment, or for the Glitch artist to give you an 
image/video to use with other tools.

I call the art that I create with Acid Cam "Acid Glitch", but you can call it whatever you want. It is just meant to be a tool to use with other programs to help aid in the creation of artistic images/videos.

Acid Cam's effects are created by using Acid Cam 'filters', or pieces of code that distort an image and can be combined to produce some interesting effects. The project currently has over 1,600 filters to mix and match and supports output as MPEG-4 or H.264. 

5.20.19: I upgraded the program's OpenCV library files to version 3.4.6, and some of it has changed. The OpenCV header files included now have a warning from possible misuse of a comma. Also, I am on Mojave, and the GUI scales windows up and down automatically while still having a resolution of 5K.  Problem is OpenCV has code that resizes the window, and it is not sized correctly. I will work on fixing these issues, but the version of OpenCV I compiled contains OpenCL support. Just on my computer with the new libraries, it moves much faster.

To watch all the videos I have created with this project checkout my YouTube playlist:
https://www.youtube.com/playlist?list=PLUgo--lX2kmr6ikfz0hPlez1-jbGAYuTZ

Download old versions of Acid Cam for macOS: http://lostsidedead.biz/osx

NOTE: Some of the filters that contain either a 720 or 1080 at the end require a lot of ram if you do not have enough the program will exit.

View Facebook page for Information and  sample videos: http://facebook.com/AcidCam

OpenCV3_4_1 (Newest)  Version is on the master branch  now, to use the old 2.4  version switch to oldmacosx branch
or for OpenCV 3 go to OpenCV3.

Newest version of the program now requires macOS High Sierra to run. 

To compile first clone then open with Xcode and build.

![ScreenShot](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/AcidCam2_ScreenShot.png?raw=true "screenshot")

The video Acid Cam outputs is saved at a very high bitrate. Sometimes you will need to lower the bitrate to view the video with a video player. I usually import the video into Final Cut Pro X then lower the bitrate by exporting it as H.264. Another program you could use would be Handbrake. The following image is of MediaInfo on a 4K video created with Acid Cam. The overall bitrate is very high.

![mediainfo](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/mediainfo4k.png?raw=true "mediainfo_ss")

or use FFMPEG like this:

    $ ffmpeg -i "input.file.mp4" -c:v libx265 -tag:v hvc1 -crf 18 output.file.mp4

This project contains the compiled static libraries / include files required to build the program.

Project home page: http://lostsidedead.com/blog/?index=212

Watch this video for information how to compile: https://youtu.be/xMUnRKj4J7Y

Watch this video for information how to use the program: https://youtu.be/DpBrX2W8DAI

Watch this video for a tutorial on how to create custom filter plugins: https://youtu.be/hOnBUJIdAyU

Information about the static build of OpenCV in this project:

General configuration for OpenCV 3.4.6 =====================================

Version control:               unknown



Platform:

Timestamp:                   2019-05-20T20:32:42Z

Host:                        Darwin 17.7.0 x86_64

CMake:                       3.14.1

CMake generator:             Unix Makefiles

CMake build tool:            /usr/bin/make

Configuration:               Release



CPU/HW features:

Baseline:

Dispatched code generation:  SSE4_1 SSE4_2 FP16 AVX AVX2 AVX512_SKX

requested:                 SSE4_1 SSE4_2 AVX FP16 AVX2 AVX512_SKX

SSE4_1 (12 files):         + SSE SSE2 SSE3 SSSE3 SSE4_1

SSE4_2 (1 files):          + SSE SSE2 SSE3 SSSE3 SSE4_1 POPCNT SSE4_2

FP16 (0 files):            + SSE SSE2 SSE3 SSSE3 SSE4_1 POPCNT SSE4_2 FP16 AVX

AVX (5 files):             + SSE SSE2 SSE3 SSSE3 SSE4_1 POPCNT SSE4_2 AVX

AVX2 (26 files):           + SSE SSE2 SSE3 SSSE3 SSE4_1 POPCNT SSE4_2 FP16 FMA3 AVX AVX2

AVX512_SKX (2 files):      + SSE SSE2 SSE3 SSSE3 SSE4_1 POPCNT SSE4_2 FP16 FMA3 AVX AVX2 AVX_512F AVX512_SKX



C/C++:

Built as dynamic libs?:      NO

C++ Compiler:                /Library/Developer/CommandLineTools/usr/bin/c++  (ver 10.0.0.10001044)

C++ flags (Release):         -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winit-self -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -Wno-comment -fdiagnostics-show-option -Wno-long-long -Qunused-arguments -Wno-semicolon-before-method-body -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden -O3 -DNDEBUG  -DNDEBUG

C++ flags (Debug):           -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winit-self -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -Wno-comment -fdiagnostics-show-option -Wno-long-long -Qunused-arguments -Wno-semicolon-before-method-body -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden -g  -O0 -DDEBUG -D_DEBUG

C Compiler:                  /Library/Developer/CommandLineTools/usr/bin/cc

C flags (Release):           -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winit-self -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -Wno-comment -fdiagnostics-show-option -Wno-long-long -Qunused-arguments -Wno-semicolon-before-method-body -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden -O3 -DNDEBUG  -DNDEBUG

C flags (Debug):             -fsigned-char -W -Wall -Werror=return-type -Werror=non-virtual-dtor -Werror=address -Werror=sequence-point -Wformat -Werror=format-security -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes -Wundef -Winit-self -Wpointer-arith -Wshadow -Wsign-promo -Wuninitialized -Winit-self -Wno-delete-non-virtual-dtor -Wno-unnamed-type-template-args -Wno-comment -fdiagnostics-show-option -Wno-long-long -Qunused-arguments -Wno-semicolon-before-method-body -ffunction-sections -fdata-sections  -fvisibility=hidden -fvisibility-inlines-hidden -g  -O0 -DDEBUG -D_DEBUG

Linker flags (Release):      -Wl,-dead_strip  

Linker flags (Debug):        -Wl,-dead_strip  

ccache:                      NO

Precompiled headers:         NO

Extra dependencies:          avformat m z avcodec avresample swscale avutil -framework VideoDecodeAcceleration bz2 -framework Accelerate -framework AVFoundation -framework CoreGraphics -framework CoreMedia -framework CoreVideo -framework QuartzCore -framework Cocoa -framework OpenCL /System/Library/Frameworks/Accelerate.framework

3rdparty dependencies:       libprotobuf libjpeg-turbo libwebp libpng libtiff IlmImf zlib ittnotify quirc ippiw ippicv



OpenCV modules:

To be built:                 calib3d core dnn features2d flann highgui imgcodecs imgproc ml objdetect shape stitching superres ts video videoio world

Disabled:                    java_bindings_generator photo python2 python_bindings_generator

Disabled by dependency:      videostab

Unavailable:                 cudaarithm cudabgsegm cudacodec cudafeatures2d cudafilters cudaimgproc cudalegacy cudaobjdetect cudaoptflow cudastereo cudawarping cudev java js python3 python3 viz

Applications:                -

Documentation:               NO

Non-free algorithms:         NO



GUI: 

Cocoa:                       YES

VTK support:                 NO



Media I/O: 

ZLib:                        build (ver 1.2.11)

JPEG:                        build-libjpeg-turbo (ver 2.0.2-62)

WEBP:                        build (ver encoder: 0x020e)

PNG:                         build (ver 1.6.36)

TIFF:                        build (ver 42 - 4.0.10)

OpenEXR:                     build (ver 1.7.1)

HDR:                         YES

SUNRASTER:                   YES

PXM:                         YES



Video I/O:

DC1394:                      NO

FFMPEG:                      YES

avcodec:                   YES (ver 57.25.0)

avformat:                  YES (ver 57.7.2)

avutil:                    YES (ver 55.20.0)

swscale:                   YES (ver 4.0.0)

avresample:                YES (ver 3.0.0)

AVFoundation:                YES



Parallel framework:            GCD



Trace:                         YES (with Intel ITT)



Other third-party libraries:

Intel IPP:                   2019.0.0 Gold [2019.0.0]

at:                   /Volumes/LostSideDrive-2/Downloads/opencv3/3rdparty/ippicv/ippicv_mac/icv

Intel IPP IW:                sources (2019.0.0)

at:                /Volumes/LostSideDrive-2/Downloads/opencv3/3rdparty/ippicv/ippicv_mac/iw

Lapack:                      YES (/System/Library/Frameworks/Accelerate.framework)

Eigen:                       YES (ver 3.3.1)

Custom HAL:                  NO

Protobuf:                    build (3.5.1)

OpenCL:                        YES (no extra features)

Include path:                NO

Link libraries:              -framework OpenCL

Python (for build):            /usr/bin/python2.7

Install to:                    /usr/local/opencv_3.4.6

- Jared Bruni
