/*****************************************************************************/
/*!
\file		MatrixSSE.cpp
\author 	Bryan Dismas Yeo, bryan.yeo, 250002511
\par    	email: bryan.yeo\@digipen.edu
\date   	March 14, 2012

Copyright (C) 2012 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/*****************************************************************************/

#include "Simian/SimianPlatform.h"
#if SIMIAN_SSE_SUPPORT == 1

#include "Simian/MatrixSSE.h"

namespace Simian
{
    Matrix::Matrix(   f32 m11, f32 m12, f32 m13, f32 m14,
                        f32 m21, f32 m22, f32 m23, f32 m24,
                        f32 m31, f32 m32, f32 m33, f32 m34,
                        f32 m41, f32 m42, f32 m43, f32 m44)
    : sse1_(_mm_set_ps(m14, m13, m12, m11)),
      sse2_(_mm_set_ps(m24, m23, m22, m21)),
      sse3_(_mm_set_ps(m34, m33, m32, m31)),
      sse4_(_mm_set_ps(m44, m43, m42, m41))
    {
    }

    Matrix::Matrix(f32* m)
    : sse1_(_mm_set_ps(m[3], m[2], m[1], m[0])),
      sse2_(_mm_set_ps(m[7], m[6], m[5], m[4])),
      sse3_(_mm_set_ps(m[11], m[10], m[9], m[8])),
      sse4_(_mm_set_ps(m[15], m[14], m[13], m[12]))
    {
    }

    Matrix::Matrix(f32 (*m)[4])
    : sse1_(_mm_set_ps(m[0][3], m[0][2], m[0][1], m[0][0])),
      sse2_(_mm_set_ps(m[1][3], m[1][2], m[1][1], m[1][0])),
      sse3_(_mm_set_ps(m[2][3], m[2][2], m[2][1], m[2][0])),
      sse4_(_mm_set_ps(m[3][3], m[3][2], m[3][1], m[3][0]))
    {
    }

    Matrix::Matrix(const Matrix& mat)
    : sse1_(mat.sse1_),
      sse2_(mat.sse2_),
      sse3_(mat.sse3_),
      sse4_(mat.sse4_)
    {
    }

    //--------------------------------------------------------------------------

    Matrix Matrix::operator+(const Matrix& mat) const
    {
        __m128 row1 = _mm_add_ps(sse1_, mat.sse1_);
        __m128 row2 = _mm_add_ps(sse2_, mat.sse2_);
        __m128 row3 = _mm_add_ps(sse3_, mat.sse3_);
        __m128 row4 = _mm_add_ps(sse4_, mat.sse4_);

        return Matrix(
            row1.m128_f32[0], row1.m128_f32[1], row1.m128_f32[2], row1.m128_f32[3],
            row2.m128_f32[0], row2.m128_f32[1], row2.m128_f32[2], row2.m128_f32[3],
            row3.m128_f32[0], row3.m128_f32[1], row3.m128_f32[2], row3.m128_f32[3],
            row4.m128_f32[0], row4.m128_f32[1], row4.m128_f32[2], row4.m128_f32[3]);
    }

    Matrix& Matrix::operator+=(const Matrix& mat)
    {
        sse1_ = _mm_add_ps(sse1_, mat.sse1_);
        sse2_ = _mm_add_ps(sse2_, mat.sse2_);
        sse3_ = _mm_add_ps(sse3_, mat.sse3_);
        sse4_ = _mm_add_ps(sse4_, mat.sse4_);

        return *this;
    }

    Matrix Matrix::operator-(const Matrix& mat) const
    {
        __m128 row1 = _mm_sub_ps(sse1_, mat.sse1_);
        __m128 row2 = _mm_sub_ps(sse2_, mat.sse2_);
        __m128 row3 = _mm_sub_ps(sse3_, mat.sse3_);
        __m128 row4 = _mm_sub_ps(sse4_, mat.sse4_);

        return Matrix(
            row1.m128_f32[0], row1.m128_f32[1], row1.m128_f32[2], row1.m128_f32[3],
            row2.m128_f32[0], row2.m128_f32[1], row2.m128_f32[2], row2.m128_f32[3],
            row3.m128_f32[0], row3.m128_f32[1], row3.m128_f32[2], row3.m128_f32[3],
            row4.m128_f32[0], row4.m128_f32[1], row4.m128_f32[2], row4.m128_f32[3]);
    }

