#include "Habit.h"

Habit::Habit(){}
Habit::~Habit(){}


EnjoLib::Str Habit::Print() const
{
    return name;
}

void Habit::ParseSchedule() /// TODO: Figure out a way to parse it only once
{
    
}