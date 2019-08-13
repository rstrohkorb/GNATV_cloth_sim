/**
 * @file VisGraph.h
 * @brief graph visualization for the b-spline curves controlling the cloth sim
 * @author Rachel Strohkorb
*/

#ifndef VISGRAPH_H_
#define VISGRAPH_H_

#include <memory>

#include <QChart>
#include <QScatterSeries>
#include <QSplineSeries>
#include <QGraphicsWidget>

/**
 * @enum GraphType
 * @brief the three types of possible graphs
*/
enum GraphType { WEFT, WARP, SHEAR };

/**
 * @class VisGraph
 * @brief encases graph visualization/modification for warp/weft/shear curves on cloth
*/
class VisGraph : public QChart
{
public:
Q_OBJECT
    // delete default constructor
    VisGraph() = delete;
    /**
     * @brief constructor, sets up plots to default data
    */
    VisGraph(QWidget *_parent = nullptr, GraphType _gt);
    /**
     * @brief destructor, defined for qt purposes
    */
    ~VisGraph();

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
    /**
     * @brief adjust the selected point left
    */
    void adjLeft(const QPointF &point);
    /**
     * @brief adjust the selected point right
    */
    void adjRight(const QpointF &point);

private:
    // MEMBER VARIABLES
    std::unique_ptr<QSplineSeries> m_spline;
    std::unique_ptr<QScatterSeries> m_basic;
    std::unique_ptr<QScatterSeries> m_highlight;
    std::unique_ptr<QScatterSeries> m_adjUp;
    std::unique_ptr<QScatterSeries> m_adjDown;
    std::unique_ptr<QScatterSeries> m_adjLeft;
    std::unique_ptr<QScatterSeries> m_adjRight;

    QPointF m_selectedPoint;
    GraphType m_type;
};

#endif
