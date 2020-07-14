

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

#include "ac.h"

void ac::ShiftLinesDown(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    int index = 0;
    static int counter = 0;
    static int counter_max = 32;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[index], z, i);
            pixel = pix;
        }
        ++counter;
        if(counter > counter_max) {
            counter = 0;
            if(rand()%50==0) {
                counter_max += 10;
                if(counter_max > frame.rows/4)
                    counter_max = 32;
            }
            ++index;
            if(index > collection.size()-1) {
                index = 0;
            }
        }
    }
    AddInvert(frame);
}

void ac::PictureStretch(cv::Mat &frame) {
    
    int index = 0;
    cv::Mat copy1 = frame.clone();
    int offset = 50+rand()%150;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            int value_offset = 0;
            if(index == 0) {
                value_offset = frame.cols+offset;
                index = 1;
            } else {
                value_offset = frame.cols-offset;
                index = 0;
            }
            int col = AC_GetFX(frame.cols, i, value_offset);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(copy1, z, col);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::PictureStretchPieces(cv::Mat &frame) {
    static int counter = 0;
    static int index = 0;
    static int counter_max = 2;
    int offset = 50+rand()%100;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            int value_offset = 0;
            if(index == 0) {
                value_offset = frame.cols+offset;
            } else {
                value_offset = frame.cols-offset;
            }
            int col = AC_GetFX(frame.cols, i, value_offset);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(copy1, z, col);
                pixel = pix;
            }
            ++counter;
            if(counter > counter_max) {
                counter = 0;
                counter_max = 2;
                index = (index == 0) ? 1 : 0;
            }
        }
        ++counter;
        if(counter > counter_max) {
            counter = 0;
            counter_max = 2;
            index = (index == 0) ? 1 : 0;
        }
    }
    AddInvert(frame);
}

void ac::VisualSnow(cv::Mat &frame) {
    static int max = 2;
    static int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int rx = rand()%max;
            int ry = rand()%max;
            if(index == 0) {
                rx = -rx;
                ry = -ry;
            }
            int offset_x = frame.cols+rx;
            int offset_y = frame.rows+ry;
            int col_x = AC_GetFX(frame.cols, i, offset_x);
            int col_y = AC_GetFZ(frame.rows, z, offset_y);
            if(col_x >= 0 && col_x < frame.cols && col_y >= 0 && col_y < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy1, col_y, col_x);
                //pixel = pix;
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    index = (index == 0) ? 1 : 0;
    static int dir = 1;
    if(dir == 1) {
        max += 10;
        if(max > frame.cols/6) {
            dir = 0;
        }
    } else {
        max -= 10;
        if(max <= 10) {
            max = 10;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::VisualSnowX2(cv::Mat &frame) {
    static int max = 2;
    static int index = 0;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int rx = rand()%max;
            int ry = rand()%max;
            if(index == 0) {
                rx = -rx;
            } else {
                ry = -ry;
            }
            int offset_x = frame.cols+rx;
            int offset_y = frame.rows+ry;
            int col_x = AC_GetFX(frame.cols, i, offset_x);
            int col_y = AC_GetFZ(frame.rows, z, offset_y);
            if(col_x >= 0 && col_x < frame.cols && col_y >= 0 && col_y < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy1, col_y, col_x);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
                }
            }
        }
    }
    index = (index == 0) ? 1 : 0;
    static int dir = 1;
    if(dir == 1) {
        max += 50;
        if(max > frame.cols/4) {
            dir = 0;
        }
    } else {
        max -= 50;
        if(max <= 10) {
            max = 10;
            dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::LineGlitch(cv::Mat &frame) {
    static int length = 1;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    int counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel = pix;
            ++counter;
            if(counter > length) {
                counter = 0;
                ++offset;
                if(offset > collection.size()-1)
                    offset = 0;
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        length += 10;
        if(length > 400)
            dir = 0;
    } else {
        length -= 10;
        if(length <= 10)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::SlitReverse64(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int dir = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel = pix;
        }
        if(dir == 1) {
            ++offset;
            if(offset > (collection.size()-1))
                dir = rand()%2;
        } else {
            --offset;
            if(offset <= 1)
                dir = rand()%2;
        }
    }
    AddInvert(frame);
}

void ac::SlitReverse64_Increase(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    static int dir = 1;
    static int max = 1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel = pix;
        }
        if(dir == 1) {
            ++offset;
            if(offset > max) {
                dir = rand()%2;
                static int m_dir = 1;
                if(m_dir == 1) {
                    ++max;
                    if(max > (collection.size()-1))
                        m_dir = 0;
                } else {
                    --max;
                    if(max <= 1)
                        m_dir = 1;
                }
            }
        } else {
            --offset;
            if(offset <= 1)
                dir = rand()%2;
            
        }
    }
    AddInvert(frame);
}

