#include <gtest/gtest.h>
#include <iostream>
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
    EXPECT_TRUE(m.numJacobians() == 0);
    EXPECT_TRUE(m.nullJacobians());
}

TEST(MassPoint,userctor)
{
    MassPoint m1(ngl::Vec3(1.0f), 0, 0.4f, true);
    MassPoint m2(ngl::Vec3(1.0f), 0, 0.4f);
    EXPECT_TRUE(m1.pos() == ngl::Vec3(1.0f));
    EXPECT_FLOAT_EQ(m1.mass(), 0.4f);
    EXPECT_TRUE(m1.fixed());
    EXPECT_TRUE(m2.pos() == ngl::Vec3(1.0f));
    EXPECT_FLOAT_EQ(m2.mass(), 0.4f);
    EXPECT_FALSE(m2.fixed());
}

TEST(MassPoint,setVariables)
{
    MassPoint m;
    m.setMass(4.0f);
    m.setPos(ngl::Vec3(2.2f));
    m.setVel(ngl::Vec3(3.0f));
    EXPECT_FLOAT_EQ(m.mass(), 4.0f);
    EXPECT_TRUE(m.pos() == ngl::Vec3(2.2f));
    EXPECT_TRUE(m.vel() == ngl::Vec3(3.0f));
    m.setFixed(true);
    EXPECT_TRUE(m.fixed());
    EXPECT_TRUE(m.vel() == ngl::Vec3(0.0f));
    m.setVel(ngl::Vec3(3.0f));
    EXPECT_TRUE(m.vel() == ngl::Vec3(0.0f));
}

TEST(MassPoint,forceModifiers)
{
    MassPoint m;
    m.addForce(ngl::Vec3(1.0f));
    EXPECT_TRUE(m.forces() == ngl::Vec3(1.0f));
    m.addForce(ngl::Vec3(1.0f));
    EXPECT_TRUE(m.forces() == ngl::Vec3(2.0f));
    m.resetForce();
    EXPECT_TRUE(m.forces() == ngl::Vec3(0.0f));
    m.addForce(ngl::Vec3(3.0f));
    m.setFixed(true);
    EXPECT_TRUE(m.forces() == ngl::Vec3(0.0f));
    m.addForce(ngl::Vec3(3.0f));
    EXPECT_TRUE(m.forces() == ngl::Vec3(0.0f));
}

TEST(MassPoint,jacobianModifiers)
{
    MassPoint m;
    m.addJacobian(2, ngl::Mat3(1.0f));
    EXPECT_FALSE(m.nullJacobians());
    EXPECT_TRUE(m.numJacobians() == 1);
    EXPECT_TRUE(m.fetchJacobian(2) == ngl::Mat3(1.0f));
    EXPECT_TRUE(m.getJacobianDiag() == ngl::Vec3(0.0f));
    m.multJacobians(0.5f);
    EXPECT_TRUE(m.fetchJacobian(2) == ngl::Mat3(0.5f));
    EXPECT_TRUE(m.getJacobianDiag() == ngl::Vec3(0.0f));
    std::vector<size_t> keytest;
    keytest.push_back(2);
    EXPECT_TRUE(m.jacobainKeys() == keytest);
    m.resetJacobians();
    EXPECT_TRUE(m.numJacobians() == 1);
    EXPECT_TRUE(m.nullJacobians());
    m.addJacobian(0, ngl::Mat3(1.0f));
    EXPECT_TRUE(m.getJacobianDiag() == ngl::Vec3(1.0f));
}

