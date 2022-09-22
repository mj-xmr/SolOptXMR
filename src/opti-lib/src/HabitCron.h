#ifndef __HABITCRON_H
#define __HABITCRON_H

#include <Template/VecT.hpp>

class Habit;

class HabitCron
{
public:
	HabitCron(bool verbose = false);
	virtual ~HabitCron();
	
	EnjoLib::VecT<int> GetNextHoursOn(const Habit & hab, int horizonDays = 10) const; /// TODO: Pass on horizon days

protected:

private:
    bool m_verbose = false;

};

#endif // __HABITCRON_H
