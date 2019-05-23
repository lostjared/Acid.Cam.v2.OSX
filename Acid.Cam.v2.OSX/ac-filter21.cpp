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

#include "ac.h"

void ac::ColorShiftXor(cv::Mat &frame) {
    static int rgb_value[3] = { 32, 64, 128 };
    static int dir[3] = {1,1,1};
    static int speed = 6;
    static int speed_dir[3] = {1,1,1};
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] -= (pixel[j]^rgb_value[j]);
                }
                
            }
        }
    };
    
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            rgb_value[j] += (rand()%speed);
            if(rgb_value[j] >= 255) {
                rgb_value[j] = rand()%255;
                dir[j] = 0;
                if(speed_dir[j] == 1) {
                    speed += 2;
                    if(speed > 15) {
                        speed_dir[j] = 0;
                        speed = 15;
                    }
                } else {
                    speed -= 2;
                    if(speed <= 3) {
                        speed_dir[j] = 1;
                        speed = 3;
                    }
                }
            }
        } else {
            rgb_value[j] -= (rand()%speed);
            if(rgb_value[j] <= 0) {
                rgb_value[j] = rand()%255;
                dir[j] = 1;
                if(speed_dir[j] == 1) {
                    speed += 2;
                    if(speed > 15) {
                        speed_dir[j] = 0;
                        speed = 15;
                    }
                } else {
                    speed -= 2;
                    if(speed <= 3) {
                        speed_dir[j] = 1;
                        speed = 3;
                    }
                }
            }
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::RandomSquares(cv::Mat &frame) {
    cv::Vec3b color(rand()%255, rand()%255, rand()%255);
    int num_values = 10+(rand()%frame.rows/10);
    for(int j = 0; j < num_values; ++j) {
        ac::Rect rc(5+(rand()%frame.cols-10), 5+(rand()%frame.rows-10),75+(rand()%200), 20+(rand()%25));
        for(int i = rc.x; i < rc.x+rc.w; ++i) {
            for(int z = rc.y; z < rc.y+rc.h; ++z) {
                if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        pixel[q] = pixel[q]^color[q];
                    }
                }
            }
        }
                    
    }
}

void ac::RandomImageSquares(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Vec3b color(rand()%255, rand()%255, rand()%255);
    int num_values = 10+(rand()%frame.rows/10);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int j = 0; j < num_values; ++j) {
        ac::Rect rc(5+(rand()%frame.cols-10), 5+(rand()%frame.rows-10),75+(rand()%200), 20+(rand()%25));
        for(int i = rc.x; i < rc.x+rc.w; ++i) {
            for(int z = rc.y; z < rc.y+rc.h; ++z) {
                if(i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        pixel[q] = pixel[q]^pix[q];
                    }
                }
            }
        }
        
    }
}
void ac::Random_FilterX2_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "Random_FilterX2_SubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    Random_Filter(copy1);
    CallFilter(subfilter, copy1);
    CallFilter(subfilter, copy2);
    Random_Filter(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
}

void ac::FuzzyFilter(cv::Mat &frame) {
    pushSubFilter(ac::filter_map["ColorVariableBlend"]);
    Random_FilterX2_SubFilter(frame);
    popSubFilter();
}

void ac::XorMultiply(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    MedianBlur(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                unsigned int value = pixel[0]^pixel[1]^pixel[2];
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^static_cast<unsigned char>(value*alpha);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.1, 2.5, 1.0);
}

void ac::Grayscale(cv::Mat &frame) {
    cv::Mat change;
    cv::cvtColor(frame, change, cv::COLOR_BGR2GRAY);
    cv::cvtColor(change, frame, cv::COLOR_GRAY2BGR);
    AddInvert(frame);
}

void ac::ColorShadowBlend(cv::Mat &frame) {
    MedianBlur(frame);
    MedianBlur(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            cv::Vec3b value;
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    value[j] +=  pixel[j];
                    value[j] /= 2;
                    pixel[j] = pixel[j]^value[j];
                }
            
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::FlashMatrixTrails(cv::Mat &frame) {
    static double value[3] = {1,1,1};
    static int dir[3] = {1,1,1};
    MedianBlur(frame);
    ColorVariableBlend(frame);
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int cur_frame = collection.size()-1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b old_pixel = collection.frames[cur_frame].at<cv::Vec3b>(z, i);
                    if(pixel == old_pixel)
                        continue;
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]*value[j]) ^ static_cast<unsigned char>(old_pixel[j]*value[j]);
                }
            }
        }
    };
    --cur_frame;
    if(cur_frame <= 1)
        cur_frame = collection.size()-1;
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    for(int j = 0; j < 3; ++j) {
        AlphaMovementMaxMin(value[j], dir[j], 0.1, 3.0, 1.0);
    }
}

void ac::GhostTrails(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    cv::Mat copy2 = collection.frames[collection.size()-1].clone();
    cv::Mat copy3 = collection.frames[collection.size()/2].clone();
    cv::Mat copy_output;
    AlphaBlend(copy2, copy3, copy_output, 0.5);
    AlphaBlendDouble(copy1, copy_output, frame, 0.3, 0.7);
    AddInvert(frame);
}

