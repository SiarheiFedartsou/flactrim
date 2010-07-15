#ifndef BITISTREAM_H
#define BITISTREAM_H

#include <iostream>
#include <fstream>
#include <limits>
#include <stdint.h>
#include "defines.hpp"
#include "exceptions.hpp"
#include "ByteStream.hpp"

namespace Bytes
{
	inline uint16_t BytesCat(uint8_t first, uint8_t  second)
	{
		uint16_t word;
        word = static_cast<uint16_t>(first) << BITSINBYTE;
        word += second;
        return word;
    }
};

class BitIStream
{
public:
	BitIStream(std::string fileName);
	std::string ReadString(unsigned int byteCount);

	std::string PeekString(unsigned int byteCount) 
	{
		uint8_t buffer[byteCount];
		byteBuffer.PeekBuffer(buffer, byteCount);
		return std::string((char *)buffer);
	};

	void ReadFlag(bool * flag);

	template <typename T> void ReadInteger(T * buf, unsigned short bitCount)
	{
		ReadBigEndian(buf, bitCount);
	};

	void ReadAlignBuffer(uint8_t * buffer, size_t count)
	{
		byteBuffer.ReadBuffer(buffer, count);
	}
	//this method is very slow
	void ReadBuffer(uint8_t * buffer, size_t count)
	{
		int i = 0;
		while (count > 0)
		{
			buffer[i] = ReadByte();
			i++;
			count--;
		}
	}
	
	template<typename T> T ReadUnary(T * buf)
	{
		uint8_t bit = 0;
		*buf = 1;
		bit = ReadByte(1);
		while (bit == 0)
		{
			*buf+=1;
			bit = ReadByte(1);
		}
		return *buf;
	}

	void SkipBytes(size_t byteCount)
	{
		while (byteCount--) byteBuffer.SkipByte();
    }

	

private:
	ByteIBuffer<IOBUFFERSIZE> byteBuffer;
	unsigned short firstByteDigitsCount;
	uint8_t ReadByte(unsigned short bitCount = BITSINBYTE);

	template<typename T> void ReadBigEndian(T * buf, unsigned short bitCount)
	{
		if (std::numeric_limits<T>::digits < bitCount) throw TypeOverflow();
		T ret = 0;
		while (bitCount / BITSINBYTE > 0)
		{
			ret += ReadByte();
			if (bitCount / BITSINBYTE > 1) // for all exclude last full byte
				ret <<= BITSINBYTE; 
			bitCount -= BITSINBYTE;
		}
	//	ret >>= BITSINBYTE;
		ret <<= bitCount;
		
		if (bitCount != 0) 
		{

			ret += ReadByte(bitCount);
		}
		else ret >>= bitCount;
		*buf = ret;
    };

};

#endif
