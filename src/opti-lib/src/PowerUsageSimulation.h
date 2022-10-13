#ifndef __POWERUSAGESIMULATION_H
#define __POWERUSAGESIMULATION_H

#include <Statistical/MatrixFwd.hpp>

#include <cstdlib>

class OptiEnProfitDataModel;

class PowerUsageSimulation
{
public:
	PowerUsageSimulation(const OptiEnProfitDataModel & dataModel);
	virtual ~PowerUsageSimulation();

    struct SimResult
    {
        double sumHashes = 0;
        double sumPowerUsage = 0;

        void Add(const SimResult & other)
        {
            sumHashes += other.sumHashes;
            sumPowerUsage += other.sumPowerUsage;
        }
    };

	SimResult Simulate(int i, int currHour, const EnjoLib::Matrix & dataMat,
                        double bonusMul, bool isInitialLoad, double hours=1) const;

protected:

private:
    const OptiEnProfitDataModel & m_dataModel;

};

#endif // __POWERUSAGESIMULATION_H
