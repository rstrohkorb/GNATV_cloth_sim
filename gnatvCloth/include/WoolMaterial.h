#ifndef WOOL_MATERIAL_H_
#define WOOL_MATERIAL_H_

#include "AbstractClothMaterial.h"

class WoolMaterial : public AbstractClothMaterial
{
public:
    // Constructor
    WoolMaterial() : AbstractClothMaterial(0.15f) {;}

    // Strain-stress behavior functions
    float weft(float _strain);
    float warp(float _strain);
    float shear(float _strain);

    float weftPrime(float _strain);
    float warpPrime(float _strain);
    float shearPrime(float _strain);
};

#endif
