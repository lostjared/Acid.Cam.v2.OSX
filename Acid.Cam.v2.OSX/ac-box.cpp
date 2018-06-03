#include"ac.h"

unsigned int ac::Box::frame_width = 0;
unsigned int ac::Box::frame_height = 0;

void ac::Box::initBox(int width, int height) {
    x = (rand()%width);
    y = (rand()%height);
    w = rand()%25;
    h = rand()%25;
    steps = rand()%10;
    index = 0;
    do {
        frame_index = rand()%28;
    } while(frame_index == 13 || frame_index == 14);
}

void ac::Box::drawBox(cv::Mat &frame) {
    cv::Mat temp;
    temp.create(cvSize(w, h), CV_8UC3);
    for(int yy = y, pos_y = 0; yy < y+h && yy < frame_height; ++yy, ++pos_y) {
        for(int ii = x,pos_x = 0; ii < x+w && ii < frame_width; ++ii, ++pos_x) {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(yy, ii);
            cv::Vec3b &target = temp.at<cv::Vec3b>(pos_y, pos_x);
            target = pixel;
        }
    }
    ac::draw_func[frame_index](temp);
    for(int z = y, pos_y = 0; z < y+h && z < frame_height; ++z, ++pos_y) {
        for(int i = x, pos_x = 0; i < x+w && i < frame_width; ++i, ++pos_x) {
            if(i < frame.cols && z < frame.rows) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                pixel = temp.at<cv::Vec3b>(pos_y,pos_x);
            }
        }
    }
}

void ac::Box::sizeBox() {
    if(index > steps) {
        initBox(frame_width, frame_height);
        return;
    }
    ++index;
    unsigned int r1 = rand()%10;
    unsigned int r2 = rand()%10;
    if(w+r1 > frame_width) {
        initBox(frame_width, frame_height);
        return;
    } else {
        w += r1;
    }
    if(h+r2 > frame_height) {
        initBox(frame_width, frame_height);
        return;
    } else {
        h += r2;
    }
}

