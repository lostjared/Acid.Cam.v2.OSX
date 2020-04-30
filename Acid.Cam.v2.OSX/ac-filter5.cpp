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


#include"ac.h"

void ac::RandomFilteredSquare(cv::Mat &frame) {
    static std::unique_ptr<Box[]> boxes;
    int num_boxes = static_cast<int>(frame.cols/0.5);
    if(boxes == 0 || (frame.cols != Box::frame_width)) {
        boxes.reset(new Box[num_boxes]);
        Box::frame_width = frame.cols;
        Box::frame_height = frame.rows;
        for(int i = 0; i < num_boxes; ++i)
            boxes.get()[i].initBox(frame.cols, frame.rows);
    }
    
    for(int i = 0; i < num_boxes; ++i) {
        boxes.get()[i].sizeBox();
        boxes.get()[i].drawBox(frame);
    }
}

void ac::RandomQuads(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int j = 0; j < 4; ++j) {
        int frame_index = 0;
        do {
            frame_index = rand()%28;
        } while(frame_index == 13 || frame_index == 14);
        CallFilter(frame_index, collection.frames[j]);
    }
    cv::Size quarter(frame.cols/2, frame.rows/2);
    ac::copyMat(collection.frames[0],0, 0, frame, ac::Rect(0, 0, quarter));
    ac::copyMat(collection.frames[1],frame.cols/2, 0, frame, ac::Rect(frame.cols/2,0, quarter));
    ac::copyMat(collection.frames[2],frame.cols/2, frame.rows/2, frame, ac::Rect(frame.cols/2, frame.rows/2, quarter));
    ac::copyMat(collection.frames[3],0, frame.rows/2, frame, ac::Rect(0,frame.rows/2, quarter));
}

void ac::QuadCosSinMultiply(cv::Mat &frame) {
    cv::Mat frame_copy = frame.clone();
    cv::Size quarter(frame.cols/2, frame.rows/2);
    DrawFunction procFunc = getFilter(ac::draw_strings[15]);
    procFunc(frame_copy);
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, 0, frame, ac::Rect(0, 0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, 0, frame, ac::Rect(frame.cols/2,0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, frame.rows/2, frame, ac::Rect(frame.cols/2, frame.rows/2, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, frame.rows/2, frame, ac::Rect(0,frame.rows/2, quarter));
}

void ac::QuadRandomFilter(cv::Mat &frame) {
    
    if(testSize(frame) == false)
        return;
    
    cv::Mat frame_copy = frame.clone();
    cv::Size quarter(frame.cols/2, frame.rows/2);
    DrawFunction baseFilter = getRandomFilter();
    baseFilter(frame_copy);
    DrawFunction procFunc = getRandomFilter();
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, 0, frame, ac::Rect(0, 0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, 0, frame, ac::Rect(frame.cols/2,0, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,frame.cols/2, frame.rows/2, frame, ac::Rect(frame.cols/2, frame.rows/2, quarter));
    procFunc(frame_copy);
    ac::copyMat(frame_copy,0, frame.rows/2, frame,ac::Rect(0,frame.rows/2, quarter));
}

void ac::RollRandom(cv::Mat &frame) {
    
    if(testSize(frame) == false)
        return;
    
    SquareVertical_Roll(frame);
    DrawFunction rand_func = getRandomFilter();
    rand_func(frame);
}

void ac::AverageRandom(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    cv::Mat frame_copy = frame.clone(), frame_copy2 = frame.clone();
    DrawFunction func = getRandomFilter();
    func(frame_copy);
    func(frame_copy2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix1 = frame_copy.at<cv::Vec3b>(z, i);
            cv::Vec3b pix2 = frame_copy2.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j] + pix1[j] + pix2[j]);
                pixel[j] /= static_cast<unsigned char>(1.5);
                pixel[j] = static_cast<unsigned char>(pixel[j] * (1+alpha));
            }
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::HorizontalStripes(cv::Mat &frame) {
    if(frame.empty() || frame.rows < 25 || frame.cols < 25)
        return;
    static cv::Scalar value(1.0, 1.0, 1.0);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                double rval = rand()%10;
                double val = rval * 0.001;
                value[j] += val;
                if(value[j] > 255) value[j] = 0;
                pixel[j] = pixel[j] ^ static_cast<int>(value[j]);
            }
        }
    }
    MedianBlur(frame);
    Bitwise_XOR(frame);
}

