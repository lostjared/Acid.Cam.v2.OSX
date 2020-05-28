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

void ac::ImageSquareShrinkAlpha(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int frame_offset_z = 0, frame_offset_i = 0;
    static int dir = 1;
    static int speed = 32;
    static double alpha = 1.0;
    static int dir1 = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
    AlphaMovementMaxMin(alpha, dir1, 0.01, 1.0, 0.3);
}

void ac::ImageSquareExpand(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int frame_offset_z = (frame.rows/9), frame_offset_i = (frame.cols/16);
    static int dir = 0;
    static int speed = 32;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
        
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
    
}

void ac::ImageKaleidoscopeNoBlur(cv::Mat &frame) {
    if(blend_set == false)
        return;
    ImageSquareShrinkFast(frame);
    MedianBlendMultiThreadByEight(frame);
    BlendWithSource75(frame);
    Mirror_ReverseColor(frame);
    MirrorRightTopToBottom(frame);
}

void ac::KaleidoscopeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "KaleidoscopeSubFilter")
        return;
    CallFilter(subfilter, frame);
    BlendWithSource75(frame);
    Mirror_ReverseColor(frame);
    MirrorRightTopToBottom(frame);
}

void ac::ImageSquareShrinkSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageSquareShrinkSubFilter")
        return;
    static int frame_offset_z = 0, frame_offset_i = 0;
    static int dir = 1;
    static int speed = 32;
    static double alpha = 1.0;
    static int dir1 = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
                }
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
    AlphaMovementMaxMin(alpha, dir1, 0.01, 1.0, 0.3);
}

void ac::DifferenceReplaceSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "DifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) > getPixelCollection()) {
                            pixel[j] = rep[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::InvertedDifferenceReplaceSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "InvertedDifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) < getPixelCollection()) {
                            pixel[j] = rep[j];
                        } else {
                            pixel[j] = 0;
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ImageInvertedDifferenceReplaceSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageInvertedDifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) < getPixelCollection()) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (0.3 * rep[j]));
                        } else {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (0.3 * img_pix[j]));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ImageDifferenceReplaceSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "DifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) > getPixelCollection()) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (rep[j] * 0.3));
                        } else {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (img_pix[j] * 0.3));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::DifferenceReplaceSubFilterAlphaBlend(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "DifferenceReplaceSubFilterAlphaBlend")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) > getPixelCollection()) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (rep[j] * (1-alpha)));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.2);
}

void ac::GradientGlitch(cv::Mat &frame) {
    static unsigned char val = 0;
    int inc = (frame.rows/255)+1;
    for(int i = 0; i < frame.cols; ++i) {
        val = 1;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] = static_cast<unsigned char>((pixel[0] * val) * 0.5);
            pixel[2] = static_cast<unsigned char>((pixel[2] * ~val) * 0.5);
            
            if((z%inc) == 0)
                ++val;
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    AddInvert(frame);
}

void ac::ImageGradientBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static unsigned char val = 0;
    int inc = (frame.rows/255)+1;
    static int index = 0;
    static double alpha = 1.0;
    static int dir = 1;
    for(int i = 0; i < frame.cols; ++i) {
        val = 1;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (pix[j] * (1-alpha)));
            }
            pixel[index] = val;
            if((z%inc) == 0)
                ++val;
        }
    }
    static int counter = 0;
    ++counter;
    int fps = static_cast<int>(ac::fps) * 3;
    if(counter > fps) {
        counter = 0;
        ++index;
        if(index > 2)
            index = 0;
    }
    BlendWithSource50(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.2);
}

