#ifndef BITISTREAM_H
#define BITISTREAM_H

#include <iostream>
#include <fstream>
#include <limits>
#include <exception>
#include <stdint.h>
#include "defines.hpp"

class TypeOverflow : public std::exception {};
class UnexpectedEOF : public std::exception {};

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

template<size_t size>
class ByteIBuffer
{
public:
	void Init(std::string fileName)
	{
		fStream.open(fileName.c_str() , std::ios::binary);
		ReadMoreBytes();
	};
	operator char * const()
 	{
		return (char *)byteBuffer;
	};
	uint8_t GetNextByte()
	{
		currentPos++;
		if (IsNeedMoreBytes()) ReadMoreBytes();
		return byteBuffer[currentPos - 1];
	};

	uint8_t PeekNextByte()
	{
		return byteBuffer[currentPos];
	}

	//This function is not working normaly, if count > size
	//Also this function is not tested in extreme conditions :-) 
	//But it is not very importantly, because it is only used to skip ID3 tags at begin of FLAC file(see flactrim.cpp)
	void PeekBuffer(uint8_t * buffer, size_t count)
	{
		size_t currentPeekPos = currentPos;
		int i = 0;
		while (currentPeekPos <= currentBufferSize && count != 0)
		{
			buffer[i] = byteBuffer[currentPeekPos];
			currentPeekPos++;
			i++;
			buffer[i] = 0;
			count--;
		}

		if (count == 0) return;
		else
		{
			size_t currentPeekBufferSize = 0;
			const size_t bufferSize = size;
			const size_t filePos = fStream.tellg();
	    	fStream.read((char *)peekByteBuffer, bufferSize);
			fStream.seekg(filePos);
	    	currentPeekBufferSize = fStream.gcount() - 1;
	    	currentPeekPos = 0;

			while (currentPeekPos <= currentPeekBufferSize && count != 0)
		    {	
				buffer[i] = byteBuffer[currentPeekPos];
				currentPeekPos++;
				i++;
				buffer[i] = 0;
				count--;
			}


		}
	};



	void SkipByte()
	{
		currentPos++;
		if (IsNeedMoreBytes()) ReadMoreBytes();
	};

	~ByteIBuffer()
	{
		fStream.close();
	};
	  

private:
	bool IsNeedMoreBytes() { return currentPos > currentBufferSize; }
	void ReadMoreBytes()
	{
        const size_t bufferSize = size;
	    fStream.read((char *)byteBuffer, bufferSize);
		if (fStream.gcount() == 0) throw UnexpectedEOF();
	    currentBufferSize = fStream.gcount() - 1;
	    currentPos = 0;
	};
	uint8_t byteBuffer[size];
    size_t currentPos;
    size_t currentBufferSize;

	uint8_t peekByteBuffer[size];
	std::ifstream fStream;
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
	template <typename T> void ReadInteger(T * buf, unsigned short bitCount)
	{
		ReadBigEndian(buf, bitCount);
	};
	template<typename T> void operator>>(T& op)
	{
       	ReadInteger(&op, sizeof(T));
	}; 
	//template <typename T> void PeekInteger(T * buf, unsigned short bitCount) {};
	void Skip(unsigned int byteCount)
	{
		while (byteCount--) byteBuffer.SkipByte();
    }

private:
	ByteIBuffer<8192> byteBuffer;
	unsigned short firstByteDigitsCount;
	uint8_t ReadByte(unsigned short bitCount = BITSINBYTE);

    template <typename T> void ReadBigEndian(T * buf, unsigned short bitCount)
	{
	   if (std::numeric_limits<T>::digits < bitCount) throw TypeOverflow();
	   T ret = 0;
	   while (bitCount / BITSINBYTE > 0)
	   {
		   ret += ReadByte();
		   ret <<= BITSINBYTE;
		   bitCount -= BITSINBYTE;
	   }
	   ret >>= BITSINBYTE;
	   ret <<= bitCount;
	   if (bitCount != 0) ret += ReadByte(bitCount);
	   else ret >>= bitCount;
	   *buf = ret;
    };

};

#endif
