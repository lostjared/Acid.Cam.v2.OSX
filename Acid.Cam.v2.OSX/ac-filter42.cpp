
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

void ac::VerticalYSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VerticalYSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int i = 0; i < frame.cols; ++i) {
        int num_clr = rand()%frame.rows;
        int index = 0;
        bool on = true;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            ++index;
            if(on == true && index < num_clr) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
            if(index > num_clr) {
                on = !on;
                index = 0;
                num_clr = rand()%frame.rows-i-1;
            }
        }
    }
    AddInvert(frame);
}

void ac::ScanlineLessSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ScanlineSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        int num_clr = rand()%frame.cols;
        int index = 0;
        bool on = true;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            ++index;
            if(on == true && index < num_clr) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
            if(index > num_clr) {
                on = !on;
                index = 0;
                num_clr = rand()%frame.cols-i-1;
                num_clr += 25;
            }
        }
    }
    AddInvert(frame);
    
}
void ac::VerticalYLessSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VerticalYSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int i = 0; i < frame.cols; ++i) {
        int num_clr = rand()%(frame.rows);
        int index = 0;
        bool on = true;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            ++index;
            if(on == true && index < num_clr) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
            if(index > num_clr) {
                on = !on;
                index = 0;
                num_clr = rand()%frame.rows-i-1;
                num_clr += 25;
            }
        }
    }
    AddInvert(frame);
}

void ac::HorizontalColorOffset(cv::Mat &frame) {
    static double color = 1.0;
    cv::Mat frame_copy = frame.clone();
    int offset = 0;
    static int index = 0;
    static int coffset = 0;
    static int cdir = 1;
    static double speed = 1.0/frame.cols;
    
    for(int z = 0; z < frame.rows; ++z) {
        offset = rand()%frame.cols;
        for(int i = offset; i < frame.cols; ++i) {
            ++index;
            if(index > frame.cols)
                index = 0;
            
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            if(cdir == 1) {
                color += speed;
                if(color >= 1) {
                    color = 1;
                    cdir = 0;
                }
            } else {
                color -= speed;
                if(color <= 0.3) {
                    color = 0.3;
                    cdir = 1;
                }
            }
            pixel[coffset] = static_cast<int>(pixel[coffset] * color);
        }
        for(int i = 0; i < offset; ++i) {
            ++index;
            if(index > frame.cols)
                index = 0;
            
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
            if(cdir == 1) {
                color += speed;
                if(color >= 1) {
                    color = 1;
                    cdir = 0;
                }
            } else {
                color -= speed;
                if(color <= 0.3) {
                    color = 0.3;
                    cdir = 1;
                }
            }
            pixel[coffset] = static_cast<int>(pixel[coffset] * color);
        }
        
        if((z%50) == 0) {
            static int cdir = 1;
            if(cdir == 1) {
                ++coffset;
                if(coffset > 2) {
                    coffset = 2;
                    cdir = 0;
                }
            } else {
                --offset;
                if(offset <= 0) {
                    coffset = 0;
                    cdir = 1;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::HorizontalColorOffsetLargeSizeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "HorizontalColorOffsetLargeSizeSubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = true;
    static int counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            if(on == false && (rand()%100)==0) {
                on = !on;
            } else {
                on = !on;
            }
            offset_y = rand()%(frame.rows);
            counter = 0;
        }
    }
    AddInvert(frame);
}

void ac::ErodeKernelSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ErodeKernelSubFilter")
        return;
    
    cv::Mat copyz = frame.clone();
    CallFilter(subfilter, copyz);
    cv::Mat out;
    cv::Mat temp;
    cv::erode(frame, temp, out);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copy1 = temp.at<cv::Vec3b>(z, i);
            cv::Vec3b copyf = copyz.at<cv::Vec3b>(z, i);
            if(searchColors(copy1) == SEARCH_PIXEL) {
                pixel = copyf;
            }
        }
    }
    AddInvert(frame);
}

void ac::DilateKernelSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DilateKernelSubFilter")
        return;
    
    cv::Mat copyz = frame.clone();
    CallFilter(subfilter, copyz);
    cv::Mat out;
    cv::Mat temp;
    cv::dilate(frame, temp, out);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copy1 = temp.at<cv::Vec3b>(z, i);
            cv::Vec3b copyf = copyz.at<cv::Vec3b>(z, i);
            if(searchColors(copy1) == SEARCH_PIXEL) {
                pixel = copyf;
            }
        }
    }
    AddInvert(frame);
}


void ac::ErodeKernelOffSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ErodeKernelOffSubFilter")
        return;
    
    cv::Mat copyz = frame.clone();
    CallFilter(subfilter, copyz);
    cv::Mat out;
    cv::Mat temp;
    cv::erode(frame, temp, out);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copy1 = temp.at<cv::Vec3b>(z, i);
            cv::Vec3b copyf = copyz.at<cv::Vec3b>(z, i);
            if(searchColors(copy1) != SEARCH_PIXEL) {
                pixel = copyf;
            }
        }
    }
    AddInvert(frame);
}

