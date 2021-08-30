
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
 
 Copyright (c) 2020, Jared Bruni
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
    static std::string temp_sub = solo_filter[rand()%(solo_filter.size()-1)];
    
    if (++frame_count > static_cast<int>(fps)) {
        frame_count = 0;
        ++seconds;
    }
    if(seconds > 1) {
        frame_count = 0;
        seconds = 0;
        if(filter_num > ac::getFilterCount()-2) {
            filter_num = 0;
        } else {
            ++filter_num;
            temp_sub = solo_filter[rand()%(solo_filter.size()-1)];
        }
    }
    std::string filter_name = ac::draw_strings[filter_num];
    if(filter_name == "EachFilterSubFilter") {
        if(filter_num < ac::getFilterCount()-2) {
            filter_name = ac::draw_strings[++filter_num];
            temp_sub = solo_filter[rand()%(solo_filter.size()-1)];
        }
        else {
            filter_num = 0;
            filter_name = ac::draw_strings[filter_num];
            temp_sub = solo_filter[rand()%(solo_filter.size()-1)];
        }
    }
#ifdef DEBUG_MODE
    if(filter_name.find("SubFilter") == std::string::npos)
        std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
    else
        std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << " SubFilter: " << temp_sub << "\n";
#endif
    int temp = getFilterByName(temp_sub);
    int cur = getFilterByName(filter_name);
    if(temp == cur) {
        temp = getFilterByName("MedianBlendIncrease");
    }
    pushSubFilter(temp);
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
#ifdef DEBUG_MODE
    std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
#endif
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
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.33) + static_cast<unsigned char>(p.col[j]) * 0.33) + static_cast<unsigned char>(p.add[j] * 0.33);
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
                if(z < frame->rows-1 && i < frame->cols-1 && z < reimage.rows-1 && i < reimage.cols-1) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    PixelValues &p = pix_container.pix_values[i][z];
                    cv::Vec3b &ipix = reimage.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        if(p.dir[j] == 1) {
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
                    if(!(abs(off) <= getPixelCollection())) {
                        pixel[j] = sub[j];
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelCollectionMatrixSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionMatrixSubFilter")
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
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= getPixelCollection())) {
                            pixel[j] = sub[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::PixelCollectionIncreaseSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionIncreaseSubFilter")
        return;
    static constexpr int MAX = 8;
    static double increase = 5;
    static int dir = 1;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= static_cast<int>(increase))) {
                            pixel[j] = sub[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(increase, dir, 6.0, 70.0, 6.0);
    AddInvert(frame);
}

void ac::PixelCollectionImageBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static constexpr int MAX = 32;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat orig = frame.clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b source = reimage.at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = reimage.template at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= 55)) {
                            pixel[j] = sub[j];
                            pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * source[j]));
                            
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorPulseIncrease(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    
    // reset to default
    if(reset_alpha == true) {
        for(int j = 0; j < 3; ++j) {
            alpha[j] = 0;
            start[j] = 0;
            stop[j] = 0;
        }
        inc = 4.0;
        dir[0] = 1;
        dir[1] = 0;
        dir[2] = 1;
        start_init[0] = 5;
        start_init[1] = 5;
        start_init[2] = 5;
        stop_max[0] = 200;
        stop_max[1] = 210;
        stop_max[2] = 100;
        start_max[0] = 25;
        start_max[1] = 10;
        start_max[2] = 5;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.5) + ((alpha[j] * 0.5)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::PixelCollectionLongMatrixSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "PixelCollectionLongMatrixSubFilter")
        return;
    static constexpr int MAX = 32;
    static MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    cv::Mat copyf = frame.clone();
    CallFilter(subfilter, copyf);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int v = 1; v < collection.size() && !collection.frames[v].empty(); ++v) {
                    cv::Vec3b value = collection.frames[v].at<cv::Vec3b>(z, i);
                    cv::Vec3b sub = copyf.at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int off = pixel[j]-value[j];
                        if(!(abs(off) <= 55)) {
                            pixel[j] = sub[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorPulseImageIncreaseSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ColorPulseImageIncreaseSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.33) + ((alpha[j] * 0.33)) + (0.33 * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::ColorPulseRandom(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0,0,0}, start_init[3] = {static_cast<double>(rand()%5), static_cast<double>(rand()%5), static_cast<double>(rand()%5)}, start_max[3] = {static_cast<double>(rand()%25), static_cast<double>(rand()%10), static_cast<double>(rand()%5)};
    static double stop[3] = {0,0,0}, stop_init[3] = {static_cast<double>(rand()%5), static_cast<double>(rand()%5), static_cast<double>(rand()%5)}, stop_max[3] = {static_cast<double>(rand()%200), static_cast<double>(rand()%210), static_cast<double>(rand()%180)};
    static double inc = static_cast<double>(rand()%4)+4.0;
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.5) + ((alpha[j] * 0.5)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AddInvert(frame);
}

void ac::ColorPulseImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double scale = 1.0;
    static int dir1 = 1;
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] =  static_cast<unsigned char>((pixel[j] * 0.33) + ((alpha[j] * 0.33)) + (0.33 * (scale * img[j])));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AlphaMovementMaxMin(scale, dir1, 0.01,5.0, 0.1);
    AddInvert(frame);
}