void ac::MedianBlend_Random_Filter(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int i = 0; i < collection.size()-1; ++i) {
        Random_Filter(collection.frames[i]);
    }
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::IntertwineMirrorEnergy(cv::Mat &frame) {
    static MatrixCollection<720> collection;
    collection.shiftFrames(frame);
    int index = 0;
    int numeric_value = frame.rows/(collection.size()/2);
    int row_counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b time_pixel = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = time_pixel;
        }
        ++row_counter;
        if(row_counter > numeric_value) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            row_counter = 0;
        }
    }
    DarkNegate(frame);
    MirrorBottomToTop(frame);
    MirrorLeft(frame);
    MirrorBlend(frame);
    AddInvert(frame);
}

void ac::IntertwineMultipleRows(cv::Mat &frame) {
    static MatrixCollection<720> collection;
    for(int i = 0; i < 1+(rand()%4); ++i)
        collection.shiftFrames(frame);
    int index = 0;
    int row_counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b time_pixel = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = time_pixel;
        }
        ++row_counter;
        if(row_counter > 1) {
            ++index;
            if(index > collection.size()-1)
                index = 0;
            row_counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::GhostTwitch(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone(), output, img_out;
    cv::Mat &img1 = collection.frames[rand()%(collection.size()-1)], &img2 = collection.frames[rand()%(collection.size()-1)], &img3 = collection.frames[rand()%(collection.size()-1)];
    AlphaBlend(img1, img2, output,0.5);
    AlphaBlend(img3, output, img_out, 0.5);
    AlphaBlend(copy1, img_out, frame, 0.5);
    AddInvert(frame);
}

void ac::GhostTwitchTrails(cv::Mat &frame) {
    GhostTwitch(frame);
    GhostTrails(frame);
    AddInvert(frame);
}

void ac::Dyskinesia(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    frame = collection.frames[rand()%(collection.size()-1)].clone();
    GhostTrails(frame);
    AddInvert(frame);
}

void ac::Mirror_Rainbow_Blur(cv::Mat &frame) {
    MedianBlendMultiThread(frame);
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int index = 1, dir = 1;
    cv::Mat copy1 = collection.frames[index].clone(), copy2 = collection.frames[collection.size()-2].clone();
    MirrorLeft(copy1);
    MirrorRight(copy2);
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
    if(dir == 1) {
        ++index;
        if(index > collection.size()-2) {
            index = collection.size()-2;
            dir = 0;
        }
    } else {
        --index;
        if(index <= 1) {
            index = 1;
            dir = 1;
        }
    }
    GhostTrails(frame);
    AddInvert(frame);
}

void ac::MirrorMedianBlur(cv::Mat &frame) {
    MirrorAll(frame);
    MedianBlur(frame);
    ColorTransitionRandom(frame);
    SmoothFrame32(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::VideoTwitch(cv::Mat &frame) {
    static cv::Mat stored;
    static int changed = 0, offset = 0;
    ++changed;
    ++offset;
    if(stored.empty() || (stored.size() != frame.size())) {
        stored = frame.clone();
    }
    else if((changed%6) == 0)
        stored = frame.clone();
    
    if(offset >= 4) {
        offset = 0;
    } else {
        frame = stored.clone();
    }
    AddInvert(frame);
    
}

void ac::ErodeFilter(cv::Mat &frame) {
    static int erosion_size = 2, dir = 1;
    cv::Mat frame_cp = frame.clone();
    cv::UMat copy1 = frame_cp.getUMat(cv::ACCESS_FAST);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size( 2*erosion_size + 1, 2*erosion_size+1 ),cv::Point(erosion_size, erosion_size) );
    cv::erode(copy1, frame, element );
    if(dir == 1) {
        ++erosion_size;
        if(erosion_size >= 21)
            dir = 0;;
    } else {
        --erosion_size;
        if(erosion_size <= 2)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::DilateFilter(cv::Mat &frame) {
    static int dilation_size = 2, dir = 1;
    cv::Mat frame_cp = frame.clone();
    cv::UMat copy1 = frame_cp.getUMat(cv::ACCESS_FAST);
    cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),cv::Point( dilation_size, dilation_size ) );
    dilate( copy1, frame, element );
    if(dir == 1) {
        ++dilation_size;
        if(dilation_size >= 21)
            dir = 0;;
    } else {
        --dilation_size;
        if(dilation_size <= 2)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::MirrorLeftBottomToTop(cv::Mat &frame) {
    MirrorLeft(frame);
    MirrorBottomToTop(frame);
    AddInvert(frame);
}

void ac::MirrorRightTopToBottom(cv::Mat &frame) {
    MirrorRight(frame);
    MirrorTopToBottom(frame);
}


void ac::BilateralFilter(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    cv::UMat value = copy1.getUMat(cv::ACCESS_FAST);
    bilateralFilter (value, frame, 15, 80, 80 );
}

void ac::BilateralFilterFade(cv::Mat &frame) {
    static const int max = 200, min = 4;
    static int offset = min, dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::UMat value = copy1.getUMat(cv::ACCESS_FAST);
    bilateralFilter(value, frame, 15, offset, offset);
    if(dir == 1) {
        ++offset;
        if(offset > max) {
            offset = max;
            dir = 0;
        }
    } else {
        --offset;
        if(offset <= min) {
            offset = min;
            dir = 1;
        }
    }
}

void ac::BilateralBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    BilateralFilter(frame);
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Scalar value;
                for(int j = 0; j < collection.size(); ++j) {
                    cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                    for(int q = 0; q < 3; ++q) {
                        value[q] += pixel[q];
                    }
                }
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int val = 1+static_cast<int>(value[j]);
                    pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}