void ac::SlitStretch(cv::Mat &frame) {
    static int counter = 0;
    static int index = 0;
    static int counter_max = 2;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int m_index = 0;
    int offset = 50+rand()%100;
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i< frame.cols; ++i) {
            int value_offset = 0;
            if(index == 0) {
                value_offset = frame.cols+offset;
            } else {
                value_offset = frame.cols-offset;
            }
            int col = AC_GetFX(frame.cols, i, value_offset);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(collection.frames[m_index], z, col);
                pixel = pix;
            }
            ++counter;
            if(counter > counter_max) {
                counter = 0;
                counter_max = 2;
                index = (index == 0) ? 1 : 0;
                m_index++;
                if(m_index > (collection.size()-1))
                    m_index = 0;
            }
        }
        ++counter;
        if(counter > counter_max) {
            counter = 0;
            counter_max = 2;
            index = (index == 0) ? 1 : 0;
            m_index++;
            if(m_index > (collection.size()-1))
                m_index = 0;
        }
    }
    AddInvert(frame);
}

void ac::LineLeftRight(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    int ra = rand()%frame.cols;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int rx = frame.cols;
            int cx = 0;
            cx = AC_GetFX(frame.cols, i, rx+ra);
            if(cx >= 0 && cx < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(collection.frames[offset], z, cx);
                pixel = pix;
            }
        }
            ++offset;
            if(offset > (collection.size()-1))
                offset = 0;
    }
    AddInvert(frame);
}

void ac::LineLeftRightResize(cv::Mat &frame) {
    static MatrixCollection<128> collection;
    collection.shiftFrames(frame);
    static int offset = 0;
    int ra = rand()%frame.cols;
    int rb = rand()%frame.rows;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int rx = frame.cols;
            int ry = frame.rows;
            int cx = 0, cy = 0;
            cx = AC_GetFX(frame.cols, i, rx+ra);
            cy = AC_GetFZ(frame.rows, z, ry+rb);
            if(cx >= 0 && cx < frame.cols && cy >= 0 && cy < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(collection.frames[offset], cy, cx);
                pixel = pix;
            }
        }
        ++offset;
        if(offset > (collection.size()-1))
            offset = 0;
    }
    AddInvert(frame);
  
}

void ac::SoloInOrder(cv::Mat &frame) {
    static bool off = false;
    if(off == true) return;
    static int index = 0;
    static int frame_counter = 0;
    CallFilter(ac::solo_filter[index], frame);
    std::string name = ac::solo_filter[index];
    static int seconds = 0;
    ++frame_counter;
    if(frame_counter > ac::fps) {
        ++seconds;
        frame_counter = 0;
        if(seconds >= 2) {
            seconds = 0;
            index++;
            if(index > ac::solo_filter.size()-1) {
                off = true;
                index = 0;
            }
        }
    }
    cv::putText(frame, name,cv::Point(40, 40),cv::FONT_HERSHEY_DUPLEX,1.0,CV_RGB(255, 255, 255), 2);
}

