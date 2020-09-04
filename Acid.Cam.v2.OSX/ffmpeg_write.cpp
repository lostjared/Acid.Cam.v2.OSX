
#include "ffmpeg_write.h"
#include<sstream>
#include<fstream>
#ifndef _WIN32
#include<unistd.h>
#endif
// ffmpeg -y -s 640x480  -pixel_format bgr24 -f rawvideo -r 30 -i pipe: -vcodec libx265 -pix_fmt yuv420p -crf 24 test.mp4
#ifdef __APPLE__
std::string ffmpeg_path_str = "ffmpeg";
#endif
extern void sendString(const std::string &s);

char buffer[1024*1024];
std::fstream file;
int stdout_save;

FILE *open_ffmpeg(const char *output, const char *codecx, const char *res, const char *dst_res, const char *fps, const char *crf) {
#ifndef _WIN32
    std::string tag;
    if(std::string(codecx)=="libx265")
        tag = "-tag:v hvc1";
    
    std::ostringstream stream;
    stream << ffmpeg_path_str << " -y -s " << dst_res << " -pixel_format bgr24 -f rawvideo -r " << fps << " -i pipe: -vcodec " << codecx << " -pix_fmt yuv420p " <<  tag << " -crf " << crf << " " <<  output;
    std::cout<<"acidcam: " << stream.str() << "\n";
    
#ifndef _WIN32
    FILE *fptr = popen(stream.str().c_str(), "w");
#else
    FILE *fptr = _popen(stream.str().c_str(), "w");
#endif
    
    if(!fptr) {
        std::cerr << "Error: could not open ffmpeg\n";
        return 0;
    }
    
    return fptr;
#endif
    return 0;
}

//ffmpeg -f avfoundation -i ":iShowU Audio Capture" -acodec libmp3lame -ab 128k -f mp3 -

void list_devices() {
#ifdef __APPLE__
    std::string s = ffmpeg_path_str + " -list_devices true -f avfoundation -i dummy";
    FILE *fptr = popen(s.c_str(), "r");
    if(!fptr) {
        std::cout << "acidcam: Error: could not read file...\n";
        exit(0);
    }
    while(!feof(fptr)) {
        char buf[1024];
        fgets(buf, 1024, fptr);
        std::cout << buf;
    }
    pclose(fptr);
#endif
}

void write_ffmpeg(FILE *fptr, cv::Mat &frame) {
    fwrite(frame.ptr(), sizeof(char), frame.total()*frame.elemSize(), fptr);
}

void close_stdout() {
    
}

void mux_audio(const char *output, const char *src, const char *final_file) {
#ifndef _WIN32
    std::ostringstream stream;
    stream << ffmpeg_path_str << " -y -i " << output << " -i " << src << " -c copy -map 0:v:0 -map 1:a:0? -shortest " << final_file;
    std::cout << "acidcam: " << stream.str() << "\n";
#ifndef _WIN32
    FILE *fptr = popen(stream.str().c_str(), "r");
#else
    FILE *fptr = _popen(stream.str().c_str(), "r");
#endif
    if(!fptr) {
        std::cerr << "Error: could not open ffmpeg\n";
        return;
    }
    while(!feof(fptr)) {
        char buf[256];
        fgets(buf, 256, fptr);
        std::cout << buf;
#ifdef SYPHON_SERVER
        sendString(buf);
#endif
    }
#ifndef _WIN32
    pclose(fptr);
#else
    _pclose(fptr);
#endif
#endif
}
