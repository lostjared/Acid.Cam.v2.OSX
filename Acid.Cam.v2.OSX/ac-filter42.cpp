
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            if(on == false && (rand()%250)==0) {
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b copy1 = pixelAt(frame,z, i);
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b copy1 = pixelAt(frame,z, i);
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
        cv::Point2f(float(src.cols-1), 0),
        cv::Point2f(float(src.cols-1), float(src.rows-1)),
        cv::Point2f(0, float(src.rows-1))
    };
    static float pos_x[4] = {0.1f,0.9f, 0.8f, 0.2f};
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
        cv::Point2f(float(src.cols-1), 0),
        cv::Point2f(float(src.cols-1), float(src.rows-1)),
        cv::Point2f(0, float(src.rows-1))
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
        cv::Point2f(float(src.cols-1), 0),
        cv::Point2f(float(src.cols-1), float(src.rows-1)),
        cv::Point2f(0, float(src.rows-1))
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
            pos_x[i] += float((1+(rand()%5) * 0.001));
        else
            pos_x[i] -=  float((1+(rand()%5) * 0.001));
        
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
    speed += 0.01f;
    if(speed > 0.01f)
        speed = 0.001f;
    
    src = dst.clone();
    AddInvert(src);
}


void ac::User(cv::Mat &) {
    
}

void ac::WarpRandom(cv::Mat &src) {
    static cv::Point2f srcQuad[] = {
        cv::Point2f(0, 0),
        cv::Point2f(float(src.cols-1), 0),
        cv::Point2f(float(src.cols-1), float(src.rows-1)),
        cv::Point2f(0, float(src.rows-1))
    };
    static float pos_x[4] = {0.1f,1.0f, 1.0f,0.1f};
    static float pos_y[4] = {0, 0, 1.0f, 1.0f};
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
            pos_x[i] += float((1+rand()%10) * (0.001));
        } else {
            pos_x[i] -= float((1+rand()%10) * (0.001));
        }
        if(dir2[i] == true) {
            pos_y[i] += float((1+rand()%10) * (0.001));
        } else {
            pos_y[i] -= float((1+rand()%10) * (0.001));
        }
    }
    AddInvert(src);
}

void ac::WarpStretch(cv::Mat &src) {
    static cv::Point2f srcQuad[] = {
        cv::Point2f(0, 0),
        cv::Point2f(float(src.cols-1), 0),
        cv::Point2f(float(src.cols-1), float(src.rows-1)),
        cv::Point2f(0, float(src.rows-1))
    };
    static float pos_x[4] = {0 ,1.0f, 1.0f,0};
    static float pos_y[4] = {0, 0, 1.0f, 1.0f};
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
            pos_x[i] += float(((0.05 * (1+rand()%10))));
        } else {
            pos_x[i] -= float(((0.05 * (1+rand()%10))));
        }
        
        if(dir2[i] == true) {
            pos_y[i] += float(((0.05 * (1+rand()%10))));
        } else {
            pos_y[i] -= float(((0.05 * (1+rand()%10))));
        }
    }
    AddInvert(src);
}

void ac::RandomLineGlitchSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "RandomLineGlitchSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    int max = 100;
    bool on = false;
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
            if(on == false && (rand()%max)==0)
                on = true;
            
            static int index = 0;
            
            if(on == true) {
                pixel = pix;
                ++index;
                if(index > 50) {
                    index = 0;
                    on = false;
                    i += 150;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::VerticalColorOffsetLargeSizeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VerticalColorOffsetLargeSizeSubFilter")
        return;
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    static int offset_y = (rand()%(frame.rows));
    bool on = true;
    static int counter = 0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(on == true) {
                cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
        ++counter;
        if((counter%(offset_y+1)==0)) {
            if(on == false && (rand()%250)==0) {
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

void ac::PreviousFrameXor(cv::Mat &frame) {
    static cv::Mat prev = frame.clone();
    if(prev.size() != frame.size())
        prev = frame.clone();
    cv::Mat copy1 = frame.clone();
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix = prev.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^pix[j];
            }
        }
    }
    prev = copy1.clone();
    AddInvert(frame);
}

void ac::LightScanlineSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "LightScanlineSubFilter")
        return;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    static int max_off_val = 100;
    for(int z = 0; z < frame.rows; ++z) {
        if(( rand()%10)==0) {
            bool on = true;
            int off = rand()%25;
            int max_off = rand()%max_off_val;
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                if(on == true) {
                    pixel = pix;
                }
                ++off;
                if(off > max_off) {
                    off = 0;
                    max_off = rand()%max_off_val;
                    on = !on;
                }
            }
        }
    }
    static bool dir1 = true;
    if(dir1 == true) {
        ++max_off_val;
        if(max_off_val > 350) {
            max_off_val = 350;
            dir1 = false;
        }
    } else {
        --max_off_val;
        if(max_off_val < 100) {
            max_off_val = 100;
            dir1 = true;
        }
    }
    AddInvert(frame);
}