void ac::ImageInOrder(cv::Mat &frame) {
    static bool off = false;
    if(off == true) return;
    static int index = 0;
    static int frame_counter = 0;
    std::string name = ac::svImage[index];
    std::cout << "[" << index << "," << ac::svImage.size() << "] = Calling ... " << name << "\n";
    CallFilter(ac::svImage[index], frame);
    static int seconds = 0;
    ++frame_counter;
    if(frame_counter > ac::fps) {
        ++seconds;
        frame_counter = 0;
        if(seconds >= 2) {
            seconds = 0;
            index++;
            if(index > ac::svImage.size()-1) {
                off = true;
                index = 0;
            }
        }
    }
    cv::putText(frame, name,cv::Point(40, 40),cv::FONT_HERSHEY_DUPLEX,1.0,CV_RGB(255, 255, 255), 2);
}

void ac::SubInOrder(cv::Mat &frame) {
    static bool off = false;
    if(off == true) return;
    static int index = 0;
    static int frame_counter = 0;
    static int sub_index = 0;

    std::string name = ac::vSub[index];
    std::string sub_name = solo_filter[sub_index];
    
    if(name != sub_name && sub_name.find("SubFilter") == std::string::npos) {
        std::cout << "[" << index << "," << ac::vSub.size() << "] = Calling ... " << name << "\n";
        pushSubFilter(ac::filter_map[sub_name]);
        CallFilter(name, frame);
        popSubFilter();
    }
    
    static int seconds = 0;
    ++frame_counter;
    if(frame_counter > ac::fps) {
        ++seconds;
        frame_counter = 0;
        if(seconds >= 2) {
            seconds = 0;
            index++;
            ++sub_index;
            if(sub_index > solo_filter.size()-1)
                sub_index = 0;

            if(index > ac::vSub.size()-1) {
                off = true;
                index = 0;
            }
        }
    }
    std::ostringstream stream;
    stream << name << " sub: " << sub_name;
    cv::putText(frame, stream.str(),cv::Point(40, 40),cv::FONT_HERSHEY_DUPLEX,1.0,CV_RGB(255, 255, 255), 2);
}

void ac::RGBLineTrails(cv::Mat &frame) {
    static MatrixCollection<12> collection;
    collection.shiftFrames(frame);
    int offset = 0;
    static int index = 0;
    for(int z = 0; z  < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel[index] = pix[index];
            
        }
        ++offset;
        if(offset > (collection.size()-1))
            offset = 0;
    }
    ++index;
    if(index > 2)
        index = 0;
    
    AddInvert(frame);
}

void ac::RGBCollectionBlend(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int off_x = 0, off_y = 8, off_z = 14;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b rgb[3];
            rgb[0] = collection.frames[off_x].at<cv::Vec3b>(z, i);
            rgb[1] = collection.frames[off_y].at<cv::Vec3b>(z, i);
            rgb[2] = collection.frames[off_z].at<cv::Vec3b>(z, i);
            pixel[0] = rgb[0][0];
            pixel[1] = rgb[1][1];
            pixel[2] = rgb[2][2];
        }
    }
    ++off_x;
    ++off_y;
    ++off_z;
    
    if(off_x > (collection.size()-1))
        off_x = 0;
   
    if(off_y > (collection.size()-1))
        off_y = 0;
    
    if(off_z > (collection.size()-1))
        off_z = 0;
    
    BlendWithSource25(frame);
    AddInvert(frame);
}

void ac::RGBCollectionIncrease(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    static int off_x = 0, off_y = 1, off_z = 2;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b rgb[6];
            rgb[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            rgb[1] = collection.frames[8].at<cv::Vec3b>(z, i);
            rgb[2] = collection.frames[14].at<cv::Vec3b>(z, i);
            pixel[0] = rgb[off_x][0];
            pixel[1] = rgb[off_y][1];
            pixel[2] = rgb[off_z][2];
        }
    }
    ++off_x;
    ++off_y;
    ++off_z;
    if(off_x > 2)
        off_x = 0;
    
    if(off_y > 2)
        off_y = 0;
    
    if(off_z > 2)
        off_z = 0;
    AddInvert(frame);
}

