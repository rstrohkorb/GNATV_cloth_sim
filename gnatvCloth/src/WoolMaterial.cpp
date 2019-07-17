#include "WoolMaterial.h"

float WoolMaterial::weft(float _strain)
{
    auto t3 = 762873.0f * (_strain * _strain * _strain);
    auto t2 = -27370.0f * (_strain * _strain);
    auto t1 = 1618.7f * _strain;
    auto t0 = -4.339f;
    return t3 + t2 + t1 + t0;
}

float WoolMaterial::warp(float _strain)
{
    auto t3 = 1.0e9f * (_strain * _strain * _strain);
    auto t2 = -4.0e6f * (_strain * _strain);
    auto t1 = 9829.3f * _strain;
    auto t0 = 9.9052f;
    return t3 + t2 + t1 + t0;
}

float WoolMaterial::shear(float _strain)
{
    auto t3 = 9653.4f * (_strain * _strain * _strain);
    auto t2 = 5.0e-11f * (_strain * _strain);
    auto t1 = 234.77f * _strain;
    auto t0 = 1.0e-12f;
    return t3 + t2 + t1 + t0;
}

float WoolMaterial::weftPrime(float _strain)
{
    auto t2 = 762873.0f * 3.0f * (_strain * _strain);
    auto t1 = -27370.0f * 2.0f * _strain;
    auto t0 = 1618.7f;
    return t2 + t1 + t0;
}

float WoolMaterial::warpPrime(float _strain)
{
    auto t2 = 1.0e9f * 3.0f * (_strain * _strain);
    auto t1 = -4.0e6f * 2.0f * _strain;
    auto t0 = 9829.3f;
    return t2 + t1 + t0;
}

float WoolMaterial::shearPrime(float _strain)
{
    auto t2 = 9653.4f * 3.0f * (_strain * _strain);
    auto t1 = 5.0e-11f * 2.0f * _strain;
    auto t0 = 234.77f;
    return t2 + t1 + t0;
}
