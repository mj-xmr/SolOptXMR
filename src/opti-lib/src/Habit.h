#ifndef __HABIT_H
#define __HABIT_H

#include <Util/Str.hpp>

class Habit
{
public:
	Habit();
	virtual ~Habit();
	
	EnjoLib::Str Print() const;
	void ParseSchedule();
	
	EnjoLib::Str name;
	EnjoLib::Str schedule;
	double watt = 1;
	double watt_asleep = 1;
	double duration_hours = 1;

protected:

private:
    

};

#endif // __HABIT_H
