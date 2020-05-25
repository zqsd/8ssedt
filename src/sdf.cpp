#include "sdf.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace std;

// dev from https://github.com/Lisapple/8SSEDT

#define MAX_DISTANCE 255

struct Case {
    int dx, dy, d2;

    Case(int d = MAX_DISTANCE) {
        dx = d;
        dy = d;
        d2 = 2 * d * d;
    }

    Case& operator=(int d) {
        dx = dy = d;
        d2 = 2 * d * d;
        return *this;
    }

    void compare(Case& c, int dx, int dy) {
        int ndx = c.dx == MAX_DISTANCE ? MAX_DISTANCE : c.dx + dx,
            ndy = c.dy == MAX_DISTANCE ? MAX_DISTANCE : c.dy + dy;
        int nd2 = ndx * ndx + ndy * ndy;
        if(nd2 < d2) {
            this->dx = ndx;
            this->dy = ndy;
            this->d2 = nd2;
        }
    }

    friend ostream& operator << (ostream& o, const Case& c);
};

ostream& operator << (ostream& o, const Case& c) {
    return o << "(" << c.dx << " " << c.dy << " " << c.d2 << ")";
}

struct Grid {
    Case* data;
    int w, h;

    Grid(int _w, int _h) : w(_w), h(_h) {
        data = new Case[(w + 2) * (h + 2)];
    }

    ~Grid() {
        delete[] data;
    }

    Case& operator()(int x, int y) {
        if(x < -1 || x > w || y < -1 || y > h) {
            throw std::range_error("Invalid coordinate");
        }
        int i = (y + 1) * (w + 2) + (x + 1);
        
        return data[i];
    }
};

void generate(Grid& grid) {
    int w = grid.w,
        h = grid.h;

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {

            Case& c = grid(x, y);
            if(c.d2 > 0) {
                c.compare(grid(x - 1, y), -1, 0);
                c.compare(grid(x, y - 1), 0, -1);
                c.compare(grid(x - 1, y - 1), -1, -1);
                c.compare(grid(x + 1, y - 1), 1, -1);
            }

        }

        for(int x = w - 1; x >= 0; x--) {
            Case& c = grid(x, y);
            c.compare(grid(x + 1, y), 1, 0);
        }
    }
    
    for(int y = h - 1; y >= 0; y--) {
        for(int x = w - 1; x >= 0; x--) {
            Case& c = grid(x, y);
            if(c.d2 > 0) {
                c.compare(grid(x + 1, y), 1, 0);
                c.compare(grid(x, y + 1), 0, 1);
                c.compare(grid(x + 1, y + 1), 1, 1);
                c.compare(grid(x - 1, y + 1), -1, 1);
            }
        }
        for(int x = 0; x < w; x++) {
            Case& c = grid(x, y);
            c.compare(grid(x - 1, y), -1, 0);
        }
    }
}

template<unsigned int stride, typename T>
void set(uint8_t* data, int x, int y, int w, int channel, int threshold, T value) {
    data[stride * (y * w + x) + channel] = (int8_t)(std::round(std::min<T>(255, std::max<T>(value + threshold, 0))));
}

template<typename D, unsigned int stride>
void sdf::generate(D* dst, uint8_t* src, int w, int h, int channel, int threshold, float radius) {
    Grid gridI(w, h),
         gridO(w, h);

    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            if(src[stride * (y * w + x) + channel] >= threshold) {
                gridI(x, y) = 0;
                gridO(x, y) = MAX_DISTANCE;
            }
            else {
                gridI(x, y) = MAX_DISTANCE;
                gridO(x, y) = 0;
            }
        }
    }

    generate(gridI);
    generate(gridO);

    float radius_ratio = 127.0f / radius;
    for(int y = 0; y < h; y++) {
        for(int x = 0; x < w; x++) {
            float distanceO = sqrt((float)gridO(x, y).d2);
            float distanceI = sqrt((float)gridI(x, y).d2);
            float distance = distanceI - std::max<float>(distanceO - 1.0f, 0.0f);
            float value = distance * radius_ratio;
            set<stride>(dst, x, y, w, channel, threshold, value);
        }
    }
}

Napi::Value sdf::GenerateWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    if(info.Length() >= 6 && info.Length() <= 7) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return Napi::Number::New(env, 0);
    }

    assert(info[0].IsTypedArray());
    assert(info[1].IsTypedArray());
    assert(info[2].IsNumber());
    assert(info[3].IsNumber());
    assert(info.Length() == 4 || info.Length() == 5 && info[4].IsObject());

    Napi::Uint8Array dst = info[0].As<Napi::Uint8Array>();
    uint8_t* dstPtr = reinterpret_cast<uint8_t*>(dst.ArrayBuffer().Data());
    int width = info[2].As<Napi::Number>().Int32Value();
    int height = info[3].As<Napi::Number>().Int32Value();

    Napi::Uint8Array src = info[1].As<Napi::Uint8Array>();
    uint8_t* srcPtr = reinterpret_cast<uint8_t*>(src.ArrayBuffer().Data());

    int stride = 1;
    int channel = 0;
    int threshold = 127;
    float radius = 127.0f;
    if(info.Length() > 4) {
        Napi::Object params = info[4].As<Napi::Object>();

        if(params.Has("stride") && params.Get("stride").IsNumber())
            stride = params.Get("stride").As<Napi::Number>().Int32Value();
        if(params.Has("channel") && params.Get("channel").IsNumber())
            channel = params.Get("channel").As<Napi::Number>().Int32Value();
        if(params.Has("threshold") && params.Get("threshold").IsNumber())
            threshold = params.Get("threshold").As<Napi::Number>().Int32Value();
        if(params.Has("radius") && params.Get("radius").IsNumber())
            radius = params.Get("radius").As<Napi::Number>().FloatValue();
    }

    if(stride == 1) {
        sdf::generate<uint8_t, 1>(dstPtr, srcPtr, width, height, channel, threshold, radius);
    }
    else if(stride == 4) {
        sdf::generate<uint8_t, 4>(dstPtr, srcPtr, width, height, channel, threshold, radius);
    }
    else {
        Napi::TypeError::New(env, "Unsupported stride").ThrowAsJavaScriptException();
    }
    
    return env.Undefined();
}

Napi::Object sdf::Init(Napi::Env env, Napi::Object exports) {
    exports.Set("generate", Napi::Function::New(env, sdf::GenerateWrapped));

    return exports;
}