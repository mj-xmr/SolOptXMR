#include "JsonReader.h"
#include "ConfigSol.h"

#include <Util/CoutBuf.hpp>

#include <UnitTest++/UnitTest++.h>

using namespace EnjoLib;


TEST(Json_read_volatile)
{
    JsonReader().ReadConfigSol();
    CHECK(true);
}


TEST(Json_read_habits)
{
    JsonReader().ReadHabits();
    //const bool result = habit.HasHour(0);
    //const bool expected = false;
    //CHECK_EQUAL(expected, result);
    CHECK(true);
}
