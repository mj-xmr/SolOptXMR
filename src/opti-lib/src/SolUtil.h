#ifndef __SOLUTIL_H
#define __SOLUTIL_H

#include <Util/CoutBuf.hpp>

class SolUtil
{
public:
	SolUtil();
	virtual ~SolUtil();

protected:

private:

};

#ifdef VERBOSE_FUNCTIONS
    #define SOL_LOG(str) { LOGL << "VERBOSE: " << str << EnjoLib::Nl; }
#else
    #define SOL_LOG(str) { }
#endif // VERBOSE_FUNCTIONS

#endif // __SOLUTIL_H
