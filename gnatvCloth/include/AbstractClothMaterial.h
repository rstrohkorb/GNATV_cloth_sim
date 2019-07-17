#ifndef ABSTRACT_CLOTH_MATERIAL_H_
#define ABSTRACT_CLOTH_MATERIAL_H_

class AbstractClothMaterial
{
public:
    // User Constructor
    AbstractClothMaterial(float _mass) : m_mass(_mass) {;}

    // Getter
    virtual float mass() const { return m_mass; }

    // Strain-stress operations
    virtual float weft(float _strain);
    virtual float warp(float _strain);
    virtual float shear(float _strain);

    virtual float weftPrime(float _strain);
    virtual float warpPrime(float _strain);
    virtual float shearPrime(float _strain);

    // Virtual destructor
    virtual ~AbstractClothMaterial() {;}

private:
    float m_mass;
};

#endif
