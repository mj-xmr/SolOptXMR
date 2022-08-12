#include "JsonReader.h"
#include "ConfigSol.h"

#include <Util/CoutBuf.hpp>

#include <UnitTest++/UnitTest++.h>

using namespace EnjoLib;

TEST(Json_ReadComputers)
{
    JsonReader().ReadComputers();
    CHECK(true);
}

TEST(Json_ReadBatteries)
{
    JsonReader().ReadBatteries();
    CHECK(true);
}

TEST(Json_ReadSystem)
{
    JsonReader().ReadSystem();
    CHECK(true);
}

TEST(Json_ReadHabits)
{
    JsonReader().ReadHabits();
    //const bool result = habit.HasHour(0);
    //const bool expected = false;
    //CHECK_EQUAL(expected, result);
    CHECK(true);
}

TEST(Json_ReadConfigSol)
{
    JsonReader().ReadConfigSol();
    CHECK(true);
}
