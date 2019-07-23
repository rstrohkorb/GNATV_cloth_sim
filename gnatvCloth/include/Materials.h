#ifndef MATERIALS_H_
#define MATERIALS_H_

#include <vector>

static float wool_mass = 0.15f;

static float wool_weftStart = 0.0f;
static float wool_warpStart = 0.0f;
static float wool_shearStart = -0.14f;

static float wool_weftStep = 0.004078f;
static float wool_warpStep = 0.00048614f;
static float wool_shearStep = 0.01f;

static float wool_shearSlope = 899.9f;

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

#endif
