#include "BitOStream.hpp"

BitOStream::BitOStream(std::string fileName) : freeDigitsCount(BITSINBYTE)
{
    byteBuffer.Init(fileName);
	WriteByte(3, 7);
	WriteByte(1, 4);
	WriteByte(3, 7);
	WriteByte(3, 6);
	unsigned int n = 323443;
	WriteLittleEndian(n, sizeof(n)*BITSINBYTE);
	//WriteByte(4, 4);
	//WriteByte(6, 7);
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
}
