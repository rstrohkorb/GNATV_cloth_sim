#include "VisGraph.h"
#include "Materials.h"

#include <QtCore/QtMath>
#include <iostream>
#include <fstream>

VisGraph::VisGraph(QtCharts::QChart *_chart, QWidget *_parent, GraphType _gt) : QtCharts::QChartView (_chart, _parent), m_type(_gt)
{
    // instantiate plain pointer series
    m_spline = new QtCharts::QSplineSeries();
    m_basic = new QtCharts::QScatterSeries();
    m_highlight = new QtCharts::QScatterSeries();
    m_adjUp = new QtCharts::QScatterSeries();
    m_adjDown = new QtCharts::QScatterSeries();
    // set parameters based on graph type
    // highlight series grabs 3rd element by default
    switch(m_type)
    {
    case WEFT:
    {
        // add data
        for(size_t i = 0; i < wool_weftData.size(); ++i)
        {
            if(i == 3)
            {
                m_highlight->append(static_cast<double>(i * wool_weftStep),
                                    static_cast<double>(wool_weftData[i]));
            }
            else
            {
                m_basic->append(static_cast<double>(i * wool_weftStep),
                                static_cast<double>(wool_weftData[i]));
            }
            m_spline->append(static_cast<double>(i * wool_weftStep),
                             static_cast<double>(wool_weftData[i]));
        }
        // set the 'ui' placement options
        m_yMin = 0.0;
        m_yMax = static_cast<double>(wool_weftData.back());
        m_yStep = static_cast<double>(wool_weftData.back() / wool_weftData.size());
        m_xMin = 0.0;
        m_xMax = static_cast<double>(wool_weftStep * (wool_weftData.size() - 1));
        m_xStep = static_cast<double>(wool_weftStep);
        // set title
        chart()->setTitle("Weft");
    } break;
    case WARP:
    {
        // add data
        for(size_t i = 0; i < wool_warpData.size(); ++i)
        {
            if(i == 3)
            {
                m_highlight->append(static_cast<double>(i * wool_warpStep),
                                    static_cast<double>(wool_warpData[i]));
            }
            else
            {
                m_basic->append(static_cast<double>(i * wool_warpStep),
                                static_cast<double>(wool_warpData[i]));
            }
            m_spline->append(static_cast<double>(i * wool_warpStep),
                             static_cast<double>(wool_warpData[i]));
        }
        // set the 'ui' placement options
        m_yMin = 0.0;
        m_yMax = static_cast<double>(wool_warpData.back());
        m_yStep = static_cast<double>(wool_warpData.back() / wool_warpData.size());
        m_xMin = 0.0;
        m_xMax = static_cast<double>(wool_warpStep * (wool_warpData.size() - 1));
        m_xStep = static_cast<double>(wool_warpStep);
        // set title
        chart()->setTitle("Warp");
    } break;
    case SHEAR:
    {
        // add data
        for(size_t i = 0; i < wool_shearData.size(); ++i)
        {
            if(i == 3)
            {
                m_highlight->append(static_cast<double>(i * wool_shearStep + wool_shearStart),
                                    static_cast<double>(wool_shearData[i]));
            }
            else
            {
                m_basic->append(static_cast<double>(i * wool_shearStep + wool_shearStart),
                                static_cast<double>(wool_shearData[i]));
            }
            m_spline->append(static_cast<double>(i * wool_shearStep + wool_shearStart),
                             static_cast<double>(wool_shearData[i]));
        }
        // set the 'ui' placement options
        m_yMin = static_cast<double>(wool_shearData.front());
        m_yMax = static_cast<double>(wool_shearData.back());
        m_yStep = static_cast<double>((wool_shearData.back() * 2) / wool_shearData.size());
        m_xMin = static_cast<double>(wool_shearStart);
        m_xMax = static_cast<double>(wool_shearStart + (wool_shearStep * (wool_shearData.size() - 1)));
        m_xStep = static_cast<double>(wool_shearStep);
        // set title
        chart()->setTitle("Shear");
    } break;
    }
    // 'ui' options determined
    m_adjUp->append((2 * m_xStep) + m_xMin, m_yMax - m_yStep*2);
    m_adjDown->append((2 * m_xStep) + m_xMin, m_yMax - (6 * m_yStep));
    // visual options for all the plots
    QPen splinePen;
    splinePen.setWidthF(2.5);
    m_spline->setPen(splinePen);
    m_spline->setColor(QRgb(0x3d7543));
    m_basic->setColor(QRgb(0x3d7543));
    m_basic->setMarkerSize(8.0);
    m_highlight->setColor(QRgb(0x51a7c4));
    m_highlight->setMarkerSize(10.0);
    m_adjUp->setColor(QRgb(0xe8443f));
    m_adjUp->setMarkerSize(12.0);
    m_adjDown->setColor(QRgb(0xe8443f));
    m_adjDown->setMarkerSize(12.0);
    // add the plots
    chart()->addSeries(m_spline);
    chart()->addSeries(m_basic);
    chart()->addSeries(m_highlight);
    chart()->addSeries(m_adjUp);
    chart()->addSeries(m_adjDown);
    // chart visual options
    chart()->setDropShadowEnabled(false);
    chart()->legend()->setVisible(false);
    chart()->createDefaultAxes();
    // hook up signals/slots
    connect(m_basic, &QtCharts::QScatterSeries::clicked, this, &VisGraph::selectPoint);
    connect(m_adjUp, &QtCharts::QScatterSeries::clicked, this, &VisGraph::adjUp);
    connect(m_adjDown, &QtCharts::QScatterSeries::clicked, this, &VisGraph::adjDown);
}

