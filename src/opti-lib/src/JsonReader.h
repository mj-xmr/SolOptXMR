#ifndef __JSONREADER_H
#define __JSONREADER_H

#include <Template/Array.hpp>
#include "Computer.h"

class JsonReader
{
public:
	JsonReader();
	virtual ~JsonReader();
	EnjoLib::Array<Computer> ReadComputers(bool verbose = false) const;

protected:

private:

};

#endif // __JSONREADER_H
