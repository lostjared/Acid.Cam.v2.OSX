/*
 *
 *  Acid Cam functions for OpenCV
 * written by Jared Bruni
 * http://lostsidedead.com
 * (C) 2016 under GPL
 */

#include "ac.h"
#include "fractal.h"

namespace ac {
    bool isNegative = false, noRecord = false, pass2_enabled = false, blendW = false, slide_Show = false, slide_Rand = false, strobe_It = false, switch_Back = false, blur_First = false;
    bool images_Enabled = false, fps_force = false,iRev = false;
    bool blur_Second = false;
    cv::Mat orig_frame;
    cv::Mat blendW_frame;
    cv::Mat image_files[4];
    double alpha = 1.0f, tr = 0.01f;
    double fps = 29.97;
    int draw_offset = 0;
    bool snapShot = false;
    int color_order = 0;
    int snapshot_Type = 0;
    
    std::string draw_strings[] = { "Self AlphaBlend", "Self Scale", "StrobeEffect", "Blend #3", "Negative Paradox", "ThoughtMode", "RandTriBlend", "Blank", "Tri", "Distort", "CDraw", "Type", "NewOne", "Blend Fractal","Blend Fractal Mood", "CosSinMultiply", "Color Accumlate1", "Color Accumulate2", "Color Accumulate3", "filter8","filter3","Rainbow Blend","Rand Blend","New Blend", "Alpha Flame Filters", "Pixel Scale", "PixelSort", "GlitchSort","Random Filter", "Blend with Image", "Blend with Image #2", "Blend with Image #3", "GaussianBlur", "Median Blur", "Blur Distortion", "Diamond Pattern", "MirrorBlend","Pulse","Sideways Mirror","Mirror No Blend","Plugin", "Custom","Blend With Image #1",  "TriBlend with Image", "Image Strobe", "Image distraction" };

    DrawFunction draw_func[] = { SelfAlphaBlend, SelfScale, StrobeEffect, Blend3, NegParadox, ThoughtMode, RandTriBlend, Blank, Tri, Distort, CDraw,Type,NewOne,blendFractal,blendFractalMood,cossinMultiply, colorAccumulate1, colorAccumulate2, colorAccumulate3,filter8,filter3,rainbowBlend,randBlend,newBlend,
        alphaFlame, pixelScale,pixelSort, glitchSort,randomFilter,imageBlend,imageBlendTwo,imageBlendThree,GaussianBlur, MedianBlur, BlurDistortion,DiamondPattern,MirrorBlend,Pulse,SidewaysMirror,MirrorNoBlend,plugin,custom,blendWithImage, triBlendWithImage,imageStrobe, imageDistraction,0};
    int draw_max = 45;
    double translation_variable = 0.001f, pass2_alpha = 0.75f;
    inline void swapColors(cv::Mat &frame, int x, int y);
}

inline void ac::swapColors(cv::Mat &frame, int x, int y) {
    if(color_order == 0) return;
    cv::Vec3b &cur = frame.at<cv::Vec3b>(x,y);
    cv::Vec3b temp;
    temp = cur;
    switch(color_order) {
        case 1:
            cur[0] = temp[2];
            cur[1] = temp[1];
            cur[2] = temp[0];
            break;
        case 2:
            cur[0] = temp[1];
            cur[1] = temp[2];
            cur[2] = temp[0];
            break;
        case 3:
            cur[0] = temp[2];
            cur[1] = temp[0];
            cur[2] = temp[1];
            break;
        case 4:
            cur[0] = temp[1];
            cur[1] = temp[0];
            cur[2] = temp[2];
            break;
    }
}

inline void ac::invert(cv::Mat &frame, int x, int y) {
    cv::Vec3b &cur = frame.at<cv::Vec3b>(x,y);
    if(isNegative == true) {
        cur[0] = ~cur[0];
        cur[1] = ~cur[1];
        cur[2] = ~cur[2];
    }
    if(iRev == true) {
        cv::Vec3b temp;
        temp = cur;
        cur[2] = temp[0];
        cur[1] = temp[1];
        cur[0] = temp[2];
    }
}

inline void ac::randAlpha(double &alpha) {
    static int max = 1, total = 25;
    static double d = 0.1f;
    static bool dir = true;
    alpha = (d) * (1 + (rand() % max));
    if (dir == true)
        ++max;
    else
        --max;
    if (max > total) {
        dir = false;
        total++;
        d += 0.1f;
    } else if (max <= 1) {
        dir = true;
    }
}

void ac::resetAll() {
    slide_Show = false;
    slide_Rand = false;
    alpha = 1.0f;
    tr = 0.1f;
    blur_First = false;
    iRev = false;
    isNegative = false;
    blur_Second = false;
}


void ac::enablePass2(bool pass2_enabled, bool pass2_alpha) {
    ac::pass2_enabled = pass2_enabled;
    ac::pass2_alpha = pass2_alpha;
}

void ac::SelfAlphaBlend(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &colorval = frame.at<cv::Vec3b>(z, i);
            colorval[0] += colorval[0]*alpha;
            colorval[1] += colorval[1]*alpha;
            colorval[2] += colorval[2]*alpha;
            swapColors(frame, z, i);
            if(isNegative == true) {
                invert(frame, z, i);
            }
        }
    }
    static int direction = 1;
    if(direction == 1) {
        alpha += 0.1f;
        if(alpha > 10) { alpha = 10; direction = 2; }
    } else {
        alpha -= 0.05f;
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }
    }
}

