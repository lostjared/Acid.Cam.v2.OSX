# Acid.Cam.v2.OSX

![ScreenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/acid.cam.img1.jpg?raw=true "screenshot1")

[VERSION: 2.84.0 (macOS)]

Acid Cam, a project under Open Source license, was initiated in 2011. Its inaugural iteration, developed in Objective-C, was tailored exclusively for webcam utilization. The current version, part of the 2.0 series, has been entirely redeveloped in C++ for core functionalities, with Objective-C++ employed for the macOS user interface.

Acid Cam employs video distortion techniques to generate artistic visuals. It is engineered for integration with additional software (notably Syphon) or standalone operation. The software is available across multiple platforms, including macOS, Linux, and Windows, and offers both graphical and command-line interfaces. A notable feature is its live webcam capability, which is optimized at lower resolutions to prevent filter overloading. Primarily, the software excels in video mode, supporting the processing of numerous filters at resolutions up to 2160p 4K.

Critical Technical Specifications:

Memory Requirements: A minimum of 8 GB of RAM is required for processing 1080p video content. For 4K video, a higher RAM allocation is advisable. The application allows for the adjustment of Max Stored Frames to manage memory consumption relative to video resolution. For instance, on a system equipped with 32 GB of RAM, setting the frame cap at 400 frames is recommended for 2160p videos using intensive filters such as Random. This cap can be adjusted in the preferences to suit the available system memory.
Frame Allocation for Filters: Certain filters necessitate an increase in the default frame allocation, often indicated by the inclusion of '640' or '720' in their titles. Adequate RAM is essential for these filters, and users should adjust the frame limit accordingly prior to initiating a session.
Bitrate Considerations for Output Videos:

Acid Cam generates videos at significantly high bitrates, which may necessitate compression for compatibility with standard video players. A common workflow involves importing the high-bitrate footage into Final Cut Pro X and exporting it in H.264 format to reduce the bitrate. Alternatively, Handbrake can be employed for bitrate adjustment. The high bitrate characteristic of Acid Cam's output, particularly in 4K videos, underscores the software's capacity for producing high-quality visual art.

![mediainfo](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/mediainfo4k.png?raw=true "mediainfo_ss")

or use FFMPEG like this:

    $ ffmpeg -i "input.file.mp4" -c:v libx265 -tag:v hvc1 -crf 22 output.file.mp4

This iteration of the software shares code with libacidcam, leading to updates in shared components reflecting in both projects simultaneously. libacidcam is developed using C++ with OpenCV and employs Autotools, while the macOS application utilizes Objective-C++ alongside Cocoa and OpenCV, with Xcode as the development environment.

The inception of this project was driven by the desire to offer entertainment and serve as a resource for Glitch artists, providing them with distinctive images or videos for further creative application. The art produced with Acid Cam, termed "Acid Glitch," is open for interpretation, allowing users to define their creations in their own terms. Essentially, Acid Cam is designed as a complementary tool for other software, facilitating the creation of artistic images or videos.

Acid Cam generates its unique effects through "filters" – segments of code designed to alter an image in various ways, which can be layered together to achieve intriguing effects. Currently, the project boasts over 2,000 filters available for experimentation, supporting outputs in MPEG-4 or H.264 formats.

As of May 20, 2019, the software underwent an upgrade to its OpenCV library components to version 3.4.6, introducing changes including warnings in the OpenCV header files regarding potential comma misuse. Operating on Mojave, I've observed that the GUI automatically adjusts window sizes, yet maintains a 5K resolution. This adjustment conflicts with OpenCV's window resizing code, leading to improper sizing. Efforts to resolve these issues are underway, particularly as the compiled version of OpenCV now includes OpenCL support, significantly enhancing processing speed on my setup.

Moreover, Acid Cam has been confirmed to be compatible with Elgato Camlink 4K. This compatibility allows for the use of any HDMI-enabled device as a webcam source in Acid Cam. When used in conjunction with OBS, it enables live streaming of video game playthroughs or personal camcorder footage with real-time filtering and audio integration.
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


he Custom Filter Window stands as a pivotal feature within the application, enabling users to sequence multiple filters for consecutive execution, where the output of one serves as the input for the next, creating a dynamic filter chain. Users have the flexibility to activate or deactivate filters within a custom sequence and possess the capability to both save and retrieve these custom configurations. A unique aspect involves creating "user-defined" filter lists, which are custom selections of filters that are assigned distinctive names for easy reuse. It's essential to specify an output directory with read/write permissions by clicking the 'dir' button before initiating this process.

A "subfilter" refers to the application of a particular filter within another filter designated by the inclusion of "SubFilter" in its name, significantly expanding the potential for varied outputs.

For those interested in previous iterations of Acid Cam for macOS, they can be found at: http://lostsidedead.biz/osx.

Note: Filters labeled with "720" or "1080" may consume substantial RAM. Insufficient memory will cause the application to terminate.

Updates, information, and sample videos are available on our Facebook page: http://facebook.com/AcidCam.

The latest version of Acid Cam, compatible with OpenCV version 3.4.1, resides on the master branch. Users wishing to utilize OpenCV version 2.4 or OpenCV 3 should switch to the oldmacosx branch or the OpenCV3 branch, respectively.

The most current version of the software now necessitates macOS High Sierra for operation.

To begin compilation, clone the repository, then proceed to open the project in Xcode and execute the build process.
![ScreenShot](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/AcidCam2_ScreenShot.png?raw=true "screenshot")

Project home page: http://lostsidedead.com

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
