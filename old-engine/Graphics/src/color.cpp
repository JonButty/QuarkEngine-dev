#include "color.h"
#include "mathDef.h"
#include <xutility>
#include <math.h>
#include <complex>

namespace GFX
{
    const Color Color::Black(0,0,0,0);
    const Color Color::White(1,1,1,1);
    const Color Color::Red(1,0,0,1);
    const Color Color::Green(0,1,0,1);
    const Color Color::Blue(0,0,1,1);

    Color::Color( float r,
                  float g,
                  float b,
                  float a )
    {
        color_[C_RED]   = std::min(1.0f,std::max(0.0f,r));
        color_[C_GREEN] = std::min(1.0f,std::max(0.0f,g));
        color_[C_BLUE]  = std::min(1.0f,std::max(0.0f,b));
        color_[C_ALPHA] = std::min(1.0f,std::max(0.0f,a));
    }
    
    Color::Color( const Color& val )
    {
        color_[C_RED]   = val.color_[C_RED];
        color_[C_GREEN] = val.color_[C_GREEN];
        color_[C_BLUE]  = val.color_[C_BLUE];
        color_[C_ALPHA] = val.color_[C_ALPHA];
    }

    //--------------------------------------------------------------------------

    float Color::R() const
    {
        return (*this)(C_RED);
    }

    void Color::R( float val )
    {
        color_[C_RED] = val;
    }

    float Color::G() const
    {
        return (*this)(C_GREEN);
    }

    void Color::G( float val )
    {
        color_[C_GREEN] = val;
    }

    float Color::B() const
    {
        return (*this)(C_BLUE);
    }

    void Color::B( float val )
    {
        color_[C_BLUE] = val;
    }

    float Color::A() const
    {
        return (*this)(C_ALPHA);
    }

    void Color::A( float val )
    {
        color_[C_ALPHA] = val;
    }

    //--------------------------------------------------------------------------

    Color Color::operator+( const Color& val ) const
    {
        return Color(color_[C_RED] + val.color_[C_RED],
                     color_[C_GREEN] + val.color_[C_GREEN],
                     color_[C_GREEN] + val.color_[C_BLUE],
                     color_[C_ALPHA] + val.color_[C_ALPHA]);
    }

    Color Color::operator-( const Color& val ) const
    {
        return Color(color_[C_RED] - val.color_[C_RED],
                     color_[C_GREEN] - val.color_[C_GREEN],
                     color_[C_GREEN] - val.color_[C_BLUE],
                     color_[C_ALPHA] - val.color_[C_ALPHA]);
    }

    void Color::operator+=( const Color& val )
    {
        color_[C_RED] += val.color_[C_RED];
        color_[C_GREEN] += val.color_[C_GREEN];
        color_[C_GREEN] += val.color_[C_BLUE];
        color_[C_ALPHA] += val.color_[C_ALPHA];
        std::min(1.0f,color_[C_RED]);
        std::min(1.0f,color_[C_GREEN]);
        std::min(1.0f,color_[C_GREEN]);
        std::min(1.0f,color_[C_ALPHA]);
    }

    void Color::operator-=( const Color& val )
    {
        color_[C_RED] -= val.color_[C_RED];
        color_[C_GREEN] -= val.color_[C_GREEN];
        color_[C_GREEN] -= val.color_[C_BLUE];
        color_[C_ALPHA] -= val.color_[C_ALPHA];
        std::max(0.0f,color_[C_RED]);
        std::max(0.0f,color_[C_GREEN]);
        std::max(0.0f,color_[C_GREEN]);
        std::max(0.0f,color_[C_ALPHA]);
    }

    Color& Color::operator=( const Color& val )
    {
        color_[C_RED]   = val.color_[C_RED];
        color_[C_GREEN] = val.color_[C_GREEN];
        color_[C_BLUE]  = val.color_[C_BLUE];
        color_[C_ALPHA] = val.color_[C_ALPHA];
        return *this;
    }

    Color& Color::operator=( float val )
    {
        color_[C_RED]   = val;
        color_[C_GREEN] = val;
        color_[C_BLUE]  = val;
        color_[C_ALPHA] = val;
        return *this;
    }

    Color& Color::operator()( float r,
                              float g,
                              float b,
                              float a )
    {
        color_[C_RED]   = r;
        color_[C_GREEN] = g;
        color_[C_BLUE]  = b;
        color_[C_ALPHA] = a;
        return *this;
    }

    float Color::operator()( Channel id ) const
    {
        return color_[id];
    }

