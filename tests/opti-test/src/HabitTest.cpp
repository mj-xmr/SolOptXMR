#include "Habit.h"

#include <UnitTest++/UnitTest++.h>

TEST(Habit_empty_unusable1)
{
    Habit habit;
    bool result = habit.HasHour(0);
    bool expected = false;
    CHECK_EQUAL(expected, result);
}
