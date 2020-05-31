# Acid.Cam.v2.OSX

![ScreenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/acid.cam.img1.jpg?raw=true "screenshot1")

[VERSION: 2.74.0 (macOS)]


If you want to support the development of this software (not required) you can do so here: https://lostsidedead.biz/showlove/anyamount.html

Acid Cam is an Open Source project I initially began working on back in 2011. The first version of the program was written in Objective-C and worked with webcams only. The version of Acid Cam this document describes in the 2.0 series, which was rewritten from scratch in C++ with the macOS user interface being Objective-C++.
Acid Cam distorts video to create art. It is designed to be used with other software (Syphon) or on its own. There are multiple versions of the program for macOS, Linux, and Windows, as well as a command-line version of the program. The program also has a live webcam feature. If you wish to use this feature, you should use a lower resolution and not stack up to many filters. The majority of the time, this program is more useful in video mode, where it can process multiple filters in up to 2160p 4K resolution.

Some important Notes:

To be able to use all the filters in this app, your system should have at least 8 GB of Ram for 1080p video. For 4K higher Ram is recommended, but you can set the Max Stored Frames based on the resolution you are doing and when it is reached it will release the frames. On my system for 2160p video when using something like a Random filter I set it to 400 frames for my system with 32 GB ram. You can set it lower based on your system ram level It is in the preferences window. Some filters require more than the default amount of allocated frames. Usually these have the name 640 or 720 in the title. If you are running with enough Ram, please set the require frame limit before starting your session!

The video Acid Cam outputs is saved at a very high bitrate. Sometimes you will need to lower the bitrate to view the video with a video player. I usually import the video into Final Cut Pro X then lower the bitrate by exporting it as H.264. Another program you could use would be Handbrake. The following image is of MediaInfo on a 4K video created with Acid Cam. The overall bitrate is very high.

![mediainfo](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/mediainfo4k.png?raw=true "mediainfo_ss")

or use FFMPEG like this:

    $ ffmpeg -i "input.file.mp4" -c:v libx265 -tag:v hvc1 -crf 22 output.file.mp4

This version shares some code with libacidcam, so sometimes, when I update the parts that are shared show up in both projects. libacidcam uses C++/OpenCV and Autotools, and this macOS program uses Objective-C++/Cocoa/OpenCV and Xcode
This project was created in hopes of providing some entertainment, or for the Glitch artist to give you an image/video to use with other tools.
I call the art that I create with Acid Cam "Acid Glitch," but you can call it whatever you want. It is just meant to be a tool to use with other programs to help aid in the creation of artistic images/videos.
Acid Cam's effects are created by using Acid Cam 'filters', or pieces of code that distort an image and can be combined to produce some exciting effects. The project currently has over 2,000 filters to mix and match and supports output as MPEG-4 or H.264.
5.20.19: I upgraded the program's OpenCV library files to version 3.4.6, and some of it has changed. The OpenCV header files included now have a warning from possible misuse of a comma. Also, I am on Mojave, and the GUI scales windows up and down automatically while still having a resolution of 5K. The problem is OpenCV has code that resizes the window, and it is not sized correctly. I will work on fixing these issues, but the version of OpenCV I compiled contains OpenCL support. Just on my computer with the new libraries, it moves much faster.
I can confirm Acid Cam works with Elgato Camlink 4K. With this device, you can use any HDMI device as a Webcam source for Acid Cam. Combined with OBS, you can live stream your self filtering video games/yourself on your camcorder in real-time with audio.

The Start Session 

![creenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/session.png?raw=true "screenshot1")

The start session window is where you choose if you want to use a Webcam or a video file. You also can select the desired output format or if you want to record it. Checking repeat causes the video to loop. You can resize the output frame and stretch it if required. If stretch is not checked, the video will retain its original aspect ratio.

The Activity Log:

![creenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/log.png?raw=true "screenshot1")


The Activity Log shows information about the video and allows you to select a single filter, You can check fade, and it will fade from filter to filter when you change it. Negate returns the opposite or negative of the video. The RGB sliders offset the colors RGB values. The RGB order lets you rearrange the order of the component RGB values. The color map is using only a specific set of colors. Near the bottom of the window displays how many stored frames are allocated. The program caches frames to for a lot of the filters, and when it reaches a certain number, they are automatically released. You can see how many frames you want to go before each release. You can also release manually through the programs menu.

The preferences window:

![creenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/pref.png?raw=true "screenshot1")


The preferences window: contains a series of options you can set to change how some of the filters operate. Process mode is how the index variable is incremented and decremented for some of the older filters. Movement speed is for most animated filters movements speed changes how fast it moves in and out. Custom cycle delay is for filters that have the word delay in the name and how many frames it should wait before changing. BlendWithSource is how much percentage of the image do you wish to restore. Threads are how many threads to use for the filters that are multithreaded. Key tolerance is for the Image windows Chroma Key features. Pixel difference is for the ones that detect a change in the image, how much of a difference for it to trigger that it has changed. Max stored frames is how many frames to store before resetting the cache. FFMPEG Path is if you optionally want to install FFMPEG, you can point it to the path for the program to find it. Syphon output is for using a Syphon server what resolution you want to use. Frame wait is for how long to wait for ones that have wait in the name. Stretch Line is for StretchLineRowInc and StretchLineColInc how much distortion you want. The higher the value, the more distorted it gets.

![creenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/image.png?raw=true "screenshot1")


The image window you select a series of images, and they appear in the dropdown list. You can then select one and set it as one of the image types like Blend Image. You can also set a chroma key, and you can set a second video file. The Cycle option is how many frames do you want to wait before it changes the currently selected image from the dropdown list you selected. You can see to shuffle the images, move at random or in order.

![creenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/keys.png?raw=true "screenshot1")


The Chroma key window you select a series of color ranges. You can set the component colors then set whether you want a range of tolerance. When you have your keys in the list, click set colors to activate it. Then select a filter that uses a chroma key.

![creenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/custom.png?raw=true "screenshot1")


The custom filter window, this is probably the most important window in the application. You can select multiple filters and have them executed one after the other, with ones output being the input to the next in a chain. You can toggle a filter on or off while in a custom. You can save and load a custom. User-defined is when you select a list of filters and assign it a new name so you can use it. You can save/load these user-defined lists, and you should first set the output directory by pressing the dir button to a location you have read/write access.
Subfilter: A subfilter is when you pass a specific filter to another filter that has the word SubFilter in its name. It magnifies the amount of possible outputs.

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
