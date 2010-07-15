#include "BitIStream.hpp"


BitIStream::BitIStream(std::string fileName) : firstByteDigitsCount(BITSINBYTE)
{

	byteBuffer.Init(fileName);

}

uint8_t BitIStream::ReadByte(unsigned short bitCount)
{
	if (bitCount >= firstByteDigitsCount)
	{
            uint16_t word = 0;
		

            uint8_t firstByte = byteBuffer.GetNextByte();
            uint8_t secondByte = byteBuffer.PeekNextByte();

            word = Bytes::BytesCat(firstByte, secondByte);

            word <<= (BITSINBYTE - firstByteDigitsCount);
            word >>= sizeof(uint16_t) * BITSINBYTE  - bitCount;

            firstByteDigitsCount = (BITSINBYTE - (bitCount - firstByteDigitsCount) == 0) ? BITSINBYTE : (BITSINBYTE - (bitCount - firstByteDigitsCount));
            return word;
	}
	else
	{
            uint8_t ret = byteBuffer.PeekNextByte();
            ret <<= BITSINBYTE - firstByteDigitsCount;
            ret >>= BITSINBYTE - bitCount;
            firstByteDigitsCount -= bitCount;
       	    return ret;
	}
}

std::string BitIStream::ReadString(unsigned int byteCount)
{
    std::string ret;
    while (byteCount--) ret += std::string(1, ReadByte());
    return ret;
}

void BitIStream::ReadFlag(bool * flag)
{
	uint8_t f = 0;
	ReadBigEndian(&f, 1);
	*flag = f != 0;
}
