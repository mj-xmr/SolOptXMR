#include "JsonReader.h"
#include "ConfigSol.h"

#include <Util/CoutBuf.hpp>

#include <UnitTest++/UnitTest++.h>

using namespace EnjoLib;

TEST(Json_ReadComputers)
{
    JsonReader(false).ReadComputers();
    CHECK(true);
}

TEST(Json_ReadBatteries)
{
    JsonReader(false).ReadBatteries();
    CHECK(true);
}

TEST(Json_ReadSystem)
{
    JsonReader(false).ReadSystem();
    CHECK(true);
}

TEST(Json_ReadHabits)
{
    JsonReader(false).ReadHabits();
    //const bool result = habit.HasHour(0);
    //const bool expected = false;
    //CHECK_EQUAL(expected, result);
    CHECK(true);
}

TEST(Json_ReadConfigSol)
{
    JsonReader(false).ReadConfigSol();
    CHECK(true);
}
