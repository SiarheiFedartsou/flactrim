#ifndef BITOSTREAM_H
#define BITOSTREAM_H

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include "defines.hpp"
#include "exceptions.hpp"
#include "ByteStream.hpp"

class BitOStream
{
public:
       
	BitOStream(std::string fileName);
	void WriteString(std::string str);
	template<typename T> void WriteInteger(T buf, unsigned short bitCount)
	{
		WriteBigEndian(buf, bitCount);
    }
	template<typename T> void operator<<(T& op)
	{
		WriteInteger(&op, sizeof(T));
	};

	void WriteAlignBuffer(uint8_t * buffer, size_t count)
	{
		byteBuffer.WriteBuffer(buffer, count);
	}

	//this method is very slow
	void WriteBuffer(uint8_t * buffer, size_t count)
	{
		int i = 0;
		while (count > 0)
		{
			WriteByte(buffer[i], BITSINBYTE);
			count--;
			i++;
		}
	}


	void WriteFlag(bool flag);

	//i very question in this method validity
	void AlignByte()
	{
		if (freeDigitsCount != BITSINBYTE)
		{
			WriteByte(0, freeDigitsCount - 1);
		}
	};
private:
	ByteOBuffer<IOBUFFERSIZE> byteBuffer;
	void WriteByte(uint8_t byte, unsigned short bitCount = BITSINBYTE);
	//this method may have some problems
	template <typename T> void WriteBigEndian(T buf, unsigned short bitCount = BITSINBYTE)
 	{
		if (std::numeric_limits<T>::digits < bitCount) throw TypeOverflow();
		if (bitCount > BITSINBYTE)
		{
		//	unsigned short counter = 0;
			while (bitCount / BITSINBYTE > 0)
			{
				/*T byte = buf;
				bitCount -= BITSINBYTE;
				byte <<= counter;
				byte >>= bitCount + counter;
				WriteByte(byte);
				counter += BITSINBYTE;*/
				bitCount -= BITSINBYTE;
				T byte = (buf >> bitCount) & 0xFF;
				WriteByte(byte);
			}
			T byte = buf;
			byte <<= sizeof(T) * BITSINBYTE - bitCount;
			byte >>= sizeof(T) * BITSINBYTE - bitCount;
		/*	byte <<= counter;
			byte >>= counter + bitCount;*/
			if (bitCount != 0) WriteByte(byte, bitCount);
		}
		else 
		{
			WriteByte(buf, bitCount);
		}
	};
	unsigned int freeDigitsCount;
};
#endif
