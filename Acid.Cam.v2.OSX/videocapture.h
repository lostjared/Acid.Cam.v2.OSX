/*
 * Acid Cam v2 - OpenCV Edition
 * written by Jared Bruni ( http://lostsidedead.com )
 * (C) 2017 GPL
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
extern unsigned long file_size;
extern std::ostringstream sout;
extern std::string add_path;
extern int key;
inline std::string boolStr(bool b) { if(b) return "True"; return "False"; };
extern void stopCV();
extern void setEnabledProg();
extern void stopRecord();
#endif
