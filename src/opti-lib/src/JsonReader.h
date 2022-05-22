#ifndef __JSONREADER_H
#define __JSONREADER_H

#include "Computer.h"
#include "BatteryParams.h"
#include "System.h"

#include <Util/Str.hpp>
#include <Template/Array.hpp>

class JsonReader
{
public:
	JsonReader();
	virtual ~JsonReader();
	EnjoLib::Array<Computer>      ReadComputers(bool verbose = false) const;
	EnjoLib::Array<BatteryParams> ReadBatteries(bool verbose = false) const;
	System ReadSystem(bool verbose = false) const;

	EnjoLib::Str GetJson(const EnjoLib::Str & fileName) const;
protected:

private:

};

#endif // __JSONREADER_H
