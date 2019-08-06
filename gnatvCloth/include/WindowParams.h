/*
 * @file WindowParams.h
 * @brief stores parameters for the Qt OpenGLWindow
 * @author Jon Macey
*/

#ifndef WINDOWPARAMS_H_
#define WINDOWPARAMS_H_

struct WinParams
{
    /**
     * @brief used to store the x rotation mouse value
    */
    int spinXFace=0;
    /**
     * @brief used to store the y rotation mouse value
    */
    int spinYFace=0;
    /**
     * @brief flag to indicate if the mouse button is pressed when dragging
    */
    bool rotate=false;
    /**
     * @brief flag to indicate if the Right mouse button is pressed when dragging
    */
    bool translate=false;
    /**
     * @brief the previous x mouse value
    */
    int origX=0;
    /**
     * @brief the previous y mouse value
    */
    int origY=0;
    /**
     * @brief the previous x mouse value for Position changes
    */
    int origXPos=0;
    /**
     * @brief the previous y mouse value for Position changes
    */
    int origYPos=0;
    /**
     * @brief window width
    */
    int width=1024;
    /**
     * @brief window height
    */
    int height=720;
};
/**
 * @brief the increment for x/y translation with mouse movement
*/
constexpr float INCREMENT=0.01f;
/**
 * @brief the increment for the wheel zoom
*/
constexpr float ZOOM=0.1f;

#endif
