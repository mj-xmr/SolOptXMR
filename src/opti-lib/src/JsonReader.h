#ifndef __JSONREADER_H
#define __JSONREADER_H

#include "Computer.h"
#include "Habit.h"
#include "System.h"
#include "BatteryParams.h"

#include <Util/StrFwd.hpp>
#include <Template/Array.hpp>
#include <STD/Vector.hpp>

class ConfigSol;

class JsonReader
{
public:
	JsonReader(bool verbose = true);
	virtual ~JsonReader();
	std::vector<Computer>        ReadComputers(bool verbose = false) const;
	std::vector<BatteryParams>   ReadBatteries(bool verbose = false) const;
	std::vector<Habit>           ReadHabits   (bool verbose = false) const;
	System ReadSystem(bool verbose = false) const;
	ConfigSol ReadConfigSol(bool verbose = false) const;

	static EnjoLib::Str GetJson(const EnjoLib::Str & fileName, bool verbose);

protected:

private:
    bool m_verbose = false;

};

#endif // __JSONREADER_H
