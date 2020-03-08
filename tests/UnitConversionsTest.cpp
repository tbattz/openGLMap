#include <glm/glm.hpp>

#include "gtest/gtest.h"
#include "utilities/UnitConversions.h"



class Geo2ECEFParameterizedTestFixture : public ::testing::TestWithParam<glm::dvec3> {
protected:
    glm::dvec3 positionVector;
};

TEST_P(Geo2ECEFParameterizedTestFixture, geo2ECEF) {
    // Set tolerance
    float degTol = 1e-5;
    float hTol = 1e-3;

    // Get Position Vector
    glm::dvec3 positionVector = GetParam();

    // Convert Geodetic to ECEF
    glm::dvec3 ecefVector = UnitConversions::geo2ECEF(positionVector);

    // Convert ECEF to Geodetic
    glm::dvec3 newPositionVector = UnitConversions::ecef2Geo(ecefVector);

    // Assert Values
    ASSERT_NEAR(positionVector[0], newPositionVector[0], degTol);
    ASSERT_NEAR(positionVector[1], newPositionVector[1], degTol);
    ASSERT_NEAR(positionVector[2], newPositionVector[2], hTol);
}

INSTANTIATE_TEST_SUITE_P(geo2EcefTests, Geo2ECEFParameterizedTestFixture, ::testing::Values(
        glm::dvec3(1.0f, 1.0f, 0.0f),
        glm::dvec3(45.0f, -90.0f, 0.0f),
        glm::dvec3(0.0f, 0.0f, 1000.0f),
        glm::dvec3(-45.0f, -90.0f, 1000.0f)
        ));