void ac::DiamondStrobe(cv::Mat &frame) {
    static double pos = 1.0;// set pos to 1.0
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    static int index1 = 0, index2 = 2;
    
    ++index1;
    if(index1 > 2) index1 = 0;
    ++index2;
    if(index2 > 2) index2 = 0;
    
    for(int z = 0; z < h; ++z) {// from top to bottom
        for(int i = 0; i < w; ++i) {// from left to right
            cv::Vec3b &buffer = pixelAt(frame,z, i);// get current pixel
            // calculate the colors of the gradient diamonds
            if((i%2) == 0) {// if i % 2 equals 0
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[index1] = static_cast<unsigned char>(1-pos*buffer[0]);
                    buffer[index2] = static_cast<unsigned char>((i+z)*pos);
                } else {
                    // set pixel coomponent values
                    buffer[index1] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[index2] = static_cast<unsigned char>((i-z)*pos);
                }
            } else {
                if((z%2) == 0) {// if z % 2 equals 0
                    // set pixel component values
                    buffer[index1] = static_cast<unsigned char>(pos*buffer[0]-i);
                    buffer[index2] = static_cast<unsigned char>((i-z)*pos);
                } else {
                    // set pixel component values
                    buffer[index1] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[index2] = static_cast<unsigned char>((i+z)*pos);
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    // static direction starts off with 1
    static double pos_max = 7.0f;// pos maximum
    static int direction = 1;
    procPos(direction, pos, pos_max);
}

void ac::SmoothTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    Smooth(frame, &collection);
}


void ac::Dual_SelfAlphaRainbow(cv::Mat &frame) {
    static double alpha1 = 2.0, alpha2 = 10.0, pos_max = 10.0;
    static bool  switch_on = true;
    rainbowBlend(frame);
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(switch_on == true) {
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha1);
            } else {
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>(pixel[j]*alpha2);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
        switch_on = !switch_on;
    }
    static int direction[2] = { 1, 0 };
    procPos(direction[0], alpha1, pos_max);
    procPos(direction[1], alpha2, pos_max);
}

void ac::Dual_SelfAlphaBlur(cv::Mat &frame) {
    Dual_SelfAlphaRainbow(frame);
    MedianBlur(frame);
    MedianBlur(frame);
    Bitwise_XOR(frame);
}

void ac::SurroundPixelXor(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    for(int z = 0; z < frame.rows-3; ++z) {
        for(int i = 0; i < frame.cols-3; ++i) {
            cv::Vec3b pix[3];
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pix[0] = pixelAt(frame,z+1, i);
            pix[1] = pixelAt(frame,z, i+1);
            pix[2] = pixelAt(frame,z+1, i+1);
            cv::Scalar value;
            value[0] = pix[0][0]+pix[1][0]+pix[2][0];
            value[1] = pix[0][1]+pix[1][1]+pix[2][1];
            value[3] = pix[0][2]+pix[1][2]+pix[2][2];
            for(int j = 0; j < 3; ++j) {
                int val = static_cast<int>(value[j]);
                pixel[j] = static_cast<unsigned char>((val^pixel[j])*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::Darken(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[0] /= 6;
            pixel[1] /= 6;
            pixel[2] /= 6;
        }
    }
}

void ac::WeakBlend(cv::Mat &frame) {
    static int index = 0;
    static cv::Scalar value((rand()%5)+1,(rand()%5)+1,(rand()%5)+1);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                int val = static_cast<int>(pixel[j]+(pixel[j]*value[index]));
                val /= 2;
                pixel[j] = static_cast<unsigned char>(val);
            }
            index ++;
            if(index > 2)
                index = 0;
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    for(int j = 0; j < 3; ++j) {
        value[j] += 1+((rand()%5) * 0.5);
        if(value[j] > 10) {
            value[j] = rand()%10;
        }
    }
}

void ac::AverageVertical(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    std::unique_ptr<cv::Scalar[]> values(new cv::Scalar[frame.rows]);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                values.get()[z][j] += pixel[j];
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar val = values.get()[z];
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                val[j] /= frame.rows;
                pixel[j] += static_cast<unsigned char>(val[j]*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}


void ac::RandomCollectionAverage(cv::Mat &frame) {
    
    if(testSize(frame) == false)
        return;
    
    static MatrixCollection<8> collection;
    DrawFunction randF = getRandomFilter();
    randF(frame);
    Smooth(frame, &collection);
}

void ac::RandomCollectionAverageMax(cv::Mat &frame) {
    
    if(testSize(frame) == false)
        return;
    
    static MatrixCollection<16> collection;
    DrawFunction randF = getRandomFilter();
    randF(frame);
    Smooth(frame, &collection);
}

void ac::SmoothTrailsSelfAlphaBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlur(frame);
    SelfAlphaBlend(frame);
    Smooth(frame, &collection);
}

void ac::SmoothTrailsRainbowBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    MedianBlur(frame);
    rainbowBlend(frame);
    Smooth(frame, &collection);
}

void ac::MedianBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    int r = 3+rand()%7;
    for(int i = 0; i < r; ++i)
        MedianBlur(frame);
    
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Scalar value;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pixel = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] += pixel[q];
                }
            }
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                int val = 1+static_cast<int>(value[j]);
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ val);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}




