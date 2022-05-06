#ifndef CLIResultSolSOL_H
#define CLIResultSolSOL_H

#include "ConfigSol.h"


struct CLIResultSol
{
    CLIResultSol(const ConfigSol & confSol)
    : m_confSol(confSol)
    {

    }

    ConfigSol m_confSol;
};

#endif // CLIResultSol_H
