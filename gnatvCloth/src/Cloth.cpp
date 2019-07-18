#include <iostream>
#include <fstream>
#include <numeric>
#include <boost/algorithm/string.hpp>
#include "Cloth.h"

void Cloth::init(std::string _filename, initOrientation _o)
{
    // read in object data
    readObj(_filename);
    // finish creating triangles, compute r-weights
    for(auto& tr : m_triangles)
    {
        tr.tri.setVertices(m_mspts[tr.a].pos(), m_mspts[tr.b].pos(), m_mspts[tr.c].pos());
        tr.tri.computeR(_o);
    }
    // determine mass of the masspoints
    std::vector<std::vector<float>> massCollect;
    massCollect.resize(m_mspts.size());
    // accumulate masses of triangles connected to each masspoint
    for(auto t : m_triangles)
    {
        auto tmass = t.tri.surface_area() * m_material.mass();
        massCollect[t.a].push_back(tmass);
        massCollect[t.b].push_back(tmass);
        massCollect[t.c].push_back(tmass);
    }
    // for each masspoint, sum over collected values to get mass
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        auto totalMass = std::accumulate(massCollect[i].begin(), massCollect[i].end(), 0.0f);
        m_mspts[i].setMass(totalMass/3);
    }
}

void Cloth::render(std::vector<ngl::Vec3> &o_vertexData)
{
    o_vertexData.reserve(m_triangles.size());
    // spit out current triangle positions
    for(auto t : m_triangles)
    {
        o_vertexData.push_back(t.tri.v1());
        o_vertexData.push_back(t.tri.v2());
        o_vertexData.push_back(t.tri.v3());
    }
}

void Cloth::update(float _h)
{
    // STEP 0 - ZERO OUT CURRENT FORCES/JACOBIANS ON EACH MASSPOINT
    for(auto& m : m_mspts)
    {
        m.resetForce();
        m.resetJacobians();
    }
    // STEP 1 - FORCE CALCULATIONS PER-TRIANGLE
    for(auto tr : m_triangles)
    {
        // adds force and jacobian contributions for each point in the triangle
        forceCalcPerTriangle(tr);
    }
    // STEP 2 - LET'S INTEGRATE
}

void Cloth::readObj(std::string _filename)
{
    // import cloth data from obj
    std::ifstream clothFile;
    std::string line;
    clothFile.open(_filename);
    // read each line
    while(std::getline(clothFile, line))
    {
        switch(line[0])
        {
        case 'v':
        {
            // get vertex data
            std::vector<std::string> res;
            boost::split(res, line, [](char c){return c == ' ';});
            float p0, p1, p2;
            p0 = std::stof(res[1]);
            p1 = std::stof(res[2]);
            p2 = std::stof(res[3]);
            // Create masspoint with that position
            MassPoint m (ngl::Vec3(p0, p1, p2));
            m_mspts.push_back(m);
        }
        break;
        case 'f':
        {
            // get triangle index data
            Triref tr;
            std::vector<std::string> res;
            boost::split(res, line, [](char c){return c == ' ';});
            tr.a = stoul(res[1]) - 1;  // subtract 1 since obj files index faces at 1
            tr.b = stoul(res[2]) - 1;
            tr.c = stoul(res[3]) - 1;
            m_triangles.push_back(tr);
        }
        break;
        default: break;
        }
    }
    clothFile.close();
}

