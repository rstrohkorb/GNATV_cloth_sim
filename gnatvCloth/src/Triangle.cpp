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

void Triangle::computeR(initOrientation _o)
{
    float ua, ub, uc, va, vb, vc;
    //  handle orientation
    switch(_o)
    {
    case XY:
    {
        ua = m_a.m_x;
        va = m_a.m_y;
        ub = m_b.m_x;
        vb = m_b.m_y;
        uc = m_c.m_x;
        vc = m_c.m_y;
    } break;
    case XZ:
    {
        ua = m_a.m_x;
        va = m_a.m_z;
        ub = m_b.m_x;
        vb = m_b.m_z;
        uc = m_c.m_x;
        vc = m_c.m_z;
    } break;
    case YZ:
    {
        ua = m_a.m_y;
        va = m_a.m_z;
        ub = m_b.m_y;
        vb = m_b.m_z;
        uc = m_c.m_y;
        vc = m_c.m_z;
    } break;
    }
    // calculate r-weights
    auto invd = 1/(m_sa * 2);
    m_ru.m_x = invd * (vb - vc);
    m_ru.m_y = invd * (vc - va);
    m_ru.m_z = invd * (va - vb);
    m_rv.m_x = invd * (uc - ub);
    m_rv.m_y = invd * (ua - uc);
    m_rv.m_z = invd * (ub - ua);
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