void ac::DilateKernelOffSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DilateKernelOfffSubFilter")
        return;
    
    cv::Mat copyz = frame.clone();
    CallFilter(subfilter, copyz);
    cv::Mat out;
    cv::Mat temp;
    cv::dilate(frame, temp, out);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copy1 = temp.at<cv::Vec3b>(z, i);
            cv::Vec3b copyf = copyz.at<cv::Vec3b>(z, i);
            if(searchColors(copy1) != SEARCH_PIXEL) {
                pixel = copyf;
            }
        }
    }
    AddInvert(frame);
    
}

void ac::ColorKeySetOnSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ErodeKernelSubFilter")
        return;
    cv::Mat copyz = frame.clone();
    CallFilter(subfilter, copyz);
    cv::Mat cpx = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copy1 = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copyf = copyz.at<cv::Vec3b>(z, i);
            if(searchColors(copy1) == SEARCH_PIXEL) {
                pixel = copyf;
            }
        }
    }
    AddInvert(frame);
}

void ac::ColorKeySetOffSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ErodeKernelSubFilter")
        return;
    cv::Mat copyz = frame.clone();
    CallFilter(subfilter, copyz);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copy1 = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b copyf = copyz.at<cv::Vec3b>(z, i);
            if(searchColors(copy1) != SEARCH_PIXEL) {
                pixel = copyf;
            }
        }
    }
    AddInvert(frame);
}

