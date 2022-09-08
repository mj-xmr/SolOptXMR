#ifndef __HabitCron3rd3RD_H
#define __HabitCron3rd3RD_H

#include <Template/VecT.hpp>

class Habit;

class HabitCron3rd
{
public:
	HabitCron3rd(bool verbose = false);
	virtual ~HabitCron3rd();
	
	EnjoLib::VecT<int> GetNextHoursOn(const Habit & hab, int horizonDays = 10) const; /// TODO: Pass on horizon days

protected:

private:
    bool m_verbose = false;

};

#endif // __HabitCron3rd3RD_H
