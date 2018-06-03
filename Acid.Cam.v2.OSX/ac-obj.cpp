#include "ac.h"

ac::Point::Point() : x(0), y(0) {}
ac::Point::Point(const ac::Point &p) : x(p.x), y(p.y) {}
ac::Point::Point(int xx, int yy) : x(xx), y(yy) {}

ac::Point &ac::Point::operator=(const ac::Point &p) {
    x = p.x;
    y = p.y;
    return *this;
}

void ac::Point::setPoint(int xx, int yy) {
    x = xx;
    y = yy;
}

ac::Rect::Rect() : x(0), y(0), w(0), h(0) {}
ac::Rect::Rect(const ac::Rect &r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
ac::Rect::Rect(int xx, int yy, int ww, int hh) : x(xx), y(yy), w(ww), h(hh) {}
ac::Rect::Rect(int xx, int yy) : x(xx), y(yy), w(0), h(0) {}
ac::Rect::Rect(int xx, int yy, cv::Size s) : x(xx), y(yy), w(s.width), h(s.height) {}
ac::Rect::Rect(Point pt, int ww, int hh) : x(pt.x), y(pt.y), w(ww), h(hh) {}
ac::Rect::Rect(Point pt, cv::Size s) : x(pt.x), y(pt.y), w(s.width), h(s.height){}

ac::Rect &ac::Rect::operator=(const ac::Rect &r) {
    x = r.x;
    y = r.y;
    w = r.w;
    h = r.h;
    return *this;
}

void ac::Rect::setRect(int xx, int yy, int ww, int hh) {
    x = xx;
    y = yy;
    w = ww;
    h = hh;
}

