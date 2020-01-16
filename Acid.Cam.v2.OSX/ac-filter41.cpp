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

void ac::VariableLinesXY_Interlaced(cv::Mat &frame) {
    cv::Mat copy[2];
    copy[0] = frame.clone();
    copy[1] = frame.clone();
    VariableLinesStartRectangle(copy[0]);
    VariableLinesY(copy[1]);
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = copy[index].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        ++index;
        if(index > 1)
            index = 0;
    }
    AddInvert(frame);
}

void ac::VariableLinesExpand(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    static int switch_offset = 0;
    if(switch_offset == 0)
        VariableLinesStartRectangle(copy1);
    else
        VariableLinesY(copy1);
    collection.shiftFrames(copy1);
    static int off_count = rand()%50;
    off_count += 20;
    if(off_count > (frame.rows)) {
        off_count = rand()%50;
        ++off_count;
    }
    
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if((z%(off_count+1)) == 0) {
            ++index;
            if(index > (collection.size()-1))
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::VariableLinesExpandBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    cv::Mat copy1 = frame.clone();
    static int switch_offset = 0;
    if(switch_offset == 0)
        VariableLinesStartRectangle(copy1);
    else
        VariableLinesY(copy1);
    collection.shiftFrames(copy1);
    static int off_count = rand()%50;
    off_count += 20;
    if(off_count > (frame.rows)) {
        off_count = rand()%50;
        off_count ++;
    }
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
        if((z%(off_count+1)) == 0) {
            ++index;
            if(index > (collection.size()-1))
                index = 0;
        }
    }
    AddInvert(frame);
}

void ac::CollectionXor4(cv::Mat &frame) {
    static MatrixCollection<4> collection;
    collection.shiftFrames(frame);
    static double colors[3] = {0};
    static double alpha = 1.0;
    static int idir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            for(int q = 0; q < collection.size(); ++q) {
                cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    colors[j] += pix[j];
            }
        }
    }
    for(int j = 0; j < 3; ++j)
        colors[j] /= ((frame.rows * frame.cols) * collection.size());
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                int color = static_cast<int>(colors[j]);
                int pix_val = pixel[j]^color;
                double fval = (alpha * pix_val);
                pixel[j] = static_cast<unsigned char>((1-alpha) * pixel[j]) + static_cast<unsigned char>(fval);
            }
        }
    }
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, idir, 0.01, 1.0, 0.1);
}

void ac::PixelateExpandDistort(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int d_x = 1, d_y = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b val = copy1.at<cv::Vec3b>(z, i);
            for(int x = 0; x < d_x && i+x < frame.cols; ++x) {
                for(int y = 0; y < d_y && z+y < frame.rows; ++y) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(y+z, x+i);
                    pixel = val;
                }
            }
            i += d_x;
        }
        z += d_y;
    }
    AddInvert(frame);
    static int dir1 = 1, dir2 = 1;
    static int max_x_value = 5+(rand()%25);
    static int max_y_value = 5+(rand()%25);
    if(dir1 == 1) {
        ++d_x;
        if(d_x > max_x_value) {
            d_x = max_x_value;
            dir1 = 0;
            max_x_value = 5+(rand()%25);
        }
    } else {
        --d_x;
        if(d_x <= 1) {
            d_x = 1;
            dir1 = 1;
        }
    }
    if(dir2 == 1) {
        ++d_y;
        if(d_y > max_y_value) {
            d_y = max_y_value;
            dir2 = 0;
            max_y_value = 5+(rand()%25);
        }
    } else {
        --d_y;
        if(d_y <= 1) {
            d_y = 1;
            dir2 = 1;
        }
    }
}