void ac::ColorPulseAlpha(cv::Mat &frame) {
    static double alpha[3] = {0};
    static double start[3] = {0, 0, 0}, start_init[3] = {5, 5, 5}, start_max[3] = {25, 10, 5};
    static double stop[3] = {0,0,0}, stop_init[3] = {5, 5, 5}, stop_max[3] = {200, 210, 180};
    static double inc = 4.0;
    static int dir[3] = {1, 0, 1};
    static int index = 0;
    int colors[3] = {0};
    static double scale = 1.0;
    static int dir1 = 1;
    InitArrayPosition(colors,index);
    ++index;
    if(index > 2)
        index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[colors[j]] =  static_cast<unsigned char>((pixel[j] * scale) + ((alpha[j] * 1-scale)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    VariableScale(alpha,dir,start, start_init, start_max, stop, stop_init, stop_max, inc);
    AlphaMovementMaxMin(scale, dir1, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::ColorLower75(cv::Mat &frame) {
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(0.75 * pixel[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[3].clone();
    frames[2] = collection.frames[7].clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b cpix[3];
                cpix[0] = frames[0].at<cv::Vec3b>(z, i);
                cpix[1] = frames[1].at<cv::Vec3b>(z, i);
                cpix[2] = frames[2].at<cv::Vec3b>(z, i);
                cv::Vec3b img;
                img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.33) + (cpix[j][j] * 0.33) + (img[j] * 0.33));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorDullBlur(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.3 * pixel[j]) + (0.7 * pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageDull(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b cpix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.25) + (pix[j] * 0.25) + (cpix[j] * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageBlendWithFrame(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<16> collection;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorPulseIncrease(reimage);
    ColorPulseIncrease(frame);
    collection.shiftFrames(frame);
    collection.shiftFrames(reimage);
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    MedianBlendMultiThread(copy1);
    cv::Mat copy2 = frame.clone();
    static double alpha = 1.0;
    static int dir = 1;
    AlphaBlendDouble(copy1, copy2, frame, alpha, 1-alpha);
    AlphaMovementMaxMin(alpha,dir,0.01,1.0, 0.1);
    AddInvert(frame);
}

void ac::ColorImageBlendSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1)
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone();
    ColorPulseIncrease(copy1);
    ColorPulseRandom(copy2);
    collection.shiftFrames(reimage);
    collection.shiftFrames(copy1);
    collection.shiftFrames(copy2);
    collection.shiftFrames(copy3);
    cv::Mat copy_smooth = frame.clone();
    Smooth(copy_smooth, &collection);
    cv::Mat src_cp = frame.clone();
    CallFilter(subfilter, src_cp);
    AlphaBlendDouble(copy_smooth, src_cp, frame, alpha, 1-alpha);
    AlphaMovementMaxMin(alpha,dir,0.005,1.0, 0.1);
    AddInvert(frame);
}

void ac::ColorMatrixImageFilter(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static MatrixCollection<16> collection;
    cv::Mat reimage_copy = reimage.clone();
    ColorPulseIncrease(reimage_copy);
    collection.shiftFrames(reimage_copy);
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b pix_img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.33 * pixel[j]) + (0.33 * pix[j]) + (0.33 * pix_img[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ReduceBy50(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorXorMatrixImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    cv::Mat reimage;
    ac_resize(blend_image,reimage, frame.size());
    ColorPulseIncrease(reimage);
    collection.shiftFrames(reimage);
    cv::Mat copy1 = frame.clone();
    int r = 1+(rand()%4);
    for(int i = 0; i < r; ++i)
        MedianBlur(copy1);
    Smooth(copy1, &collection);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pixblur = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int temp = static_cast<unsigned char>(alpha * pixel[j]) ^ static_cast<unsigned char>((1-alpha) * pixblur[j]);
                    pixel[j] = pixel[j]^temp;
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AlphaMovementMaxMin(alpha, dir, 0.001, 1.0, 0.1);
    AddInvert(frame);
}

void ac::MatrixColorBlur(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    Smooth(frame, &collection);
}

void ac::ColorImageLower(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    img_pix[j] /= 4;
                    pixel[j] /= 2;
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * img_pix[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::BlurSubFilter8(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "BlurSubFilter8")
        return;
    static MatrixCollection<8> collection;
    CallFilter(subfilter, frame);
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::LightBlend(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static double alpha = 1.0;
    static int dir = 1;
    for(int z = 0; z < frame.rows-2; ++z) {
        for(int i = 0; i < frame.cols-2; ++i) {
            cv::Vec3b pixels[4];
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixels[0] = copy1.at<cv::Vec3b>(z, i+1);
            pixels[1] = copy1.at<cv::Vec3b>(z+1, i);
            pixels[2] = copy1.at<cv::Vec3b>(z+1, i+1);
            cv::Vec3b color;
            for(int j = 0; j < 3; ++j) {
                color[j] = static_cast<unsigned char>((pixels[0][j] + pixels[1][j] + pixels[2][j]) * alpha);
                color[j] /= 3;
                pixel[j] = pixel[j]^color[j];
            }
        }
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::LightBlendXor(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static double alpha = 1.0;
    static double alpha1 = 0.5;
    static int dir = 1, dir1 = 1;
    for(int z = 0; z < frame.rows-2; ++z) {
        for(int i = 0; i < frame.cols-2; ++i) {
            cv::Vec3b pixels[4];
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixels[0] = copy1.at<cv::Vec3b>(z, i+1);
            pixels[1] = copy1.at<cv::Vec3b>(z+1, i);
            pixels[2] = copy1.at<cv::Vec3b>(z+1, i+1);
            cv::Vec3b color;
            for(int j = 0; j < 3; ++j) {
                color[j] = (pixels[0][j] + pixels[1][j] + pixels[2][j]);
                color[j] /= 3;
                color[j] *= static_cast<unsigned char>(alpha);
                pixel[j] ^= static_cast<unsigned char>((alpha1 * pixel[j]) + static_cast<unsigned char>(alpha1 * color[j]));
            }
        }
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha, dir1, 0.001, 1.0, 0.1);
}

void ac::Source_ImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "Source_ImageSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    CallFilter(subfilter, copy2);
    int colors[3] = {0};
    static int index = 0;
    InitArrayPosition(colors, index);
    ++index;
    if(index > 2)
        index = 0;
    cv::Mat frames[3];
    frames[colors[0]] = reimage.clone();
    frames[colors[1]] = copy1.clone();
    frames[colors[2]] = copy2.clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b color_by_index = frames[j].at<cv::Vec3b>(z, i);
                    pixel[j] = color_by_index[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::XorZeroImage(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image,reimage, frame.size());
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = collection.frames[7].at<cv::Vec3b>(z,i); // use i, z for glitch (memory error)
                cv::Vec3b color = reimage.at<cv::Vec3b>(z, i); // use i,z for glitch (memory error)
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                    if(pixel[j] == 0)
                        pixel[j] = color[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::SlowDownFilterSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "SlowDownFilterSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    static cv::Mat stored;
    if(stored.empty() || stored.size() != frame.size()) {
        stored = copy1.clone();
    }
    static int frame_counter = 0;
    static double seconds = 0;
    static int first = -1;
    static double alpha = 1.0;
    static int dir1 = 1;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps)) {
        ++seconds;
        frame_counter = 0;
    }
    if(seconds >= 1 || first == -1) {
        seconds = 0;
        frame_counter = 0;
        first = 1;
        CallFilter(subfilter, frame);
        stored = frame.clone();
    }
    else {
        frame = stored.clone();
    }
    AlphaBlendDouble(stored, copy1,frame,alpha,(1-alpha));
    AlphaMovementMaxMin(alpha, dir1, 0.01, 1.0, 0.1);
    AddInvert(frame);
}

void ac::VariableFilterSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "SlowerFilterSubFilter")
        return;
    static MatrixCollection<4> collection;
    cv::Mat copy1 = frame.clone();
    static cv::Mat stored;
    static double sec_increase = 1.0;
    static int dir_wait = 1;
    if(stored.empty() || stored.size() != frame.size()) {
        stored = copy1.clone();
        collection.resetFrame(stored);
    }
    static int frame_counter = 0;
    static double seconds = 0;
    static int first = -1;
    static double alpha = 1.0;
    static int dir1 = 1;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps)) {
        ++seconds;
        frame_counter = 0;
    }
    if(seconds >= sec_increase || first == -1) {
        seconds = 0;
        frame_counter = 0;
        first = 1;
        CallFilter(subfilter, frame);
        stored = frame.clone();
        collection.shiftFrames(stored);
    }
    else {
        cv::Mat cpy1 = frame.clone();
        Smooth(cpy1, &collection);
        frame = cpy1.clone();
    }
    AlphaBlendDouble(stored, copy1,frame,alpha,(1-alpha));
    AlphaMovementMaxMin(alpha, dir1, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(sec_increase, dir_wait, 0.1, 4.0, 1.0);
    AddInvert(frame);
}
