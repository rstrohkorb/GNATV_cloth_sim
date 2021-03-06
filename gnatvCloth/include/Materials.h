/**
 * @file Materials.h
 * @brief a header for storing weft, warp, and shear cloth data
 * @author Rachel Strohkorb
 *
 * This data is inteded for use with boost's cubic b-spline interpolator. The data is
 * derived from force/displacement tests performed on an actual cloth sample. Any other
 * data added to this file needs the following:
 *      1. Mass of one square meter of the cloth in kg/m^2
 *      2. Weft, warp, and shear data sets measuring stress, taken at equal strain intervals
 *      3. Starting points of the data sets
*/

#ifndef MATERIALS_H_
#define MATERIALS_H_

#include <vector>

// WOOL

static float wool_mass = 0.15f; // in kg per square meter

static float wool_weftStart = 0.0f;
static float wool_warpStart = 0.0f;
static float wool_shearStart = -0.14f;

static float wool_weftStep = 0.004078f;
static float wool_warpStep = 0.00048614f;
static float wool_shearStep = 0.01f;

static std::vector<float> wool_weftData = {
    0.0f, 1.494023904f, 3.472222222f, 9.881422925f, 14.76377953f,
    24.50980392f, 29.296875f, 38.91050584f, 48.4496124f, 57.91505792f,
    67.30769231f, 81.41762452f, 95.41984733f, 118.8212928f, 146.780303f,
    183.9622642f, 211.4661654f, 238.7640449f, 275.1865672f, 315.9851301f,
    365.7407407f, 433.5793358f, 510.1102941f, 581.5018315f, 656.9343066f,
    759.0909091f, 851.4492754f, 956.6787004f, 1052.158273f
};

static std::vector<float> wool_warpData = {
    0.0f, 7.996001999f, 15.98401598f, 19.97004493f, 33.93213573f,
    41.89526185f, 49.85044865f, 59.79073244f, 79.6812749f, 99.55201593f,
    119.4029851f, 159.1248135f, 198.8071571f, 258.3209141f, 357.4975174f,
    416.8734491f, 476.1904762f, 614.7744175f, 733.3994054f, 911.3422486f,
    1069.306931f, 1197.427016f, 1404.549951f, 1601.581809f, 1897.233202f,
    2133.333333f, 2388.943731f, 2723.23631f, 2998.027613f
};

static std::vector<float> wool_shearData = {
    -59.986f, -50.987f, -44.488f, -37.989f, -32.99f,
    -28.991f, -24.992f, -20.493f, -16.994f, -12.995f,
    -9.496f, -4.997f, -2.998f, -0.999f, 0.0f,
    0.999f, 2.998f, 4.997f, 9.496f, 12.995f,
    16.994f, 20.493f, 24.992f, 28.991f, 32.99f,
    37.989f, 44.488f, 50.987f, 59.986f
};

// JUTE - shear is broken so it doesn't work

static float jute_mass = 0.42f;

static float jute_weftStart = 0.0f;
static float jute_warpStart = 0.0f;
static float jute_shearStart = -0.12f;

static float jute_weftStep = 0.008f;
static float jute_warpStep = 0.004f;
static float jute_shearStep = 0.02f;

static std::vector<float> jute_weftData = {
    0.0f, 650000.0f, 1240000.0f, 2020000.0f, 2700000.0f, 3930000.0f,
    5340000.0f, 8060000.0f, 11540000.0f, 18950000.0f, 28700000.0f,
    33650000.0f, 38480000.0f
};

static std::vector<float> jute_warpData = {
    0.0f, 1300000.0f, 2100000.0f, 2960000.0f, 4300000.0f, 10230000.0f,
    19020000.0f, 28530000.0f, 43860000.0f, 56140000.0f, 69690000.0f,
    78560000.0f, 83430000.0f
};

static std::vector<float> jute_shearData = {
    -36.75f, -22.4f, -15.73f, -11.6f, -6.7f, -3.43f, 0.0f, 3.43f,
    6.7f, 11.6f, 15.73f, 22.4f, 36.75f
};

#endif