void ac::RandomAlphaBlend(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    double alpha = 1.0, alpha_max = 6.0;
    if(testSize(frame) == false)
        return;
    DrawFunction randFunc = getRandomFilter();
    cv::Mat temp = frame.clone(), rand_frame = frame.clone();
    randFunc(rand_frame);
    collection.shiftFrames(rand_frame);
    AlphaBlend(temp,rand_frame, frame, alpha);
    Smooth(frame, &collection);
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::RandomTwoFilterAlphaBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 5.0;
    static MatrixCollection<8> collection;
    if(collection.empty()) collection.shiftFrames(frame);
    cv::Mat one, two, output;
    one = frame.clone();
    two = frame.clone();
    Random_Filter(one);
    Random_Filter(two);
    AlphaBlend(one, two, output, alpha);
    collection.shiftFrames(output);
    Smooth(frame, &collection);
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::PixelatedSquare(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 3.0;
    cv::Mat filter_1 = frame.clone();
    cv::Mat frame_copy;
    SurroundPixelXor(filter_1);
    GridFilter16x(filter_1);
    
    AlphaBlend(filter_1,frame,frame_copy,alpha);
    collection.shiftFrames(frame_copy);
    Smooth(frame, &collection);
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::AlphaBlendPosition(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 4.0;
    int pos_x = 0;
    for(int z = 0; z < frame.rows; ++z) {
        cv::Vec3b pix = pixelAt(frame,z, pos_x);
        ++pos_x;
        if(pos_x > frame.cols-1) pos_x = 0;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j]*(alpha+1))+(pix[j]*alpha));
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::BlendRowAlpha(cv::Mat &frame) {
    static int row = 0;
    static double alpha = 1.0, alpha_max = 4.0;
    for(int i = 0; i < frame.cols; ++i) {
        row++;
        if(row > frame.cols) row = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j] ^ row)*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::BlendRow(cv::Mat &frame) {
    static int row = 0;
    for(int i = 0; i < frame.cols; ++i) {
        row++;
        if(row > frame.cols) row = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j] ^ row);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
}

