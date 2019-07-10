#include <gtest/gtest.h>
#include "MassPoint.h"
#include "Triangle.h"
#include "Cloth.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(MassPoint,defaultctor)
{
    MassPoint m;
    EXPECT_TRUE(m.pos() == ngl::Vec3(0.0f));
    EXPECT_TRUE(m.vel() == ngl::Vec3(0.0f));
    EXPECT_TRUE(m.forces() == ngl::Vec3(0.0f));
    EXPECT_FLOAT_EQ(m.mass(), 1.0f);
    EXPECT_FALSE(m.fixed());
}

TEST(MassPoint,userctor)
{
    MassPoint m1(ngl::Vec3(1.0f), 0.4f, true);
    MassPoint m2(ngl::Vec3(1.0f), 0.4f);
    EXPECT_TRUE(m1.pos() == ngl::Vec3(1.0f));
    EXPECT_FLOAT_EQ(m1.mass(), 0.4f);
    EXPECT_TRUE(m1.fixed());
    EXPECT_TRUE(m2.pos() == ngl::Vec3(1.0f));
    EXPECT_FLOAT_EQ(m2.mass(), 0.4f);
    EXPECT_FALSE(m2.fixed());
}

TEST(Triangle,defaultctor)
{
    Triangle t;
    EXPECT_TRUE(t.v1() == ngl::Vec3(0.0f));
    EXPECT_TRUE(t.v2() == ngl::Vec3(1.0f, 0.0f, 0.0f));
    EXPECT_TRUE(t.v3() == ngl::Vec3(0.0f, 1.0f, 0.0f));
    EXPECT_TRUE(t.ru() == ngl::Vec3(1.0f));
    EXPECT_TRUE(t.rv() == ngl::Vec3(1.0f));
}

TEST(Triangle,userctor)
{
    Triangle t(ngl::Vec3(2.0f), ngl::Vec3(0.5f), ngl::Vec3(1.0f));
    EXPECT_TRUE(t.v1() == ngl::Vec3(2.0f));
    EXPECT_TRUE(t.v2() == ngl::Vec3(0.5f));
    EXPECT_TRUE(t.v3() == ngl::Vec3(1.0f));
}

TEST(Triangle,setVertices)
{
    Triangle t;
    t.setVertices(ngl::Vec3(2.0f), ngl::Vec3(0.5f), ngl::Vec3(1.0f));
    EXPECT_TRUE(t.v1() == ngl::Vec3(2.0f));
    EXPECT_TRUE(t.v2() == ngl::Vec3(0.5f));
    EXPECT_TRUE(t.v3() == ngl::Vec3(1.0f));
}

TEST(Cloth,defaultctor)
{
    Cloth c;
    EXPECT_TRUE(c.numMasses() == 0);
    EXPECT_TRUE(c.numTriangles() == 0);
}