TEST(MassPoint,jacobianVectorMult)
{
    MassPoint m(ngl::Vec3(0.0f), 0, 0.5f);
    m.addJacobian(0, ngl::Mat3(1.0f));
    m.addJacobian(2, ngl::Mat3(1.0f));
    std::unordered_map<size_t, ngl::Vec3> testmap;
    testmap[0] = ngl::Vec3(2.0f);
    testmap[2] = ngl::Vec3(3.0f);
    auto resA = m.jacobianVectorMult(true, testmap);
    auto resNotA = m.jacobianVectorMult(false, testmap);
    EXPECT_TRUE(resA == ngl::Vec3(-4.0f));
    EXPECT_TRUE(resNotA == ngl::Vec3(5.0f));
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
    EXPECT_TRUE(t.v1UV() == ngl::Vec2(0.0f, 0.0f));
    EXPECT_TRUE(t.v2UV() == ngl::Vec2(0.0f, 1.0f));
    EXPECT_TRUE(t.v3UV() == ngl::Vec2(1.0f, 0.0f));
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
    t.setUV1(ngl::Vec2(0.5f, 0.5f));
    t.setUV2(ngl::Vec2(0.2f, 0.2f));
    t.setUV3(ngl::Vec2(0.2f, 0.5f));
    EXPECT_TRUE(t.v1UV() == ngl::Vec2(0.5f, 0.5f));
    EXPECT_TRUE(t.v2UV() == ngl::Vec2(0.2f, 0.2f));
    EXPECT_TRUE(t.v3UV() == ngl::Vec2(0.2f, 0.5f));
}

TEST(Triangle,computeR)
{
    auto toParam = [](ngl::Vec3 _v) -> ngl::Vec2
    {
        ngl::Vec2 n;
        n.m_x = _v.m_x;
        n.m_y = _v.m_z;
        return n;
    };
    Triangle t;
    t.setVertices(ngl::Vec3(2.0f), ngl::Vec3(0.5f), ngl::Vec3(0.0f, 1.0f, 1.0f));
    t.computeR(toParam);
    EXPECT_TRUE(t.ru() == ngl::Vec3(-0.235702f, -0.471405f, 0.707107f));
    EXPECT_TRUE(t.rv() == ngl::Vec3(-0.235702f, 0.942809f, -0.707107f));
}

TEST(Cloth,userctor)
{
    Cloth c(WOOL);
    EXPECT_TRUE(c.numMasses() == 0);
    EXPECT_TRUE(c.numTriangles() == 0);
    EXPECT_FLOAT_EQ(c.mass(), 0.15f);
}

TEST(Cloth,init)
{
    std::vector<size_t> corners = {0, 1, 2, 3};
    auto toParam = [](ngl::Vec3 _v) -> ngl::Vec2
    {
        ngl::Vec2 n;
        n.m_x = _v.m_x;
        n.m_y = _v.m_z;
        return n;
    };
    Cloth c(WOOL);
    c.init("../gnatvCloth/obj/clothXZUV.obj", toParam, corners);
    EXPECT_TRUE(c.numMasses() == 289);
    EXPECT_TRUE(c.numTriangles() == 512);
    EXPECT_FLOAT_EQ(c.firstMass(), 0.017215004f);
    EXPECT_TRUE(corners == c.corners());
}

TEST(Cloth,fixCorners)
{
    std::vector<size_t> corners = {0, 1, 2, 3};
    auto toParam = [](ngl::Vec3 _v) -> ngl::Vec2
    {
        ngl::Vec2 n;
        n.m_x = _v.m_x;
        n.m_y = _v.m_z;
        return n;
    };
    Cloth c(WOOL);
    c.init("../gnatvCloth/obj/clothXZUV.obj", toParam, corners);
    std::vector<bool> c1, c2, c3, c4;
    c1 = {false, false, true, true};
    c2 = {true, false, true, false};
    c3 = {true, true, true, true};
    c4 = {false, false, false, false};

    c.fixCorners(c1);
    EXPECT_TRUE(c.isCornerFixed() == c1);
    c.fixCorners(c2);
    EXPECT_TRUE(c.isCornerFixed() == c2);
    c.fixCorners(c3);
    EXPECT_TRUE(c.isCornerFixed() == c3);
    c.fixCorners(c4);
    EXPECT_TRUE(c.isCornerFixed() == c4);
}