void ac::BlendRowByVar(cv::Mat &frame) {
    static int row = 0;
    for(int i = 0; i < frame.cols; ++i) {
        row++;
        if(row > frame.cols) row = 0;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]+(z-i)) ^ row);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel
        }
    }
}

void ac::BlendRowByDirection(cv::Mat &frame) {
    static int row = 0;
    static int direction = 1;
    for(int i = 0; i < frame.cols; ++i) {
        if(direction == 1) {
            ++row;
            if(row > frame.cols)
                direction = 0;
        } else if(direction == 0) {
            --row;
            if(row <= 0) {
                direction = 1;
            }
        }
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                if(direction == 1)
                    pixel[j] = static_cast<unsigned char>((pixel[j]+i+z) & row);
                else if(direction == 0)
                    pixel[j] = static_cast<unsigned char>((pixel[j]^row));
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

void ac::BlendAlphaXor(cv::Mat &frame) {
    static int r = 3;
    static int direction = 1;
    static double alpha = 1.0, alpha_max = 10.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                unsigned char val = static_cast<unsigned char>(r*alpha);
                pixel[j] += cv::saturate_cast<unsigned char>(pixel[j]^val);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    if(direction == 1) {
        ++r;
        if(r >= 255)
            direction = 0;
    } else if(direction == 0) {
        --r;
        if(r <= 3)
            direction = 1;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::SelfXorScale(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    static int value = 1;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j]^value)*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int direction = 1;
    if(direction == 1)  {
        ++value;
        if(value > 254)
            direction = 0;
    } else if(direction == 0) {
        --value;
        if(value <= 1)
            direction = 1;
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::BitwiseXorScale(cv::Mat &frame) {
    static cv::Mat frame1 = frame.clone();
    cv::Mat temp = frame.clone();
    
    if(frame1.size()!=frame.size())
        frame1 = temp.clone();
    
    static double alpha = 1.0, alpha_max = 2.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = frame1.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] += static_cast<unsigned char>((pixel[j]^pix[j])*alpha);
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    frame1 = temp;
    static int direction = 1;
    procPos(direction, alpha, alpha_max);
}

void ac::XorTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b value;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] ^= frame_val[q];
                }
            }
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^value[j];
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

void ac::RainbowTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b value;
            cv::Vec3b &pixel = pixelAt(frame,z, i);;
            for(int j = 1; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    value[q] += frame_val[q];
                }
            }
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^value[j];
            }
        }
    }
}
void ac::NegativeTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 8.0;
    collection.shiftFrames(frame);
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b value;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q) {
                    pixel[q] ^= pixel[q]+frame_val[q];
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::IntenseTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static double alpha = 1.0, alpha_max = 8.0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b value = pixel;
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b frame_val = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q =0; q < 3; ++q) {
                    value[q] += static_cast<unsigned char>(frame_val[q]*alpha);
                    
                }
            }
            for(int j = 0; j < 3; ++j)
                pixel[j] ^= value[j];
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
}

