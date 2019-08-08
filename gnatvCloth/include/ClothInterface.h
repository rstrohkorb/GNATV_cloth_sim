/**
 * @file ClothInterface.h
 * @brief interface between the ui and cloth so tests can easily be run
 * @author Rachel Strohkorb
*/

#ifndef CLOTH_INTERFACE_H_
#define CLOTH_INTERFACE_H_

#include <string>
#include "Cloth.h"

/**
 * @enum integrationMethod
 * @brief describes possible cloth integration methods
*/
enum IntegrationMethod {RK4, CGM};
/**
 * @enum startConfig
 * @brief describes starting configuration for the cloth object
*/
enum Config {LRXZ, LRXY, HRXZ, HRXY};
/**
 * @enum FixPtSetup
 * @brief options for fixing points within the cloth, for testing purposes
*/
enum FixPtSetup {NONE, CORNERS, HANG, FLAG, PULL_TEST};

/**
 * @class ClothInterface
 * @brief interface between ui and cloth object to make testing/baking easier
*/
class ClothInterface
{
public:
    // CONSTRUCTORS/INITIALIZERS
    /**
     * @brief default constructor, sets up cloth to default parameters
    */
    ClothInterface();
    /**
     * @brief user constructor for test suite, modifies the obj path
    */
    ClothInterface(std::string _objPath);
    /**
     * @brief user constructor
    */
    /**
     * @brief initializes cloth to input/default parameters
    */
    void initCloth();
    void initCloth(Config _config, FixPtSetup _fxpt);

    // GETTERS
    /**
     * @brief returns number of points in the cloth
    */
    size_t numClothPts() const { return m_cloth.numMasses(); }
    /**
     * @brief returns number of triangles in the cloth
    */
    size_t numClothTris() const { return m_cloth.numTriangles(); }
    /**
     * @brief returns current path to the obj files
    */
    std::string objPath() const { return m_objPath; }
    /**
     * @brief returns the currennt integration method
    */
    IntegrationMethod intMethod() const { return m_intm; }
    /**
     * @brief returns initial config handler
    */
    Config initConfig() const { return m_config; }
    /**
     * @brief returns the handler for which points in the cloth are fixed
    */
    FixPtSetup fixPointSetup() const { return m_fixpt; }

private:
    // MEMBER VARIABLES
    Cloth m_cloth = Cloth(WOOL);        /**< Cloth object */

    std::string m_objPath = "obj/";

    IntegrationMethod m_intm = CGM;     /**< Integration method */
    Config m_config = LRXZ;             /**< Starting config of cloth object */
    FixPtSetup m_fixpt = NONE;          /**< Handler for which points in the cloth are fixed */
};

#endif
