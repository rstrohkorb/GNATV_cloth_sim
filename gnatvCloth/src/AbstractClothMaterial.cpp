#include "AbstractClothMaterial.h"

float AbstractClothMaterial::weft(float _strain)
{
    // for now, just assume all weights are 1.0
    auto t3 = (_strain * _strain * _strain);
    auto t2 = (_strain * _strain);
    auto t1 = _strain;
    auto t0 = 1.0f;
    return t3 + t2 + t1 + t0;
}

float AbstractClothMaterial::warp(float _strain)
{
    auto t3 = (_strain * _strain * _strain);
    auto t2 = (_strain * _strain);
    auto t1 = _strain;
    auto t0 = 1.0f;
    return t3 + t2 + t1 + t0;
}

float AbstractClothMaterial::shear(float _strain)
{
    auto t3 = (_strain * _strain * _strain);
    auto t2 = (_strain * _strain);
    auto t1 = _strain;
    auto t0 = 1.0f;
    return t3 + t2 + t1 + t0;
}

float AbstractClothMaterial::weftPrime(float _strain)
{
    auto t2 = 3.0f * (_strain * _strain);
    auto t1 = 2.0f * _strain;
    auto t0 = 1.0f;
    return t2 + t1 + t0;
}

float AbstractClothMaterial::warpPrime(float _strain)
{
    auto t2 = 3.0f * (_strain * _strain);
    auto t1 = 2.0f * _strain;
    auto t0 = 1.0f;
    return t2 + t1 + t0;
}

float AbstractClothMaterial::shearPrime(float _strain)
{
    auto t2 = 3.0f * (_strain * _strain);
    auto t1 = 2.0f * _strain;
    auto t0 = 1.0f;
    return t2 + t1 + t0;
}
