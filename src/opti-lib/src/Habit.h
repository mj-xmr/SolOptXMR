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
	constexpr bool HasHour(int hour) const { return hour < szz; }
	
	EnjoLib::Str name;
	EnjoLib::Str schedule;
	double watt = 1;
	double watt_asleep = 0.1;
	double duration_hours = 1;
	size_t szz = 0;
	bool defaultUseSchedule = true;

protected:

private:
    EnjoLib::VecT<int> m_hoursOn;
};

#endif // __HABIT_H