void ac::MedianBlendGradientMultiThread(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    GradientColors(copy1);
    GradientColors(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::MedianBlendGradientDarkMultiThread(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    GradientColors(copy1);
    GradientColors(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    MedianBlendMultiThreadByEight(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::GradientAlphaBlend(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = frame.clone();
    GradientColors(copy1);
    GradientColors(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::GradientFilter(cv::Mat &frame) {
    static int index = 0;
    static int val = 0;
    int inc = (frame.cols/255)+1;
    int dir = 1;
    int increment_value = 1+(rand()%250);
    for(int z = 0; z < frame.rows; ++z) {
        val = increment_value;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = val;
            if((i%inc) == 0) {
                if(dir == 1) {
                    ++val;
                    if(val >= 250) {
                        dir = 0;
                        val = 250;
                    }
                } else {
                    --val;
                    if(val <= increment_value) {
                        dir = 1;
                        val = increment_value;
                    }
                }
            }
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    AddInvert(frame);
}

void ac::GradientFilterAlphaBlend(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    GradientFilter(copy1);
    GradientFilter(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
}

void ac::MedianBlendGradientFilterMultiThread(cv::Mat &frame) {
    GradientFilterAlphaBlend(frame);
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::MedianBlendGraidentFilterDarkMultiThread(cv::Mat &frame) {
    GradientFilterAlphaBlend(frame);
    MedianBlendMultiThreadByEight(frame);
    AddInvert(frame);
}

void ac::ColorOrderSwap(cv::Mat &frame) {
    static int color_order = 0;
    if(color_order > 0) {
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b temp;
                    temp = pixel;
                    switch(color_order) {
                        case 1: // RGB
                            pixel[0] = temp[2];
                            pixel[1] = temp[1];
                            pixel[2] = temp[0];
                            break;
                        case 2:// GBR
                            pixel[0] = temp[1];
                            pixel[1] = temp[0];
                            break;
                        case 3:// BRG
                            pixel[1] = temp[2];
                            pixel[2] = temp[1];
                            break;
                        case 4: // GRB
                            pixel[0] = temp[1];
                            pixel[1] = temp[2];
                            pixel[2] = temp[0];
                            break;
                    }
                    
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
    }
    ++color_order;
    if(color_order > 4) {
        color_order = 0;
    }
    AddInvert(frame);
}

void ac::ColorOrderSwapMap(cv::Mat &frame) {
    ShuffleColorMap(frame);
    ColorOrderSwap(frame);
    AddInvert(frame);
}

void ac::MedianBlendSwapMapMultiThread(cv::Mat &frame) {
    ColorOrderSwapMap(frame);
    MedianBlendMultiThread_2160p(frame);
    AddInvert(frame);
}

void ac::RandomGradientColors(cv::Mat &frame) {
    static std::default_random_engine rgen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    int ch_value[3];
    ch_value[0] = rgen()%255;
    ch_value[1] = rgen()%255;
    ch_value[2] = rgen()%255;
    int dir[] = {0, 1, 0};
    int slice = frame.cols/255;
    int cols[3] = {0};
    for(int z = 0; z < frame.rows; ++z) {
        cols[0] = ch_value[0];
        cols[1] = ch_value[1];
        cols[2] = ch_value[2];
        for(int i = 0; i < frame.cols; ++i) {
            if((i%slice) == 0) {
                for(int j = 0; j < 3; ++j) {
                    if(dir[j] == 1) {
                        ++cols[j];
                        if(cols[j] > 255) {
                            cols[j] = 255;
                            dir[j] = 0;
                        }
                    } else if(dir[j] == 0) {
                        --cols[j];
                        if(cols[j] <= ch_value[j]) {
                            cols[j] = ch_value[j];
                            dir[j] = 1;
                        }
                    }
                }
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * cols[j]));
            }
        }
    }
    AddInvert(frame);
}


void ac::GradientColorMap(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone(), copy3 = frame.clone(), temp;
    RandomGradientColors(copy1);
    RandomGradientColors(copy2);
    ShuffleColorMap(copy3);
    AlphaBlend(copy1, copy2, temp, 0.5);
    AlphaBlendDouble(copy3, temp, frame, alpha, (1-alpha));
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.1);
    AddInvert(frame);
}

void ac::GradientXor(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    RandomGradientColors(copy1);
    for(int i = 0; i < 3; ++i) {
        MedianBlur(copy1);
        MedianBlur(frame);
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::RandomSub_Filter(cv::Mat &frame) {
    std::string filter_;
    std::string subf;
    filter_ = vSub[rand()%(vSub.size()-1)];
    do {
        subf = solo_filter[rand()%(solo_filter.size()-1)];
    } while(filter_ == subf);
    int sub_t = getFilterByName(subf);
    if(sub_t == -1) {
        std::cerr << "Error: Filter " << subf << " not found!";
        return;
    }
#ifdef DEBUG_MODE
    std::cout << filter_ << ":" << subf << "\n";
#endif
    pushSubFilter(sub_t);
    CallFilter(filter_, frame);
    popSubFilter();
    AddInvert(frame);
}

void ac::ShuffleSub_Filter(cv::Mat &frame) {
    static std::vector<std::string> solo_vec(solo_filter);
    static std::vector<std::string> shuffle_vec(vSub);
    static std::default_random_engine rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static int index1 = 0, index2 = 0;
    static int lazy = 0;
    if(lazy == 0) {
        std::shuffle(solo_vec.begin(), solo_vec.end(), rng);
        std::shuffle(shuffle_vec.begin(), shuffle_vec.end(), rng);
        lazy = 1;
    }
    std::string filter_;
    std::string subf;
    filter_ = shuffle_vec[index1];
    ++index1;
    if(index1 > static_cast<int>(shuffle_vec.size()-1)) {
        std::shuffle(shuffle_vec.begin(), shuffle_vec.end(), rng);
        index1 = 0;
    }
    subf = solo_vec[index2];
    ++index2;
    if(index2 > static_cast<int>(solo_vec.size()-1)) {
        std::shuffle(solo_vec.begin(), solo_vec.end(), rng);
        index2 = 0;
    }
    int sub_t = getFilterByName(subf);
    if(sub_t == -1) {
        std::cerr << "Error: Filter " << subf << " not found!";
        return;
    }
#ifdef DEBUG_MODE
    std::cout << filter_ << ":" << subf << "\n";
#endif
    pushSubFilter(sub_t);
    CallFilter(filter_, frame);
    popSubFilter();
    AddInvert(frame);
}

void ac::Shuffle_Filter(cv::Mat &frame) {
    static std::default_random_engine rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    static std::vector<std::string> shuffle_solo(solo_filter);
    static int lazy = 0;
    static int index = 0;
    if(lazy == 0) {
        std::shuffle(shuffle_solo.begin(), shuffle_solo.end(), rng);
        lazy = 1;
    }
    std::string filter_name = shuffle_solo[index];
    ++index;
    if(index > static_cast<int>(shuffle_solo.size()-1)) {
        index = 0;
        std::shuffle(shuffle_solo.begin(), shuffle_solo.end(), rng);
    }
#ifdef DEBUG_MODE
    std::cout << "Filter: " << filter_name << "\n";
#endif
    CallFilter(filter_name, frame);
    AddInvert(frame);
}

void ac::RandomOrigFrame(cv::Mat &frame) {
    static int counter = 0;
    counter = rand()%10;
    if(counter == 5) {
        frame = ac::orig_frame.clone();
    }
    AddInvert(frame);
}

void ac::ColorVariableRectangles(cv::Mat &frame) {
    int total_lines = frame.rows-2;
    int current_line = 0;
    static double alpha = 1.0;
    static int dir = 1;
    while(current_line < total_lines) {
        int rand_height = 10+rand()%490;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        
        cv::Vec3b rand_color(rand()%255, rand()%255, rand()%255);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (rand_color[j]));
                }
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::VariableRectangles(cv::Mat &frame) {
    int total_lines = frame.rows-2;
    int current_line = 0;
    int offset = 0;
    while(current_line < total_lines) {
        int rand_height = 10+rand()%490;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_color = 55+(rand()%200);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                if(z >= 0 && z < frame.rows && i >= 0 && i < frame.cols) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    pixel[offset] += rand_color;
                }
            }
        }
        current_line += rand_height;
        offset = rand()%2;
    }
    AddInvert(frame);
}

void ac::VariableRectanglesSimple(cv::Mat &frame) {
    int total_lines = frame.rows-2;
    int current_line = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        wait = 0;
    } else {
        return;
    }

    while(current_line < total_lines) {
        int rand_height = 10+rand()%390;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame = rand()%(collection.size()-1);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                if(z >= 0 && z < frame.rows && i >= 0 && i < frame.cols) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(rand_frame < collection.size()-1) {
                        cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                        pixel = pix;
                    }
                }
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::VariableRectanglesExtra(cv::Mat &frame) {
    int total_lines = frame.rows-2;
    int current_line = 0;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        wait = 0;
    } else {
        return;
    }
    
    while(current_line < total_lines) {
        int rand_height = 10+rand()%490;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame = rand()%(collection.size()-1);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                if(z >= 0 && z < frame.rows && i >= 0 && i < frame.cols) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(rand_frame < collection.size()-1) {
                        cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                        pixel = pix;
                    }
                }
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::VariableRectangleImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    int total_lines = frame.rows-2;
    int current_line = 0;
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    
    static int wait = 0;
    ++wait;
    if(wait > getVariableWait()) {
        wait = 0;
    } else {
        return;
    }
    
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static double alpha = 1.0;
    static int dir = 1;
    while(current_line < total_lines) {
        int rand_height = 10+rand()%490;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame = rand()%(collection.size()-1);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                if(z >= 0 && z < frame.rows && i >= 0 && i < frame.cols) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                    if(rand_frame < collection.size()-1) {
                        cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                //pixel = pix;
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((alpha * pix[j]) + ((1-alpha) * img[j]));
                        }
                    }
                }
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
}

void ac::MirrorSwitchMode(cv::Mat &frame) {
    static int mode = rand()%2;
    static int counter = 0;
    int wait = static_cast<int>(ac::fps);
    if(++counter > wait/2) {
        counter = 0;
        mode = (mode == 0) ? 1 : 0;
    }
    if(mode == 0) {
        MirrorLeftBottomToTop(frame);
    } else {
        MirrorRightTopToBottom(frame);
    }
    AddInvert(frame);
}

void ac::MirrorSwitchLeftRight(cv::Mat &frame) {
    static int mode = rand()%2;
    static int counter = 0;
    int wait = static_cast<int>(ac::fps);
    if(++counter > wait/2) {
        counter = 0;
        mode = (mode == 0) ? 1 : 0;
    }
    if(mode == 0) {
        MirrorLeft(frame);
    } else {
        MirrorRight(frame);
    }
    AddInvert(frame);
}

void ac::VariableRectanglesSmall(cv::Mat &frame) {
    int total_lines = frame.rows-2;
    int current_line = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    while(current_line < total_lines) {
        int rand_height = 10+rand()%100;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame =  rand()%(collection.size()-1);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::VariableRectanglesLarge(cv::Mat &frame) {
    int total_lines = frame.rows-2;
    int current_line = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    while(current_line < total_lines) {
        int rand_height = 200+rand()%420;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame = rand()%(collection.size()-1);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                if(z >= 0 && z < frame.rows && i >= 0 && i < frame.cols) {
                    cv::Vec3b &pixel = pixelAt(frame,z, i);
                    if(rand_frame < collection.size()-1) {
                        cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                        pixel = pix;
                    }
                }
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::VariableRectanglesImageCollection(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    int total_lines = frame.rows-2;
    int current_line = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    while(current_line < total_lines) {
        int rand_height = 10+rand()%100;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame =  rand()%(collection.size()-1);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                cv::Vec3b img = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.33) + (pix[j] * 0.33) + (img[j] * 0.33));
                }
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::VariableRectanglesVariableImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "VariableRectanglesVariableImageSubFilter")
        return;
    int total_lines = frame.rows-2;
    int current_line = 0;
    static MatrixCollection<8> collection;
    static MatrixCollection<8> image_collection;
    collection.shiftFrames(frame);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    FlipRandom(reimage);
    image_collection.shiftFrames(reimage);
    while(current_line < total_lines) {
        int rand_height = 10+rand()%100;
        if(current_line+rand_height > total_lines)
            rand_height = total_lines-current_line;
        int rand_frame =  rand()%(collection.size()-1);
        for(int z = current_line; z < current_line+rand_height; ++z) {
            if(current_line > total_lines)
                break;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                cv::Vec3b img = image_collection.frames[rand_frame].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.33 * pixel[j]) + (0.33 * pix[j]) + (0.33 * img[j]));
                }
            }
        }
        current_line += rand_height;
    }
    AddInvert(frame);
}

void ac::RainbowXorStrobeBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat frames[3];
    frames[0] = collection.frames[1].clone();
    frames[1] = collection.frames[4].clone();
    frames[2] = collection.frames[7].clone();
    static int flash = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j <3; ++j) {
                    cv::Vec3b pix;
                    if(flash == 1)
                        pix = frames[j].at<cv::Vec3b>(z, i);
                    else
                        pix = frames[3-j-1].at<cv::Vec3b>(z, i);
                    
                    pixel[j] = pixel[j]^pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    flash = (flash == 0) ? 1 : 0;
}

