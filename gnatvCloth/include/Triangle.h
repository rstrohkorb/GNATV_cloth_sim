/**
 * @file Triangle.h
 * @brief Class for storing and operating on triangle data in a cloth object
 * @author Rachel Strohkorb
*/

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <functional>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>

/**
 * @class Triangle
 * @brief stores data related to the given triangle, including its default
 * warp/weft state
*/
class Triangle
{
public:
    // CONSTRUCTORS
    /**
     * @brief default constructor
    */
    Triangle()=default;
    /**
     * @brief user constructor
     * @param _a _b _c triangle vertex positions
    */
    Triangle(ngl::Vec3 _a, ngl::Vec3 _b, ngl::Vec3 _c);

    // GETTERS
    /**
     * @brief returns position of the first vertex
    */
    ngl::Vec3 v1() const { return m_a; }
    /**
     * @brief returns position of the second vertex
    */
    ngl::Vec3 v2() const { return m_b; }
    /**
     * @brief returns position of the third vertex
    */
    ngl::Vec3 v3() const { return m_c; }
    /**
     * @brief returns current triangle surface area
    */
    float surface_area() const { return m_sa; }
    /**
     * @brief returns UV coordinates of the first vertex
    */
    ngl::Vec2 v1UV() const { return m_aUV; }
    /**
     * @brief returns UV coordinates of the second vertex
    */
    ngl::Vec2 v2UV() const { return m_bUV; }
    /**
     * @brief returns UV coordinates of the third vertex
    */
    ngl::Vec2 v3UV() const { return m_cUV; }
    /**
     * @brief returns triangle resting state weights for weft direction
    */
    ngl::Vec3 ru() const { return m_ru; }
    /**
     * @brief returns triangle resting state weights for warp direction
    */
    ngl::Vec3 rv() const { return m_rv; }

    // SETTERS
    /**
     * @brief sets the three vertex positions and updates the surface area
    */
    void setVertices(const ngl::Vec3 _a, const ngl::Vec3 _b, const ngl::Vec3 _c);
    /**
     * @brief sets the UV coordinates of the first vertex
    */
    void setUV1(const ngl::Vec2 _uv1) { m_aUV = _uv1; }
    /**
     * @brief sets the UV coordinates of the second vertex
    */
    void setUV2(const ngl::Vec2 _uv2) { m_bUV = _uv2; }
    /**
     * @brief sets the UV coordinates of the third vertex
    */
    void setUV3(const ngl::Vec2 _uv3) { m_cUV = _uv3; }

    // STATE OPERATORS
    /**
     * @brief computes the resting state weights for weft/warp directions
     * @param _toParam a function that converts the triangle vertices from 3D world
     * space to 2d parametric coordinates
     *
     * It is very important that the 2D coordinates created by _toParam form a triangle
     * with the same surface area (i.e. same units) as the 3D world coordinates. The
     * simplest function results from a starting cloth state where the cloth is aligned
     * to one of the XY, XZ, or YZ planes. If this principle is not followed, the internal
     * force calculations will be incorrect, and the cloth will tear itself apart at resting
     * state.
    */
    void computeR(std::function<ngl::Vec2(ngl::Vec3)> _toParam);

private:
    // HELPER FUNCTIONS
    /*
     * @brief recalculates the surface area based on the current vertex positions
    */
    void update_sa();

    // MEMBER VARIABLES
    ngl::Vec3 m_a = ngl::Vec3(0.0f);                /**< Position of vertex A */
    ngl::Vec3 m_b = ngl::Vec3(1.0f, 0.0f, 0.0f);    /**< Position of vertex B */
    ngl::Vec3 m_c = ngl::Vec3(0.0f, 1.0f, 0.0f);    /**< Position of vertex C */

    float m_sa = 0.5f;      /**< Surface area of the triangle */

    ngl::Vec2 m_aUV = ngl::Vec2(0.0f, 0.0f);    /**< UV coord of vertex A */
    ngl::Vec2 m_bUV = ngl::Vec2(0.0f, 1.0f);    /**< UV coord of vertex B */
    ngl::Vec2 m_cUV = ngl::Vec2(1.0f, 0.0f);    /**< UV coord of vertex C */

    ngl::Vec3 m_ru = ngl::Vec3(1.0f);   /**< Resting state weights for weft direction */
    ngl::Vec3 m_rv = ngl::Vec3(1.0f);   /**< Resting state weights for warp direction */
};

#endif
