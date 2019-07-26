#include <iostream>
#include <fstream>
#include <numeric>
#include <unordered_map>
#include <boost/algorithm/string.hpp>
#include "Materials.h"
#include "Cloth.h"

Cloth::Cloth(material_type _mt)
{
    // set mass and equation values for material
    m_material = _mt;
    switch(_mt)
    {
    case WOOL:
    {
        m_mass = wool_mass;
        m_weft = boost::math::cubic_b_spline<float>(wool_weftData.begin(), wool_weftData.end(),
                                                    wool_weftStart, wool_weftStep, 0.0f);
        m_warp = boost::math::cubic_b_spline<float>(wool_warpData.begin(), wool_warpData.end(),
                                                    wool_warpStart, wool_warpStep, 0.0f);
        m_shear = boost::math::cubic_b_spline<float>(wool_shearData.begin(), wool_shearData.end(),
                                                     wool_shearStart, wool_shearStep, wool_shearSlope, wool_shearSlope);
    }
    break;
    }
}

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
        auto tmass = t.tri.surface_area() * m_mass;
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

void Cloth::update(float _h, ngl::Vec3 _externalf)
{
    // STEP 0 - ZERO OUT CURRENT FORCES/JACOBIANS ON EACH MASSPOINT
    for(auto& m : m_mspts)
    {
        m.resetForce();
        m.resetJacobians();
    }
    // STEP 1 - INTERNAL FORCE CALCULATIONS PER-TRIANGLE
    for(auto tr : m_triangles)
    {
        forceCalcPerTriangle(tr);
    }
    // STEP 2 - ADD EXTERNAL FORCES
    ngl::Vec3 fgravity, airRes;
    fgravity = ngl::Vec3(0.0f, -9.8f, 0.0f);
    for(auto &m : m_mspts)
    {
        airRes = m.vel();
        if(airRes != ngl::Vec3(0.0f))
        {
            airRes.normalize();
            airRes *= -1.0f * m.vel().lengthSquared();
        }
        m.addForce((fgravity * m.mass()) + airRes + _externalf);
    }
    // STEP 3 - LET'S INTEGRATE
    auto deltaVel = conjugateGradient(_h);
    // STEP 4 - UPDATE PARTICLE VELOCITIES AND POSITIONS
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        auto newVel = m_mspts[i].vel() + deltaVel[i];
        auto newPos = m_mspts[i].pos() + (_h * newVel);
        m_mspts[i].setVel(newVel);
        m_mspts[i].setPos(newPos);
    }
    // STEP 5 - CLEANUP AND STATE HANDLING
    for(auto& tr : m_triangles)
    {
        tr.tri.setVertices(m_mspts[tr.a].pos(), m_mspts[tr.b].pos(), m_mspts[tr.c].pos());
    }
}

void Cloth::readObj(std::string _filename)
{
    // import cloth data from obj
    std::ifstream clothFile;
    std::string line;
    clothFile.open(_filename);
    // read each line
    size_t mass_counter = 0;
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
            // Create masspoint with that position, store self id
            MassPoint m (ngl::Vec3(p0, p1, p2), mass_counter);
            m_mspts.push_back(m);
            ++mass_counter;
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
    //U.normalize();
    //V.normalize();
    // 1.2 - ACQUIRE STRAIN VALUES
    ngl::Vec3 strain;
    strain.m_x = 0.5f * (U.dot(U) - 1);
    strain.m_y = 0.5f * (V.dot(V) - 1);
    strain.m_z = U.dot(V);
    // handle near-zero values
    strain = cleanNearZero(strain);
    // 1.3 - ACQUIRE STRESS VALUES
    ngl::Vec3 stress;
    stress.m_x = m_weft(strain.m_x);
    stress.m_y = m_warp(strain.m_y);
    stress.m_z = m_shear(strain.m_z);
    // enforce stress uu and vv > 0, and handle near-zero values
    stress = cleanNearZero(stress);
    if(stress.m_x < 0.0f)
    {
        stress.m_x = 0.0f;
    }
    if(stress.m_y < 0.0f)
    {
        stress.m_y = 0.0f;
    }
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
    ngl::Mat3 Jaa, Jab, Jac, Jba, Jbb, Jbc, Jca, Jcb, Jcc, UUt, VVt, UVt, VUt;
    ngl::Vec3 stressPrime;
    stressPrime.m_x = m_weft.prime(strain.m_x);
    stressPrime.m_y = m_warp.prime(strain.m_y);
    stressPrime.m_z = m_shear.prime(strain.m_z);
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
    // Jji
    Jaa = jacobianCont(ru.m_x, ru.m_x, rv.m_x, rv.m_x);
    Jab = jacobianCont(ru.m_y, ru.m_x, rv.m_y, rv.m_x);
    Jac = jacobianCont(ru.m_z, ru.m_x, rv.m_z, rv.m_x);
    Jba = jacobianCont(ru.m_x, ru.m_y, rv.m_x, rv.m_y);
    Jbb = jacobianCont(ru.m_y, ru.m_y, rv.m_y, rv.m_y);
    Jbc = jacobianCont(ru.m_z, ru.m_y, rv.m_z, rv.m_y);
    Jca = jacobianCont(ru.m_x, ru.m_z, rv.m_x, rv.m_z);
    Jcb = jacobianCont(ru.m_y, ru.m_z, rv.m_y, rv.m_z);
    Jcc = jacobianCont(ru.m_z, ru.m_z, rv.m_z, rv.m_z);
    // 1.7 - ADD JACOBIAN CONTRIBUTIONS TO TRIANGLE POINTS
    m_mspts[tr.a].addJacobian(tr.a, Jaa);
    m_mspts[tr.a].addJacobian(tr.b, Jab);
    m_mspts[tr.a].addJacobian(tr.c, Jac);
    m_mspts[tr.b].addJacobian(tr.a, Jba);
    m_mspts[tr.b].addJacobian(tr.b, Jbb);
    m_mspts[tr.b].addJacobian(tr.c, Jbc);
    m_mspts[tr.c].addJacobian(tr.a, Jca);
    m_mspts[tr.c].addJacobian(tr.b, Jcb);
    m_mspts[tr.c].addJacobian(tr.c, Jcc);
}