void ac::ScaleUpDown(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    
    static int counter = 0;
    ++counter;
    
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
        counter = 0;
    }
    static bool rev = false;
    if(counter > (static_cast<int>(ac::fps))) {
        rev = !rev;
        counter = 0;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                if(rev == true)
                    pix_container.pix_values[i][z].col[j] += 15;
                else
                    pix_container.pix_values[i][z].col[j] -= 15;
                int total = pixel[j] + pix_container.pix_values[i][z].col[j];
                if(total >= 255) {
                    pix_container.pix_values[i][z].col[j] = 0;
                    pixel[j] = 255;
                } else
                    pixel[j] += total;
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoTransitionInOut_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VideoTransitionInOut_SubFilter")
        return;
    cv::Mat nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        CallFilter(subfilter, reframe);
        static double alpha = 0.1;
        static int dir = 1;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b repix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((1-alpha) * pixel[j]) + (alpha * repix[j]));
                }
            }
        }
        AlphaMovementMaxMin(alpha, dir, 0.001, 1.0, 0.1);
    }
    AddInvert(frame);
}

void ac::VideoDisplayPercent_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VideoDisplayPercent_SubFilter")
        return;
    cv::Mat nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        CallFilter(subfilter, reframe);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b repix = pixelAt(reframe,z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(((1-0.6) * pixel[j]) + (0.4 * repix[j]));
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MovementToBlackSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MovementToBlackSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pix1 = copy1.at<cv::Vec3b>(z, i);
            bool set_value = false;
            for(int index = 0; index < collection.size(); ++index) {
                cv::Vec3b pix = collection.frames[index].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    if(abs(pixel[j]-pix[j]) > getPixelCollection()) {
                        pixel[j] = pix1[j];
                        set_value = true;
                    }
                }
                if(set_value == true)
                    break;
            }
            if(set_value == false)
                pixel = cv::Vec3b(0, 0, 0);
        }
    }
    AddInvert(frame);
}

void ac::Black(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel = cv::Vec3b(0, 0, 0);
        }
    }
    AddInvert(frame);
}

void ac::VideoFrameRGB(cv::Mat &frame) {
    cv::Mat nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        static int offset1 = 0;
        static int offset2 = 2;
        static int dir1 = 1, dir2 = 0;
        ac_resize(nframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b repix = pixelAt(reframe,z, i);
                pixel[offset1] = static_cast<unsigned char>((0.5 * pixel[offset1]) + (0.5 * repix[offset2]));
            }
        }
        if(dir1 == 1) {
            offset1++;
            if(offset1 > 2) {
                offset1 = 2;
                dir1 = 0;
            }
        } else {
            offset1--;
            if(offset1 < 0) {
                offset1 = 0;
                dir1 = 1;
            }
        }
        
        
        if(dir2 == 1) {
            offset2++;
            if(offset2 > 2) {
                offset2 = 2;
                dir2 = 0;
            }
        } else {
            offset2--;
            if(offset2 < 0) {
                offset2 = 0;
                dir2 = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoAlphaBlendFade(cv::Mat &frame) {
    cv::Mat nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b repix = pixelAt(reframe,z, i);
                static double alpha = 1.0;
                static int dir = 1;
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((1-alpha) * pixel[j]) + (alpha * repix[j]);
                }
                AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoAlphaAddFade(cv::Mat &frame) {
    cv::Mat nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b repix = pixelAt(reframe,z, i);
                static double alpha = 1.0;
                static int dir = 1;
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]) + (alpha * repix[j]);
                }
                AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoAlphaAddFadeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "VideoAlphaAddFadeSubFilter")
        return;
    cv::Mat nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        CallFilter(subfilter, reframe);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b repix = pixelAt(reframe,z, i);
                static double alpha = 1.0;
                static int dir = 1;
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(pixel[j]) + (alpha * repix[j]);
                }
                AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
            }
        }
    }
    AddInvert(frame);
}

void ac::DelayOnOffSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "DelayOnOffSubFilter")
        return;
    static int delay = 0, seconds = 0;
    static int wait = 1+(rand()%1+getVariableWait());
    int fps = static_cast<int>(ac::fps);
    static int on = 1;
    if(on == 1)
        CallFilter(subfilter, frame);
    
    
    ++delay;
    if(delay > fps) {
        delay = 0;
        ++seconds;
        if(seconds > wait) {
            seconds = 0;
            on = (on == 0) ? 1 : 0;
            if(on == 1)
                wait = 1+(rand()%(1+getVariableWait()));
            else
                wait = 1+(rand()%(1+getVariableWait()));
        }
    }
    AddInvert(frame);
}