void Cloth::forceCalcPerTriangle(Triref tr)
{
    // 1.1 - CALC U AND V
    ngl::Vec3 ru, rv, U, V;
    ru = tr.tri.ru();
    rv = tr.tri.rv();
    U = (ru.m_x * tr.tri.v1()) + (ru.m_y * tr.tri.v2()) + (ru.m_z * tr.tri.v3());
    V = (rv.m_x * tr.tri.v1()) + (rv.m_y * tr.tri.v2()) + (rv.m_z * tr.tri.v3());
    // 1.2 - ACQUIRE STRAIN VALUES
    ngl::Vec3 strain;
    strain.m_x = 0.5f * (U.dot(U) - 1);
    strain.m_y = 0.5f * (V.dot(V) - 1);
    strain.m_z = U.dot(V);
    // 1.3 - ACQUIRE STRESS VALUES
    ngl::Vec3 stress;
    stress.m_x = m_material.weft(strain.m_x);
    stress.m_y = m_material.warp(strain.m_y);
    stress.m_z = m_material.shear(strain.m_z);
    // 1.4 - COMPUTE FORCE CONTRIBUTIONS
    ngl::Vec3 fa, fb, fc;
    auto nd = -1 * tr.tri.surface_area();
    auto forceCont = [nd, stress, U, V] (float rui, float rvi) -> ngl::Vec3
    {
        return nd * ((stress.m_x * (rui * U)) + (stress.m_y * (rvi * V)) + (stress.m_z * ((rui * V) + (rvi * U))));
    };
    fa = forceCont(ru.m_x, rv.m_x);
    fb = forceCont(ru.m_y, rv.m_y);
    fc = forceCont(ru.m_z, rv.m_z);
    // 1.5 - APPLY FORCE CONTRIBUTIONS TO TRIANGLE POINTS
    m_mspts[tr.a].addForce(fa);
    m_mspts[tr.b].addForce(fb);
    m_mspts[tr.c].addForce(fc);
    // 1.6 - COMPUTE JACOBIAN CONTRIBUTIONS
    ngl::Mat3 Jaa, Jab, Jac, Jbb, Jbc, Jcc, UUt, VVt, UVt, VUt;
    ngl::Vec3 stressPrime;
    stressPrime.m_x = m_material.weftPrime(strain.m_x);
    stressPrime.m_y = m_material.warpPrime(strain.m_y);
    stressPrime.m_z = m_material.shearPrime(strain.m_z);
    UUt = vecVecTranspose(U, U);
    VVt = vecVecTranspose(V, V);
    UVt = vecVecTranspose(U, V);
    VUt = vecVecTranspose(V, U);
    auto jacobianCont = [nd, stressPrime, stress, UUt, VVt, UVt, VUt] (float rui, float ruj, float rvi, float rvj) -> ngl::Mat3
    {
        ngl::Mat3 t1, t2, t3, t4;
        t1 = UUt * (stressPrime.m_x * ruj * rui);
        t2 = VVt * (stressPrime.m_y * rvj * rvi);
        t3 = ((UVt * (ruj * rvi)) + (VUt * (rvj * rui))) * stressPrime.m_z;
        t4 = ngl::Mat3((stress.m_x * ruj * rui) + (stress.m_y * rvj * rvi) + (stress.m_z * ((ruj * rvi) + (rvj * rui))));
        return (t1 + t2 + t3 + t4) * nd;
    };
    Jaa = jacobianCont(ru.m_x, ru.m_x, rv.m_x, rv.m_x);
    Jab = jacobianCont(ru.m_x, ru.m_y, rv.m_x, rv.m_y);
    Jac = jacobianCont(ru.m_x, ru.m_z, rv.m_x, rv.m_z);
    Jbb = jacobianCont(ru.m_y, ru.m_y, rv.m_y, rv.m_y);
    Jbc = jacobianCont(ru.m_y, ru.m_z, rv.m_y, rv.m_z);
    Jcc = jacobianCont(ru.m_z, ru.m_z, rv.m_z, rv.m_z);
    // 1.7 - ADD JACOBIAN CONTRIBUTIONS TO TRIANGLE POINTS
    m_mspts[tr.a].addJacobian(tr.a, Jaa);
    m_mspts[tr.a].addJacobian(tr.b, Jab);
    m_mspts[tr.a].addJacobian(tr.c, Jac);
    m_mspts[tr.b].addJacobian(tr.a, Jab);
    m_mspts[tr.b].addJacobian(tr.b, Jbb);
    m_mspts[tr.b].addJacobian(tr.c, Jbc);
    m_mspts[tr.c].addJacobian(tr.a, Jac);
    m_mspts[tr.c].addJacobian(tr.b, Jbc);
    m_mspts[tr.c].addJacobian(tr.c, Jcc);
}

ngl::Mat3 Cloth::vecVecTranspose(ngl::Vec3 _a, ngl::Vec3 _b)
{
    ngl::Mat3 ret;
    ret.m_00 = _a.m_x * _b.m_x;
    ret.m_01 = _a.m_x * _b.m_y;
    ret.m_02 = _a.m_x * _b.m_z;
    ret.m_10 = _a.m_y * _b.m_x;
    ret.m_11 = _a.m_y * _b.m_y;
    ret.m_12 = _a.m_y * _b.m_z;
    ret.m_20 = _a.m_z * _b.m_x;
    ret.m_21 = _a.m_z * _b.m_y;
    ret.m_22 = _a.m_z * _b.m_z;
    return ret;
}