void ac::RGBCollectionEx(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int offset_i = frame.cols, offset_z = frame.rows;
    static int index = 0;
    static int off_x = 0, off_y = 1, off_z = 2;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b rgb[6];
            int cx = AC_GetFX(frame.cols, i, offset_i+index);
            int cy = AC_GetFZ(frame.rows, z, offset_z+index);
            
            if(cy >= 0 && cy < frame.rows && cx >= 0 && cx < frame.cols) {
                rgb[0] = collection.frames[1].at<cv::Vec3b>(cy, cx);
                rgb[1] = collection.frames[15].at<cv::Vec3b>(cy, cx);
                rgb[2] = collection.frames[31].at<cv::Vec3b>(cy, cx);
                pixel[0] = rgb[off_x][0];
                pixel[1] = rgb[off_y][1];
                pixel[2] = rgb[off_z][2];
            }
        }
    }
    ++off_x;
    ++off_y;
    ++off_z;
    if(off_x > 2)
        off_x = 0;
    
    if(off_y > 2)
        off_y = 0;
    
    if(off_z > 2)
        off_z = 0;
    static int dir = 1;
    if(dir == 1) {
        ++index;
        if(index > frame.cols*2)
            dir = 0;
    } else {
        --index;
        if(index <= 1)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::RGBLongTrails(cv::Mat &frame) {
    static MatrixCollection<64> collection;
    collection.shiftFrames(frame);
    static int off_x = 0, off_y = 1, off_z = 2, off_x1 = 3, off_y1 = 4, off_z1 = 5;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b rgb[8];
            rgb[0] = collection.frames[1].at<cv::Vec3b>(z, i);
            rgb[1] = collection.frames[16].at<cv::Vec3b>(z, i);
            rgb[2] = collection.frames[31].at<cv::Vec3b>(z, i);
            rgb[3] = collection.frames[47].at<cv::Vec3b>(z, i);
            rgb[4] = collection.frames[56].at<cv::Vec3b>(z, i);
            rgb[5] = collection.frames[63].at<cv::Vec3b>(z, i);
            pixel[0] = static_cast<unsigned char>((0.5 * rgb[off_x][0]) + (0.5 * rgb[off_x1][0]));
            pixel[1] = static_cast<unsigned char>((0.5 * rgb[off_y][1]) + (0.5 * rgb[off_y1][1]));
            pixel[2] = static_cast<unsigned char>((0.5 * rgb[off_z][2]) + (0.5 * rgb[off_z1][2]));
        }
    }
    ++off_x;
    ++off_y;
    ++off_z;
    ++off_x1;
    ++off_y1;
    ++off_z1;
    
    if(off_x > 2)
        off_x = 0;
    if(off_y > 2)
        off_y = 0;
    if(off_z > 2)
        off_z = 0;
    if(off_x1 > 5)
        off_x1 = 3;
    if(off_y1 > 5)
        off_y1 = 3;
    if(off_z1 > 5)
        off_z1 = 3;
    
    AddInvert(frame);
}

void ac::VideoMatrixOutlineSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "VideoMatrixOutlineSubFilter")
        return;
    cv::Mat copyf;
    
    if(VideoFrame(copyf)) {
        cv::Mat copy1;
        ac_resize(copyf, copy1, frame.size());
        static MatrixCollection<8> collection;
        CallFilter(subfilter, frame);
        collection.shiftFrames(frame);
        MedianBlendIncrease(copy1);
        static constexpr int val = 4;
        cv::Vec3b intensity(getPixelCollection(), getPixelCollection(), getPixelCollection());
        cv::Mat &copy_frame = collection.frames[val];
        for(int z = 0; z < copy_frame.rows; ++z) {
            for(int i = 0; i < copy_frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = pixelAt(copy_frame,z, i);
                if(colorBounds(pixel, pix, intensity, intensity)) {
                    pixel = cv::Vec3b(0,0,0);
                } else {
                    pixel = copy1.at<cv::Vec3b>(z, i);
                }
            }
        }
        AddInvert(frame);
    }
}

