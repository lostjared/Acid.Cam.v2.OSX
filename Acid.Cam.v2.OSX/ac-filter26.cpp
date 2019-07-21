
/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/lostsidedead
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
 BSD 2-Clause License
 
 Copyright (c) 2019, Jared Bruni
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

/*
 //Basic Multithreaded Filter
 auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
 for(int z = offset; z <  offset+size; ++z) {
 for(int i = 0; i < cols; ++i) {
 }
 }
 };
 UseMultipleThreads(frame, getThreadCount(), callback);
 AddInvert(frame);
 
 */

#include"ac.h"


void ac::EachFilterSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "EachFilterSubFilter")
        return;
    static int filter_num = 0;
    static int frame_count = 0, seconds = 0;
    if (++frame_count > static_cast<int>(fps)) {
        frame_count = 0;
        ++seconds;
    }
    if(seconds > 4) {
        frame_count = 0;
        seconds = 0;
        if(filter_num > ac::getFilterCount()-2) {
            filter_num = 0;
        } else {
            ++filter_num;
        }
        ac::release_all_objects();
    }
    std::string filter_name = ac::draw_strings[filter_num];
    if(filter_name == "EachFilterSubFilter") {
        if(filter_num < ac::getFilterCount()-2)
            filter_name = ac::draw_strings[++filter_num];
        else {
            filter_num = 0;
            filter_name = ac::draw_strings[filter_num];
        }
    }
    //std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
    pushSubFilter(subfilter);
    CallFilter(filter_name, frame);
    popSubFilter();
}

void ac::EachFilterRandomStartSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "EachFilterRandomStartSubFilter")
        return;
    static int filter_num = rand()%getFilterCount()-3;
    static int frame_count = 0, seconds = 0;
    if (++frame_count > static_cast<int>(fps)) {
        frame_count = 0;
        ++seconds;
    }
    if(seconds > 4) {
        frame_count = 0;
        seconds = 0;
        if(filter_num > ac::getFilterCount()-2) {
            filter_num = 0;
        } else {
            ++filter_num;
            std::cout << ac::draw_strings[filter_num] << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
        }
        //        ac::release_all_objects();
    }
    std::string filter_name = ac::draw_strings[filter_num];
    if(filter_name == "EachFilterRandomStartSubFilter") {
        if(filter_num < ac::getFilterCount()-2)
            filter_name = ac::draw_strings[++filter_num];
        else {
            filter_num = 0;
            filter_name = ac::draw_strings[filter_num];
        }
    }
    //std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
    pushSubFilter(subfilter);
    CallFilter(filter_name, frame);
    popSubFilter();
}

void ac::PixelPsychosis(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 50;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.add[j] = p.col[j];
                        p.col[j] += speed;
                        if(p.col[j] >= 225) {
                            p.dir[j] = 0;
                            p.col[j] = pixel[j];
                        }
                    } else {
                        p.col[j] -= speed;
                        p.add[j] = p.col[j];
                        if(p.col[j] <= 25) {
                            p.dir[j] = 1;
                            p.col[j] = pixel[j];
                        }
                    }
                    int val = p.col[j]^p.add[j];
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (val * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    BlendWithSource25(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::PixelCloud(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static const int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.add[j] = p.col[j];
                        p.col[j] += speed;
                        if(p.col[j] >= 255) {
                            p.dir[j] = 0;
                            p.col[j] = pixel[j];
                        }
                    } else {
                        p.col[j] -= speed;
                        p.add[j] = p.col[j];
                        if(p.col[j] <= 25) {
                            p.dir[j] = 1;
                            p.col[j] = pixel[j];
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.33) + (p.col[j]) * 0.33) + (p.add[j] * 0.33);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::PiixelXorBlendFrame(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    pix_container.insert(copy1);
    static const int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.add[j] += speed;
                        if(p.add[j] >= 255) {
                            p.dir[j] = 0;
                        }
                    } else {
                        p.add[j] -= speed;
                        if(p.add[j] <= 0) {
                            p.dir[j] = 1;
                        }
                    }
                    int val = p.col[j]^p.add[j];
                    pixel[j] = static_cast<unsigned char>((0.33 * pixel[j]) + (0.33 * val) + (0.33 * p.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelImageBlendFrame(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static const int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b &ipix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[i] == 1) {
                        p.col[j] += speed;
                        if(p.col[j] == ipix[j]) {
                            p.add[j] = p.col[j];
                            p.col[j] = ipix[j];
                        } else {
                            if(p.col[j] >= 255) {
                                p.dir[j] = 0;
                            }
                        }
                    } else {
                        if(p.col[j] == ipix[j]) {
                            p.add[j] = p.col[j];
                            p.col[j] = ipix[j];
                        } else {
                            p.col[j] -= speed;
                            if(p.col[j] <= 1) {
                                p.dir[j] = 1;
                            }
                        }
                    }
                    int val = p.col[j] ^ p.add[j];
                    pixel[j] = static_cast<unsigned char>((0.25 * pixel[j]) + (0.25 * val) +  (0.50 * ipix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ColorTransition(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::PixelCollectionRandom(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b value = collection.frames[rand()%MAX].template at<cv::Vec3b>(z, i);
                    pixel[j] = value[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionSubFilter")
        return;
    static constexpr int MAX = 8;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b value = collection.frames[rand()%MAX].template at<cv::Vec3b>(z, i);
                cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int off = pixel[j]-value[j];
                    if(!(abs(off) <= 55)) {
                        pixel[j] = sub[j];
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
