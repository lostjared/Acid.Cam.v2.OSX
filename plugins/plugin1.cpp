#include<opencv2/opencv.hpp>


extern "C" void filter(cv::Mat &frame) {

	for(int z = 0; z < frame.rows; ++z) {
		for(int i = 0; i < frame.cols; ++i) {
			frame.at<cv::Vec3b>(z, i) = cv::Vec3b(0, 0, 0);
		}
	}

}