    Matrix& Matrix::operator-=(const Matrix& mat)
    {
        sse1_ = _mm_sub_ps(sse1_, mat.sse1_);
        sse2_ = _mm_sub_ps(sse2_, mat.sse2_);
        sse3_ = _mm_sub_ps(sse3_, mat.sse3_);
        sse4_ = _mm_sub_ps(sse4_, mat.sse4_);

        return *this;
    }

    Matrix Matrix::operator*(const Matrix& mat) const
    {
        __m128 row1 = mat.sse1_;
        __m128 row2 = mat.sse2_;
        __m128 row3 = mat.sse3_;
        __m128 row4 = mat.sse4_;

        _MM_TRANSPOSE4_PS(row1, row2, row3, row4);
#define GET_DP(r1, r2) _mm_cvtss_f32(_mm_dp_ps(r1, r2, 0xF1))
        return Matrix( GET_DP(sse1_, row1), GET_DP(sse1_, row2), GET_DP(sse1_, row3), GET_DP(sse1_, row4),
                       GET_DP(sse2_, row1), GET_DP(sse2_, row2), GET_DP(sse2_, row3), GET_DP(sse2_, row4),
                       GET_DP(sse3_, row1), GET_DP(sse3_, row2), GET_DP(sse3_, row3), GET_DP(sse3_, row4),
                       GET_DP(sse4_, row1), GET_DP(sse4_, row2), GET_DP(sse4_, row3), GET_DP(sse4_, row4));
#undef GET_DP
    }
    
    Matrix& Matrix::operator*=(const Matrix& mat)
    {
        __m128 row1 = mat.sse1_;
        __m128 row2 = mat.sse2_;
        __m128 row3 = mat.sse3_;
        __m128 row4 = mat.sse4_;

        _MM_TRANSPOSE4_PS(row1, row2, row3, row4);
#define GET_DP(r1, r2) _mm_cvtss_f32(_mm_dp_ps(r1, r2, 0xF1))
        f32 r1 = GET_DP(sse1_, row1);
        f32 r2 = GET_DP(sse1_, row2);
        f32 r3 = GET_DP(sse1_, row3);
        f32 r4 = GET_DP(sse1_, row4);
        sse1_ = _mm_set_ps(r4, r3, r2, r1);

        r1 = GET_DP(sse2_, row1);
        r2 = GET_DP(sse2_, row2);
        r3 = GET_DP(sse2_, row3);
        r4 = GET_DP(sse2_, row4);
        sse2_ = _mm_set_ps(r4, r3, r2, r1);

        r1 = GET_DP(sse3_, row1);
        r2 = GET_DP(sse3_, row2);
        r3 = GET_DP(sse3_, row3);
        r4 = GET_DP(sse3_, row4);
        sse3_ = _mm_set_ps(r4, r3, r2, r1);

        r1 = GET_DP(sse4_, row1);
        r2 = GET_DP(sse4_, row2);
        r3 = GET_DP(sse4_, row3);
        r4 = GET_DP(sse4_, row4);
        sse4_ = _mm_set_ps(r4, r3, r2, r1);
#undef GET_DP

        return *this;
    }

    Vector2 Matrix::operator*(const Vector2& vec) const
    {
#define GET_DP(r1, r2) _mm_cvtss_f32(_mm_dp_ps(r1, r2, 0xF1))
        return Vector2(GET_DP(sse1_, vec.sse_), GET_DP(sse2_, vec.sse_));
#undef GET_DP
    }

    Matrix& Matrix::operator=(const Matrix& mat)
    {
        sse1_ = mat.sse1_;
        sse2_ = mat.sse2_;
        sse3_ = mat.sse3_;
        sse4_ = mat.sse4_;
        return *this;
    }

    void* Matrix::operator new(size_t x)
    {
        return _aligned_malloc(x, sizeof(Simian::Matrix));
    }

    void* Matrix::operator new[](size_t x)
    {
        return _aligned_malloc(x, sizeof(Simian::Matrix));
    }

    void Matrix::operator delete(void* x)
    {
         if (x)
             _aligned_free(x);
    }

    void Matrix::operator delete[](void* x)
    {
        if (x)
            _aligned_free(x);
    }

    //--------------------------------------------------------------------------

    void Matrix::Transpose()
    {
        _MM_TRANSPOSE4_PS(sse1_, sse2_, sse3_, sse4_);
    }

    Vector2& Matrix::Transform(Vector2& vec) const
    {
#define GET_DP(r1, r2) _mm_cvtss_f32(_mm_dp_ps(r1, r2, 0x71))
        vec = Vector2(GET_DP(sse1_, vec.sse_), GET_DP(sse2_, vec.sse_));
#undef GET_DP
        return vec;
    }
}

#endif
