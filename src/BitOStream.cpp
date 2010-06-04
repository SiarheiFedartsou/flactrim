#include "BitOStream.hpp"

BitOStream::BitOStream(std::string fileName) : freeDigitsCount(BITSINBYTE)
{
    byteBuffer.Init(fileName);
}



void BitOStream::WriteByte(uint8_t byte, unsigned short bitCount)
{
    if (bitCount >= freeDigitsCount)
    {
        uint8_t cur = byteBuffer.GetCurrentByte();
		cur |= (byte << (BITSINBYTE - bitCount)) >> (BITSINBYTE - freeDigitsCount);
		byteBuffer.WriteByte(cur);

		cur = 0;

		cur |= byte << (BITSINBYTE - (bitCount - freeDigitsCount));
		byteBuffer.SetCurrentByte(cur);
		freeDigitsCount = BITSINBYTE - bitCount + freeDigitsCount;
    }
    else
    {
        uint8_t cur = byteBuffer.GetCurrentByte();
		cur |= byte << (freeDigitsCount - bitCount);
		byteBuffer.SetCurrentByte(cur);
		freeDigitsCount -= bitCount;
    }            
}

void BitOStream::WriteString(std::string str)
{
	for_each(str.begin(), str.end(), [this](char c)
	{
		WriteByte(c);
	}
	);
}