void ac::FadeRGB_Speed(cv::Mat &frame) {
    static double alpha = 0.1;
    static int dir = 1;
    static int offset = 0;
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            pixel[offset] = static_cast<unsigned char>(pixel[offset] * alpha);
        }
    }
    ++offset;
    if(offset > 2)
        offset = 0;
    AddInvert(frame);
}

void ac::RGBStrobeTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    collection.shiftFrames(frame);
    static bool strobe = false;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix[4];
            pix[0] = pixelAt(collection.frames[1], z, i);
            pix[1] = pixelAt(collection.frames[4], z, i);
            pix[2] = pixelAt(collection.frames[7], z, i);
            for(int j = 0; j < 3; ++j) {
                if(strobe == false)
                    pixel[j] = pix[3-j-1][j];
                else
                    pixel[j] = pix[j][j];
            }
        }
    }
    strobe = (strobe == true) ? false : true;
    AddInvert(frame);
}

void ac::FadeRGB_Variable(cv::Mat &frame) {
    static double alpha[3] = {0.1, 0.5, 0.7};
    static int dir[3] = {1, 0, 1};
    AlphaMovementMaxMin(alpha[0], dir[0], 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha[1], dir[1], 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha[2], dir[2], 0.01, 1.0, 0.1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.3*(pixel[j]*alpha[j])));
            }
        }
    }
    MedianBlendMultiThread2(frame);
    AddInvert(frame);
}

void ac::BoxGlitch(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    int offset = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pix = pixelAt(collection.frames[offset], z, i);
            pixel = pix;
        }
        if((rand()%50)==0) {
            ++offset;
            if(offset > collection.size()-1)
                offset = 0;
        }
    }
    AddInvert(frame);
}

void ac::VerticalPictureDistort(cv::Mat &frame) {
    
    static int offset = 0;
    if(offset == 0 || offset > frame.rows || offset < 0) {
        offset = frame.rows/4;
    }
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = offset; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int col = AC_GetFZ(frame.rows, z, z*2);
            if(col >= 0 && col < frame.rows && i >= 0 && i < frame.cols) {
                cv::Vec3b pix = pixelAt(frame, col, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
    static int counter = 0;
    ++counter;
    if(counter > static_cast<int>(ac::fps/6)) {
        counter = 0;
        static int dir = 1;
        if(dir == 1) {
            ++offset;
            if(offset > frame.rows/2)
                dir = 0;
        } else {
            --offset;
            if(offset <= 1)
                dir = 1;
        }
    }
}

void ac::ShortTrail(cv::Mat &frame) {
    static MatrixCollection<16> collection;
    collection.shiftFrames(frame);
    int r = rand()%(collection.size()-1);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[r], out, 0.7, 0.3);
    frame = out.clone();
    AddInvert(frame);
}

void ac::DiagInward(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; i += 2) {
            if(i >= 0 && i+1 < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(collection.frames[index],z, i);
                pixel = pix;
                cv::Vec3b &pixel1 = pixelAt(frame, z, i+1);
                cv::Vec3b pix1 = pixelAt(collection.frames[collection.size()-index-1],z, i+1);
                pixel1 = pix1;
            }
        }

        ++index;
        if(index > (collection.size()-1))
            index = 0;
    }
    AddInvert(frame);
}

