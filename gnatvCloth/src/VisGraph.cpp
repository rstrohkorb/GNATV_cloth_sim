#include "VisGraph.h"
#include "Materials.h"

VisGraph::VisGraph(QWidget *_parent, GraphType _gt) : QChart(_parent), m_type(_gt)
{
    // 'ui' placement optionss
    float xStep, yStep, yMax;
    // set parameters based on graph type
    switch(m_type)
    {
    case WEFT:
    {
        // add data
        for(size_t i = 0; i < wool_weftData.size(); ++i)
        {
            m_spline.append(i * wool_weftStep, wool_weftData[i]);
            m_basic.append(i * wool_weftStep, wool_weftData[i]);
        }
        // set the 'ui' placement options
        yMax = wool_weftData.back();
        yStep = wool_weftData.back() / wool_weftData.size();
        xStep = wool_weftStep;
        // set title
        setTitle("Weft");
    } break;
    case WARP:
    {
        // add data
        for(size_t i = 0; i < wool_warpData.size(); ++i)
        {
            m_spline.append(i * wool_warpStep, wool_warpData[i]);
            m_basic.append(i * wool_warpStep, wool_warpData[i]);
        }
        // set the 'ui' placement options
        yMax = wool_warpData.back();
        yStep = wool_warpData.back() / wool_warpData.size();
        xStep = wool_warpStep;
        // set title
        setTitle("Warp");
    } break;
    case SHEAR:
    {
        // add data
        for(size_t i = 0; i < wool_shearData.size(); ++i)
        {
            m_spline.append(i * wool_shearStep, wool_shearData[i]);
            m_basic.append(i * wool_shearStep, wool_shearData[i]);
        }
        // set the 'ui' placement options
        yMax = wool_shearData.back();
        yStep = wool_shearData.back() / wool_shearData.size();
        xStep = wool_shearStep;
        // set title
        setTitle("Shear");
    } break;
    }
    // 'ui' options determined
    m_adjUp.append(2 * xStep, yMax - yStep);
    m_adjDown.append(2 * xStep, yMax - (3 * yStep));
    m_adjLeft.append(xStep, yMax - (2 * yStep));
    m_adjRight.append(4 * xStep, yMax - (2 * yStep));
    // chart visual options

}
