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


#include "ac.h"

void ac::Square::setSize(const int &xx, const int &yy, const int &w, const int &h) {
    x = xx;
    y = yy;
    if(width != w || height != h) {
        width = w;
        height = h;
        image.create(cv::Size(w, h), CV_8UC3);
    }
}
void ac::Square::setPos(const int &p) {
    pos = p;
}
void ac::Square::copyImage(const cv::Mat &f) {
    for(int i = 0, src_x = x; i < width; ++i, ++src_x) {
        for(int z = 0, src_y = y; z < height; ++z, ++src_y) {
            cv::Vec3b &pixel = image.at<cv::Vec3b>(z, i);
            cv::Vec3b src = f.at<cv::Vec3b>(src_y, src_x);
            pixel = src;
        }
    }
}
void ac::Square::copyImageToTarget(int xx, int yy, cv::Mat &f) {
    for(int i = 0, dst_x = xx; i < width; ++i, ++dst_x) {
        for(int z = 0, dst_y = yy; z < height; ++z, ++dst_y) {
            cv::Vec3b &pixel = f.at<cv::Vec3b>(dst_y, dst_x);
            cv::Vec3b src = image.at<cv::Vec3b>(z, i);
            pixel = src;
        }
    }
}

void Square_Swap(ac::Square *squares, int num_w, int num_h, cv::Mat &frame, bool random = false) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame height
    int square_w=(w/num_w), square_h=(h/num_h);
    int pos = 0;
    ac::Point *points = new ac::Point[num_w*num_h];
    std::vector<ac::Square *> square_vec;
    for(int rx = 0; rx < num_w; ++rx) {
        for(int ry = 0; ry < num_h; ++ry) {
            int cx = rx*square_w;
            int cy = ry*square_h;
            points[pos].x = cx;
            points[pos].y = cy;
            squares[pos].setPos(pos);
            squares[pos].setSize(cx, cy, square_w, square_h);
            squares[pos].copyImage(frame);
            square_vec.push_back(&squares[pos]);
            ++pos;
        }
    }
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    // shuffle instead of randomize
    if(random == false) std::shuffle(square_vec.begin(), square_vec.end(),rng);
    for(int i = 0; i < pos; ++i) {
        if(random == false)
            // use shuffled
            square_vec[i]->copyImageToTarget(points[i].x, points[i].y,frame);
        else
            // use random
            square_vec[rand()%pos]->copyImageToTarget(points[i].x, points[i].y,frame);
    }
    delete [] points;
}

void ac::SquareFill(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    int r = rand()%3;
    switch(r) {
        case 0:
            SquareSwap8x4(copy1);
            break;
        case 1:
            SquareSwap4x2(copy1);
            break;
        case 2:
            SquareSwap16x8(copy1);
            break;
    }
    AlphaBlend(copy1, copy2, frame, 0.5);
    AddInvert(frame);
}

// SquareSwap
void ac::SquareSwap(cv::Mat &frame) {
    static int cnt = 0;
    switch(cnt) {
        case 0:
            SquareSwap4x2(frame);
            break;
        case 1:
            SquareSwap8x4(frame);
            break;
        case 2:
            SquareSwap16x8(frame);
            break;
        case 3:
            SquareSwap64x32(frame);
            break;
    }
    ++cnt;
    if(cnt > 3) cnt = 0;
}

void ac::SquareSwap4x2(cv::Mat &frame) {
    const int num_w = 4, num_h = 2;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwap8x4(cv::Mat &frame) {
    const int num_w = 8, num_h = 4;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwap16x8(cv::Mat &frame) {
    const int num_w = 16, num_h = 8;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwap64x32(cv::Mat &) {
    //const int num_w = 64, num_h = 32;
    //static Square squares[num_w*num_h];
    //Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareBars(cv::Mat &frame) {
    static const int num_w = 16, num_h = 1;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareBars8(cv::Mat &frame) {
    static const int num_w = 8, num_h = 1;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareSwapRand16x8(cv::Mat &frame) {
    static const int num_w = 16, num_h = 8;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame, true);
}

void ac::SquareVertical8(cv::Mat &frame) {
    static const int num_w = 1, num_h = 8;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}

void ac::SquareVertical16(cv::Mat &frame) {
    static const int num_w = 1, num_h = 16;
    static Square squares[num_w*num_h];
    Square_Swap(squares, num_w, num_h, frame);
}


void ShiftSquares(std::vector<ac::Square *> &s, int pos, bool direction=true) {
    if(direction == true) {
        for(unsigned int i = 0; i < s.size(); ++i) {
            int p = s[i]->getPos();
            if(p+1 > (int)s.size()-1) {
                s[i]->setPos(0);
            } else {
                ++p;
                s[i]->setPos(p);
            }
        }
    } else {
        for(int i = 0; i < pos; ++i) {
            int p = s[i]->getPos();
            --p;
            s[i]->setPos(p);
            if(p < 0) {
                s[i]->setPos(pos-1);
            }
        }
    }
}

void SquareVertical(const int num_w, const int num_h, ac::Square *squares, cv::Mat &frame, bool direction=true) {
    int w = frame.cols;// frame width
    int h = frame.rows;// frame height
    
    if(w <= 25 || h <= 25)
        return;
    
    int square_w=(w/num_w), square_h=(h/num_h);
    int pos = 0;
    ac::Point *points = new ac::Point[num_w*num_h];
    std::vector<ac::Square *> square_vec;
    for(int rx = 0; rx < (int)num_w; ++rx) {
        for(int ry = 0; ry < (int)num_h; ++ry) {
            int cx = rx*square_w;
            int cy = ry*square_h;
            points[pos].x = cx;
            points[pos].y = cy;
            squares[pos].setSize(cx, cy, square_w, square_h);
            squares[pos].copyImage(frame);
            square_vec.push_back(&squares[pos]);
            ++pos;
        }
    }
    ShiftSquares(square_vec,pos,direction);
    for(int i = 0; i < pos; ++i) {
        const int p = square_vec[i]->getPos();
        square_vec[i]->copyImageToTarget(points[p].x, points[p].y, frame);
    }
    delete [] points;
}

void ac::SquareVertical_Roll(cv::Mat &frame) {
    const int num_w = 1, num_h = 20;
    static Square squares[num_w*num_h];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                squares[cpos].setPos(cpos);
                ++cpos;
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame);
}

void ac::SquareSwapSort_Roll(cv::Mat &frame) {
    const int num_w = 16, num_h = 8;
    static Square squares[num_w*num_h];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                squares[cpos].setPos(cpos);
                ++cpos;
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame);
}

void ac::SquareVertical_RollReverse(cv::Mat &frame) {
    const int num_w = 1, num_h = 20;
    static Square squares[num_w*num_h];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                squares[cpos].setPos(cpos);
                cpos++;
                
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame, false);
}

void ac::SquareSwapSort_RollReverse(cv::Mat &frame) {
    const int num_w = 16, num_h = 8;
    static Square squares[(num_w*num_h)+1];
    static int lazy = 0;
    if(lazy == 0) {
        int cpos = 0;
        for(int cx = 0; cx < num_w; ++cx)
            for(int cy = 0; cy < num_h; ++cy) {
                ++cpos;
                squares[cpos].setPos(cpos);
            }
        lazy = 1;
    }
    SquareVertical(num_w, num_h, squares, frame,false);
}


