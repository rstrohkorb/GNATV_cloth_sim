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
                                                     0.0f, wool_shearStep);
        m_shearOffset = wool_shearStart;
    }
    break;
    case JUTE:
    {
        m_mass = jute_mass;
        m_weft = boost::math::cubic_b_spline<float>(jute_weftData.begin(), jute_weftData.end(),
                                                    jute_weftStart, jute_weftStep, 0.0f);
        m_warp = boost::math::cubic_b_spline<float>(jute_warpData.begin(), jute_warpData.end(),
                                                    jute_warpStart, jute_warpStep, 0.0f);
        m_shear = boost::math::cubic_b_spline<float>(jute_shearData.begin(), jute_shearData.end(),
                                                     0.0f, jute_shearStep);
        m_shearOffset = jute_shearStart;
    }
    break;
    }
}

void Cloth::init(std::string _filename, std::function<ngl::Vec2(ngl::Vec3)> _toParam,
                 std::vector<size_t> _corners, float _dampingCoefficient)
{
    // read in object data
    readObj(_filename);
    // finish creating triangles, compute r-weights
    for(auto& tr : m_triangles)
    {
        tr.tri.setVertices(m_mspts[tr.a].pos(), m_mspts[tr.b].pos(), m_mspts[tr.c].pos());
        tr.tri.computeR(_toParam);
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
    // for each masspoint, sum over collected values to get mass, set damping
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        auto totalMass = std::accumulate(massCollect[i].begin(), massCollect[i].end(), 0.0f);
        m_mspts[i].setMass(totalMass/3);
        m_mspts[i].setDamping(_dampingCoefficient);
    }
    // assign corners
    m_corners = _corners;
    // initialize the filter matrix for CG method, assuming all unconstrained
    m_filter.resize(m_mspts.size());
}

void Cloth::clear()
{
    m_mspts.clear();
    m_triangles.clear();
    m_corners.clear();
    m_filter.clear();
}

void Cloth::render(std::vector<float> &o_vertexData)
{
    // determine vertex normals
    auto vNorms = calcNormals();

    // lambda for adding the data
    auto listAdd = [&o_vertexData] (ngl::Vec3 vert, ngl::Vec3 norm, ngl::Vec2 uv) -> void
    {
        // position
        o_vertexData.push_back(vert.m_x);
        o_vertexData.push_back(vert.m_y);
        o_vertexData.push_back(vert.m_z);
        // normal
        o_vertexData.push_back(norm.m_x);
        o_vertexData.push_back(norm.m_y);
        o_vertexData.push_back(norm.m_z);
        // uv
        o_vertexData.push_back(uv.m_x);
        o_vertexData.push_back(uv.m_y);
    };

    // spit out the triangle/vertex/uv data
    o_vertexData.reserve(m_triangles.size() * 8);
    for(auto tr : m_triangles)
    {
        listAdd(tr.tri.v1(), vNorms[tr.a], tr.tri.v1UV());
        listAdd(tr.tri.v2(), vNorms[tr.b], tr.tri.v2UV());
        listAdd(tr.tri.v3(), vNorms[tr.c], tr.tri.v3UV());
    }
}

void Cloth::writeToObj(std::string _filename)
{
    // create file
    std::ofstream obj;
    obj.open(_filename);
    obj << "o Cloth\n";
    // write out vertex data
    for(auto m : m_mspts)
    {
        obj << "v ";
        obj << m.pos().m_x << " " << m.pos().m_y << " " << m.pos().m_z <<'\n';
    }
    // collect UV coords
    std::vector<ngl::Vec2> uvs;
    uvs.resize(m_mspts.size());
    for(auto tr : m_triangles)
    {
        uvs[tr.a] = tr.tri.v1UV();
        uvs[tr.b] = tr.tri.v2UV();
        uvs[tr.c] = tr.tri.v3UV();
    }
    // write out UV coords
    for(auto uv : uvs)
    {
        obj << "vt " << uv.m_x << " " << uv.m_y << '\n';
    }
    // collect vertex normals
    auto vNorms = calcNormals();
    // write out vertex normals
    for(auto n : vNorms)
    {
        obj << "vn ";
        obj << n.m_x << " " << n.m_y << " " << n.m_z << '\n';
    }
    // write out triangles
    for(auto tr : m_triangles)
    {
        obj << "f ";
        obj << (tr.a + 1) << "/" << (tr.a + 1) << "/" << (tr.a + 1) << " ";
        obj << (tr.b + 1) << "/" << (tr.b + 1) << "/" << (tr.b + 1) << " ";
        obj << (tr.c + 1) << "/" << (tr.c + 1) << "/" << (tr.c + 1) << '\n';
    }
    // close file
    obj.close();
}