void ac::SelfAlphaRGB(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    static int index = 0;
    for(int i = 0; i < frame.cols-2; ++i) {
        for(int z = 0;  z < frame.rows-2; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix[4];
            pix[0] = pixelAt(frame,z+1, i+1);
            pix[1] = pixelAt(frame,z+1, i);
            pix[2] = pixelAt(frame,z, i+1);
            pix[3] = pixel;
            switch(index) {
                case 0:
                    pixel[0] = static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[1] = static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[2] = static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
                case 1:
                    pixel[2] = static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[1] = static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[0] = static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
                case 2:
                    pixel[1] = static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[0] = static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[2] = static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
                case 3:
                    pixel[0] = pixel[0]^static_cast<unsigned char>(pix[0][0]*alpha);
                    pixel[1] = pixel[1]^static_cast<unsigned char>((pix[0][1]+pix[1][1])*alpha);
                    pixel[2] = pixel[2]^static_cast<unsigned char>((pix[0][2]+pix[1][2]+pix[2][2])*alpha);
                    break;
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    
    ++index;
    if(index > 3) index = 0;
}


void ac::BitwiseXorStrobe(cv::Mat &frame) {
    static int index = 0;
    static double alpha1 = 1.0, alpha2 = 10.0, alpha3 = 5.0,alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            switch(index) {
                case 0:
                    pixel[0] += cv::saturate_cast<unsigned char>(pixel[0] * alpha1);
                    pixel[1] += cv::saturate_cast<unsigned char>(pixel[1] * alpha2);
                    pixel[2] += cv::saturate_cast<unsigned char>(pixel[2] * alpha3);
                    break;
                case 1:
                    pixel[2] += cv::saturate_cast<unsigned char>(pixel[0] * alpha1);
                    pixel[0] += cv::saturate_cast<unsigned char>(pixel[1] * alpha2);
                    pixel[1] += cv::saturate_cast<unsigned char>(pixel[2] * alpha3);
                    break;
                case 2:
                    pixel[1] += cv::saturate_cast<unsigned char>(pixel[0] * alpha1);
                    pixel[2] += cv::saturate_cast<unsigned char>(pixel[1] * alpha2);
                    pixel[0] += cv::saturate_cast<unsigned char>(pixel[2] * alpha3);
                    break;
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir1 = 1, dir2 = 0, dir3 = 1;
    procPos(dir1, alpha1, alpha_max);
    procPos(dir2, alpha2, alpha_max);
    procPos(dir3, alpha3, alpha_max);
    
    ++index;
    if(index > 2) index = 0;
    Bitwise_XOR(frame);
}


void ac::AlphaBlendRandom(cv::Mat &frame) {
    
    if(testSize(frame) == false)
        return;
    
    static double val = 0.30;
    static int val_dir = 1;
    DrawFunction func[2];
    func[0] = getRandomFilter();
    func[1] = getRandomFilter();
    cv::Mat copy[4];
    copy[0] = frame.clone();
    copy[1] = frame.clone();
    copy[2] = frame.clone();
    func[0](copy[0]);
    func[1](copy[1]);
    AlphaBlend(copy[0], copy[1], copy[3], 0.5);
    AlphaBlend(copy[2], copy[3], frame, val);
    if(val_dir == 1) {
        val += 0.05;
        if(val >= 1.0)
            val_dir = 0;
    } else {
        val -= 0.05;
        if(val <= 0.30)
            val_dir = 1;
    }
}

void ac::ChannelSortAlphaBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    static int index = 0;
    std::vector<cv::Mat> v; // to hold the Matrix for split
    cv::split(frame, v);// split the channels into seperate matrices
    cv::Mat channels[3]; // output channels
    cv::Mat output; // for merge
    cv::sort(v[0], channels[0],cv::SORT_ASCENDING); // sort each matrix
    cv::sort(v[1], channels[1],cv::SORT_ASCENDING);
    cv::sort(v[2], channels[2],cv::SORT_ASCENDING);
    cv::sort(v[index], channels[index], cv::SORT_DESCENDING);
    cv::merge(channels, 3, output);
    ++index;
    if(index > 2) index = 0;
    cv::Mat copy = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pixadd = output.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                //pixel += pixadd;
                pixel[j] = static_cast<unsigned char>((pixel[j] * (1+alpha)) + (pixadd[j] * alpha));
            }
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10, 0.01);
}

void ac::XorChannelSort(cv::Mat &frame) {
    std::vector<cv::Mat> v; // to hold the Matrix for split
    cv::split(frame, v);// split the channels into seperate matrices
    cv::Mat channels[3]; // output channels
    cv::Mat output; // for merge
    cv::sort(v[0], channels[0],(((rand()%2) == 0) ? cv::SORT_ASCENDING : cv::SORT_DESCENDING)); // sort each matrix
    cv::sort(v[1], channels[1],(((rand()%2) == 0) ? cv::SORT_ASCENDING : cv::SORT_DESCENDING)); // sort each matrix
    cv::sort(v[2], channels[2],(((rand()%2) == 0) ? cv::SORT_ASCENDING : cv::SORT_DESCENDING)); // sort each matrix
    cv::merge(channels, 3, output);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b sorted = output.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j] ^ sorted[j];
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

void ac::GradientColors(cv::Mat &frame) {
    static int index = 0;
    static unsigned char val = 0;
    int inc = (frame.rows/255)+1;
    for(int i = 0; i < frame.cols; ++i) {
        val = 1;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = val;
            if((z%inc) == 0)
                ++val;
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}

void ac::GradientColorsVertical(cv::Mat &frame) {
    static int index = 0;
    static unsigned char val = 0;
    int inc = (frame.cols/255)+1;
    for(int z = 0; z < frame.rows; ++z) {
        val = 1;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] = val;
            
            if((i%inc) == 0)
                ++val;
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    ++index;
    if(index > 2)
        index = 0;
}

void ac::Bitwise_XOR_Average(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 8.0;
    for(int z = 0; z < frame.rows; ++z) {
        cv::Scalar sval;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                sval[j] += pixel[j];
            }
        }
        
        for(int j = 0; j < 3; ++j)
            sval[j] /= frame.cols;
        
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j] * (1+alpha)) + (static_cast<unsigned char>(sval[j])*alpha));
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 0.01);
    Bitwise_XOR(frame);
}