VisGraph::~VisGraph()
{
}

void VisGraph::outputGraphToFile()
{
    // get filename based on type
    std::string filename("graphsFromUI/");
    switch(m_type)
    {
    case WEFT: filename = filename + "weft"; break;
    case WARP: filename = filename + "warp"; break;
    case SHEAR: filename = filename + "shear"; break;
    }
    filename = filename + "_graphData.txt";
    // open file
    std::ofstream out;
    out.open(filename, std::ofstream::out | std::ofstream::trunc);
    // data start
    out << "start "<< m_xMin << "f\n";
    // data step
    out << "step "<< m_xStep << "f\n";
    // y-data
    for(auto pt : m_spline->points())
    {
        out << pt.y() << '\n';
    }
    // close file
    out.close();
}

void VisGraph::selectPoint(const QPointF &point)
{
    // remove point from m_highlight and add back to m_basic
    auto oldPoint = m_highlight->at(0);
    m_highlight->remove(oldPoint);
    m_basic->append(oldPoint);
    // remove clicked point from m_basic and put it into m_highlight
    auto clickedPoint = closestPointBasic(point);
    m_basic->remove(clickedPoint);
    m_highlight->append(clickedPoint);
}

void VisGraph::adjUp(const QPointF &point)
{
    // make sure the highlight point isn't 0,0 or an endpoint
    if(canMovePoint())
    {
        // grab the highlight point/index from m_spline
        auto splinePt = closestPointSpline(m_highlight->at(0));
        auto splineIndex = pointIndexSpline(splinePt);
        // adjust the position of the point in m_highlight up by yStep
        auto pt = m_highlight->at(0);
        m_highlight->replace(0, pt.x(), pt.y() + m_yStep/2);
        // adjust the position of the same point in m_spline
        m_spline->replace(splineIndex, splinePt.x(), splinePt.y() + m_yStep/2);
    }
}

void VisGraph::adjDown(const QPointF &point)
{
    if(canMovePoint())
    {
        // grab the highlight point/index from m_spline
        auto splinePt = closestPointSpline(m_highlight->at(0));
        auto splineIndex = pointIndexSpline(splinePt);
        // adjust the position of the point in m_highlight down by yStep
        auto pt = m_highlight->at(0);
        m_highlight->replace(0, pt.x(), pt.y() - m_yStep/2);
        // adjust the position of the same point in m_spline
        m_spline->replace(splineIndex, splinePt.x(), splinePt.y() - m_yStep/2);
    }
}

QPointF VisGraph::closestPointBasic(const QPointF &point)
{
    /**
     * Modified from the scatterinteractions example on the qtcharts git
     * in the handleClickedPoint method (lines 69 - 83)
     * https://code.qt.io/cgit/qt/qtcharts.git/tree/examples/charts/scatterinteractions/chartview.cpp?h=5.13
    */
    QPointF clickedPoint = point;
    QPointF closest(INT_MAX, INT_MAX);
    qreal distance(INT_MAX);
    const auto points = m_basic->points();
    for (const QPointF &currentPoint : points)
    {
        qreal currentDistance = qSqrt((currentPoint.x() - clickedPoint.x()) *
                                      (currentPoint.x() - clickedPoint.x()) +
                                      (currentPoint.y() - clickedPoint.y()) *
                                      (currentPoint.y() - clickedPoint.y()));
        if (currentDistance < distance)
        {
            distance = currentDistance;
            closest = currentPoint;
        }
    }
    return closest;
}

QPointF VisGraph::closestPointSpline(const QPointF &point)
{
    /**
     * Modified from the scatterinteractions example on the qtcharts git
     * in the handleClickedPoint method (lines 69 - 83)
     * https://code.qt.io/cgit/qt/qtcharts.git/tree/examples/charts/scatterinteractions/chartview.cpp?h=5.13
    */
    QPointF inputPoint = point;
    QPointF closest(INT_MAX, INT_MAX);
    qreal distance(INT_MAX);
    const auto points = m_spline->points();
    for (const QPointF &currentPoint : points)
    {
        qreal currentDistance = qSqrt((currentPoint.x() - inputPoint.x()) *
                                      (currentPoint.x() - inputPoint.x()) +
                                      (currentPoint.y() - inputPoint.y()) *
                                      (currentPoint.y() - inputPoint.y()));
        if (currentDistance < distance)
        {
            distance = currentDistance;
            closest = currentPoint;
        }
    }
    return closest;
}

int VisGraph::pointIndexSpline(const QPointF point)
{
    const auto points = m_spline->points();
    for(int i = 0; i < points.size(); ++i)
    {
        if(points[i] == point)
        {
            return i;
        }
    }
    return 0;
}

bool VisGraph::canMovePoint()
{
    auto pt = m_highlight->at(0);
    // check if we're 0,0
    if(qFuzzyCompare(0.0, pt.x()) && qFuzzyCompare(0.0, pt.y()))
    {
        return false;
    }
    // check if we're the left endpoint
    auto leftpt = m_spline->at(0);
    if(qFuzzyCompare(leftpt.x(), pt.x()) && qFuzzyCompare(leftpt.y(), pt.y()))
    {
        return false;
    }
    // check if we're the right endpoint
    auto rightpt = m_spline->at(m_spline->points().size() - 1);
    if(qFuzzyCompare(rightpt.x(), pt.x()) && qFuzzyCompare(rightpt.y(), pt.y()))
    {
        return false;
    }
    // if we've passed all the tests, we're good
    return true;
}