void Cloth::update(float _h, bool _useRK4, bool _gravityOn, std::vector<ngl::Vec3> _externalf)
{
    bool useJvel = false;
    bool useDamping = true;
    // STEP 0 - ZERO OUT CURRENT FORCES/JACOBIANS ON EACH MASSPOINT
    nullForces();
    // STEP 1 - FORCE CALCULATIONS
    forceCalc(_h, _gravityOn, _externalf, true, useJvel);
    // STEP 2 - LET'S INTEGRATE
    if(_useRK4)
    {
        rk4Integrate(_h, _gravityOn, _externalf);
    }
    else
    {
        auto deltaVel = conjugateGradient(_h, useJvel, useDamping);
        // Update particle velocities and positions
        for(size_t i = 0; i < m_mspts.size(); ++i)
        {
            auto newVel = m_mspts[i].vel() + deltaVel[i];
            auto newPos = m_mspts[i].pos() + (_h * newVel);
            m_mspts[i].setVel(newVel);
            m_mspts[i].setPos(newPos);
        }
    }
    // STEP 3 - CLEANUP AND STATE HANDLING
    for(auto& tr : m_triangles)
    {
        tr.tri.setVertices(m_mspts[tr.a].pos(), m_mspts[tr.b].pos(), m_mspts[tr.c].pos());
    }
}

void Cloth::fixCorners(std::vector<bool> _isPtFixed)
{
    for(size_t i = 0; i < _isPtFixed.size(); ++i)
    {
        // set the point as fixed
        m_mspts[m_corners[i]].setFixed(_isPtFixed[i]);
        // update the filter matrix
        if(_isPtFixed[i])
        {
            m_filter[m_corners[i]] = ngl::Mat3(0.0f);
        }
        else
        {
            m_filter[m_corners[i]] = ngl::Mat3(1.0f);
        }
    }
}

std::vector<bool> Cloth::isCornerFixed() const
{
    std::vector<bool> cornerFixed;
    for(auto i : m_corners)
    {
        cornerFixed.push_back(m_mspts[i].fixed());
    }
    return cornerFixed;
}

void Cloth::readObj(std::string _filename)
{
    std::vector<ngl::Vec2> uvs;
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
            // grab data
            std::vector<std::string> res;
            boost::split(res, line, [](char c){return c == ' ';});
            // handle texture coords
            if(res[0] == "vt")
            {
                float p0, p1;
                p0 = std::stof(res[1]);
                p1 = std::stof(res[2]);
                uvs.push_back(ngl::Vec2(p0, p1));
            }
            // handle vertex coordinates
            else if(res[0] != "vn")
            {
                // grab point data
                float p0, p1, p2;
                p0 = std::stof(res[1]);
                p1 = std::stof(res[2]);
                p2 = std::stof(res[3]);
                // Create masspoint with that position, store self id
                MassPoint m (ngl::Vec3(p0, p1, p2), mass_counter);
                m_mspts.push_back(m);
                ++mass_counter;
            }
        }
        break;
        case 'f':
        {
            // get triangle index data
            Triref tr;
            std::vector<std::string> lineres;
            std::vector<std::string> numres1;
            std::vector<std::string> numres2;
            std::vector<std::string> numres3;
            // split the line
            boost::split(lineres, line, [](char c){return c == ' ';});
            // split the first vertex, assign
            boost::split(numres1, lineres[1], [](char c){return c == '/';});
            tr.a = stoul(numres1[0]) - 1;  // subtract 1 since obj files index faces at 1
            tr.tri.setUV1(uvs[stoul(numres1[1]) - 1]);
            // split the second vertex, assign
            boost::split(numres2, lineres[2], [](char c){return c == '/';});
            tr.b = stoul(numres2[0]) - 1;  // subtract 1 since obj files index faces at 1
            tr.tri.setUV2(uvs[stoul(numres2[1]) - 1]);
            // split the third vertex, assign
            boost::split(numres3, lineres[3], [](char c){return c == '/';});
            tr.c = stoul(numres3[0]) - 1;  // subtract 1 since obj files index faces at 1
            tr.tri.setUV3(uvs[stoul(numres3[1]) - 1]);
            // push back triangle
            m_triangles.push_back(tr);
        }
        break;
        default: break;
        }
    }
    clothFile.close();
}