void ac::GradientXorSelfScale(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = pixelAt(frame,z+1, i+1);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((pixel[j] * (1+alpha))) ^ pix[j];
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 10, 0.03);
}

void ac::SmoothSourcePixel(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int total = pixel[0]+pixel[1]+pixel[2]/3;
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(((pixel[j] ^ total) * static_cast<unsigned char>(alpha)));
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.1);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
}

void ac::StrobeBlend(cv::Mat &frame) {
    int value1 = ((frame.cols/2)/255)+1;
    int num = 1, num2 = 1;
    static double alpha = 1.0, alpha_max = 8.0;
    static int index1 = 0, index2 = 2;
    static int frame_num = 0;
    
    ++frame_num;
    if(frame_num > 1) frame_num = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        num = 1;
        num2 = 1;
        for(int i = 0; i < frame.cols/2; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if((i%value1)==0)
                num++;
            if(frame_num == 0) {
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j] ^ static_cast<unsigned char>(num);
                }
            } else {
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j] ^ static_cast<unsigned char>(num/(alpha+1));
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            //pixel[index1]= 255;
        }
        for(int i = frame.cols/2; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if((i%value1)==0)
                num2++;
            if(frame_num == 0) {
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j] ^ static_cast<unsigned char>(num2);
                }
            } else {
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = pixel[j] ^ static_cast<unsigned char>(num2/(alpha+1));
                }
            }
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            //pixel[index2] = 0;
        }
    }
    if(++index1 > 2) index1 = 0;
    if(--index2 < 0) index2 = 2;
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.03);
}

void ac::FrameBars(cv::Mat &frame) {
    int diff_i = (frame.cols/255)+1;
    int diff_z = (frame.rows/255)+1;
    unsigned char val[2] = {0,0};
    static double alpha = 1.0, alpha_max = 8.0;
    static MatrixCollection<4> collection;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += static_cast<unsigned char>(val[0]*alpha) + static_cast<unsigned char>(val[1]*alpha);
            
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            
            if((i%diff_i) == 0) {
                val[0]++;
            }
        }
        if((z%diff_z) == 0) {
            val[1]++;
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
}

