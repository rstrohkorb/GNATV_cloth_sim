/**
 * @file MainWindow.h
 * @brief inherits from QMainWindow for GUI
 * @author Jon Macey with Rachel contributions
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts/QChart>
#include "NGLScene.h"
#include "VisGraph.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;   /**< UI object */
    NGLScene *m_gl;         /**< OpenGL widget */

    QtCharts::QChart *m_weftChart;  /**< Weft Chart */
    QtCharts::QChart *m_warpChart;  /**< Warp Chart */
    QtCharts::QChart *m_shearChart; /**< Shear Chart */
    VisGraph *m_weftChartView;      /**< Weft Chart Widget */
    VisGraph *m_warpChartView;      /**< Warp Chart Widget */
    VisGraph *m_shearChartView;     /**< Shear Chart Widget */
};

#endif // MAINWINDOW_H
