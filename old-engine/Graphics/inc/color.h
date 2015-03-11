#ifndef COLOR_H
#define COLOR_H

#include "configGraphicsDLL.h"

namespace GFX
{
    class ENGINE_GRAPHICS_EXPORT Color
    {
    public:
        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;

        enum Channel
        {
            C_RED,
            C_GREEN,
            C_BLUE,
            C_ALPHA,
            C_TOTAL
        };

    public:
        Color(float r = 0,
              float g = 0,
              float b = 0,
              float a = 0);
        Color(const Color& val);
    public:
        float R() const;
        void R(float val);
        float G() const;
        void G(float val);
        float B() const;
        void B(float val);
        float A() const;
        void A(float val);
    public:
        Color operator+(const Color& val) const;
        Color operator-(const Color& val) const;
        void operator+=(const Color& val);
        void operator-=(const Color& val);
        Color& operator=(const Color& val);
        Color& operator=(float val);
        Color& operator()(float r,
                          float g,
                          float b,
                          float a);
        float operator()(Channel id) const;
        void operator()(float (*val)[4]) const;
    protected:
        float color_[C_TOTAL];
    };

    class ENGINE_GRAPHICS_EXPORT HSV
        :   public Color
    {
    public:
        HSV(float h = 0,
            float s = 0,
            float v = 0,
            float a = 0);
        HSV(const HSV& val);
    public:
        float H() const;
        // [0,360]
        void H(float val);
        float S() const;
        // [0,1]
        void S(float val);
        float V() const;
        // [0,1]
        void V(float val);
        float A() const;
        void A(float val);
    private:
        float h_;
        float s_;
        float v_;
        float a_;
    };

    HSV ConvertRGBToHSV(const Color& val);
    Color ConvertHSVToRGB(const HSV& val);
}

#endif