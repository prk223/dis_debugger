#include "gtest/gtest.h"
#include "DatumDefTest.cpp"
#include "NetworkPduSourceTest.cpp"
#include "DataModelControllerTest.cpp"
#include "DatumIdentifierTest.cpp"
#include "DatumInfoTest.cpp"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