void ac::PixelateExpandDistortX(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int d_x = 1, d_y = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b val = copy1.at<cv::Vec3b>(z, i);
            for(int x = 0; x < d_x && i+x < frame.cols; ++x) {
                for(int y = 0; y < d_y && z+y < frame.rows; ++y) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(y+z, x+i);
                    pixel = val;
                }
            }
            i += d_x;
        }
        z += d_y;
    }
    AddInvert(frame);
    static int dir1 = 1;
    static int max_x_value = 5+(rand()%25);
    if(dir1 == 1) {
        ++d_x;
        if(d_x > max_x_value) {
            d_x = max_x_value;
            dir1 = 0;
            max_x_value = 5+(rand()%25);
        }
    } else {
        --d_x;
        if(d_x <= 1) {
            d_x = 1;
            dir1 = 1;
        }
    }
}

void ac::PixelateExpandDistortY(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int d_x = 1, d_y = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b val = copy1.at<cv::Vec3b>(z, i);
            for(int x = 0; x < d_x && i+x < frame.cols; ++x) {
                for(int y = 0; y < d_y && z+y < frame.rows; ++y) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(y+z, x+i);
                    pixel = val;
                }
            }
            i += d_x;
        }
        z += d_y;
    }
    AddInvert(frame);
    static int dir2 = 1;
    static int max_y_value = 5+(rand()%25);
    if(dir2 == 1) {
        ++d_y;
        if(d_y > max_y_value) {
            d_y = max_y_value;
            dir2 = 0;
            max_y_value = 5+(rand()%25);
        }
    } else {
        --d_y;
        if(d_y <= 1) {
            d_y = 1;
            dir2 = 1;
        }
    }
}

void ac::PixelateExpandDistortExtra(cv::Mat &frame) {
    static constexpr int MAX=100;
    cv::Mat copy1 = frame.clone();
    static int d_x = 1, d_y = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b val = copy1.at<cv::Vec3b>(z, i);
            for(int x = 0; x < d_x && i+x < frame.cols; ++x) {
                for(int y = 0; y < d_y && z+y < frame.rows; ++y) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(y+z, x+i);
                    pixel = val;
                }
            }
            i += d_x;
        }
        z += d_y;
    }
    AddInvert(frame);
    static int dir1 = 1, dir2 = 1;
    static int max_x_value = 5+(rand()%MAX);
    static int max_y_value = 5+(rand()%MAX);
    if(dir1 == 1) {
        ++d_x;
        if(d_x > max_x_value) {
            d_x = max_x_value;
            dir1 = 0;
            max_x_value = 5+(rand()%MAX);
        }
    } else {
        --d_x;
        if(d_x <= 1) {
            d_x = 1;
            dir1 = 1;
        }
    }
    if(dir2 == 1) {
        ++d_y;
        if(d_y > max_y_value) {
            d_y = max_y_value;
            dir2 = 0;
            max_y_value = 5+(rand()%MAX);
        }
    } else {
        --d_y;
        if(d_y <= 1) {
            d_y = 1;
            dir2 = 1;
        }
    }
}

void ac::RectangleSpin(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    for(int q = 0; q < 50; ++q) {
        int val1 = rand()%frame.rows;
        int val2 = rand()%frame.rows;
        for(int z = val1; z < val2; ++z) {
            int r_zval = rand()%frame.rows;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(r_zval, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::RectanglePlotXY(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    for(int q = 0; q < 200; ++q) {
        int val_y = rand()%frame.rows;
        int val_h = rand()%frame.rows;
        int val_x = rand()%frame.cols;
        int val_w = rand()%frame.cols;
        int pos_x = 0, pos_y = 0;
        for(int z = val_y; z < val_h; ++z) {
            pos_x = 0;
            for(int i = val_x; i < val_w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(pos_y, pos_x);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j]^pix[j];
                }
                ++pos_x;
            }
            ++pos_y;
        }
    }
    AddInvert(frame);
}

void ac::RectangleLines(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int index = 0;
    int z_offset = 0;
    for(int q = 0; q < frame.rows/2; ++q) {
        z_offset = rand()%frame.rows;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z_offset, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z_offset, i);
            pixel = pix;
        }
        ++index;
        if(index > (collection.size()-1))
            index = 0;
    }
    AddInvert(frame);
}