void ac::Sort_Vertical_Horizontal(cv::Mat &frame) {
    cv::Mat value = frame.clone();
    VerticalChannelSort(value);
    ChannelSort(value);
    Add(frame, value, false);
    
}

void ac::Sort_Vertical_Horizontal_Bitwise_XOR(cv::Mat &frame) {
    cv::Mat value = frame.clone();
    VerticalChannelSort(value);
    ChannelSort(value);
    static bool sub = false;
    if(sub == false)
        Add(frame, value, false);
    else
        Sub(frame, value, false);
    sub = (sub == true) ? false : true;
    Bitwise_XOR(frame);
}


void ac::Scalar_Average_Multiply(cv::Mat &frame) {
    cv::Mat copy = frame.clone();
    VerticalChannelSort(frame);
    cv::Scalar average;
    ScalarAverage(frame, average);
    Transform(copy, frame, [&](cv::Vec3b &pixel, int , int ) {
        for(int j = 0; j < 3; ++j) {
            pixel[j] *= static_cast<unsigned char>(average[j]);
        }
    });
    cv::Mat out = frame.clone();
    AlphaBlend(out, copy, frame, 0.5);
}

void ac::Scalar_Average(cv::Mat &frame) {
    cv::Mat copy = frame.clone();
    cv::Scalar value;
    ScalarAverage(frame, value);
    Transform(copy, frame,[&](cv::Vec3b &pixel, int , int ) {
        for(int j = 0; j < 3; ++j)
            pixel[j] *= static_cast<unsigned char>(value[j]);
    });
    cv::Mat out = frame.clone();
    AlphaBlend(copy, out, frame, 0.5);
}

void ac::Total_Average(cv::Mat &frame) {
    cv::Mat frames[3];
    static double alpha = 1.0, alpha_max = 8.0;
    frames[0] = frame.clone();
    frames[1] = frame.clone();
    frames[2] = frame.clone();
    
    SelfScale(frames[0]);
    rainbowBlend(frames[1]);
    SelfAlphaRGB(frames[2]);
    
    unsigned long total[3];
    
    for(int j = 0; j < 3; ++j)
        TotalAverageOffset(frames[j], total[j]);
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += static_cast<unsigned char>((alpha*total[j]));
            
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    
}

void ac::FlashWhite(cv::Mat &frame) {
    static cv::Vec3b white(255,255,255);
    static bool state = false;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(state)
                pixelAt(frame,z, i) = white;
        }
    }
    state = (state == true) ? false : true;
}

void ac::FlashBlackAndWhite(cv::Mat &frame) {
    static int index = 0;
    static cv::Vec3b white(255,255,255), black(0,0,0);
    if(index != 1 && index != 3) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                switch(index) {
                    case 0:
                        pixelAt(frame,z, i) = white;
                        break;
                    case 2:
                        pixelAt(frame,z, i) = black;
                }
            }
        }
    }
    ++index;
    if(index > 3) index = 0;
}

void ac::GaussianBlend(cv::Mat &frame) {
    static double alpha = 1.0, alpha_max = 3.0;
    static MatrixCollection<8> collection;
    unsigned int r = 3+(rand()%10);
    for(unsigned int q = 0; q < r; ++q)
        GaussianBlur(frame);
    
    collection.shiftFrames(frame);
    int value[3] = { rand()%255, rand()%255, rand()%255 };
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j] ^ value[j]) * alpha);
            
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max, 15, 0.1);
    collection.shiftFrames(frame);
    Smooth(frame, &collection);
    
}

void ac::RandomXor(cv::Mat &frame) {
    int r_color[3] = { rand()%255, rand()%255, rand()%255 };
    static double alpha = 1.0, alpha_max = 3.0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>((pixel[j] ^ r_color[j]) * alpha);
            
            swapColors(frame, z, i);
            // if isNegative true invert pixel
            if(isNegative) invert(frame, z, i);
        }
    }
    static int dir = 1;
    procPos(dir, alpha, alpha_max);
    
}

