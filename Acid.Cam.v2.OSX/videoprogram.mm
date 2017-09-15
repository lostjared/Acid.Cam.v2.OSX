/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com / https://github.com/lostjared )
 
 
 GitHub: http://github.com/lostjared
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 BSD 2-Clause License
 
 Copyright (c) 2017, Jared Bruni
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


#import"AC_Controller.h"
#include<fstream>
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#undef check
#include"videocapture.h"

unsigned int int_Seed = (unsigned int)(time(0));
bool breakProgram = false, programRunning = false, stopProgram = false;
unsigned int total_frames = 0;
void ProcFrame(cv::Mat &frame);
std::unique_ptr<cv::VideoCapture> capture_camera(new cv::VideoCapture());
std::unique_ptr<cv::VideoCapture> capture_video(new cv::VideoCapture());
int frame_cnt, key;
int video_total_frames = 0;
NSTimer *renderTimer;
std::ostringstream sout;
std::unique_ptr<cv::VideoWriter> writer;
std::fstream file;
unsigned long file_size;
std::string add_path;


std::string input_name = "";

void stopCV() {
    if(camera_active == true) {
        camera_active = false;
        return;
    }
    if(renderTimer != nil && renderTimer.valid) {
        [renderTimer invalidate];
        renderTimer = nil;
        cv::destroyWindow("Acid Cam v2");
        cv::destroyWindow("Controls");
        if(!ac::noRecord && writer->isOpened()) {
            sout << "Wrote to Video File: " << ac::fileName << "\n";
            writer->release();
        }
        sout << (video_total_frames+frame_cnt) << " Total frames\n";
        sout << ((video_total_frames+frame_cnt)/ac::fps) << " Seconds\n";
        file.close();
        // flush to log
        flushToLog(sout);
        setEnabledProg();
        [controller stopCV_prog];
        if(breakProgram == true) {
            [NSApp terminate:nil];
        }
    }
}


int program_main(int outputType, std::string input_file, bool noRecord, std::string outputFileName, int capture_width, int capture_height, int capture_device, int frame_countx, float pass2_alpha, std::string file_path) {
    programRunning = true;
    sout << "Acid Cam v" << ac::version << " Initialized ..\n";
    add_path="default";
    input_name = input_file;
    srand(static_cast<unsigned int>(time(0)));
    ac::translation_variable = 0.01;
    ac::tr = ac::translation_variable;
    ac::fileName = file_path+outputFileName;
    ac::noRecord = noRecord;
    ac::pass2_alpha = pass2_alpha;
    unsigned int seed = int_Seed;
    srand(seed);
    add_path = file_path;
    stopProgram = false;
    ac::tr =  0.3;
    ac::fps = 29.97;
    file_size = 0;
    video_total_frames = 0;
    writer.reset(new cv::VideoWriter());
    try {
        if(camera_mode == 0 && capture->isOpened() == false) capture->open(capture_device);
        else if(camera_mode == 1)  {
            capture->open(input_file);
            total_frames = capture->get(CV_CAP_PROP_FRAME_COUNT);
        }
        if (!capture->isOpened()) {
            std::cerr << "Error could not open Camera device..\n";
            return -1;
        } else
            sout << "Acid Cam Capture device [" << ((camera_mode == 0) ? "Camera" : "Video") << "] opened..\n";
        int aw = capture->get(CV_CAP_PROP_FRAME_WIDTH);
        int ah = capture->get(CV_CAP_PROP_FRAME_HEIGHT);
        sout << "Resolution: " << aw << "x" << ah << "\n";
        ac::fps = capture->get(CV_CAP_PROP_FPS);
        if(ac::fps_force == false && input_file.size() != 0) ac::fps = capture->get(CV_CAP_PROP_FPS);
        sout << "FPS: " << ac::fps << "\n";
        cv::Mat frame;
        capture->read(frame);
        cv::Size frameSize = frame.size();
        if(camera_mode == 0 && capture_width != 0 && capture_height != 0) {
            capture->set(CV_CAP_PROP_FRAME_WIDTH, capture_width);
            capture->set(CV_CAP_PROP_FRAME_HEIGHT, capture_height);
            sout << "Resolution set to " << capture_width << "x" << capture_height << "\n";
            frameSize = cv::Size(capture_width, capture_height);
        }
        setSliders(total_frames);
        if(ac::noRecord == false) {
            if(outputType == 0)
                writer->open(ac::fileName, CV_FOURCC('m','p','4','v'),  ac::fps, frameSize, true);
            else
                writer->open(ac::fileName, CV_FOURCC('X','V','I','D'),  ac::fps, frameSize, true);
            
            if(writer->isOpened() == false) {
                sout << "Error video file could not be created.\n";
                exit(0);
            }
            cv::Mat outframe;
            cv::resize(frame, outframe, frameSize);
            frame = outframe;
            ac::orig_frame = frame.clone();
            old_frame = frame.clone();
            blend_image = frame.clone();
            if(disableFilter == false) ac::draw_func[ac::draw_offset](frame);
            writer->write(frame);
            file.open(ac::fileName.c_str(), std::ios::in);
        }
        if(ac::noRecord == false)
            sout << "Now recording .. format " << ((outputType == 0) ? "MPEG-4 (Quicktime)" : "XvID") << " \n";
        else
            sout << "Recording disabled ..\n";
        cv::namedWindow("Acid Cam v2");
        // flush to log
        flushToLog(sout);
        frame_cnt = 0;
        [[NSRunLoop currentRunLoop] addTimer:renderTimer
                                     forMode:NSEventTrackingRunLoopMode];
        
        [[NSRunLoop currentRunLoop] addTimer:renderTimer
                                     forMode:NSDefaultRunLoopMode];
        return 0;
    }
    catch(std::exception &e) {
        std::cerr << e.what() << " was thrown!\n";
    }
    catch(...) {
        std::cerr << "Unknown error thrown.\n";
    }
    return 0;
}

std::ostringstream strout;

void jumptoFrame(int frame) {
    capture->set(CV_CAP_PROP_POS_FRAMES,frame);
    frame_cnt = frame;
}