void ac::DifferenceFillLinesXor(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int index = 0;
    int z_offset = 0;
    for(int q = 0; q < frame.rows; ++q) {
        z_offset = rand()%frame.rows;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z_offset, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z_offset, i);
            if(pixel != pix) {
                for(int j = 0; j < 3; ++j) {
                    if(abs(pixel[j]-pix[j]) > 75)
                        pixel[j] = pixel[j]^pix[j];
                }
            }
        }
        ++index;
        if(index > (collection.size()-1))
            index = 0;
    }
    AddInvert(frame);
}

void ac::DistortPixelate(cv::Mat &frame) {
    static MatrixCollection<24> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int index = 0; index < collection.size(); ++index) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                if(pixel != pix) {
                    for(int j = 0; j < 3; ++j) {
                        if(abs(pixel[j]-pix[j]) > 30)
                            pixel[j] = pixel[j]*rand()%255;
                    }
                    break;
                }
            }
        }
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::DistortPixelate64_Slow(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int index = 0; index < collection.size(); ++index) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                if(pixel != pix) {
                    for(int j = 0; j < 3; ++j) {
                        if(abs(pixel[j]-pix[j]) > 30)
                            pixel[j] = pixel[j]*rand()%255;
                    }
                    break;
                }
            }
        }
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::DistortPixelate128_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DistortPixelate128_SubFilter")
        return;
    static MatrixCollection<128> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
            bool p_found = false;
            for(int index = 0; index < collection.size(); ++index) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                   if(abs(pixel[j]-pix[j]) > 30) {
                        p_found = true;
                        goto out_of_loop;
                    }
                }
            }
        out_of_loop:
            if(p_found == true) {
                pixel = pix1;
                break;
            }
        }
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

void ac::DifferenceFillLines_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "DifferenceFillLines_SubFilter")
        return;
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(frame);
    int index = 0;
    int z_offset = 0;
    for(int q = 0; q < frame.rows; ++q) {
        z_offset = rand()%frame.rows;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z_offset, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z_offset, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z_offset, i);
            if(pixel != pix) {
                for(int j = 0; j < 3; ++j) {
                    if(abs(pixel[j]-pix[j]) > 75)
                        pixel[j] = pixel[j]^pix1[j];
                }
            }
        }
        ++index;
        if(index > (collection.size()-1))
            index = 0;
    }
    AddInvert(frame);
}

void ac::DifferenceFillLinesBlend_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "DifferenceFillLinesBlend_SubFilter")
        return;
    static MatrixCollection<32> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(frame);
    int index = 0;
    int z_offset = 0;
    for(int q = 0; q < frame.rows; ++q) {
        z_offset = rand()%frame.rows;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z_offset, i);
            cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z_offset, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z_offset, i);
            if(pixel != pix) {
                for(int j = 0; j < 3; ++j) {
                    if(abs(pixel[j]-pix[j]) > 75)
                        pixel[j] = static_cast<unsigned int>((0.5 * pixel[j]) + (0.5 * pix1[j]));
                }
            }
        }
        ++index;
        if(index > (collection.size()-1))
            index = 0;
    }
    AddInvert(frame);
}

void ac::DistortPixelate24_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DistortPixelate24_SubFilter")
        return;
    static MatrixCollection<24> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
            bool p_found = false;
            for(int index = 0; index < collection.size(); ++index) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(abs(pixel[j]-pix[j]) > 30) {
                        p_found = true;
                        goto out_of_loop;
                    }
                }
            }
        out_of_loop:
            if(p_found == true) {
                pixel = pix1;
                break;
            }
        }
    }
    MatrixCollectionAuraTrails(frame);
    AddInvert(frame);
}

// 866 953 1002