void ac::Warp(cv::Mat &src) {
    static cv::Point2f srcQuad[] = {
        cv::Point2f(0, 0),
        cv::Point2f(src.cols-1, 0),
        cv::Point2f(src.cols-1, src.rows-1),
        cv::Point2f(0, src.rows-1)
    };
    static float pos_x[4] = {0.1,0.9, 0.8, 0.2};
    static float pos_y[4] = {0.1f, 0.25f, 0.9f, 0.7f};
    static bool dir1[4] = {1, 0, 1, 0};
    static bool dir2[4] = {1, 0, 1, 0};
    cv::Point2f dstQuad[] = {
        cv::Point2f(src.cols*pos_x[0], src.rows*pos_y[0]),
        cv::Point2f(src.cols*pos_x[1], src.rows*pos_y[1]),
        cv::Point2f(src.cols*pos_x[2], src.rows*pos_y[2]),
        cv::Point2f(src.cols*pos_x[3], src.rows*pos_y[3])
    };
    cv::Mat warp_mat = cv::getPerspectiveTransform(srcQuad, dstQuad);
    cv::Mat dst;
    cv::warpPerspective(src, dst, warp_mat, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
    for(int i = 0; i < 4; ++i) {
        cv::circle(dst, dstQuad[i], 5, cv::Scalar(0,0,0), -1, cv::LINE_8);
    }
    for(int i = 0; i < 4; ++i) {
        if(dir1[i] == true) pos_x[i] += (1+(rand()%10)) * 0.001f;
        else pos_x[i] -= (1+(rand()%10)) * 0.001f;
        if(dir2[i] == true) pos_y[i] += (1+(rand()%10)) * 0.001f;
        else pos_y[i] -= (1+(rand()%10)) * 0.001f;
        if(pos_x[i] >= 1) {
            dir1[i] = false;
        } else if(pos_x[i] <= 0.1) {
            dir1[i] = true;
        }
        if(pos_y[i] >= 1) {
            dir2[i] = false;
        } else if(pos_y[i] <= 0.1) {
            dir2[i] = true;
        }
    }
    src = dst.clone();
    AddInvert(src);
}

void ac::WarpDir(cv::Mat &src) {
    static cv::Point2f srcQuad[] = {
        cv::Point2f(0, 0),
        cv::Point2f(src.cols-1, 0),
        cv::Point2f(src.cols-1, src.rows-1),
        cv::Point2f(0, src.rows-1)
    };
    static float pos_x[4] = {1.0,1.0, 1.0, 1.0};
    static float pos_y[4] = {1.0, 1.0, 1.0, 1.0};
    static bool dir1[4] = {1, 0, 1, 0};
    static bool dir2[4] = {1, 0, 1, 0};
    cv::Point2f dstQuad[] = {
        cv::Point2f(src.cols*pos_x[0], src.rows*pos_y[0]),
        cv::Point2f(src.cols*pos_x[1], src.rows*pos_y[1]),
        cv::Point2f(src.cols*pos_x[2], src.rows*pos_y[2]),
        cv::Point2f(src.cols*pos_x[3], src.rows*pos_y[3])
    };
    cv::Mat warp_mat = cv::getPerspectiveTransform(srcQuad, dstQuad);
    cv::Mat dst;
    cv::warpPerspective(src, dst, warp_mat, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
    for(int i = 0; i < 4; ++i) {
        cv::circle(dst, dstQuad[i], 5, cv::Scalar(0,0,0), -1, cv::LINE_8);
    }
    for(int i = 0; i < 4; ++i) {
        if(dir1[i] == true) pos_x[i] += (1+(rand()%10)) * 0.001f;
        else pos_x[i] -= (1+(rand()%10)) * 0.001f;
        if(dir2[i] == true) pos_y[i] += (1+(rand()%10)) * 0.001f;
        else pos_y[i] -= (1+(rand()%10)) * 0.001f;
        if(pos_x[i] >= 1) {
            dir1[i] = false;
        } else if(pos_x[i] <= 0.1) {
            dir1[i] = true;
        }
        if(pos_y[i] >= 1) {
            dir2[i] = false;
        } else if(pos_y[i] <= 0.1) {
            dir2[i] = true;
        }
    }
    src = dst.clone();
    AddInvert(src);
}

void ac::WarpTopLeft(cv::Mat &src) {
    static cv::Point2f srcQuad[] = {
        cv::Point2f(0, 0),
        cv::Point2f(src.cols-1, 0),
        cv::Point2f(src.cols-1, src.rows-1),
        cv::Point2f(0, src.rows-1)
    };
    static float speed = 0.001f;
    static float pos_x[4] = {0.1f,0.2f, 0.4f, 0.1f};
    static float pos_y[4] = {0.1f, 0.55f, 0.9f, 0.7f};
    static bool dir1[4] = {1, 0, 1, 0};
    static bool dir2[4] = {1, 0, 1, 0};
    cv::Point2f dstQuad[] = {
        cv::Point2f(src.cols*pos_x[0], src.rows*pos_y[0]),
        cv::Point2f(src.cols*pos_x[1], src.rows*pos_y[1]),
        cv::Point2f(src.cols*pos_x[2], src.rows*pos_y[2]),
        cv::Point2f(src.cols*pos_x[3], src.rows*pos_y[3])
    };
    cv::Mat warp_mat = cv::getPerspectiveTransform(srcQuad, dstQuad);
    cv::Mat dst;
    cv::warpPerspective(src, dst, warp_mat, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
    for(int i = 0; i < 4; ++i) {
        cv::circle(dst, dstQuad[i], 5, cv::Scalar(0,0,0), -1, cv::LINE_8);
    }
    for(int i = 0; i < 4; ++i) {
        if(pos_x[i] >= 1) {
            dir1[i] = false;
        } else if (pos_x[i] <= 0.1f) {
            dir1[i] = true;
        }
        if(dir1[i] == true)
            pos_x[i] += (1+(rand()%5) * 0.001);
        else
            pos_x[i] -=  (1+(rand()%5) * 0.001);
        
        if(pos_y[i] >= 1) {
            dir2[i] = false;
        } else if (pos_y[i] <= 0.1f) {
            dir2[i] = true;
        }
        if(dir2[i] == true)
            pos_y[i] += (1+(rand()%5) * speed);
        else
            pos_y[i] -=  (1+(rand()%5) * speed);

    }
    speed += 0.01;
    if(speed > 0.01)
        speed = 0.001f;
    
    src = dst.clone();
    AddInvert(src);
}


void ac::User(cv::Mat &frame) {
    
}

void ac::WarpRandom(cv::Mat &src) {
    static cv::Point2f srcQuad[] = {
        cv::Point2f(0, 0),
        cv::Point2f(src.cols-1, 0),
        cv::Point2f(src.cols-1, src.rows-1),
        cv::Point2f(0, src.rows-1)
    };
    static float pos_x[4] = {0.1,1.0, 1.0,0.1};
    static float pos_y[4] = {0, 0, 1.0, 1.0};
    static bool dir1[4] = {true, false, false, true};
    static bool dir2[4] = {true, true, false, false};
    cv::Point2f dstQuad[] = {
        cv::Point2f(src.cols*pos_x[0], src.rows*pos_y[0]),
        cv::Point2f(src.cols*pos_x[1], src.rows*pos_y[1]),
        cv::Point2f(src.cols*pos_x[2], src.rows*pos_y[2]),
        cv::Point2f(src.cols*pos_x[3], src.rows*pos_y[3])
    };
    cv::Mat warp_mat = cv::getPerspectiveTransform(srcQuad, dstQuad);
    cv::Mat dst;
    cv::warpPerspective(src, dst, warp_mat, src.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
    for(int i = 0; i < 4; ++i) {
        cv::circle(dst, dstQuad[i], 5, cv::Scalar(0,0,0), -1, cv::LINE_8);
    }
    src = dst.clone();
    for(int i = 0; i < 4; ++i) {
        if(pos_x[i] <= 0.1) {
            dir1[i] = true;
        } else if(pos_x[i] >= 1) {
            dir1[i] = false;
        }
        if(pos_y[i] <= 0.1) {
            dir2[i] = true;
        } else if(pos_y[i] >= 1) {
            dir2[i] = false;
        }
        if(dir1[i] == true) {
            pos_x[i] += (1+rand()%10) * (0.001);
        } else {
            pos_x[i] -= (1+rand()%10) * (0.001);
        }
        if(dir2[i] == true) {
            pos_y[i] += (1+rand()%10) * (0.001);
        } else {
            pos_y[i] -= (1+rand()%10) * (0.001);
        }
    }
    AddInvert(src);
}
