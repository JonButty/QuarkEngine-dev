#ifndef VECTORTEST_H
#define VECTORTEST_H

#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

TEST(MathEngineTest, Vec2Add) 
{
    Math::Vec2I a(Math::Vec2I::UnitX);
    Math::Vec2I b(Math::Vec2I::UnitY);
    Math::Vec2I result(a + b);
    EXPECT_EQ(result.X(),1);
    EXPECT_EQ(result.Y(),1);
}

TEST(MathEngineTest, Vec2Sub) 
{
    Math::Vec2I a(1,1);
    Math::Vec2I result(a - Math::Vec2I::UnitX - Math::Vec2I::UnitY);
    EXPECT_EQ(result.X(),0);
    EXPECT_EQ(result.Y(),0);
}

TEST(MathEngineTest, Vec2Mul) 
{
    int val = 3;
    Math::Vec2I a(Math::Vec2I::UnitX);
    Math::Vec2I result(a * 3);
    EXPECT_EQ(result.X(),val);
    EXPECT_EQ(result.Y(),0);
}

TEST(MathEngineTest, Vec2Div) 
{
    int val = 3;
    Math::Vec2I a(Math::Vec2I::UnitX * val);
    Math::Vec2I result(a / val);
    EXPECT_EQ(result.X(),1);
    EXPECT_EQ(result.Y(),0);
}

TEST(MathEngineTest, Vec2ProjectOnto) 
{
    float val = 3;
    Math::Vec2F a(val,val);
    a = a.ProjectOnto(Math::Vec2F::UnitX);

    EXPECT_EQ(a.X(),val);
    EXPECT_EQ(a.Y(),0);
}

TEST(MathEngineTest, Vec2PReflectAbout) 
{
    float val = 3;
    Math::Vec2F a(val,val);
    a = a.ReflectAbout(Math::Vec2F::UnitY);

    EXPECT_EQ(a.X(),-val);
    EXPECT_EQ(a.Y(),val);

    val = 3;
    a.X(val);
    a.Y(val);
    a = a.ReflectAbout(Math::Vec2F::UnitX);

    EXPECT_EQ(a.X(),val);
    EXPECT_EQ(a.Y(),-val);
}
#endif