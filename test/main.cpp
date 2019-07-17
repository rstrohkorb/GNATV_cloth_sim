#include <gtest/gtest.h>
#include <iostream>
#include "MassPoint.h"
#include "Triangle.h"
#include "WoolMaterial.h"
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

TEST(MassPoint,setMass)
{
    MassPoint m;
    m.setMass(4.0f);
    EXPECT_FLOAT_EQ(m.mass(), 4.0f);
}

TEST(Triangle,defaultctor)
{
    Triangle t;
    EXPECT_TRUE(t.v1() == ngl::Vec3(0.0f));
    EXPECT_TRUE(t.v2() == ngl::Vec3(1.0f, 0.0f, 0.0f));
    EXPECT_TRUE(t.v3() == ngl::Vec3(0.0f, 1.0f, 0.0f));
    EXPECT_FLOAT_EQ(t.surface_area(), 0.0f);
    EXPECT_TRUE(t.ru() == ngl::Vec3(1.0f));
    EXPECT_TRUE(t.rv() == ngl::Vec3(1.0f));
}

TEST(Triangle,userctor)
{
    Triangle t(ngl::Vec3(2.0f), ngl::Vec3(0.5f), ngl::Vec3(0.0f, 1.0f, 1.0f));
    EXPECT_TRUE(t.v1() == ngl::Vec3(2.0f));
    EXPECT_TRUE(t.v2() == ngl::Vec3(0.5f));
    EXPECT_TRUE(t.v3() == ngl::Vec3(0.0f, 1.0f, 1.0f));
    EXPECT_FLOAT_EQ(t.surface_area(), 1.0606601);
}

TEST(Triangle,setVertices)
{
    Triangle t;
    t.setVertices(ngl::Vec3(2.0f), ngl::Vec3(0.5f), ngl::Vec3(0.0f, 1.0f, 1.0f));
    EXPECT_TRUE(t.v1() == ngl::Vec3(2.0f));
    EXPECT_TRUE(t.v2() == ngl::Vec3(0.5f));
    EXPECT_TRUE(t.v3() == ngl::Vec3(0.0f, 1.0f, 1.0f));
    EXPECT_FLOAT_EQ(t.surface_area(), 1.0606601);
}

TEST(Triangle,computeR)
{
    Triangle t1(ngl::Vec3(2.0f, 2.0f, 0.0f), ngl::Vec3(0.5f, 0.5f, 0.0f), ngl::Vec3(0.0f, 1.0f, 0.0f));
    Triangle t2(ngl::Vec3(2.0f, 0.0f, 2.0f), ngl::Vec3(0.5f, 0.0f, 0.5f), ngl::Vec3(0.0f, 0.0f, 1.0f));
    Triangle t3(ngl::Vec3(0.0f, 2.0f, 2.0f), ngl::Vec3(0.0f, 0.5f, 0.5f), ngl::Vec3(0.0f, 0.0f, 1.0f));
    t1.computeR(XY);
    t2.computeR(XZ);
    t3.computeR(YZ);
    EXPECT_TRUE(t1.ru() == t2.ru() && t1.ru() == t3.ru());
    EXPECT_TRUE(t1.rv() == t2.rv() && t1.rv() == t3.rv());
    EXPECT_TRUE(t1.ru() == ngl::Vec3(-0.333333f, -0.6666667f, 1.0f));
    EXPECT_TRUE(t1.rv() == ngl::Vec3(-0.333333f, 1.3333333f, -1.0f));
}

TEST(WoolMaterial,defaultctor)
{
    WoolMaterial wm;
    EXPECT_FLOAT_EQ(wm.mass(), 0.15f);
}

TEST(WoolMaterial,strainStressFuncs)
{
    WoolMaterial wm;
    EXPECT_FLOAT_EQ(wm.weft(0.06f), 159.03156f);
    EXPECT_FLOAT_EQ(wm.warp(0.06f), 202199.66f);
    EXPECT_FLOAT_EQ(wm.shear(0.06f), 16.171333f);
    EXPECT_FLOAT_EQ(wm.weftPrime(0.06f), 6573.33f);
    EXPECT_FLOAT_EQ(wm.warpPrime(0.06f), 1.0329829e7f);
    EXPECT_FLOAT_EQ(wm.shearPrime(0.06f), 339.02673f);
}

TEST(Cloth,userctor)
{
    WoolMaterial wm;
    Cloth c(wm);
    EXPECT_TRUE(c.numMasses() == 0);
    EXPECT_TRUE(c.numTriangles() == 0);
    EXPECT_FLOAT_EQ(c.mass(), 0.15f);
}

TEST(Cloth,init)
{
    WoolMaterial wm;
    Cloth c(wm);
    c.init("../gnatvCloth/obj/clothObject.obj", XY);
    EXPECT_TRUE(c.numMasses() == 900);
    EXPECT_TRUE(c.numTriangles() == 1682);
    EXPECT_FLOAT_EQ(c.firstMass(), 0.0029726522);
}