void Cloth::nullForces()
{
    for(auto& m : m_mspts)
    {
        m.resetForce();
        m.resetJacobians();
    }
}

void Cloth::forceCalc(float _h, bool _gravityOn, std::vector<ngl::Vec3> _externalf, bool _calcJacobians, bool _useJvel)
{
    // Internal force calculations per triangle
    for(auto tr : m_triangles)
    {
        forceCalcPerTriangle(tr, _calcJacobians, _useJvel);
    }
    // gravity
    ngl::Vec3 fgravity, airRes;
    if(_gravityOn)
    {
        fgravity = ngl::Vec3(0.0f, -9.8f, 0.0f);
    }
    else
    {
        fgravity  = ngl::Vec3(0.0f);
    }
    // air resistance
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        if(_gravityOn)
        {
            airRes = m_mspts[i].vel();
            if(airRes != ngl::Vec3(0.0f))
            {
                airRes.normalize();
                airRes *= -1.0f * m_mspts[i].vel().lengthSquared();
            }
        }
        else
        {
            airRes = ngl::Vec3(0.0f);
        }
        m_mspts[i].addForce((fgravity * m_mspts[i].mass()) + airRes + _externalf[i]);
    }
}

void Cloth::forceCalcPerTriangle(Triref _tr, bool _calcJacobians, bool _useJvel)
{
    // 1.1 - CALC U AND V
    ngl::Vec3 ru, rv, U, V;
    ru = _tr.tri.ru();
    rv = _tr.tri.rv();
    U = (ru.m_x * m_mspts[_tr.a].pos()) + (ru.m_y * m_mspts[_tr.b].pos()) + (ru.m_z * m_mspts[_tr.c].pos());
    V = (rv.m_x * m_mspts[_tr.a].pos()) + (rv.m_y * m_mspts[_tr.b].pos()) + (rv.m_z * m_mspts[_tr.c].pos());
    //U = cleanNearZero(U);
    //V = cleanNearZero(V);
    // 1.2 - ACQUIRE STRAIN/STRESS VALUES
    auto strain = calcStrain(U, V);
    auto stress = calcStress(strain);
    // 1.3 - COMPUTE FORCE CONTRIBUTIONS
    ngl::Vec3 fa, fb, fc;
    auto nd = -1 * _tr.tri.surface_area();
    auto forceCont = [nd, stress, U, V] (float rui, float rvi) -> ngl::Vec3
    {
        return nd * ((stress.m_x * (rui * U)) + (stress.m_y * (rvi * V)) + (stress.m_z * ((rui * V) + (rvi * U))));
    };
    fa = forceCont(ru.m_x, rv.m_x);
    fb = forceCont(ru.m_y, rv.m_y);
    fc = forceCont(ru.m_z, rv.m_z);
    // 1.4 - APPLY FORCE CONTRIBUTIONS TO TRIANGLE POINTS
    m_mspts[_tr.a].addForce(fa);
    m_mspts[_tr.b].addForce(fb);
    m_mspts[_tr.c].addForce(fc);
    // 1.5 - COMPUTE JACOBIAN CONTRIBUTIONS
    if(_calcJacobians)
    {
        computeJpos(_tr, U, V, strain, stress);
        if(_useJvel)
        {
            computeJvel(_tr, U, V);
        }
    }
}

