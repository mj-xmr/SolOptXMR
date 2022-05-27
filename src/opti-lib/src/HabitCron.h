#ifndef __HABITCRON_H
#define __HABITCRON_H

#include <Template/VecT.hpp>

class Habit;

class HabitCron
{
public:
	HabitCron();
	virtual ~HabitCron();
	
	EnjoLib::VecT<int> GetNextHoursOn(const Habit & hab, int horizonDays = 10) const; /// TODO: Pass on horizon days

protected:

private:

};

#endif // __HABITCRON_H
