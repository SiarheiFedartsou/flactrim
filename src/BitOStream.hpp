#ifndef BITOSTREAM_H
#define BITOSTREAM_H

#include <stdint.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>
#include "defines.hpp"
#include "exceptions.hpp"



template<size_t size>
class ByteOBuffer
{
public:
	void Init(std::string fileName)
	{
		fStream.open(fileName.c_str() , std::ios::binary);
		currentPos = 0;
		currentByte = 0;
	}

	void Flush()
	{
		fStream.write((char *)byteBuffer, currentPos);
		currentPos = 0;
	}

	void WriteByte(uint8_t byte)
	{
		byteBuffer[currentPos] = byte;
		currentPos++;
		if (currentPos - 1 == size) Flush();
	}

	void WriteBuffer(uint8_t * buffer, size_t count)
	{
		for (int i = 0; i < count; i++) WriteByte(buffer[i]);
	}

	uint8_t GetCurrentByte()
	{
		return currentByte;
	}

	void SetCurrentByte(uint8_t _currentByte)
	{
		currentByte = _currentByte;		
	}



	~ByteOBuffer()
	{
		Flush();
		fStream.close();
	}


private:
	uint8_t byteBuffer[size];
	size_t currentPos;
	std::ofstream fStream;
	uint8_t currentByte;
};

class BitOStream
{
public:
       
	BitOStream(std::string fileName);
	void WriteString(std::string str);
	template<typename T> void WriteInteger(T buf, unsigned short bitCount)
	{
		WriteLittleEndian(buf, bitCount);
    }
	template<typename T>
	void operator<<(T& op)
	{
		WriteInteger(&op, sizeof(T));
	};
private:
	ByteOBuffer<8192> byteBuffer;
	void WriteByte(uint8_t byte, unsigned short bitCount = BITSINBYTE);
	//this method may have some problems
	template <typename T> void WriteLittleEndian(T buf, unsigned short bitCount = BITSINBYTE)
 	{
		if (std::numeric_limits<T>::digits < bitCount) throw TypeOverflow();
		if (bitCount > BITSINBYTE)
		{
			unsigned short counter = 0;
			while (bitCount / BITSINBYTE > 0)
			{
				T byte = buf;
				bitCount -= BITSINBYTE;
				byte <<= counter;
				byte >>= bitCount + counter;
				WriteByte(byte);
				counter += BITSINBYTE;
			}
			T byte = buf;
			byte <<= counter;
			byte >>= counter + bitCount;
			if (bitCount != 0) WriteByte(byte, bitCount);
		}
		else WriteByte(buf, bitCount);
	}; 
       unsigned int freeDigitsCount;
};
#endif