std::vector<ngl::Vec3> Cloth::conjugateGradient(float _h, bool _useJvel, bool _useDamping)
{
    // 3.1 - SET INITIAL VALUES
    std::vector<ngl::Vec3> r, p, vel, hforce, x, Ap, b, bfp, z;
    std::vector<ngl::Mat3> Pi, PiInv;
    float alpha, rsold, rsnew, rstest, epsilon;

    r.resize(m_mspts.size());
    b.resize(m_mspts.size());
    bfp.reserve(m_mspts.size());
    vel.reserve(m_mspts.size());
    hforce.reserve(m_mspts.size());
    x.resize(m_mspts.size());
    z.reserve(m_mspts.size());
    Ap.reserve(m_mspts.size());
    Pi.reserve(m_mspts.size());
    PiInv.reserve(m_mspts.size());

    // set velocity and force vectors
    for(auto m : m_mspts)
    {
        vel.push_back(m.vel());
        hforce.push_back(m.forces() * _h);
    }

    // set jmatrix stuff (before premultiply, for damping)
    auto jvt = jMatrixMultOp(false, _useJvel, _useDamping, _h, vel);

    // premultiply j-matrices
    for(auto &m : m_mspts)
    {
        m.multJpos(_h * _h);
        if(_useJvel)
        {
            m.multJvel(_h);
        }
    }

    // set the preconditioning matrix Pi (diag = 1/A diag) and its inverse
    Pi = createPrecon(_useJvel, _useDamping, _h);
    for(auto m : Pi)
    {
        auto minv = m;
        minv.m_00 = 1/m.m_00;
        minv.m_11 = 1/m.m_11;
        minv.m_22 = 1/m.m_22;
        PiInv.push_back(minv);
    }
    // determine b = hforce + h^2Jvt
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        b[i] = hforce[i] + jvt[i];
    }
    auto bfilter = b;
    filter(bfilter);
    // determine r and rstest
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        bfp.push_back(Pi[i] * bfilter[i]);
    }
    rstest = vecVecDotOp(bfilter, bfp);
    r = bfilter; // if x not init to 0, r = filter(b - Ax)

    // lambda for multiplying r and PiInv
    auto multPiInv = [PiInv] (std::vector<ngl::Vec3> r) -> std::vector<ngl::Vec3>
    {
        std::vector<ngl::Vec3> PiInvR;
        PiInvR.reserve(PiInv.size());
        for(size_t i = 0; i < PiInv.size(); ++i)
        {
            PiInvR.push_back(PiInv[i] * r[i]);
        }
        return PiInvR;
    };

    // other loop variables
    p = multPiInv(r);
    filter(p);
    rsnew = vecVecDotOp(r, p);
    epsilon = 1e-5f;

    // 3.2 - CONJUGATE GRADIENT METHOD LOOP
    for(size_t k = 0; k < m_mspts.size(); ++k)
    //while(rsnew > (epsilon * rstest))
    {
        Ap = jMatrixMultOp(true, _useJvel, _useDamping, _h, p);
        filter(Ap);
        auto testval = vecVecDotOp(p, Ap);
        alpha = rsnew / vecVecDotOp(p, Ap);
        for(size_t i = 0; i < m_mspts.size(); ++i)
        {
            x[i] += alpha * p[i];
            r[i] = r[i] - (alpha * Ap[i]);
        }
        z = multPiInv(r);
        rsold = rsnew;
        rsnew = vecVecDotOp(r, z);
        for(size_t i = 0; i < m_mspts.size(); ++i)
        {
            p[i] = z[i] + ((rsnew/rsold) * p[i]);
        }
        filter(p);
        if(rsnew < (epsilon * rstest))
        {
            break;
        }
    }
    return x;
}

