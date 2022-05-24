#include "Habit.h"
#include "HabitCron.h"

Habit::Habit(){}
Habit::~Habit(){}

EnjoLib::Str Habit::Print() const
{
    return name;
}

void Habit::ParseSchedule() /// TODO: Figure out a way to parse it only once
{
    m_hoursOn = HabitCron().GetNextHoursOn(*this);
}

bool Habit::IsOn(int hour) const
{
    if (hour > m_hoursOn.size())
    {
        //return false;
    }
    return m_hoursOn.at(hour) != 0;
}
