#include <stdint.h>
#include <iostream>
#include <math.h>

using namespace std;

uint32_t GetBlockSize()
{
	return 4096;
}

uint8_t GetPredictorOrder()
{
	return 12;
}


int main()
{
	uint8_t partitionOrder = 9;
	cout << GetBlockSize() << " " <<  pow(2, partitionOrder) << " " << (int)GetPredictorOrder() << endl;
	size_t samplesCount = GetBlockSize() / pow(2, partitionOrder) - GetPredictorOrder();
	std::cout << 3 << " " << samplesCount << std::endl;

}