void Cloth::rk4Integrate(float _h, bool _gravityOn, std::vector<ngl::Vec3> _externalf)
{
    std::vector<ngl::Vec3> initpos, initvel, k1pos, k1vel, k2pos, k2vel, k3pos, k3vel, k4pos, k4vel;
    // 3.1 - DETERMINE INIT/K1 VALUES
    for(auto m : m_mspts)
    {
        initpos.push_back(m.pos());
        initvel.push_back(m.vel());
        k1pos.push_back(m.vel());
        k1vel.push_back(m.forces());
    }
    // 3.1.5 - UPDATE CLOTH STATE TO MATCH K1, RECALC FORCES
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        m_mspts[i].setPos(initpos[i] + (k1pos[i] * _h * 0.5f));
        m_mspts[i].setVel(initvel[i] + (k1vel[i] * _h * 0.5f));
    }
    nullForces();
    forceCalc(_h, _gravityOn, _externalf, false);
    // 3.2 - DETERMINE K2 VALUES
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        k2pos.push_back(initvel[i] + (k1vel[i] * _h * 0.5f));
        k2vel.push_back(m_mspts[i].forces());
    }
    // 3.2.5 - UPDATE CLOTH STATE TO MATCH K2, RECALC FORCES
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        m_mspts[i].setPos(initpos[i] + (k2pos[i] * _h * 0.5f));
        m_mspts[i].setVel(initvel[i] + (k2vel[i] * _h * 0.5f));
    }
    nullForces();
    forceCalc(_h, _gravityOn, _externalf, false);
    // 3.3 - DETERMINE K3 VALUES
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        k3pos.push_back(initvel[i] + (k2vel[i] * _h * 0.5f));
        k3vel.push_back(m_mspts[i].forces());
    }
    // 3.3.5 - UPDATE CLOTH STATE TO MATCH K3, RECALC FORCES
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        m_mspts[i].setPos(initpos[i] + (k3pos[i] * _h));
        m_mspts[i].setVel(initvel[i] + (k3vel[i] * _h));
    }
    nullForces();
    forceCalc(_h, _gravityOn, _externalf, false);
    // 3.4 - DETERMINE K4 VALUES
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        k4pos.push_back(initvel[i] + (k3vel[i] * _h));
        k4vel.push_back(m_mspts[i].forces());
    }
    // 3.5 - COMBINE K TERMS TO FORM DELTA VEL AND DELTA POS, UPDATE VEL AND POS
    ngl::Vec3 deltavel, deltapos;
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        deltavel = (1.0f/6.0f) * (k1vel[i] + (2 * (k2vel[i] + k3vel[i])) + k4vel[i]);
        deltapos = (1.0f/6.0f) * (k1pos[i] + (2 * (k2pos[i] + k3pos[i])) + k4pos[i]);

        m_mspts[i].setPos(initpos[i] + (deltapos * _h));
        m_mspts[i].setVel(initvel[i] + (deltavel * _h));
    }
}

ngl::Vec3 Cloth::calcStrain(ngl::Vec3 _u, ngl::Vec3 _v)
{
    // calc strain
    ngl::Vec3 strain;
    strain.m_x = 0.5f * (_u.dot(_u) - 1);
    strain.m_y = 0.5f * (_v.dot(_v) - 1);
    strain.m_z = _u.dot(_v);
    // enforce strain uu and vv > 0, uv > offset, and handle near-zero values
    //strain = cleanNearZero(strain);
    if(strain.m_x < 0.0f)
    {
        strain.m_x = 0.0f;
    }
    if(strain.m_y < 0.0f)
    {
        strain.m_y = 0.0f;
    }
    if(strain.m_z < m_shearOffset)
    {
        strain.m_z = m_shearOffset;
    }
    return strain;
}

