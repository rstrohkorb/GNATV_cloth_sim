#include <string>
#include <vector>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>
#include "ClothInterface.h"

ClothInterface::ClothInterface()
{
    initCloth();
}
ClothInterface::ClothInterface(std::string _objPath)
{
    m_objPath = _objPath;
    initCloth();
}

void ClothInterface::initCloth()
{
    initCloth(m_config, m_fixpt);
}

void ClothInterface::initCloth(Config _config, FixPtSetup _fxpt)
{
    // prepare variables to init cloth
    std::string filename;
    std::function<ngl::Vec2(ngl::Vec3)> toParam;
    std::vector<size_t> fixpts;
    std::vector<bool> whichFixPts;
    float damping = 9.0f;
    // set variables based on config
    switch (_config)
    {
    case LRXZ:
    {
        filename = m_objPath + "clothLowResXZ.obj";
        toParam = [](ngl::Vec3 _v) -> ngl::Vec2
        {
            ngl::Vec2 n;
            n.m_x = _v.m_x;
            n.m_y = _v.m_z;
            return n;
        };
        // set all possible fixpts
    }
    break;
    case LRXY:
    {
        filename = m_objPath + "clothLowResXY.obj";
        toParam = [](ngl::Vec3 _v) -> ngl::Vec2
        {
            ngl::Vec2 n;
            n.m_x = _v.m_x;
            n.m_y = _v.m_y;
            return n;
        };
        // set all possible fixpts
    }
    break;
    case HRXZ:
    {
        filename = m_objPath + "clothHiResXZ.obj";
        toParam = [](ngl::Vec3 _v) -> ngl::Vec2
        {
            ngl::Vec2 n;
            n.m_x = _v.m_x;
            n.m_y = _v.m_z;
            return n;
        };
        // set all possible fixpts
    }
    break;
    case HRXY:
    {
        filename = m_objPath + "clothHiResXY.obj";
        toParam = [](ngl::Vec3 _v) -> ngl::Vec2
        {
            ngl::Vec2 n;
            n.m_x = _v.m_x;
            n.m_y = _v.m_y;
            return n;
        };
        // set all possible fixpts
    }
    break;
    }
    //TODO: switch case to determine which points are actually fixed

    // init/reinit cloth
    m_cloth.clear();
    m_cloth.init(filename, toParam, fixpts, damping);
    m_cloth.fixCorners(whichFixPts);
}
