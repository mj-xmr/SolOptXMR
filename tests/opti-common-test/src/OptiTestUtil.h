#ifndef __OPTITESTUTIL_H
#define __OPTITESTUTIL_H

#include <Util/Str.hpp>

class Computer;

class OptiTestUtil
{
public:
	OptiTestUtil();
	virtual ~OptiTestUtil();
	
	static EnjoLib::Str compSched_macAddr;
    static EnjoLib::Str compSched_hostname;
    
    Computer GetCompTestSched() const;

protected:

private:

};

#endif // __OPTITESTUTIL_H