std::vector<ngl::Vec3> Cloth::conjugateGradient(float _h)
{
    // 3.1 - PREMULTIPLY EVERY J-MATRIX BY H^2
    for(auto &m : m_mspts)
    {
        m.multJacobians(_h * _h);
    }
    // 3.2 - SET INITIAL VALUES
    std::vector<ngl::Vec3> r, p, vel, hforce, x, Ap;
    r.resize(m_mspts.size());
    vel.reserve(m_mspts.size());
    hforce.reserve(m_mspts.size());
    x.resize(m_mspts.size());
    Ap.reserve(m_mspts.size());
    // set velocity and force vectors
    for(auto m : m_mspts)
    {
        vel.push_back(m.vel());
        hforce.push_back(m.forces() * _h);
    }
    // set initial r = b = hf + h^2Jv
    auto jvt = jMatrixMultOp(false, vel);
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        r[i] = hforce[i] + jvt[i];
    }
    // other loop variables
    p = r;
    //ngl::Mat3 alpha, rsold, rsnew;
    float alpha, rsold, rsnew;
    rsold = vecVecDotOp(r, r);
    float epsilon = 1e-5f;
    size_t k = 0;
    // 3.3 - CONJUGATE GRADIENT METHOD LOOP
//    do
//    {
//        Ap = jMatrixMultOp(true, p);
//        alpha = divMat3(rsold, vecVecDotOp(p, Ap));
//        for(size_t i = 0; i < m_mspts.size(); ++i)
//        {
//            x[i] += alpha * p[i];
//            r[i] = r[i] - (alpha * Ap[i]);
//        }
//        rsnew = vecVecDotOp(r, r);
//        for(size_t i = 0; i < m_mspts.size(); ++i)
//        {
//            p[i] = r[i] + (divMat3(rsnew, rsold) * p[i]);
//        }
//        ++k;
//        rsold = rsnew;
//    } while(gtMat3(rsnew, rsold * (epsilon * epsilon)));

    //while(rsold > epsilon)
    while( k == 0)
    {
        Ap = jMatrixMultOp(true, p);
        auto testval = vecVecDotOp(p, Ap);
        alpha = rsold / vecVecDotOp(p, Ap);
        for(size_t i = 0; i < m_mspts.size(); ++i)
        {
            x[i] += alpha * p[i];
            r[i] = r[i] - (alpha * Ap[i]);
        }
        rsnew = vecVecDotOp(r, r);
        for(size_t i = 0; i < m_mspts.size(); ++i)
        {
            p[i] = r[i] + ((rsnew/rsold) * p[i]);
        }
        ++k;
        rsold = rsnew;
    }
    return x;
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

