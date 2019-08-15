#include <string>
#include <vector>
#include <random>
#include <fstream>
#include <iostream>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>
#include "ClothInterface.h"
#include "FixPtTestDefaults.h"

ClothInterface::ClothInterface()
{
    initCloth();
}
ClothInterface::ClothInterface(std::string _objPath)
{
    m_objPath = _objPath;
    initCloth();
}

ClothInterface::ClothInterface(IntegrationMethod _intm, Config _cnfg, FixPtSetup _fxpts,
                               std::string _objPath) :
    m_intm(_intm), m_config(_cnfg), m_fixpt(_fxpts), m_objPath(_objPath)
{
    initCloth();
    fixClothPts();
}

void ClothInterface::initCloth()
{
    // init variables
    std::string filename;
    std::function<ngl::Vec2(ngl::Vec3)> toParam;
    std::vector<size_t> fixpts;
    float damping = 9.0f;
    // decide variables based on config
    switch (m_config)
    {
    case LRXZ:
    {
        filename = m_objPath + "clothLowResXZ.obj";
        toParam = toParamXZ;
        fixpts = lowResXZFixpts;
    } break;
    case LRXY:
    {
        filename = m_objPath + "clothLowResXY.obj";
        toParam = toParamXY;
        fixpts = lowResXYFixpts;
    } break;
    case HRXZ:
    {
        filename = m_objPath + "clothHiResXZ.obj";
        toParam = toParamXZ;
        fixpts = hiResFixpts;
    } break;
    case HRXY:
    {
        filename = m_objPath + "clothHiResXY.obj";
        toParam = toParamXY;
        fixpts = hiResFixpts;
    } break;
    case WEFTXZ:
    {
        filename = m_objPath + "weftTestCloth.obj";
        toParam = toParamXZ;
        fixpts = wwsTestFixpts;
    } break;
    case WARPXZ:
    {
        filename = m_objPath + "warpTestCloth.obj";
        toParam = toParamXZ;
        fixpts = wwsTestFixpts;
    } break;
    }
    // init cloth
    m_cloth.clear();
    m_cloth.init(filename, toParam, fixpts, damping);
    // set sideLength, reset update counter
    m_sideLength = (fixpts.size() - 4) / 2;
    m_updateCount = 0;
}

void ClothInterface::fixClothPts()
{
    std::vector<bool> whichFixPts;
    // switch on _fixpt
    switch(m_fixpt)
    {
    case NONE:
    {
        for(size_t i = 0; i < (m_sideLength * 2) + 4; ++i)
        {
            whichFixPts.push_back(0);
        }
    } break;
    case CORNERS:
    {
        whichFixPts = {1, 1, 1, 1};
        for(size_t i = 0; i < (m_sideLength * 2); ++i)
        {
            whichFixPts.push_back(0);
        }
    } break;
    case HANG:
    {
        whichFixPts = {0, 0, 1, 1};
        for(size_t i = 0; i < (m_sideLength * 2); ++i)
        {
            whichFixPts.push_back(0);
        }
    } break;
    case FLAG:
    {
        whichFixPts = {1, 0, 1, 0};
        for(size_t i = 0; i < (m_sideLength * 2); ++i)
        {
            whichFixPts.push_back(0);
        }
    } break;
    case WEFT_TEST:
    {
        whichFixPts = {1, 1, 1, 1};
        if(m_config == WEFTXZ || m_config == WARPXZ)
        {
            for(size_t i = 0; i < (m_sideLength * 2); ++i)
            {
                whichFixPts.push_back(1);
            }
        }
        else
        {
            for(size_t i = 0; i < (m_sideLength); ++i)
            {
                whichFixPts.push_back(1);
            }
            for(size_t i = 0; i < (m_sideLength); ++i)
            {
                whichFixPts.push_back(0);
            }
        }
    } break;
    case WARP_TEST:
    {
        whichFixPts = {1, 1, 1, 1};
        if(m_config == WEFTXZ || m_config == WARPXZ)
        {
            for(size_t i = 0; i < (m_sideLength * 2); ++i)
            {
                whichFixPts.push_back(1);
            }
        }
        else
        {
            for(size_t i = 0; i < (m_sideLength); ++i)
            {
                whichFixPts.push_back(0);
            }
            for(size_t i = 0; i < (m_sideLength); ++i)
            {
                whichFixPts.push_back(1);
            }
        }
    } break;
    }
    // fix the points in the cloth
    m_cloth.fixCorners(whichFixPts);
}

void ClothInterface::reinitClothToGraphs()
{
    m_cloth.clear();
    m_cloth = Cloth(CUSTOM);
    initCloth();
    fixClothPts();
}

void ClothInterface::setConfig(Config _config)
{
    m_config = _config;
    initCloth();
}

void ClothInterface::setFixPtSetup(FixPtSetup _fixpt)
{
    m_fixpt = _fixpt;
    fixClothPts();
}

void ClothInterface::setClothPtPos(size_t _id, ngl::Vec3 _pos)
{
    m_cloth.setPosAtPoint(_id, _pos);
    m_cloth.newtonRelax();
}

