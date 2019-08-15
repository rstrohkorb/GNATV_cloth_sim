/**
 * @file ClothInterface.h
 * @brief interface between the ui and cloth so tests can easily be run
 * @author Rachel Strohkorb
*/

#ifndef CLOTH_INTERFACE_H_
#define CLOTH_INTERFACE_H_

#include <string>
#include <ngl/Vec3.h>
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
enum Config {LRXZ, LRXY, HRXZ, HRXY, WEFTXZ, WARPXZ};
/**
 * @enum FixPtSetup
 * @brief options for fixing points within the cloth, for testing purposes
*/
enum FixPtSetup {NONE, CORNERS, HANG, FLAG, WEFT_TEST, WARP_TEST};

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
     * @brief user constructor for general use
    */
    ClothInterface(IntegrationMethod _intm, Config _cnfg, FixPtSetup _fxpts, std::string _objPath = "obj/");
    /**
     * @brief initializes cloth to member parameters
    */
    void initCloth();
    /**
     * @brief set up the fixed points in the cloth based on fixptsetup
    */
    void fixClothPts();
    /**
     * @brief reinit the cloth object to the graph data from the UI
    */
    void reinitClothToGraphs();

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
    /**
     * @brief returns whether or not the wind is on
    */
    bool isWindOn() const { return m_windOn; }

    // SETTERS
    /**
     * @brief set the initial config and reinit the cloth object
    */
    void setConfig(Config _config);
    /**
     * @brief set the fixpoint configuration and refix the cloth points
    */
    void setFixPtSetup(FixPtSetup _fixpt);
    /**
     * @brief sets which integration method will be used
    */
    void setIntMethod(IntegrationMethod _intm) { m_intm = _intm; }
    /**
     * @brief turns wind on/off
    */
    void setWindState(bool _isWindOn) { m_windOn = _isWindOn; }
    /**
     * @brief sets the given cloth point to the given position and relaxes the model
    */
    void setClothPtPos(size_t _id, ngl::Vec3 _pos);

    // RUN CLOTH SIM
    /**
     * @brief run a single step of the cloth sim using our integration method
    */
    void updateCloth(float _h);
    /**
     * @brief spit out cloth data to render
    */
    void renderCloth(std::vector<float> &o_vertexData);

    // RUN FORCE/DISPLACEMENT TESTS
    /**
     * @brief run force/displacement test in the weft direction
    */
    void runWeftTest();
    /**
     * @brief run force/displacement test in the warp direction
    */
    void runWarpTest();
    // runShearTest

private:
    // HELPER FUNCTIONS


    // MEMBER VARIABLES
    Cloth m_cloth = Cloth(WOOL);        /**< Cloth object */

    IntegrationMethod m_intm = CGM;     /**< Integration method */
    Config m_config = LRXZ;             /**< Starting config of cloth object */
    FixPtSetup m_fixpt = NONE;          /**< Handler for which points in the cloth are fixed */
    size_t m_sideLength = 15;           /**< Length of the cloth's side, for fixing points */

    std::string m_objPath = "obj/";     /**< Path to the obj files */

    bool m_windOn = false;                                  /**< Whether or not the wind external force is turned on */
    ngl::Vec3 m_windVector = ngl::Vec3(1.0f, 0.0f, 1.0f);   /**< Current base wind vector */
    size_t m_updateCount = 0;                               /**< Count of how many updates we've done in this config */
};

#endif
