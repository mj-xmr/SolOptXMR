#ifndef __JSONREADER_H
#define __JSONREADER_H

#include "Computer.h"
#include "Habit.h"
#include "System.h"
#include "BatteryParams.h"

#include <Util/Str.hpp>
#include <Template/Array.hpp>

class ConfigSol;

class JsonReader
{
public:
	JsonReader();
	virtual ~JsonReader();
	EnjoLib::Array<Computer>        ReadComputers(bool verbose = false) const;
	EnjoLib::Array<BatteryParams>   ReadBatteries(bool verbose = false) const;
	EnjoLib::Array<Habit>           ReadHabits   (bool verbose = false) const;
	System ReadSystem(bool verbose = false) const;
	ConfigSol ReadConfigSol(bool verbose = false) const;

	static EnjoLib::Str GetJson(const EnjoLib::Str & fileName);
	
protected:

private:

};

#endif // __JSONREADER_H
