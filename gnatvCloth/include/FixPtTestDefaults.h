/**
 * @file FixPtTestDefaults.h
 * @brief defines the points to fix for the cloth tests on the 4 default obj files
 * @author Rachel Strohkorb
*/

#ifndef FIXPTTESTDEFAULTS_H_
#define FIXPTTESTDEFAULTS_H_

#include <vector>
#include <functional>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>

// define all possible fixpoints for the .obj file defaults
// point order:
//      corners
//      weft test holds
//      warp test holds
static std::vector<size_t> lowResXZFixpts = {
    0, 1, 2, 3,
    81, 25, 121, 9, 93, 37, 133, 4, 85, 29, 125, 13, 97, 41, 137,
    138, 42, 98, 14, 126, 30, 86, 5, 134, 38, 94, 10, 122, 26, 82
};
static std::vector<size_t> lowResXYFixpts = {
    0, 1, 2, 3,
    27, 26, 25, 24, 23, 22, 21, 20,
    4, 5, 6, 7, 8, 9, 10, 11
};
static std::vector<size_t> hiResFixpts = {
    0, 1, 2, 3,
    33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93
};

// define the 2 basic toParam functions to initialize the cloth with
static auto toParamXY = [](ngl::Vec3 _v) -> ngl::Vec2
{
    ngl::Vec2 n;
    n.m_x = _v.m_x;
    n.m_y = _v.m_y;
    return n;
};
static auto toParamXZ = [](ngl::Vec3 _v) -> ngl::Vec2
{
    ngl::Vec2 n;
    n.m_x = _v.m_x;
    n.m_y = _v.m_z;
    return n;
};

// define which points need to be pulled for each of the weft/warp/shear tests
static std::vector<size_t> weftTestPullPts = {
    1, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 3
};
static std::vector<size_t> warpTestPullPts = {
    2, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 3
};
static std::vector<size_t> shearTestPullRight = {
    0, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 2
};

#endif
