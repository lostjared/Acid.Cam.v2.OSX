#include<opencv2/opencv.hpp>

// self alpha blend
extern "C" void filter(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j] + (alpha * pixel[j]));
            }
        }
    }
    if(dir == 1) {
        alpha += 0.01;
        if(alpha > 2.0) {
            dir = 0;
        }
    } else {
        alpha -= 0.01;
        if(alpha <= 0.3) {
            dir = 1;
        }
    }
}

