#include <glm/glm.hpp>

#include "gtest/gtest.h"
#include "utilities/UnitConversions.h"


/* Geodetic to ECEF to Geodetic */

class Geo2ECEFParameterizedTestFixture : public ::testing::TestWithParam<glm::dvec3> {
    // geoPositionVector
};

TEST_P(Geo2ECEFParameterizedTestFixture, geo2ECEF) {
    // Set tolerance
    float degTol = 1e-5;
    float hTol = 1e-3;

    // Get Position Vector
    glm::dvec3 geoPositionVector = GetParam();

    // Convert Geodetic to ECEF
    glm::dvec3 ecefVector = UnitConversions::geo2ECEF(geoPositionVector);

    // Convert ECEF to Geodetic
    glm::dvec3 newGeoPositionVector = UnitConversions::ecef2Geo(ecefVector);

    // Assert Values
    ASSERT_NEAR(geoPositionVector[0], newGeoPositionVector[0], degTol);
    ASSERT_NEAR(geoPositionVector[1], newGeoPositionVector[1], degTol);
    ASSERT_NEAR(geoPositionVector[2], newGeoPositionVector[2], hTol);
}

INSTANTIATE_TEST_SUITE_P(geo2EcefTests, Geo2ECEFParameterizedTestFixture, ::testing::Values(
        glm::dvec3(1.0f, 1.0f, 0.0f),
        glm::dvec3(45.0f, -90.0f, 0.0f),
        glm::dvec3(0.0f, 0.0f, 1000.0f),
        glm::dvec3(-45.0f, -90.0f, 1000.0f)
));

/* Ecef to Enu to Ecef */

class Ecef2EnuParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<glm::dvec3, glm::dvec3>> {
    // ecefVector
    // origin
};

TEST_P(Ecef2EnuParameterizedTestFixture, ecef2Enu) {
    // Set tolerance
    float tol = 1.0f;

    // Get Inputs
    glm::dvec3 ecefVector = std::get<0>(GetParam());
    glm::dvec3 origin = std::get<1>(GetParam());
    glm::dvec3 ecefOrigin = UnitConversions::geo2ECEF(origin);

    // Convert ECEF to ENU
    glm::dvec3 enuVector = UnitConversions::ecef2ENU(ecefVector, ecefOrigin, origin);

    // Convert ENU to ECEF
    glm::dvec3 newEcefVector = UnitConversions::enu2Ecef(enuVector, origin);

    // Assert Values
    ASSERT_NEAR(ecefVector[0], newEcefVector[0], tol);
    ASSERT_NEAR(ecefVector[1], newEcefVector[1], tol);
    ASSERT_NEAR(ecefVector[2], newEcefVector[2], tol);
}

INSTANTIATE_TEST_SUITE_P(ecef2EnuTests, Ecef2EnuParameterizedTestFixture, ::testing::Values(
        std::make_tuple(glm::dvec3(1000.0f, 1000.0f, 0.0f), glm::dvec3(0.0f, 0.0f, 0.0f)),
        std::make_tuple(glm::dvec3(4500.0f, -9000.0f, 0.0f), glm::dvec3(0.0f, 0.0f, 0.0f)),
        std::make_tuple(glm::dvec3(1000.0f, 1000.0f, 0.0f), glm::dvec3(-37.846863f, 144.970602f, 50.0f)),
        std::make_tuple(glm::dvec3(4500.0f, -9000.0f, 0.0f), glm::dvec3(-37.846863f, 144.970602f, 50.0f))
));

/* Geodetic to Enu to Geodetic */

class Geo2EnuParameterizedTestFixture : public ::testing::TestWithParam<std::tuple<glm::dvec3, glm::dvec3>> {
    // geoPosition
    // origin
};

TEST_P(Geo2EnuParameterizedTestFixture, geo2Enu) {
    // Set tolerance
    float tol = 1.0f;

    // Get Inputs
    glm::dvec3 geoPosition = std::get<0>(GetParam());
    glm::dvec3 origin = std::get<1>(GetParam());

    // Convert Geodetic to ENU
    glm::dvec3 positionVector = UnitConversions::geo2ENU(geoPosition, origin);

    // Convert ECEF to Geodetic
    glm::dvec3 newGeoPositionVector = UnitConversions::enu2Geo(positionVector, origin);

    // Assert Values
    ASSERT_NEAR(geoPosition[0], newGeoPositionVector[0], tol);
    ASSERT_NEAR(geoPosition[1], newGeoPositionVector[1], tol);
    ASSERT_NEAR(geoPosition[2], newGeoPositionVector[2], tol);
}

INSTANTIATE_TEST_SUITE_P(geo2EnuTests, Geo2EnuParameterizedTestFixture, ::testing::Values(
        std::make_tuple(glm::dvec3(1.0f, 1.0f, 0.0f), glm::dvec3(0.0f, 0.0f, 0.0f)),
        std::make_tuple(glm::dvec3(45.0f, -90.0f, 0.0f), glm::dvec3(0.0f, 0.0f, 0.0f)),
        std::make_tuple(glm::dvec3(0.0f, 0.0f, 1000.0f), glm::dvec3(-37.846863f, 144.970602f, 50.0f)),
        std::make_tuple(glm::dvec3(-45.0f, -90.0f, 1000.0f), glm::dvec3(-37.846863f, 144.970602f, 50.0f))
));




