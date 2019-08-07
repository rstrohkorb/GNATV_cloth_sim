/**
 * @file MainWindow.h
 * @brief inherits from QMainWindow for GUI
 * @author Jon Macey
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NGLScene.h"

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
    Ui::MainWindow *m_ui;
        /// @brief our openGL widget
        NGLScene *m_gl;
};

#endif // MAINWINDOW_H