ngl::Vec3 Cloth::calcStrainPrime(ngl::Vec3 _u, ngl::Vec3 _up, ngl::Vec3 _v, ngl::Vec3 _vp)
{
    ngl::Vec3 strainp;
    strainp.m_x = _u.dot(_up);
    strainp.m_y = _v.dot(_vp);
    strainp.m_z = _u.dot(_vp) + _v.dot(_up);
    cleanNearZero(strainp);
    if(strainp.m_x < 0)
    {
        strainp.m_x = 0;
    }
    if(strainp.m_y < 0)
    {
        strainp.m_y = 0;
    }
    if(strainp.m_z < m_shearOffset)
    {
        strainp.m_z = m_shearOffset;
    }
    return strainp;
}

ngl::Vec3 Cloth::calcStress(ngl::Vec3 _strain)
{
    // calc stress
    ngl::Vec3 stress;
    stress.m_x = m_weft(_strain.m_x);
    stress.m_y = m_warp(_strain.m_y);
    stress.m_z = m_shear(_strain.m_z - m_shearOffset);
    // enforce stress uu and vv > 0, and handle near-zero values
    //stress = cleanNearZero(stress);
    if(stress.m_x < 0.0f)
    {
        stress.m_x = 0.0f;
    }
    if(stress.m_y < 0.0f)
    {
        stress.m_y = 0.0f;
    }
    return stress;
}

void Cloth::computeJpos(Triref _tr, ngl::Vec3 _u, ngl::Vec3 _v, ngl::Vec3 _strain, ngl::Vec3 _stress)
{
    // prepare initial values
    ngl::Mat3 Jaa, Jab, Jac, Jba, Jbb, Jbc, Jca, Jcb, Jcc, UUt, VVt, UVt, VUt;
    auto nd = -1 * _tr.tri.surface_area();
    ngl::Vec3 ru, rv, stressPrime;
    ru = _tr.tri.ru();
    rv = _tr.tri.rv();
    UUt = vecVecTranspose(_u, _u);
    VVt = vecVecTranspose(_v, _v);
    UVt = vecVecTranspose(_u, _v);
    VUt = vecVecTranspose(_v, _u);
    // calculate stressprime
    stressPrime.m_x = m_weft.prime(_strain.m_x);
    stressPrime.m_y = m_warp.prime(_strain.m_y);
    stressPrime.m_z = m_shear.prime(_strain.m_z - m_shearOffset);
    cleanNearZero(stressPrime);
    // lambda for jacobian calculations
    auto jposCont = [nd, stressPrime, _stress, UUt, VVt, UVt, VUt] (float rui, float ruj, float rvi, float rvj) -> ngl::Mat3
    {
        ngl::Mat3 t1, t2, t3, t4;
        t1 = UUt * (stressPrime.m_x * ruj * rui);
        t2 = VVt * (stressPrime.m_y * rvj * rvi);
        t3 = ((UVt * (ruj * rvi)) + (VUt * (rvj * rui))) * stressPrime.m_z;
        t4 = ngl::Mat3((_stress.m_x * ruj * rui) + (_stress.m_y * rvj * rvi) + (_stress.m_z * ((ruj * rvi) + (rvj * rui))));
        return (t1 + t2 + t3 + t4) * nd;
    };
    // calculate Jji
    Jaa = jposCont(ru.m_x, ru.m_x, rv.m_x, rv.m_x);
    Jab = jposCont(ru.m_y, ru.m_x, rv.m_y, rv.m_x);
    Jac = jposCont(ru.m_z, ru.m_x, rv.m_z, rv.m_x);
    Jba = jposCont(ru.m_x, ru.m_y, rv.m_x, rv.m_y);
    Jbb = jposCont(ru.m_y, ru.m_y, rv.m_y, rv.m_y);
    Jbc = jposCont(ru.m_z, ru.m_y, rv.m_z, rv.m_y);
    Jca = jposCont(ru.m_x, ru.m_z, rv.m_x, rv.m_z);
    Jcb = jposCont(ru.m_y, ru.m_z, rv.m_y, rv.m_z);
    Jcc = jposCont(ru.m_z, ru.m_z, rv.m_z, rv.m_z);
    // add position jacobian contributions to triangle points
    m_mspts[_tr.a].addJpos(_tr.a, Jaa);
    m_mspts[_tr.a].addJpos(_tr.b, Jab);
    m_mspts[_tr.a].addJpos(_tr.c, Jac);
    m_mspts[_tr.b].addJpos(_tr.a, Jba);
    m_mspts[_tr.b].addJpos(_tr.b, Jbb);
    m_mspts[_tr.b].addJpos(_tr.c, Jbc);
    m_mspts[_tr.c].addJpos(_tr.a, Jca);
    m_mspts[_tr.c].addJpos(_tr.b, Jcb);
    m_mspts[_tr.c].addJpos(_tr.c, Jcc);
}