    void Color::operator()( float (*val)[4] ) const
    {
        (*val)[C_RED]   = color_[C_RED];
        (*val)[C_GREEN] = color_[C_GREEN];
        (*val)[C_BLUE]  = color_[C_BLUE];
        (*val)[C_ALPHA] = color_[C_ALPHA];
    }

    //--------------------------------------------------------------------------

    HSV::HSV( float h,
              float s,
              float v,
              float a )
              : h_(h),
                s_(s),
                v_(v),
                a_(a)
    {
    }

    HSV::HSV( const HSV& val )
        :   h_(val.h_),
            s_(val.s_),
            v_(val.v_),
            a_(val.a_)
    {
    }

    //--------------------------------------------------------------------------

    float HSV::H() const
    {
        return h_;
    }

    void HSV::H( float val )
    {
        h_ = val;
    }

    float HSV::S() const
    {
        return s_;
    }

    void HSV::S( float val )
    {
        s_ = val;
    }

    float HSV::V() const
    {
        return v_;
    }

    void HSV::V( float val )
    {
        v_ = val;
    }

    float HSV::A() const
    {
        return a_;
    }

    void HSV::A( float val )
    {
        a_ = val;
    }

    //--------------------------------------------------------------------------

    HSV ConvertRGBToHSV( const Color& val )
    {
        HSV hsv;
        float minChannel = Math::Min(val.R(),val.G(),val.B());
        float maxChannel = Math::Max(val.R(),val.G(),val.B());
        hsv.V(maxChannel);

        if(std::abs(maxChannel) < Math::EPSILON)
        {
            hsv.H(0);
            hsv.S(0); 
            return hsv;
        }

        // Normalize values
        float red   = val.R()/hsv.V();
        float green = val.G()/hsv.V();
        float blue  = val.B()/hsv.V();
        minChannel /= hsv.V();
        maxChannel /= hsv.V();

        // Compute Saturation
        // The saturation (S in HSV) is controlled by how widely separated the
        // RGB values are. When the values are close together, the color will be
        // close to gray. When they're far apart, the color will be more intense
        // or pure
        hsv.S(maxChannel - minChannel);
        if(std::abs(hsv.S()) < Math::EPSILON) 
        {
            hsv.H(0);
            return hsv;
        }

        // Normalize saturation to 1
        red   = (red - minChannel)/hsv.S();
        green = (green - minChannel)/hsv.S();
        blue  = (blue - minChannel)/hsv.S();
        minChannel = Math::Min(red,green,blue);
        maxChannel = Math::Max(red,green,blue);

        // Compute Hue
        // Finally, hue, which determines whether the color is red, blue, green,
        // yellow, and so on, is the most complex to compute. Red is at 0 
        // degrees, green is at 120 degrees, and blue is at 240 degrees. The 
        // maximum RGB color controls our starting point, and the difference of 
        // the other two colors determines how far we move away from it, up to 
        // 60 degrees away (halfway to the next primary color)
        if(maxChannel == red) 
        {
            hsv.H(60.0f * (green - blue));
            if(hsv.H() < 0.0f) 
                hsv.H(hsv.H() + 360.0f);
        } 
        else if (std::abs(maxChannel - green) < Math::EPSILON) 
            hsv.H(120.0f + 60.0f * (blue - red));
        else 
            hsv.H(240.0f + 60.0f * (red - green));
        return hsv;
    }

    Color ConvertHSVToRGB( const HSV& val )
    {
        Color color;
        if(val.S() == 0) 
        {
            // achromatic (grey)
            color.R(val.V());
            color.G(val.V());
            color.B(val.V());
            return color;
        }

        float hue = val.H()/60.f;			// sector 0 to 5
        int i = static_cast<int>(floor(hue));
        float f = hue - i;			// factorial part of h
        float p = val.V() * ( 1 - val.S() );
        float q = val.V() * ( 1 - val.S() * f );
        float t = val.V() * ( 1 - val.S() * ( 1 - f ) );

        switch(i)
        {
        case 0:
            color.R(val.V());
            color.G(t);
            color.B(p); 
            break;
        case 1:
            color.R(q);
            color.G(val.V());
            color.B(p); 
            break;
        case 2:
            color.R(p);
            color.G(val.V());
            color.B(t); 
            break;
        case 3:
            color.R(p);
            color.G(q);
            color.B(val.V()); 
            break;
        case 4: 
            color.R(t);
            color.G(p);
            color.B(val.V()); 
            break;
        case 5: 
            color.R(val.V());
            color.G(p);
            color.B(q); 
            break;
        }
        return color;
    }
}