#ifndef __SOLUTIL_H
#define __SOLUTIL_H

#include <Util/CoutBuf.hpp>
#include <Util/Str.hpp>
#include <Util/VecD.hpp>

class SolUtil
{
public:
	SolUtil();
	virtual ~SolUtil();
	
	EnjoLib::Str GetPercentToAscii(double val, double minimum = 0, double maximum = 1);
	EnjoLib::Str GetPercentToAscii(const EnjoLib::VecD & val, double minimum = 0, double maximum = 1, bool decoration = false);

protected:

private:

};

#ifdef VERBOSE_FUNCTIONS
    #define SOL_LOG(str) { LOGL << "VERBOSE: " << str << EnjoLib::Nl; }
#else
    #define SOL_LOG(str) { }
#endif // VERBOSE_FUNCTIONS

#endif // __SOLUTIL_H