void Cloth::computeJvel(Triref _tr, ngl::Vec3 _u, ngl::Vec3 _v)
{
    // flag for debug
    if((_tr.a == 0) || (_tr.b == 0) || (_tr.c == 0))
    {
        std::cout<<"flag/n";
    }
    // prepare initial values
    ngl::Mat3 Jaa, Jab, Jac, Jba, Jbb, Jbc, Jca, Jcb, Jcc, UUt, VVt, UVt, VUt;
    ngl::Vec3 ru, rv, Up, Vp, strainp, stressp;
    auto nd = -1 * _tr.tri.surface_area();
    UUt = vecVecTranspose(_u, _u);
    VVt = vecVecTranspose(_v, _v);
    UVt = vecVecTranspose(_u, _v);
    VUt = vecVecTranspose(_v, _u);
    ru = _tr.tri.ru();
    rv = _tr.tri.rv();
    // compute Uprime and Vprime
    Up = (ru.m_x * m_mspts[_tr.a].vel()) + (ru.m_y * m_mspts[_tr.b].vel()) + (ru.m_z * m_mspts[_tr.c].vel());
    Vp = (rv.m_x * m_mspts[_tr.a].vel()) + (rv.m_y * m_mspts[_tr.b].vel()) + (rv.m_z * m_mspts[_tr.c].vel());
    cleanNearZero(Up);
    cleanNearZero(Vp);
    // compute strainprime
    strainp = calcStrainPrime(_u, Up, _v, Vp);
    // compute stressprime
    stressp.m_x = m_weft.prime(strainp.m_x);
    stressp.m_y = m_warp.prime(strainp.m_y);
    stressp.m_z = m_shear(strainp.m_z - m_shearOffset);
    cleanNearZero(stressp);
    // lambda for jacobian calculations
    auto jvelCont = [nd, stressp, UUt, VVt, UVt, VUt] (float rui, float ruj, float rvi, float rvj) -> ngl::Mat3
    {
        ngl::Mat3 t1, t2, t3;
        t1 = UUt * (stressp.m_x * ruj * rui);
        t2 = VVt * (stressp.m_y * rvj * rvi);
        t3 = ((UVt * (ruj * rvi)) + (VUt * (rvj * rui))) * stressp.m_z;
        return (t1 + t2 + t3) * nd;
    };
    // calculate Jji
    Jaa = jvelCont(ru.m_x, ru.m_x, rv.m_x, rv.m_x);
    Jab = jvelCont(ru.m_y, ru.m_x, rv.m_y, rv.m_x);
    Jac = jvelCont(ru.m_z, ru.m_x, rv.m_z, rv.m_x);
    Jba = jvelCont(ru.m_x, ru.m_y, rv.m_x, rv.m_y);
    Jbb = jvelCont(ru.m_y, ru.m_y, rv.m_y, rv.m_y);
    Jbc = jvelCont(ru.m_z, ru.m_y, rv.m_z, rv.m_y);
    Jca = jvelCont(ru.m_x, ru.m_z, rv.m_x, rv.m_z);
    Jcb = jvelCont(ru.m_y, ru.m_z, rv.m_y, rv.m_z);
    Jcc = jvelCont(ru.m_z, ru.m_z, rv.m_z, rv.m_z);
    // add velocity jacobian contributions to triangle points
    m_mspts[_tr.a].addJvel(_tr.a, Jaa);
    m_mspts[_tr.a].addJvel(_tr.b, Jab);
    m_mspts[_tr.a].addJvel(_tr.c, Jac);
    m_mspts[_tr.b].addJvel(_tr.a, Jba);
    m_mspts[_tr.b].addJvel(_tr.b, Jbb);
    m_mspts[_tr.b].addJvel(_tr.c, Jbc);
    m_mspts[_tr.c].addJvel(_tr.a, Jca);
    m_mspts[_tr.c].addJvel(_tr.b, Jcb);
    m_mspts[_tr.c].addJvel(_tr.c, Jcc);
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

std::vector<ngl::Vec3> Cloth::jMatrixMultOp(const bool _isA, const bool _useJvel, const bool _useDamping, float _h, std::vector<ngl::Vec3> _vec)
{
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
        }
        // pass them into the masspoint for the vector multiplication
        nvec[i] = m_mspts[i].jacobianVectorMult(_isA, _useJvel, _useDamping, vecPass, _h);
        // mult final result by h^2 if we're doing the Jvt operation
        if(!_isA)
        {
            nvec[i] *= (_h * _h);
        }
    }
    return nvec;
}

