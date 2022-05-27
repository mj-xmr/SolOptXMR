#include "Habit.h"
#include "HabitCron.h"

#include <UnitTest++/UnitTest++.h>

TEST(Habit_empty_unusable1)
{
    Habit habit;
    const bool result = habit.HasHour(0);
    const bool expected = false;
    CHECK_EQUAL(expected, result);
}

TEST(Habit_noSchedule_alwaysOn)
{
    Habit habit;
    habit.ParseSchedule();
    for (int i = 0; i < 24; ++i)
    {
        const bool result = habit.HasHour(i);
        const bool expected = true;
        CHECK_EQUAL(expected, result);
    }
    for (int i = 0; i < 24; ++i)
    {
        if (habit.HasHour(i))
        {
            const bool result = habit.IsOn(i);
            const bool expected = true;
            CHECK_EQUAL(expected, result);
        }
    }
}

static void _TestHabit_scheduleOnceAday(int durationHours)
{
    Habit habit;
    habit.duration_hours = durationHours;
    habit.schedule = "0 0 1 * * *";
    habit.ParseSchedule();
    for (int i = 0; i < 24; ++i)
    {
        const bool result = habit.HasHour(i);
        const bool expected = true;
        CHECK_EQUAL(expected, result);
    }
    int numOn = 0;
    int numOff = 0;
    for (int i = 0; i < 24; ++i)
    {
        if (habit.IsOn(i))
        {
            ++numOn;
        }
        else
        {
            ++numOff;
        }
    }
    CHECK_EQUAL(durationHours,      numOn);     // Only one on
    CHECK_EQUAL(24 - durationHours, numOff);    // The rest was off
}

TEST(Habit_scheduleOnceAday)
{
    _TestHabit_scheduleOnceAday(1);
}

TEST(Habit_scheduleOnceAday2Hours)
{
    /// TODO: This might not work at each possible hour across the day. Needs a START TIME simulation, rather than choosing the current hour.
    //_TestHabit_scheduleOnceAday(2);
}

