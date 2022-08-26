#ifndef __SOLUTIL_H
#define __SOLUTIL_H

#include <Util/CoutBuf.hpp>
#include <Util/Str.hpp>
#include <Util/VecD.hpp>

//#include <gmpxx.h>

class SolUtil
{
public:
	SolUtil();
	virtual ~SolUtil();

	EnjoLib::Str GetT() const;
	EnjoLib::VecD GenSolar(int days, double aplitude) const;
	EnjoLib::VecD GenSolar(const EnjoLib::VecD & aplitudes) const;

	EnjoLib::Str GetLinesAsSingle(const EnjoLib::Str & fileName) const;
	double round(double val, int decimals = 0) const;

	/*
	mpz_class GetFactorial(int inp) const;
	mpz_class GetPermutations(int available) const;
	mpz_class GetCombinations(int available, int selections) const;
    */
protected:

private:

};

#ifdef VERBOSE_FUNCTIONS
    #define SOL_LOG(str) { LOGL << "VERBOSE: " << str << EnjoLib::Nl; }
#else
    #define SOL_LOG(str) { }
#endif // VERBOSE_FUNCTIONS

#endif // __SOLUTIL_H
