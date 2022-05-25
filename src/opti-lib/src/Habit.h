#ifndef __HABIT_H
#define __HABIT_H

#include <Util/Str.hpp>
#include <Template/VecT.hpp>

class Habit
{
public:
	Habit();
	virtual ~Habit();
	
	EnjoLib::Str Print() const;
	void ParseSchedule();
	bool IsOn(int hour) const;
	
	EnjoLib::Str name;
	EnjoLib::Str schedule;
	double watt = 1;
	double watt_asleep = 0.1;
	double duration_hours = 1;

protected:

private:
    EnjoLib::VecT<int> m_hoursOn;
};

#endif // __HABIT_H
