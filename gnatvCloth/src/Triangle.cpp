#include <iostream>
#include "Triangle.h"

Triangle::Triangle(ngl::Vec3 _a, ngl::Vec3 _b, ngl::Vec3 _c) : m_a(_a), m_b(_b), m_c(_c)
{
    // set surface area
    update_sa();
}

void Triangle::setVertices(const ngl::Vec3 _a, const ngl::Vec3 _b, const ngl::Vec3 _c)
{
    m_a = _a;
    m_b = _b;
    m_c = _c;
    update_sa();
}

void Triangle::update_sa()
{
    // calculate current surface area of the triangle by using
    // magnitude of cross product
    ngl::Vec3 p1(m_b.m_x - m_a.m_x, m_b.m_y - m_a.m_y, m_b.m_z - m_a.m_z);
    ngl::Vec3 p2(m_c.m_x - m_a.m_x, m_c.m_y - m_a.m_y, m_c.m_z - m_a.m_z);
    auto crs = p1.cross(p2);
    auto d = crs.length();
    m_sa = d/2;
}
