# Acid.Cam.v2.OSX

![ScreenShot1](https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/screens/acid.cam.img1.jpg?raw=true "screenshot1")

[VERSION: 2.49.0 (macOS)]

To be able to use all the filters in this app your system should have at lest 8 GB of ram.

An important note:

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

This project contains the compiled static libraries / include files required to build the program.

Project home page: http://lostsidedead.com/blog/?index=212

Watch this video for information how to compile: https://youtu.be/xMUnRKj4J7Y

Watch this video for information how to use the program: https://youtu.be/DpBrX2W8DAI

Watch this video for a tutorial on how to create custom filter plugins: https://youtu.be/hOnBUJIdAyU

- Jared Bruni
