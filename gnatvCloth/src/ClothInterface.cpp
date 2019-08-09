#include <string>
#include <vector>
#include <random>
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
        whichFixPts = {1, 0, 1, 0};
        for(size_t i = 0; i < m_sideLength; ++i)
        {
            whichFixPts.push_back(1);
        }
    } break;
    case WARP_TEST:
    {
        whichFixPts = {1, 1, 0, 0};
        for(size_t i = 0; i < m_sideLength; ++i)
        {
            whichFixPts.push_back(0);
        }
        for(size_t i = 0; i < m_sideLength; ++i)
        {
            whichFixPts.push_back(1);
        }
    } break;
    }
    // fix the points in the cloth
    m_cloth.fixCorners(whichFixPts);
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
                externalf[i] = m_windVector * dist(gen);
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
    m_cloth.update(_h, _useRK4, externalf);
    // increment counter
    ++m_updateCount;
}

void ClothInterface::renderCloth(std::vector<float> &o_vertexData)
{
    m_cloth.render(o_vertexData);
}
