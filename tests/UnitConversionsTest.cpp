#include <glm/glm.hpp>

#include "gtest/gtest.h"
#include "utilities/UnitConversions.h"



TEST(UnitConversionsTest, geo2ECEF) {
    // Create Position Vector
    glm::dvec3 positionVector = glm::dvec3(1.0f, 1.0f, 1.0f);

    // Convert Geodetic to ECEF
    glm::dvec3 ecefVector = UnitConversions::geo2ECEF(positionVector);

    // Assert Value
    EXPECT_EQ(ecefVector, glm::dvec3(2.0f, 2.0f, 2.0f));
}