void ClothInterface::updateCloth(float _h)
{
    // make external forces
    std::vector<ngl::Vec3> externalf;
    externalf.resize(m_cloth.numMasses());
    std::default_random_engine gen(m_updateCount);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    if(m_windOn)
    {
        // create gusts of wind in the xz direction
        for(size_t i = 0; i < externalf.size(); ++i)
        {
            auto windFactor = ((m_updateCount % 20)) + 1.0f;
            externalf[i] = m_windVector * dist(gen) * windFactor;
        }
    }
    else
    {
        // run some wind for the first several updates of an XY config so it has a reason to move onto the z-axis
        if(((m_config == LRXY) || (m_config == HRXY)) && (m_updateCount < 50))
        {
            for(size_t i = 0; i < externalf.size(); ++i)
            {
                externalf[i].m_x = m_windVector.m_x * dist(gen);
                externalf[i].m_z = m_windVector.m_z * dist(gen);
            }
        }
    }
    // determine which integration method to use
    bool _useRK4 = false;
    if(m_intm == RK4)
    {
        _useRK4 = true;
    }
    // send to cloth update
    m_cloth.update(_h, _useRK4, true, externalf);
    // increment counter
    ++m_updateCount;
}

void ClothInterface::renderCloth(std::vector<float> &o_vertexData)
{
    m_cloth.render(o_vertexData);
}

void ClothInterface::runWeftTest()
{
    // save previous settings
    auto old_config = m_config;
    auto old_fixpt = m_fixpt;
    auto old_windOn = m_windOn;
    // change to settings needed for test
    m_config = WEFTXZ;
    m_fixpt = WEFT_TEST;
    m_windOn = false;
    // reinit
    initCloth();

    // open file
    std::ofstream out;
    out.open("results/weft_test_results.txt", std::ofstream::out | std::ofstream::trunc);
    out << "Weft Test Results\n\n";
    // take an initial measurement of the elements being pulled - going in positive x
    std::vector<float> initPos;
    initPos.reserve(weftTestPullPts.size());
    for(auto k : weftTestPullPts)
    {
        initPos.push_back(m_cloth.posAtPoint(k).m_x);
    }
    // test loop
    size_t numTests = weftTestForces.size();
    for(size_t i = 0; i < numTests; ++i)
    {
        // make external forces
        std::vector<ngl::Vec3> externalf;
        externalf.resize(m_cloth.numMasses());
        for(auto k : weftTestPullPts)
        {
            externalf[k] = ngl::Vec3(weftTestForces[i], 0.0f, 0.0f);
        }
        // run updates with this external force applied
        size_t numLoops = 20;
        for(size_t j = 0; j < numLoops; ++j)
        {
            m_cloth.update(0.01f, true, false, externalf);
        }
        // record displacements
        out << "Test " << i << ", Force " << weftTestForces[i] << " resulted in the following average displacement: \n";
        float result = 0.0f;
        for(size_t k = 0; k < weftTestPullPts.size(); ++k)
        {
            result += m_cloth.posAtPoint(weftTestPullPts[k]).m_x - initPos[k];
            //out << m_cloth.posAtPoint(weftTestPullPts[k]).m_x - initPos[k] << ' ';
        }
        result = result / weftTestPullPts.size();
        out << result << '\n';
        //out << '\n';
//        // make external forces - 0 vector
//        std::vector<ngl::Vec3> externalf;
//        externalf.resize(m_cloth.numMasses());
//        // displace the end points by the displacement amount in positive x direction
//        for(size_t j = 0; j < weftTestPullPts.size(); ++j)
//        {
//            auto newPos = m_cloth.posAtPoint(weftTestPullPts[j]);
//            newPos.m_x = initPos[j] + weftTestDisp[i];
//            m_cloth.setPosAtPoint(weftTestPullPts[j], newPos);
//        }
//        // run internal forces
//        m_cloth.forceCalc(0.01f, false, externalf, false);
//        // output the sum of the forces at the test points
//        ngl::Vec3 result;
//        out << "Test " << i << ", Displacement "<< weftTestDisp[i]<< " resulted in the total edge force:\n";
//        for(size_t k = 0; k < weftTestPullPts.size(); ++k)
//        {
//            result += m_cloth.forcesAtPoint(weftTestPullPts[k]).m_x;
//            //out << m_cloth.forcesAtPoint(weftTestPullPts[k]).m_x << ' ';
//        }
//        //out << '\n';
//        out << result.m_x << ' ' << result.m_y << ' ' << result.m_z << '\n';
//        // run updates until equilibrium is reached
//        size_t equilibriumLoops = 10;
//        for(size_t l = 0; l < equilibriumLoops; ++l)
//        {
//            m_cloth.update(0.01f, true, false, externalf);
//        }
//        result.null();
//        // forces on these points after equilibrium
//        out << "After Equilibrium: \n";
//        for(size_t k = 0; k < weftTestPullPts.size(); ++k)
//        {
//            result += m_cloth.forcesAtPoint(weftTestPullPts[k]).m_x;
//        }
//        out << result.m_x << ' ' << result.m_y << ' ' << result.m_z << '\n';
        // print out update so we know what's going on
        std::cout<<"Weft Test: finished loop "<< i << '\n';
    }

    // close file
    out.close();
    // now that we're done, reinit back to old settings
    m_config = old_config;
    m_fixpt = old_fixpt;
    m_windOn = old_windOn;
    initCloth();
}

void ClothInterface::runWarpTest()
{

}
