#include<opencv2/opencv.hpp>

// strobe
extern "C" void filter(cv::Mat &frame) {
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[index] = 255;
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}