float Cloth::vecVecDotOp(std::vector<ngl::Vec3> _a, std::vector<ngl::Vec3> _b)
{
    float result = 0.0f;
    for(size_t i = 0; i < _a.size(); ++i)
    {
        result += _a[i].dot(_b[i]);
    }
    return result;
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

std::vector<ngl::Vec3> Cloth::calcNormals()
{
    std::vector<ngl::Vec3> vNorms;
    vNorms.resize(m_mspts.size());
    // calculate triangle norms, accumulate for each masspoint
    for(auto tr : m_triangles)
    {
        ngl::Vec3 triNormal, edge1, edge2;
        edge1 = tr.tri.v2() - tr.tri.v1();
        edge2 = tr.tri.v3() - tr.tri.v1();
        triNormal = edge1.cross(edge2);
        if(triNormal != ngl::Vec3(0.0f))
        {
            triNormal.normalize();
        }
        vNorms[tr.a] += triNormal;
        vNorms[tr.b] += triNormal;
        vNorms[tr.c] += triNormal;
    }
    // normalize to create the final normals
    for(auto &vn : vNorms)
    {
        if(vn != ngl::Vec3(0.0f))
        {
            vn.normalize();
        }
    }
    return vNorms;
}

std::vector<ngl::Mat3> Cloth::createPrecon(bool _useJvel, bool _useDamping, float _h)
{
    std::vector<ngl::Mat3> precon;
    precon.resize(m_mspts.size());
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        float prem0, prem1, prem2;
        auto posdiag = m_mspts[i].getJposDiag();
        auto mass = m_mspts[i].mass();
        if(_useJvel)
        {
            auto veldiag = m_mspts[i].getJvelDiag();
            prem0 = mass - veldiag.m_x - posdiag.m_x;
            prem1 = mass - veldiag.m_y - posdiag.m_y;
            prem2 = mass - veldiag.m_z - posdiag.m_z;
        }
        else
        {
            prem0 = mass - posdiag.m_x;
            prem1 = mass - posdiag.m_y;
            prem2 = mass - posdiag.m_z;
        }
        if(_useDamping)
        {
            auto dampingVal = (m_mspts[i].numJacobians() - 1) * _h * m_mspts[i].dampingCoefficient();
            prem0 += dampingVal;
            prem1 += dampingVal;
            prem2 += dampingVal;
        }
        precon[i].m_00 = 1/prem0;
        precon[i].m_11 = 1/prem1;
        precon[i].m_22 = 1/prem2;
    }
    return precon;
}

void Cloth::filter(std::vector<ngl::Vec3> &io_vec)
{
    // apply filter matrix to the input vector
    for(size_t i = 0; i < m_filter.size(); ++i)
    {
        io_vec[i] = m_filter[i] * io_vec[i];
    }
}
