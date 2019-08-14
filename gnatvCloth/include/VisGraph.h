/**
 * @file VisGraph.h
 * @brief graph visualization for the b-spline curves controlling the cloth sim
 * @author Rachel Strohkorb
*/

#ifndef VISGRAPH_H_
#define VISGRAPH_H_

#include <memory>

#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QChartView>

/**
 * @enum GraphType
 * @brief the three types of possible graphs
*/
enum GraphType { WEFT, WARP, SHEAR };

/**
 * @class VisGraph
 * @brief encases graph visualization/modification for warp/weft/shear curves on cloth
*/
class VisGraph : public QtCharts::QChartView
{
Q_OBJECT
public:
    // delete default constructor
    VisGraph() = delete;
    /**
     * @brief constructor, sets up plots to default data
    */
    VisGraph(QtCharts::QChart *_chart, QWidget *_parent = nullptr, GraphType _gt = WEFT);
    /**
     * @brief destructor, defined for qt purposes
    */
    ~VisGraph();

public slots:
    /**
     * @brief output graph data to file so the cloth can read it
    */
    void outputGraphToFile();

private slots:
    /**
     * @brief select a point from the base graph to highlight/adjust
    */
    void selectPoint(const QPointF &point);
    /**
     * @brief adjust the selected point up
    */
    void adjUp(const QPointF &point);
    /**
     * @brief adjust the selected point down
    */
    void adjDown(const QPointF &point);

private:
    // HELPER FUNCTIONS
    /**
     * @brief grab the closest point from the m_basic series to the input point
    */
    QPointF closestPointBasic(const QPointF &point);
    /**
     * @brief grab the closest point from the m_spline series to the input point
    */
    QPointF closestPointSpline(const QPointF &point);
    /**
     * @brief grab the index related to the input point from the m_spline series
    */
    int pointIndexSpline(const QPointF point);
    /**
     * @brief returns true if the highlight point isn't 0,0 or an endpoint
    */
    bool canMovePoint();

    // MEMBER VARIABLES
    QtCharts::QSplineSeries *m_spline;
    QtCharts::QScatterSeries *m_basic;
    QtCharts::QScatterSeries *m_highlight;
    QtCharts::QScatterSeries *m_adjUp;
    QtCharts::QScatterSeries *m_adjDown;

    QPointF m_selectedPoint;
    GraphType m_type;

    double m_xMin, m_xMax, m_yMin, m_yMax, m_xStep, m_yStep;
};

#endif
