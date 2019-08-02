#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include <functional>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>

class Triangle
{
public:
    // Explicitly set all the default functions
    Triangle()=default;                                   // default constructor
    ~Triangle()=default;                                  // destructor
    Triangle(const Triangle &)=default;                   // copy constructor
    Triangle(Triangle &&)=default;                        // move constructor
    Triangle & operator = (const Triangle &) = default;   // assignment operator
    Triangle & operator = (Triangle &&) = default;        // move assignment operator

    // User Ctor
    Triangle(ngl::Vec3 _a, ngl::Vec3 _b, ngl::Vec3 _c);

    // Getters/Setters
    ngl::Vec3 v1() const { return m_a; }
    ngl::Vec3 v2() const { return m_b; }
    ngl::Vec3 v3() const { return m_c; }
    float surface_area() const { return m_sa; }
    ngl::Vec2 v1UV() const { return m_aUV; }
    ngl::Vec2 v2UV() const { return m_bUV; }
    ngl::Vec2 v3UV() const { return m_cUV; }
    ngl::Vec3 ru() const { return m_ru; }
    ngl::Vec3 rv() const { return m_rv; }

    void setVertices(const ngl::Vec3 _a, const ngl::Vec3 _b, const ngl::Vec3 _c);
    void setUV1(const ngl::Vec2 _uv1) { m_aUV = _uv1; }
    void setUV2(const ngl::Vec2 _uv2) { m_bUV = _uv2; }
    void setUV3(const ngl::Vec2 _uv3) { m_cUV = _uv3; }

    // compute r-weights
    void computeR(std::function<ngl::Vec2(ngl::Vec3)> _toParam);

private:
    // Member Variables

    // triangle vertex positions
    ngl::Vec3 m_a = ngl::Vec3(0.0f);
    ngl::Vec3 m_b = ngl::Vec3(1.0f, 0.0f, 0.0f);
    ngl::Vec3 m_c = ngl::Vec3(0.0f, 1.0f, 0.0f);

    // surface area
    float m_sa = 0.0f;

    // uvs
    ngl::Vec2 m_aUV = ngl::Vec2(0.0f, 0.0f);
    ngl::Vec2 m_bUV = ngl::Vec2(0.0f, 1.0f);
    ngl::Vec2 m_cUV = ngl::Vec2(1.0f, 0.0f);

    // r-weights
    ngl::Vec3 m_ru = ngl::Vec3(1.0f);
    ngl::Vec3 m_rv = ngl::Vec3(1.0f);

    // Helper functions
    void update_sa();
};

#endif
