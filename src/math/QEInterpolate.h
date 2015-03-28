/*!*****************************************************************************

\class 	QEInterpolate
\date 	2015/03/28
\author	Jonathan Butt
\brief	
\todo  Integrate scripting into interpolation

*******************************************************************************/

#ifndef QEINTERPOLATE_H
#define QEINTERPOLATE_H

template <typename T>
class QEInterpolation
{
public:
    static T Linear(T intp)
    {
        return intp;
    }

    static T EaseSquareIn(T intp)
    {
        return intp * intp;
    }

    static T EaseSquareOut(T intp)
    {
        intp = 1 - intp;
        intp *= intp;
        intp = 1 - intp;
        return intp;
    }

    static T EaseSquareInOut(T intp)
    {
        if(intp < 0.5f)
        {
            intp /= 0.5f;
            intp = EaseSquareIn(intp);
            intp *= 0.5f;
        }
        else
        {
            intp = (intp - 0.5f) / 0.5f;
            intp = EaseSquareOut(intp);
            intp = intp * 0.5f + 0.5f;
        }
        return intp;
    }

    static T EaseCubicIn(T intp)
    {
        return intp * intp * intp;
    }

    static T EaseCubicOut(T intp)
    {
        intp = 1 - intp;
        intp *= intp * intp;
        intp = 1 - intp;
        return intp;
    }

    static T EaseCubicInOut(T intp)
    {
        if(intp < 0.5f)
        {
            intp /= 0.5f;
            intp = EaseCubicIn(intp);
            intp *= 0.5f;
        }
        else
        {
            intp = (intp - 0.5f) / 0.5f;
            intp = EaseCubicOut(intp);
            intp = intp * 0.5f + 0.5f;
        }
        return intp;
    }

    static T Bounce(T intp)
    {
        if(dt < 0.5f)
        {
            float intp = dt / 0.5f;
            return Interpolation::EaseSquareIn(intp);
        }
        else if(dt < 0.65f)
        {
            float intp = (dt - 0.5f) / 0.15f;
            return 1.0f - Interpolation::EaseSquareOut(intp) * 0.2f;
        }
        else if(dt < 0.8f)
        {
            float intp = (dt - 0.65f) / 0.15f;
            return 0.8f + Interpolation::EaseSquareIn(intp) * 0.2f;
        }
        else if(dt < 0.9f)
        {
            float intp = (dt - 0.8f) / 0.1f;
            return 1.0f - Interpolation::EaseSquareOut(intp) * 0.05f;
        }
        else
        {
            float intp = (dt - 0.9f) / 0.1f;
            return 0.95f + Interpolation::EaseSquareIn(intp) * 0.05f;
        }
    }

    static T Overshoot(T intp)
    {
        if(intp < 0.5f)
        {
            intp /= 0.5f;
            return std::sin(-Math::PI * 0.5f + intp * Math::PI * 0.5f) + 1;
        }
        else if(intp < 0.7f)
        {
            intp = (intp - 0.5f) / 0.2f;
            float inc = std::sin(intp * Math::PI * 0.5f);
            inc *= 0.2f;
            return 1.0f + inc;
        }
        else
        {
            intp = (intp - 0.7f) / 0.3f;
            float inc = (std::sin(Math::PI * 0.5f + intp * Math::PI) + 1.0f) * 0.5f;
            inc *= 0.2f;
            return 1.0f + inc;
        }
    }
};

template <typename T,typename U>
static U Interpolate(const T& initial,
                        const T& final,
                        U intp,
                        U(*interpolator)(U))
{
    return initial + (*interpolator)(intp)* (final - initial);
}

#endif