/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com / https://github.com/lostjared )
 
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

#ifndef Acid_Cam_2_videocapture_h
#define Acid_Cam_2_videocapture_h

#include"ac.h"
#include<sstream>
#include<memory>
#include<string>


extern bool breakProgram, programRunning, stopProgram;
extern bool isPaused;
extern bool rec_Audio;
extern std::string input_name;
extern unsigned int total_frames;
extern int program_main(int outputType, std::string input_file, bool noRecord, std::string outputFileName, int capture_width, int capture_height, int capture_device, int frame_count, float pass2_alpha, std::string file_path);
extern std::unique_ptr<cv::VideoWriter> writer;
extern std::fstream file;
extern std::unique_ptr<cv::VideoCapture> capture_camera;
extern std::unique_ptr<cv::VideoCapture> capture_video;
extern cv::VideoCapture *capture;
extern int frame_cnt;
extern int video_total_frames;
extern unsigned long file_size;
extern std::ostringstream sout;
extern std::string add_path;
extern int key;
inline std::string boolStr(bool b) { if(b) return "True"; return "False"; };
extern void stopCV();
extern void setEnabledProg();
extern void stopRecord();
#endif