std::vector<ngl::Vec3> Cloth::jMatrixMultOp(const bool _isA, std::vector<ngl::Vec3> _vec)
{
    size_t numtrue = 0;
    size_t numfalse = 0;
    std::vector<ngl::Vec3> nvec;
    nvec.resize(_vec.size());
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        // grab the values of _vec needed for multiplying with the J-matrices of this mass
        std::unordered_map<size_t, ngl::Vec3> vecPass;
        auto keys = m_mspts[i].jacobainKeys();
        for(auto k : keys)
        {
            vecPass[k] = _vec[k];
            // test for global symmetry with the j-matrices
            if(!_isA)
            {
                if(m_mspts[i].fetchJacobian(k) == m_mspts[k].fetchJacobian(i))
                {
                    ++numtrue;
                }
                else
                {
                    ++numfalse;
                    std::cout<<"Discontinuity between "<<i<<" and "<<k<<'\n';
                }
            }
        }
        // pass them into the masspoint for the vector multiplication
        nvec[i] = m_mspts[i].jacobianVectorMult(_isA, vecPass);
    }
    std::cout<<"Total number of equal J-matrices: "<<numtrue<<'\n';
    std::cout<<"Total number of non-equal J-matrices: "<<numfalse<<'\n';
    return nvec;
}

float Cloth::vecVecDotOp(std::vector<ngl::Vec3> _a, std::vector<ngl::Vec3> _b)
{
//    ngl::Mat3 result = ngl::Mat3(0.0f);
//    for(size_t i = 0; i < _a.size(); ++i)
//    {
//        result += vecVecTranspose(_a[i], _b[i]);
//    }
//    return result;
    float result = 0.0f;
    for(size_t i = 0; i < _a.size(); ++i)
    {
        result += _a[i].dot(_b[i]);
    }
    return result;
}

ngl::Mat3 Cloth::divMat3(ngl::Mat3 _a, ngl::Mat3 _b)
{
    ngl::Mat3 res;
    if(FCompare(_b.m_00, 0.0f))
    {
        res.m_00 = 0.0f;
    }
    else
    {
        res.m_00 = _a.m_00 / _b.m_00;
    }
    if(FCompare(_b.m_01, 0.0f))
    {
        res.m_01 = 0.0f;
    }
    else
    {
        res.m_01 = _a.m_01 / _b.m_01;
    }
    if(FCompare(_b.m_02, 0.0f))
    {
        res.m_02 = 0.0f;
    }
    else
    {
        res.m_02 = _a.m_02 / _b.m_02;
    }
    if(FCompare(_b.m_10, 0.0f))
    {
        res.m_10 = 0.0f;
    }
    else
    {
        res.m_10 = _a.m_10 / _b.m_10;
    }
    if(FCompare(_b.m_11, 0.0f))
    {
        res.m_11 = 0.0f;
    }
    else
    {
        res.m_11 = _a.m_11 / _b.m_11;
    }
    if(FCompare(_b.m_12, 0.0f))
    {
        res.m_12 = 0.0f;
    }
    else
    {
        res.m_12 = _a.m_12 / _b.m_12;
    }
    if(FCompare(_b.m_20, 0.0f))
    {
        res.m_20 = 0.0f;
    }
    else
    {
        res.m_20 = _a.m_20 / _b.m_20;
    }
    if(FCompare(_b.m_21, 0.0f))
    {
        res.m_21 = 0.0f;
    }
    else
    {
        res.m_21 = _a.m_21 / _b.m_21;
    }
    if(FCompare(_b.m_22, 0.0f))
    {
        res.m_22 = 0.0f;
    }
    else
    {
        res.m_22 = _a.m_22 / _b.m_22;
    }
    return res;
}

bool Cloth::gtMat3(ngl::Mat3 _a, ngl::Mat3 _b)
{
    bool b0, b1, b2, b3, b4, b5, b6, b7, b8;
    b0 = _a.m_00 > _b.m_00;
    b1 = _a.m_01 > _b.m_01;
    b2 = _a.m_02 > _b.m_02;
    b3 = _a.m_10 > _b.m_10;
    b4 = _a.m_11 > _b.m_11;
    b5 = _a.m_12 > _b.m_12;
    b6 = _a.m_20 > _b.m_20;
    b7 = _a.m_21 > _b.m_21;
    b8 = _a.m_22 > _b.m_22;
    return b0 && b1 && b2 && b3 && b4 && b5 && b6 && b7 && b8;
}

ngl::Vec3 Cloth::cleanNearZero(ngl::Vec3 io_a)
{
    if(FCompare(io_a.m_x, 0.0f))
    {
        io_a.m_x = 0.0f;
    }
    if(FCompare(io_a.m_y, 0.0f))
    {
        io_a.m_y = 0.0f;
    }
    if(FCompare(io_a.m_z, 0.0f))
    {
        io_a.m_z = 0.0f;
    }
    return io_a;
}
