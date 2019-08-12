/**
 * @file NGLScene.h
 * @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
 * @author Jonathan Macey (with a few Rachel contributions)
 * @date 10/9/13
 * Revision History :
 * This is an initial version used for the new NGL6 / Qt 5 demos
*/

#ifndef NGLSCENE_H_
#define NGLSCENE_H_

#include <ngl/Vec3.h>
#include <ngl/Vec4.h>
#include <ngl/Mat4.h>
#include <ngl/AbstractVAO.h>

#include "WindowParams.h"
#include "ClothInterface.h"

#include <QEvent>
#include <QResizeEvent>
// this must be included after NGL includes else we get a clash with gl libs
#include <QOpenGLWidget>

/**
 * @class NGLScene
 * @brief our main glwindow widget for NGL applications all drawing elements are
 * put in this file
*/

class NGLScene : public QOpenGLWidget
{
Q_OBJECT
public:
    /**
     * @brief ctor for our NGL drawing class
     * @param [in] parent the parent window to the class
    */
    NGLScene(QWidget *_parent);
    /**
     * @brief dtor must close down ngl and release OpenGL resources
    */
    ~NGLScene() override;
    /**
     * @brief events that run every clock cycle once timer starts
    */
    void timerEvent(QTimerEvent *_event) override;
    /**
     * @brief the initialize class is called once when the window is created and we have a valid GL context
     * use this to setup any default GL stuff
    */
    void initializeGL() override;
    /**
     * @brief this is called everytime we want to draw the scene
    */
    void paintGL() override;
    /**
     * @brief this is called everytime we resize the window
    */
    void resizeGL(int _w, int _h) override;

public slots:
    /**
     * @brief a slot to toggle wireframe mode
    */
    void toggleWireframe(bool _mode);
    /**
     * @brief slot to start the cloth sim
    */
    void startSim();
    /**
     * @brief slot to stop the cloth sim
    */
    void stopSim();
    /**
     * @brief slot to reset the cloth sim
    */
    void resetSim();
    /**
     * @brief change starting configuration of the cloth
    */
    void changeConfig(int _config);
    /**
     * @brief change which points in the cloth are fixed
    */
    void changeFixpt(int _fixpt);
    /**
     * @brief change the integration method used to run cloth sim
    */
    void changeIntMethod(int _intm);
    /**
     * @brief turn on/off the wind
    */
    void toggleWind(bool _isWindOn);
    /**
     * @brief run force/displacement test in weft direction
    */
    void runWeftTest();
    /**
     * @brief run force/displacement test in warp direction
    */
    //void runWarpTest();
    /**
     * @brief run force/displacement test in shear direction
    */
    //void runShearTest();

private:
    /**
     * @brief Qt Event called when a key is pressed
     * @param [in] _event the Qt event to query for size etc
    */
    void keyPressEvent(QKeyEvent *_event) override;
    /**
     * @brief this method is called every time a mouse is moved
     * @param _event the Qt Event structure
    */
    void mouseMoveEvent (QMouseEvent * _event ) override;
    /**
     * @brief this method is called everytime the mouse button is pressed
     * inherited from QObject and overridden here.
     * @param _event the Qt Event structure
    */
    void mousePressEvent ( QMouseEvent *_event) override;
    /*
     * @brief this method is called everytime the mouse button is released
     * inherited from QObject and overridden here.
     * @param _event the Qt Event structure
    */
    void mouseReleaseEvent ( QMouseEvent *_event ) override;
    /**
     * @brief this method is called everytime the mouse wheel is moved
     * inherited from QObject and overridden here.
     * @param _event the Qt Event structure
    */
    void wheelEvent( QWheelEvent *_event) override;
    /**
     * @brief prepares ngl checker shader for use for the next object to be drawn
     * @param _tx current transformation, usually mouse rotation
    */
    void loadMatrixToCheckerShader(const ngl::Mat4 &_tx);

    // MEMBER VARIABLES
    WinParams m_win;                /**< Windows parameters for mouse control etc. */
    ngl::Vec3 m_modelPos;           /**< Position for the model */
    ngl::Mat4 m_view;               /**< View matrix */
    ngl::Mat4 m_project;            /**< Project matrix */
    ngl::Vec4 m_lightPos;           /**< Light position in world space */

    std::unique_ptr<ngl::AbstractVAO> m_clothVAO;   /**< VAO for the cloth triangles */

    ClothInterface m_ci;            /**< ClothInterface object */

    bool m_wireframe = false;       /**< Whether or not the cloth is visualized in wireframe */
    int m_timerId;                  /**< Id for starting/stopping the timer */
};



#endif