void ac::DiagSquareInward(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static constexpr int SIZE_COL = 8;
    static int offset = 0;
    static bool on_ = true;
    for(int z = 0; z < frame.rows; z += SIZE_COL) {
        for(int i = 0; i < frame.cols; i += SIZE_COL) {
            for(int x = 0; x < SIZE_COL; ++x) {
                for(int y = 0; y < SIZE_COL; ++y) {
                    if((z+y) < frame.rows && (i+x) < frame.cols) {
                        cv::Vec3b &pixel = pixelAt(frame, z+y, i+x);
                        int pos = 0;
                        if(on_) {
                            pos = offset;
                        } else {
                            pos = collection.size()-offset-1;
                        }
                        if(pos >= 0 && pos < collection.size()-1) {
                            cv::Vec3b pix = pixelAt(collection.frames[pos], z+y, i+x);
                            pixel = pix;
                        }
                    }
                }
            }
            ++offset;
            if(offset > collection.size()-1)
                offset = 0;
            on_ = (on_ == true) ? false : true;
        }
    }
    AddInvert(frame);
}

void ac::DiagSquareInwardResize(cv::Mat &frame) {
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int SIZE_COL = 8;
    static int offset = 0;
    static bool on_ = true;
    for(int z = 0; z < frame.rows; z += SIZE_COL) {
        for(int i = 0; i < frame.cols; i += SIZE_COL) {
            for(int x = 0; x < SIZE_COL; ++x) {
                for(int y = 0; y < SIZE_COL; ++y) {
                    if((z+y) < frame.rows && (i+x) < frame.cols) {
                        
                        cv::Vec3b &pixel = pixelAt(frame, z+y, i+x);
                        int pos = 0;
                        if(on_) {
                            pos = offset;
                        } else {
                            pos = collection.size()-offset-1;
                        }
                        if(pos >= 0 && pos < collection.size()-1) {
                            cv::Vec3b pix = pixelAt(collection.frames[pos], z+y, i+x);
                            pixel = pix;
                        }
                    }
                }
            }
            ++offset;
            if(offset > collection.size()-1)
                offset = 0;
            on_ = (on_ == true) ? false : true;
        }
    }
    static bool dir = true;
    if(dir == true) {
        SIZE_COL += 2;
        if(SIZE_COL > 64)
            dir = false;
    } else {
        SIZE_COL -= 2;
        if(SIZE_COL <= 8)
            dir = true;
    }
    AddInvert(frame);
}

void ac::DiagSquareInwardResizeXY(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int SIZE_COL_W = 8, SIZE_COL_H = 16;
    static int offset = 0;
    static bool on_ = true;
    for(int z = 0; z < frame.rows; z += SIZE_COL_H) {
        for(int i = 0; i < frame.cols; i += SIZE_COL_W) {
            for(int x = 0; x < SIZE_COL_W; ++x) {
                for(int y = 0; y < SIZE_COL_H; ++y) {
                    if((z+y) < frame.rows && (i+x) < frame.cols) {
                        cv::Vec3b &pixel = pixelAt(frame, z+y, i+x);
                        int pos = 0;
                        if(on_) {
                            pos = offset;
                        } else {
                            pos = collection.size()-offset-1;
                        }
                        if(pos >= 0 && pos < collection.size()-1) {
                            cv::Vec3b pix = pixelAt(collection.frames[pos], z+y, i+x);
                            pixel = pix;
                        }
                    }
                }
            }
            ++offset;
            if(offset > collection.size()-1)
                offset = 0;
            on_ = (on_ == true) ? false : true;
        }
    }
    static bool dir = true, dir2 = false;
    if(dir == true) {
        SIZE_COL_W += rand()%2;
        if(SIZE_COL_W > 64)
            dir = false;
    } else {
        SIZE_COL_W -= rand()%2;
        if(SIZE_COL_W <= 8)
            dir = true;
    }
    if(dir2 == true) {
        SIZE_COL_H += rand()%2;
        if(SIZE_COL_H > 64)
            dir2 = false;
    } else {
        SIZE_COL_H -= rand()%2;
        if(SIZE_COL_H <= 8)
            dir2 = true;
    }
    AddInvert(frame);
}