void ac::SelfScale(cv::Mat &frame) {
    static double pos = 1.0;
    int w = frame.cols;
    int h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] = pixel[0] * pos;
            pixel[1] = pixel[1] * pos;
            pixel[2] = pixel[2] * pos;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double pos_max = 7.0f;
    if(direction == 1) {
        pos += 0.05;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 0.5f;
        }
    } else if(direction == 0) {
        pos -= 0.05;
        if(pos <= 1.0) {
            if(pos_max > 15) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::StrobeEffect(cv::Mat &frame) {
    static unsigned int passIndex = 0;
    static double alpha = 1.0f;
    for (int z = 0; z < frame.cols - 2; ++z) {
        for (int i = 0; i < frame.rows - 2; ++i) {
            cv::Vec3b &colors = frame.at<cv::Vec3b>(i, z);
            switch (passIndex) {
                case 0:
                    colors[0] = colors[0] * (-alpha);
                    colors[1] = colors[1] * alpha;
                    colors[2] = colors[2] * alpha;
                    break;
                case 1:
                    colors[0] += colors[0] * alpha;
                    colors[1] += colors[1] * (-alpha);
                    colors[2] += colors[2] * alpha;
                    break;
                case 2:
                    colors[0] = colors[0] * alpha;
                    colors[1] = colors[1] * alpha;
                    colors[2] = colors[2] * (-alpha);
                    break;
                case 3: {
                    cv::Vec3b &color1 = frame.at<cv::Vec3b>(i + 1, z);
                    cv::Vec3b &color2 = frame.at<cv::Vec3b>(i + 2, z);
                    colors[0] += colors[0] * alpha;
                    colors[1] += color1[1] * alpha;
                    colors[2] += color2[2] * alpha;
                    break;
                }
            }
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    ++passIndex;
    if(passIndex > 3) passIndex = 0;
    static double max = 4.0f;
    if(alpha < 0)
        tr = translation_variable;
    else if(alpha > max) {
        tr = -translation_variable;
        max += 3.0f;
        if(max > 23) max = 4.0f;
    }
    alpha += tr;
}

void ac::Blend3(cv::Mat &frame) {
    static int i=0,z=0;
    static double rValue[3] = { 0, 0, 0 };
    for (z = 0; z < frame.cols; ++z) {
        for (i = 0; i < frame.rows; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            for (int j = 0; j < 3; ++j)
                color_value[j] += color_value[j] * rValue[j];
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    for (int q = 0; q < 3; ++q)
        rValue[q] += ((rand() % 10) > 5) ? -0.1f : 0.1f;
}

void ac::NegParadox(cv::Mat &frame) {
    static double alpha = 1.0f;
    for (int z = 0; z < frame.cols - 3; ++z) {
        for (int i = 0; i < frame.rows - 3; ++i) {
            cv::Vec3b colors[4];
            colors[0] = frame.at<cv::Vec3b>(i, z);
            colors[1] = frame.at<cv::Vec3b>(i + 1, z);
            colors[2] = frame.at<cv::Vec3b>(i + 2, z);
            colors[3] = frame.at<cv::Vec3b>(i + 3, z);
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            color_value[0] += (colors[0][0] * alpha) + (colors[1][0] * alpha);
            color_value[1] += (colors[1][1] * alpha) + (colors[3][1] * alpha);
            color_value[2] += (colors[1][2] * alpha) + (colors[2][2] * alpha);
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    static double trans_var = 0.1f;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > 15)
        trans_var = -translation_variable;
    alpha += trans_var;
}

void ac::ThoughtMode(cv::Mat &frame) {
    static double alpha = 1.0f, trans_var = 0.1f;;
    static int mode = 0;
    static int sw = 1, tr = 1;
    for(int z = 2; z < frame.cols-2; ++z) {
        for(int i = 2; i < frame.rows-4; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            if(sw == 1) color_value[0]+= color_value[mode]*alpha;
            if(tr == 0) color_value[mode] -= color_value[rand()%2]*alpha;
            color_value[mode] += color_value[mode]*alpha;
            mode++;
            if(mode >= 3) mode = 0;
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
   	}
    sw = !sw;
    tr = !tr;
    static double max = 4.0f;
    if(alpha < 0)
        trans_var = translation_variable;
    else if(alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if(max > 23) max = 4.0f;
    }
    alpha += trans_var;
}

void ac::Pass2Blend(cv::Mat &frame) {
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &color1 = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b color2 = orig_frame.at<cv::Vec3b>(z, i);
            for(int q = 0; q < 3; ++q)
                color1[q] = color1[q]+color2[q]*(unsigned char)ac::pass2_alpha;
        }
    }
}

void ac::RandTriBlend(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0;
    int counter = 0;
    cv::Vec3b colors[6];
    for (z = 2; z < frame.cols - 2; ++z) {
        for (i = 2; i < frame.rows - 2; ++i) {
            colors[0] = frame.at<cv::Vec3b>(i, z);
            colors[1] = frame.at<cv::Vec3b>(i + 1, z);
            colors[2] = frame.at<cv::Vec3b>(i + 2, z);
            // chaos
            counter = rand() % 3;
            if (counter == 0) {
                colors[3][0] = (colors[0][0] + colors[1][0] + colors[2][0])
                * alpha;
                colors[3][1] = (colors[0][1] + colors[1][1]) * alpha;
                colors[3][2] = (colors[0][2]) * alpha;
                counter++;
            } else if (counter == 1) {
                colors[3][0] = (colors[0][0]) * alpha;
                colors[3][1] = (colors[0][1] + colors[1][1]) * alpha;
                colors[3][2] = (colors[0][2] + colors[1][2] + colors[2][2]) * alpha;
                counter++;
            } else {
                colors[3][0] = (colors[0][0]) * alpha;
                colors[3][2] = (colors[0][1] + colors[1][1]) * alpha;
                colors[3][1] = (colors[0][2] + colors[1][2] + colors[2][2]) * alpha;
            }
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            color_value = colors[3];
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    static double max = 4.0f, trans_var = translation_variable;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

void ac::Blank(cv::Mat &frame) {
    static double alpha = 1.0f;
    static unsigned char val[3] = { 0 };
    static bool color_switch = false;
    for(int z = 0; z < frame.cols; ++z) {
        for(int i = 0; i < frame.rows; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            for(int j = 0; j < 3; ++j) {
                val[j] = (alpha*color_value[j]) / (2-j+1);
                color_value[j] += val[2-j] / (j+1);
                if(color_switch == true) color_value[j] = -color_value[j];
            }
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    color_switch = !color_switch;
    static double max = 4.0f, trans_var = translation_variable;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

void ac::Tri(cv::Mat &frame) {
    static double alpha = 1.0f;
    for(int z = 0; z < frame.cols-3; ++z) {
        for(int i = 0; i < frame.rows-3; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            cv::Vec3b colors[2];
            colors[0] = frame.at<cv::Vec3b>(i+1, z);
            colors[1] = frame.at<cv::Vec3b>(i+2, z);
            color_value[0] += color_value[0]*alpha;
            color_value[1] += color_value[1]+colors[0][1]+colors[1][1]*alpha;
            color_value[2] += color_value[2]+colors[0][2]+colors[1][2]*alpha;
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

void ac::Distort(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            color_value[0] = (i*alpha)+color_value[0];
            color_value[2] = (z*alpha)+color_value[2];
            color_value[1] = (alpha*color_value[1]);
            if(strobe_It == true) color_value[1] = ((i+z)*alpha)+color_value[1];
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = 0.1f;
    else if (alpha > max) {
        trans_var = -0.1f;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

void ac::CDraw(cv::Mat &frame) {
    static int i=0,z=0;
    static double rad = 80.0f;
    static double deg = 1.0f;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            int cX = int(rad * cosf(deg));
            int cY = int(rad * sinf(deg));
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            color_value[0] = color_value[0]*(cX * alpha);
            color_value[1] = color_value[1]*(cY * alpha);
            color_value[2] = color_value[2]*alpha;
            deg += 0.1f;
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    alpha += 0.1f;
    rad += 0.1f;
    if(rad > 90) rad = 0;
    if(alpha > 20) alpha = 0;
}
// Light Strobe
// first cycle through the image
// add a variable to each individual pixel (the input sould be different each frame)
// reason for this is adding to the captured image each frame causes a animation a distortion
// each frame the largest value is calculated by adding the rgb values together for one element each iteration.
// test this first
void ac::Type(cv::Mat &frame) {
    signed int i = 0, z = 0;
    static double add_r = 1.0;
    static int off = 0;
    for(z = 0; z < frame.rows; ++z) {
        for(i = 0; i < frame.cols; ++i) {
            cv::Vec3b &current = frame.at<cv::Vec3b>(z, i);
            current[0] += add_r+current[0];
            current[1] += add_r+current[1];
            current[2] += add_r+current[2];
            current[off] = current[0]+current[1]+current[2];
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    ++off;
    if(off > 2) off = 0;
    add_r += rand()%255;
    if(add_r > 255) add_r = 0;
}

void ac::NewOne(cv::Mat &frame) {
    for(int z = 0; z < frame.cols; ++z) {
        for(int i = 1; i < frame.rows-1; ++i) {
            cv::Vec3b &colv = frame.at<cv::Vec3b>(i, z);
            cv::Vec3b &cola = frame.at<cv::Vec3b>((frame.rows-1)-i, (frame.cols-1)-z);//frame.at<cv::Vec3b>((frame.cols-1)-z, (frame.rows-1)-i);
            unsigned int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            unsigned int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            unsigned int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            for(unsigned int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            colv[0] += alpha*R;
            colv[1] += alpha*G;
            colv[2] += alpha*B;
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static double max = 8.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = 0.1f;
    else if (alpha > max) {
        trans_var = -0.1f;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

void ac::blendFractal(cv::Mat &frame) {
    frac::FractalLogic();
    frac::DrawFractal(frame, ac::isNegative);
}

void ac::blendFractalMood(cv::Mat &frame) {
    // random
    unsigned char color = 0;
    color = rand()%255;
    static bool shift = true;
    static bool shift_value = true;
    for(int z = 0; z < frame.cols; ++z) {
        for(int i = 0; i < frame.rows; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            color_value[0] += (shift == shift_value) ? color : -color;
            color_value[1] += (shift == shift_value) ? -color : color;
            color_value[2] += (shift == shift_value) ? color : -color;
            shift_value = !shift_value;
        }
    }
    shift = ! shift;
    frac::FractalLogic();
    frac::DrawFractal(frame, ac::isNegative);
}

// blend with Image functions
inline int ac::GetFX(cv::Mat &frame, int x, int nw) {
    double xp = (double)x * (double)frame.rows / (double)nw;
    return (int)xp;
}

inline int ac::GetFY(cv::Mat &frame, int y, int nh) {
    double yp = (double)y * (double)frame.cols / (double)nh;
    return (int)yp;
}

void ac::blendWithImage(cv::Mat &frame) {
    if(!blendW_frame.data)
        return;
    static double alpha = 1.0f;
    static double beta = 1.0f;
    for(int z = 0; z < frame.cols; ++z) {
        for(int i = 0; i < frame.rows; ++i) {
            int q = GetFX(blendW_frame, i, frame.rows);
            int j = GetFY(blendW_frame, z, frame.cols);
            cv::Vec3b &frame_one = frame.at<cv::Vec3b>(i, z);
            cv::Vec3b &frame_two = blendW_frame.at<cv::Vec3b>(q, j);
            for(int p = 0; p < 3; ++p)
                frame_one[p] += (frame_one[p]*alpha)+(frame_two[p]*beta);
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
    beta += -trans_var;
}

void ac::triBlendWithImage(cv::Mat &frame) {
    if(images_Enabled == false) return;
    static double alpha = 1.0f, beta = 1.0f;
    static int i = 0, z = 0, j = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b colors[3];
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            int cx[3] =  { ac::GetFX(image_files[0], i, frame.rows), ac::GetFX(image_files[1], i, frame.rows), ac::GetFX(image_files[2], i, frame.rows) };
            int cy[3] =  { ac::GetFY(image_files[0], z, frame.cols),  ac::GetFY(image_files[1], z, frame.cols), ac::GetFY(image_files[2], z, frame.cols) };
            for(j = 0; j < 3; ++j) {
                colors[j] = image_files[j].at<cv::Vec3b>(cx[j], cy[j]);
            }
            color_value[0] = ((color_value[0]+colors[0][0])/2)*alpha;
            color_value[1] += ((color_value[1]+colors[0][1])/2)*alpha;
            color_value[2] = ((color_value[2]+colors[0][2]+colors[1][2]+colors[2][2])/3)*beta;
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
    beta += -trans_var;
}


void ac::imageStrobe(cv::Mat &frame) {
    if(images_Enabled == false) return;
    static double alpha = 1.0f;
    static int i = 0, z = 0, j = 0, image_offset = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b colors[3];
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            int cx[3] =  { ac::GetFX(image_files[0], i, frame.rows), ac::GetFX(image_files[1], i, frame.rows), ac::GetFX(image_files[2], i, frame.rows) };
            int cy[3] =  { ac::GetFY(image_files[0], z, frame.cols),  ac::GetFY(image_files[1], z, frame.cols), ac::GetFY(image_files[2], z, frame.cols) };
            for(j = 0; j < 3; ++j)
                colors[j] = image_files[j].at<cv::Vec3b>(cx[j], cy[j]);
            for(j = 0; j < 3; ++j)
                color_value[j] += (colors[image_offset][j]*alpha);
            swapColors(frame, i, z);
            if(isNegative == true) {
                invert(frame, i, z);
            }
        }
    }
    ++image_offset;
    if(image_offset >= 4) image_offset = 0;
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = translation_variable;
    else if (alpha > max) {
        trans_var = -translation_variable;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

void ac::imageDistraction(cv::Mat &frame) {
    if(images_Enabled == false) return;
    static double alpha = 1.0f;
    static int i = 0, z = 0, im_off = 2;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &color_value = frame.at<cv::Vec3b>(i, z);
            int cX = GetFX(image_files[im_off], i, frame.rows), cY = GetFY(image_files[im_off], z, frame.cols);
            cv::Vec3b manip_color = image_files[im_off].at<cv::Vec3b>(cX, cY);
            color_value[0] = (z*alpha)+color_value[0];
            color_value[1] = manip_color[1]*alpha;
            color_value[2] = (i*alpha)+color_value[2];
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    ++im_off;
    if(im_off >= 4) im_off = 0;
    static double max = 4.0f, trans_var = 0.1f;
    if (alpha < 0)
        trans_var = 0.1f;
    else if (alpha > max) {
        trans_var = -0.1f;
        max += 3.0f;
        if (max > 23)
            max = 4.0f;
    }
    alpha += trans_var;
}

void ac::cossinMultiply(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            buffer[0] += 1+(sinf(alpha))*z;
            buffer[1] += 1+(cosf(alpha))*i;
            buffer[2] += (buffer[0]+buffer[1]+buffer[2])/3;
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static double trans_var = 0.05f;
    if(alpha > 24) alpha = 1.0f;
    alpha += trans_var;
}

void ac::colorAccumulate1(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            buffer[0] += (buffer[2]*alpha);
            buffer[1] += (buffer[0]*alpha);
            buffer[2] += (buffer[1]*alpha);
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static double trans_var = 0.05f;
    alpha += trans_var;
    if(alpha > 24) alpha = 1.0f;
}
void ac::colorAccumulate2(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            buffer[0] += (buffer[2]*alpha)+i;
            buffer[1] += (buffer[0]*alpha)+z;
            buffer[2] += (buffer[1]*alpha)+i-z;
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static double trans_var = 0.05f;
    alpha += trans_var;
    if(alpha > 24) alpha = 1.0f;
}
void ac::colorAccumulate3(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            buffer[0] = (-buffer[2])+z;
            buffer[1] = (-buffer[0])+i;
            buffer[2] = (-buffer[1])+alpha;
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static double trans_var = 0.05f;
    alpha += trans_var;
    if(alpha > 24) alpha = 1.0f;
}

void ac::filter8(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0, q = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            for(q = 0; q < 3; ++q) {
                buffer[q] = buffer[q]+((i+z)*alpha);
                
            }
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static int direction = 1;
    if(direction == 1) {
        alpha += 0.05f;
        if(alpha > 3) { alpha = 3; direction = 2; }
    } else {
        alpha -= 0.05f;
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }
    }
}

void ac::filter3(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int i = 0, z = 0, q = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            for(q = 0; q < 3; ++q) {
                buffer[q] = buffer[0]+(buffer[q])*(alpha);
            }
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static int direction = 1;
    if(direction == 1) {
        alpha += 0.1f;
        if(alpha > 6) { alpha = 6; direction = 2; }
    } else {
        alpha -= 0.05f;
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }
    }
}

void ac::rainbowBlend(cv::Mat &frame) {
    static double alpha = 1.0f;
    static int rb = 0, gb = 0, bb = 0;
    if(rb == 0) {
        rb = rand()%255;
    } else ++rb;
    if(gb == 0) {
        gb = rand()%255;
    } else ++gb;
    if(bb == 0) {
        bb = rand()%255;
    } else ++bb;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            buffer[0] += alpha*rb;
            buffer[1] += alpha*gb;
            buffer[2] += alpha*bb;
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    if(rb > 255) rb = 0;
    if(gb > 255) gb = 0;
    if(bb > 255) bb = 0;
    static int direction = 1;
    if(direction == 1) {
        alpha += 0.1f;
        if(alpha > 6) { alpha = 6; direction = 2; }
    } else {
        alpha -= 0.05f;
        if(alpha <= 0.1f) { alpha = 0.1f; direction = 1; }
    }
}

void ac::randBlend(cv::Mat &frame) {
    unsigned char rr = rand()%255;
    unsigned char rg = rand()%255;
    unsigned char rb = rand()%255;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            buffer[0] += rr;
            buffer[1] += rg;
            buffer[2] += rb;
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
}

void ac::newBlend(cv::Mat &frame) {
    static int pos = 300;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            buffer[0] = buffer[2]+(1+(i*z)/pos);
            buffer[1] = buffer[1]+(1+(i*z)/pos);
            buffer[2] = buffer[0]+(1+(i*z)/pos);
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static unsigned int dir = 1;
    if(dir == 1) {
        pos += 25;
        if(pos > 1024) {
            pos = 1024;
            dir = 2;
        }
    }
    else {
        pos -= 25;
        if(pos < 100) {
            pos = 100;
            dir = 1;
        }
    }
}

void ac::pixelScale(cv::Mat &frame) {
    static double pos = 1.0f;
    static int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            cv::Vec3b buf = buffer;
            buffer[0] = (buf[0]*pos)+(buf[0]-buffer[2]);
            buffer[1] = (buf[1]*pos)+(buf[1]+buffer[1]);
            buffer[2] = (buf[2]*pos)+(buf[2]-buffer[0]);
            swapColors(frame, i, z);
            if(isNegative) invert(frame, i, z);
        }
    }
    static int direction = 1;
    static double pos_max = 3.0f;
    if(direction == 1) {
        pos += 0.1f;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 0.5f;
        }
    } else if(direction == 0) {
        pos -= 0.1f;
        if(pos <= 0) {
            if(pos_max > 15) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::glitchSort(cv::Mat &frame) {

    static double pos = 1.0f;
    int w = frame.cols;
    int h = frame.rows;
    static std::vector<unsigned int> v;
    v.reserve(w);
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &value = frame.at<cv::Vec3b>(z, i);
            unsigned int vv = 0;
            unsigned char *cv = (unsigned char*)&vv;
            cv[0] = value[0];
            cv[1] = value[1];
            cv[2] = value[2];
            cv[3] = 0;
            v.push_back(vv);
        }
        std::sort(v.begin(), v.end());
        for(int i = 0; i < w; ++i) {
            unsigned char *value = (unsigned char*)&v[i];
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] = pixel[0] + (pos)*value[0];
            pixel[1] = pixel[1] + (pos)*value[1];
            pixel[2] = pixel[2] + (pos)*value[2];
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
            
        }
        v.erase(v.begin(), v.end());
    }
    static int direction = 1;
    static double pos_max = 7.0f;
    if(direction == 1) {
        pos += 0.05;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 0.5f;
        }
    } else if(direction == 0) {
        pos -= 0.05;
        if(pos <= 1) {
            if(pos_max > 15) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::pixelSort(cv::Mat &frame) {
    int w = frame.cols;
    int h = frame.rows;
    static std::vector<unsigned int> v;
    v.reserve(w);
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            //unsigned int value = frame.at<unsigned int>(z, i);
            cv::Vec3b &value = frame.at<cv::Vec3b>(z, i);
            unsigned int vv = 0;
            unsigned char *cv = (unsigned char*)&vv;
            cv[0] = value[0];
            cv[1] = value[1];
            cv[2] = value[2];
            cv[3] = 0;
            v.push_back(vv);
        }
        std::sort(v.begin(), v.end());
        for(int i = 0; i < w; ++i) {
            unsigned char *value = (unsigned char*)&v[i];
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[0] += value[0];
            pixel[1] += value[1];
            pixel[2] += value[2];
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
        v.erase(v.begin(), v.end());
    }
}

void ac::randomFilter(cv::Mat &frame) {
    int num;
    do {
        num = rand()%(draw_max-6);
    } while(ac::draw_strings[num] == "Random Filter");
    draw_func[num](frame);
}

int current_filterx = 0;
int bytesPerSample = 0;
int bytesPerRow = 0;
int width = 0;
int height = 0;
int red = 0;
int green = 0;
int blue = 0;
int offset = 0;
int randomNumber = 0;
int reverse = 0;
bool negate = false;

void changePixel(cv::Mat &full_buffer, int i, int z, cv::Vec3b &buffer, double pos, double *count) {
    switch(current_filterx) {
        case 0:
        {
            double value = pos;
            buffer[0] = (unsigned char) value*buffer[0];
            buffer[1] = (unsigned char) value*buffer[1];
            buffer[2] = (unsigned char) value*buffer[2];
        }
            break;
        case 1:
        {
            double value = pos;
            buffer[0] = (unsigned char) value*buffer[0];
            buffer[1] = (unsigned char) (-value)*buffer[1];
            buffer[2] = (unsigned char) value*buffer[2];
        }
            break;
        case 2:
        {
            buffer[0] += buffer[0]*-pos;
            buffer[1] += buffer[1]*pos;
            buffer[2] += buffer[2]*-pos;
        }
            break;
        case 3:
        {
            int current_pos = pos*0.2f;
            buffer[0] = (i*current_pos)+buffer[0];
            buffer[2] = (z*current_pos)+buffer[2];
            buffer[1] = (current_pos*buffer[1]);
        }
            break;
        case 4:
        {
            int current_pos = pos*0.2f;
            buffer[0] = (z*current_pos)+buffer[0];
            buffer[1] = (i*current_pos)+buffer[1];
            buffer[2] = ((i+z)*current_pos)+buffer[2];
        }
            break;
        case 5:
        {
            int current_pos = pos*0.2f;
            buffer[0] = -(z*current_pos)+buffer[0];
            buffer[1] = -(i*current_pos)+buffer[1];
            buffer[2] = -((i+z)*current_pos)+buffer[2];
        }
            break;
            
        case 6:
        {
            int zq = z+1, iq = i+1;
            if(zq > height-1 || iq > width-1) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            buffer[0] += (i*pos)+temp[0];
            buffer[1] += (z*pos)+temp[1];
            buffer[2] += (i/(z+1))+temp[2];
        }
            break;
        case 7:
        {
            unsigned char colv[4], cola[4];
            colv[0] = buffer[0];
            colv[1] = buffer[1];
            colv[2] = buffer[2];
            cola[0] = buffer[2];
            cola[1] = buffer[1];
            cola[2] = buffer[0];
            unsigned int alpha = (int)pos;
            unsigned int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            unsigned int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            unsigned int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            for(unsigned int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            buffer[0] += alpha*R;
            buffer[1] += alpha*G;
            buffer[2] += alpha*B;
        }
            break;
        case 8:
        {
            unsigned char colv[4], cola[4];
            colv[0] = buffer[0];
            colv[1] = buffer[1];
            colv[2] = buffer[2];
            int iq = (width-i-1);
            int zq = (height-z-1);
            cv::Vec3b &t = full_buffer.at<cv::Vec3b>(zq, iq);
            cola[0] = t[0];
            cola[1] = t[1];
            cola[2] = t[2];
            unsigned int alpha = (int)pos;
            unsigned int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            unsigned int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            unsigned int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            for(unsigned int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            buffer[0] += alpha*R;
            buffer[1] += alpha*G;
            buffer[2] += alpha*B;
        }
            break;
        case 9:
        {
            double alpha = pos;
            unsigned char colorz[3][3];
            colorz[0][0] = buffer[0];
            colorz[0][1] = buffer[1];
            colorz[0][2] = buffer[2];
            int total_r = colorz[0][0] +colorz[0][1]+colorz[0][2];
            total_r /= 3;
            total_r *= alpha;
            int iq = i+1;
            if(iq > width) return;
            int zq = z;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            colorz[1][0] = temp[0];
            colorz[1][1] = temp[1];
            colorz[1][2] = temp[2];
            int total_g = colorz[1][0]+colorz[1][1]+colorz[1][2];
            total_g /= 3;
            total_g *= alpha;
            buffer[0] = (unsigned char)total_r;
            buffer[1] = (unsigned char)total_g;
            buffer[2] = (unsigned char)total_r+total_g*alpha;
            
        }
            break;
        case 10:
        {
            buffer[0] = ((i+z)*pos)/(i+z+1)+buffer[0]*pos;
            buffer[1] += ((i*pos)/(z+1))+buffer[1];
            buffer[2] += ((z*pos)/(i+1))+buffer[2];
        }
            break;
        case 11:
        {
            buffer[0] += (buffer[2]+(i*pos))/(pos+1);
            buffer[1] += (buffer[1]+(z*pos))/(pos+1);
            buffer[2] += buffer[0];
        }
            break;
        case 12:
        {
            buffer[0] += (i/(z+1))*pos+buffer[0];
            buffer[1] += (z/(i+1))*pos+buffer[1];
            buffer[2] += ((i+z)/(pos+1)+buffer[2]);
        }
            break;
        case 13:
        {
            buffer[0] += (pos*(i/(pos+1))+buffer[2]);
            buffer[1] += (pos*(z/(pos+1))+buffer[1]);
            buffer[2] += (pos*((i*z)/(pos+1)+buffer[0]));
        }
            break;
        case 14:
        {
            buffer[0] = ((i+z)*pos)/(i+z+1)+buffer[0]*pos;
            buffer[1] += (buffer[1]+(z*pos))/(pos+1);
            buffer[2] += ((i+z)/(pos+1)+buffer[2]);
        }
            break;
        case 15:
        {
            buffer[0] = (i%(z+1))*pos+buffer[0];
            buffer[1] = (z%(i+1))*pos+buffer[1];
            buffer[2] = (i+z%((int)pos+1))+buffer[2];
        }
            break;
        case 16:
        {
            unsigned int r = 0;
            r = (buffer[0]+buffer[1]+buffer[2])/3;
            buffer[0] += pos*r;
            buffer[1] += -(pos*r);
            buffer[2] += pos*r;
        }
            break;
        case 17:
        {
            unsigned long r = 0;;
            r = (buffer[0]+buffer[1]+buffer[2])/(pos+1);
            buffer[0] += r*pos;
            r = (buffer[0]+buffer[1]+buffer[2])/3;
            buffer[1] += r*pos;
            r = (buffer[0]+buffer[1]+buffer[2])/5;
            buffer[2] += r*pos;
        }
            break;
        case 18:
        {
            buffer[0] += 1+(sinf(pos))*z;
            buffer[1] += 1+(cosf(pos))*i;
            buffer[2] += (buffer[0]+buffer[1]+buffer[2])/3;
        }
            break;
        case 19:
        {
            buffer[0] += (buffer[2]-i)*((((int)pos+1)%15)+2);
            buffer[1] += (buffer[1]-z)*((((int)pos+1)%15)+2);
            buffer[2] += buffer[0]-(i+z)*((((int)pos+1)%15)+2);
        }
            break;
        case 20:
        {
            buffer[0] += (buffer[0]+buffer[1]-buffer[2])/3*pos;
            buffer[1] -= (buffer[0]-buffer[1]+buffer[2])/6*pos;
            buffer[2] += (buffer[0]-buffer[1]-buffer[2])/9*pos;
        }
            break;
        case 21:
        {
            int iq = i, zq = z+1;
            if(zq > height-2) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+2;
            if(zq > height-2) return;
            cv::Vec3b &temp2 = full_buffer.at<cv::Vec3b>(zq, iq);
            int ir, ig, ib;
            ir = buffer[0]+temp[0]-temp2[0];
            ig = buffer[1]-temp[1]+temp2[1];
            ib = buffer[2]-temp[2]-temp2[2];
            if(z%2 == 0) {
                if(i%2 == 0) {
                    buffer[0] = ir+(0.5*pos);
                    buffer[1] = ig+(0.5*pos);
                    buffer[2] = ib+(0.5*pos);
                } else {
                    buffer[0] = ir+(1.5*pos);
                    buffer[1] = ig+(1.5*pos);
                    buffer[2] = ib+(1.5*pos);
                }
            } else {
                if(i%2 == 0) {
                    buffer[0] += ir+(0.1*pos);
                    buffer[1] += ig+(0.1*pos);
                    buffer[2] += ib+(0.1*pos);
                } else {
                    buffer[0] -= ir+(i*pos);
                    buffer[1] -= ig+(z*pos);
                    buffer[2] -= ib+(0.1*pos);
                }
            }
        }
            break;
        case 22:
        {
            if((i%2) == 0) {
                if((z%2) == 0) {
                    buffer[0] = 1-pos*buffer[0];
                    buffer[2] = (i+z)*pos;
                } else {
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i-z)*pos;
                }
            } else {
                if((z%2) == 0) {
                    buffer[0] = pos*buffer[0]-i;
                    buffer[2] = (i-z)*pos;
                } else {
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i+z)*pos;
                }
            }
        }
            break;
        case 23:
        {
            buffer[0] = buffer[0]+buffer[1]*2+pos;
            buffer[1] = buffer[1]+buffer[0]*2+pos;
            buffer[2] = buffer[2]+buffer[0]+pos;
        }
            break;
        case 24:
        {
            buffer[0] += buffer[2]+pos;
            buffer[1] += buffer[1]+pos;
            buffer[2] += buffer[0]+pos;
        }
            break;
        case 25:
        {
            buffer[0] += (buffer[2]*pos);
            buffer[1] += (buffer[0]*pos);
            buffer[2] += (buffer[1]*pos);
        }
            break;
        case 26:
        {
            buffer[0] += (buffer[2]*pos)+i;
            buffer[1] += (buffer[0]*pos)+z;
            buffer[2] += (buffer[1]*pos)+i-z;
        }
            break;
        case 27:
        {
            buffer[0] = (-buffer[2])+z;
            buffer[1] = (-buffer[0])+i;
            buffer[2] = (-buffer[1])+pos;
        }
            break;
        case 28:
        {
            buffer[0] = buffer[2]+(1+(i*z)/pos);
            buffer[1] = buffer[1]+(1+(i*z)/pos);
            buffer[2] = buffer[0]+(1+(i*z)/pos);
        }
            break;
        case 29:
        {
            int iq = i, zq = z+1;
            if(zq > height-2) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+2;
            if(zq > height-2) return;
            cv::Vec3b &temp2 = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+3;
            if(zq > height-2) return;
            cv::Vec3b &temp3 = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+4;
            if(zq > height-2) return;
            cv::Vec3b &temp4 = full_buffer.at<cv::Vec3b>(zq, iq);
            unsigned char col[4];
            col[0] = (temp[0]+temp2[0]+temp3[0]+temp4[0])/4;
            col[1] = (temp[1]+temp2[1]+temp3[1]+temp4[1])/4;
            col[2] = (temp[2]+temp2[2]+temp3[2]+temp4[2])/4;
            buffer[0] = col[0]*pos;
            buffer[1] = col[1]*pos;
            buffer[2] = col[2]*pos;
        }
            break;
        case 30:
        {
            double rad = 100.0;
            double degree = 0.01*pos;
            int x = (int)rad * cos(degree);
            int y = (int)rad * sin(degree);
            int z = (int)rad * tanf((double)degree);
            buffer[0] = buffer[0]+x;
            buffer[2] = buffer[1]+y;
            buffer[1] = buffer[1]+z;
        }
            break;
        case 31:
        {
            int average= (buffer[0]+buffer[1]+buffer[2]+1)/3;
            buffer[0] += buffer[2]+average*(pos);
            buffer[1] += buffer[0]+average*(pos);
            buffer[2] += buffer[1]+average*(pos);
        }
            break;
        case 32:
        {
            unsigned int value = 0;
            value  = ~buffer[0] + ~buffer[1] + ~buffer[2];
            value /= 2;
            buffer[0] = buffer[0]+value*pos;
            value /= 2;
            buffer[1] = buffer[1]+value*pos;
            value /= 2;
            buffer[2] = buffer[2]+value*pos;
        }
            break;
        case 33:
        {
            buffer[0] += *count*pos;
            buffer[1] += *count*pos;
            buffer[2] += *count*pos;
            *count += 0.00001f;
            if(*count > 255) *count = 0;
        }
            break;
        case 34:
        {
            buffer[0] += pos*(randomNumber+pos);
            buffer[1] += pos*(randomNumber+z);
            buffer[2] += pos*(randomNumber+i);
        }
            break;
        case 35:
        {
            buffer[0] += *count *z;
            buffer[1] += *count *pos;
            buffer[2] += *count *z;
            *count += 0.0000001f;
        }
            break;
        case 36:
        {
            buffer[0] += sinf(M_PI+pos)*pos;
            buffer[1] += cosf(M_PI+pos)*pos;
            buffer[2] += tanf(M_PI+pos)*pos;
        }
            break;
    }
    buffer[0] += red;
    buffer[1] += green;
    buffer[2] += blue;
    if(negate == true) {
        buffer[0] = ~buffer[0];
        buffer[1] = ~buffer[1];
        buffer[2] = ~buffer[2];
    }
    unsigned char buf[3];
    buf[0] = buffer[0];
    buf[1] = buffer[1];
    buf[2] = buffer[2];
    switch(reverse) {
        case 0://normal
            break;
        case 1:
            buffer[0] = buf[2];
            buffer[1] = buf[1];
            buffer[2] = buf[0];
            break;
        case 2:
            buffer[0] = buf[1];
            buffer[1] = buf[2];
            buffer[2] = buf[0];
            break;
        case 3:
            buffer[0] = buf[2];
            buffer[1] = buf[0];
            buffer[2] = buf[1];
            break;
        case 4:
            buffer[0] = buf[1];
            buffer[1] = buf[0];
            buffer[2] = buf[2];
            break;
    }
}

void ac::alphaFlame(cv::Mat &frame) {
    static double pos = 1.0f;
    double count = 1.0f;
    static int i = 0, z = 0;
    width = frame.cols;
    height = frame.rows;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            changePixel(frame, z, i, buffer, pos, &count);
        }
    }
    static int direction = 1;
    if(direction == 1) {
        pos += 0.1f;
        if(pos > 512) {
            pos = 512;
            direction = 0;
        }
    }
    else {
        pos -= 0.1f;
        if(pos < 1) {
            pos = 1;
            direction = 1;
        }
    }
}

int AC_GetFX(int oldw,int x, int nw) {
    float xp = (float)x * (float)oldw / (float)nw;
    return (int)xp;
}

int AC_GetFZ(int oldh, int y, int nh) {
    float yp = (float)y * (float)oldh / (float)nh;
    return (int)yp;
}

void ac::imageBlend(cv::Mat &frame) {
    static double pos = 1.0f;
    if(blend_set == true) {
        int i,z;
        for(i = 0; i < frame.cols; ++i) {
            for(z = 0; z < frame.rows; ++z) {
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b &current = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);
                current[0] = current[0]+(im[0]*pos);
                current[1] = current[1]+(im[1]*pos);
                current[2] = current[2]+(im[2]*pos);
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
        }
    }
    static int direction = 1;
    static double pos_max = 7.0f;
    if(direction == 1) {
        pos += 0.05;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 0.5f;
        }
    } else if(direction == 0) {
        pos -= 0.05;
        if(pos <= 1) {
            if(pos_max > 15) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::imageBlendTwo(cv::Mat &frame) {
    static double pos = 1.0f;
    if(blend_set == true) {
        int i,z;
        for(i = 0; i < frame.cols; ++i) {
            for(z = 0; z < frame.rows; ++z) {
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b &current = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);
                current[0] = im[0]+(current[0]*pos);
                current[1] = im[1]+(current[1]*pos);
                current[2] = im[2]+(current[2]*pos);
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
            }
        }
    }
    static int direction = 1;
    static double pos_max = 7.0f;
    if(direction == 1) {
        pos += 0.05;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 0.5f;
        }
    } else if(direction == 0) {
        pos -= 0.05;
        if(pos <= 1) {
            if(pos_max > 15) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::imageBlendThree(cv::Mat &frame) {
    if(blend_set == true) {
        static double pos = 1.0f;
        for(int i = 0; i < frame.cols; ++i) {
            for(int z = 0; z < frame.rows; ++z) {
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);
                pixel[0] += (pixel[0]^im[0]);
                pixel[1] += (pixel[1]^im[1]);
                pixel[2] += (pixel[2]^im[2])*pos;
                swapColors(frame, z, i);
                if(isNegative) invert(frame, z, i);
                
            }
        }
        static int direction = 1;
        static double pos_max = 7.0f;
        if(direction == 1) {
            pos += 0.005;
            if(pos > pos_max) {
                pos = pos_max;
                direction = 0;
                pos_max += 0.5f;
            }
        } else if(direction == 0) {
            pos -= 0.005;
            if(pos <= 1) {
                if(pos_max > 15) pos_max = 1.0f;
                direction = 1;
            }
        }
    }
}

void ac::GaussianBlur(cv::Mat &frame) {
    cv::Mat out;
    cv::GaussianBlur(frame, out, cv::Size(5, 5), 0, 0);
    frame = out;
}

void ac::MedianBlur(cv::Mat &frame) {
    cv::Mat out;
    cv::medianBlur(frame, out, 5);
    frame = out;
}

void ac::BlurDistortion(cv::Mat &frame) {
    cv::Mat out;
    static unsigned int index = 1, direction = 1;
    cv::GaussianBlur(frame, out, cv::Size(index, index), 0, 0);
    if(direction == 1) {
    	if(index >= 51) direction = 0;
    	else index += 2;
    } else {
        if(index <= 1) direction = 1;
        else index -= 2;
    }
    frame = out;
}

void ac::DiamondPattern(cv::Mat &frame) {
    static double pos = 1.0;
    int w = frame.cols;
    int h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            if((i%2) == 0) {
                if((z%2) == 0) {
                    buffer[0] = 1-pos*buffer[0];
                    buffer[2] = (i+z)*pos;
                } else {
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i-z)*pos;
                }
            } else {
                if((z%2) == 0) {
                    buffer[0] = pos*buffer[0]-i;
                    buffer[2] = (i-z)*pos;
                } else {
                    buffer[0] = pos*buffer[0]-z;
                    buffer[2] = (i+z)*pos;
                }
            }
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double pos_max = 7.0f;
    if(direction == 1) {
        pos += 0.05;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 0.5f;
        }
    } else if(direction == 0) {
        pos -= 0.05;
        if(pos <= 1.0) {
            if(pos_max > 15) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::MirrorBlend(cv::Mat &frame) {
    static double pos = 1.0;
    int w = frame.cols;
    int h = frame.rows;
    cv::Mat orig;
    orig = frame.clone();
    for(int z = 2; z < h-3; ++z) {
        for(int i = 2; i < w-3; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));
            buffer[0] += pix1[0]*pos;
            buffer[1] += pix1[1]*pos;
            buffer[2] += pix1[2]*pos;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double pos_max = 2.0f;
    if(direction == 1) {
        pos += 0.1;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 1.0f;
        }
    } else if(direction == 0) {
        pos -= 0.1;
        if(pos <= 1.0) {
            if(pos_max > 2.0f) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::Pulse(cv::Mat &frame) {
    static double pos = 1.0;
    int w = frame.cols;
    int h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            buffer[0] += buffer[0]*pos;
            buffer[1] += buffer[1]*pos;
            buffer[2] += buffer[2]*pos;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double pos_max = 3.0f;
    if(direction == 1) {
        pos += 0.1;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 1.0f;
        }
    } else if(direction == 0) {
        pos -= 0.1;
        if(pos <= 1.0) {
            if(pos_max > 3.0f) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::SidewaysMirror(cv::Mat &frame) {
    static double pos = 1.0;
    int w = frame.cols;
    int h = frame.rows;
    cv::Mat orig;
    orig = frame.clone();
    for(int z = 2; z < h-3; ++z) {
        for(int i = 2; i < w-3; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));
            cv::Vec3b &pix2 = orig.at<cv::Vec3b>(z, (w-i));
            buffer[0] += (pix1[0]+pix2[0])*pos;
            buffer[1] += (pix1[1]+pix2[1])*pos;
            buffer[2] += (pix1[2]+pix2[2])*pos;
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double pos_max = 4.0f;
    if(direction == 1) {
        pos += 0.1;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 1.0f;
        }
    } else if(direction == 0) {
        pos -= 0.1;
        if(pos <= 1.0) {
            if(pos_max > 4.0f) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::MirrorNoBlend(cv::Mat &frame) {
    static double pos = 1.0;
    int w = frame.cols;
    int h = frame.rows;
    cv::Mat orig;
    orig = frame.clone();
    for(int z = 2; z < h-3; ++z) {
        for(int i = 2; i < w-3; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b &pix1 = orig.at<cv::Vec3b>((h-z), (w-i));
            cv::Vec3b &pix2 = orig.at<cv::Vec3b>(z, (w-i));
            cv::Vec3b &pix3 = orig.at<cv::Vec3b>((h-z), i);
             buffer[0] = (pix1[0]+pix2[0]+pix3[0]);
             buffer[1] = (pix1[1]+pix2[1]+pix3[1]);
             buffer[2] = (pix1[2]+pix2[2]+pix3[2]);
            swapColors(frame, z, i);
            if(isNegative) invert(frame, z, i);
        }
    }
    static int direction = 1;
    static double pos_max = 4.0f;
    if(direction == 1) {
        pos += 0.1;
        if(pos > pos_max) {
            pos = pos_max;
            direction = 0;
            pos_max += 1.0f;
        }
    } else if(direction == 0) {
        pos -= 0.1;
        if(pos <= 1.0) {
            if(pos_max > 4.0f) pos_max = 1.0f;
            direction = 1;
        }
    }
}

void ac::custom(cv::Mat &frame) {
    custom_filter(frame);
}
