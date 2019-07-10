#ifndef TRIANGLE_H_
#define TRIANGLE_H_

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
    Triangle(ngl::Vec3 _a, ngl::Vec3 _b, ngl::Vec3 _c) :
        m_a(_a), m_b(_b), m_c(_c) {;}

    // Getters/Setters
    ngl::Vec3 v1() const { return m_a; }
    ngl::Vec3 v2() const { return m_b; }
    ngl::Vec3 v3() const { return m_c; }
    ngl::Vec3 ru() const { return m_ru; }
    ngl::Vec3 rv() const { return m_rv; }

    void setVertices(const ngl::Vec3 _a, const ngl::Vec3 _b, const ngl::Vec3 _c);


private:
    // Member Variables

    // triangle vertex positions
    ngl::Vec3 m_a = ngl::Vec3(0.0f);
    ngl::Vec3 m_b = ngl::Vec3(1.0f, 0.0f, 0.0f);
    ngl::Vec3 m_c = ngl::Vec3(0.0f, 1.0f, 0.0f);

    // r-weights
    ngl::Vec3 m_ru = ngl::Vec3(1.0f);
    ngl::Vec3 m_rv = ngl::Vec3(1.0f);

};

#endif
