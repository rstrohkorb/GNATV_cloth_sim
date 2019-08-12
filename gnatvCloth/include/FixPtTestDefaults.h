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
//      weft test pulls
//      warp test holds
//      warp test pulls
static std::vector<size_t> lowResXZFixpts = {
    0, 1, 2, 3,
    81, 25, 121, 9, 93, 37, 133, 4, 85, 29, 125, 13, 97, 41, 137,
    139, 43, 99, 15, 127, 31, 87, 6, 135, 39, 95, 11, 123, 27, 83,
    138, 42, 98, 14, 126, 30, 86, 5, 134, 38, 94, 10, 122, 26, 82,
    84, 28, 124, 12, 96, 40, 136, 7, 88, 32, 128, 16, 100, 44, 140
};
static std::vector<size_t> lowResXYFixpts = {
    0, 1, 2, 3,
    27, 26, 25, 24, 23, 22, 21, 20,
    28, 29, 30, 31, 32, 33, 34, 35,
    4, 5, 6, 7, 8, 9, 10, 11,
    19, 18, 17, 16, 15, 14, 13, 12
};
static std::vector<size_t> hiResFixpts = {
    0, 1, 2, 3,
    33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4,
    34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
    64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93,
    123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94
};

static std::vector<size_t> wwsTestFixpts = {
    0, 1, 2, 3,
    28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    //27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4
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
    1, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 0
};
static std::vector<size_t> warpTestPullPts = {
    2, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 3
};
static std::vector<size_t> shearTestPullRight = {
    0, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 2
};

// forces for weft/warp/shear tests
static std::vector<float> weftTestForces = {
    0.0f, 0.3f, 0.7f, 2.0f, 3.0f, 5.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 17.0f, 20.0f, 25.0f, 31.0f, 39.0f,
    45.0f, 51.0f, 59.0f, 68.0f, 79.0f, 94.0f, 111.0f, 127.0f, 144.0f, 167.0f, 188.0f, 212.0f, 234.0f
};
static std::vector<float> warpTestForces = {
    0.0f, 0.4f, 0.8f, 1.0f, 1.7f, 2.1f, 2.5f, 3.0f, 4.0f, 5.0f, 6.0f, 8.0f, 10.0f,
    13.0f, 18.0f, 21.0f, 24.0f, 31.0f, 37.0f, 46.0f, 54.0f, 60.5f,71.0f, 81.0f,
    96.0f, 108.0f, 121.0f, 138.0f, 152.0f
};
static std::vector<float> shearTestForces = {
    -12.0f, -10.2f, -8.9f, -7.6f, -6.6f, -5.8f, -5.0f, -4.1f, -3.4f, -2.6f, -1.9f,
    -1.0f, -0.6f, -0.2f, 0.0f, 0.2f, 0.6f, 1.0f, 1.9f, 2.6f, 3.4f, 4.1f, 5.0f, 5.8f,
    6.6f, 7.6f, 8.9f, 10.2f, 12.0f
};

#endif
