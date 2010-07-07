#ifndef BYTESTREAM_HPP
#define BYTESTREAM_HPP

#include <iostream>

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

	void ReadBuffer(uint8_t * buffer, size_t count)
	{
		for (size_t i = 0; i < count; i++) buffer[i] = GetNextByte();
	}



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
		if (currentPos == size) Flush();
		byteBuffer[currentPos] = byte;
		currentPos++;
	}

	void WriteBuffer(uint8_t * buffer, size_t count)
	{
		for (size_t i = 0; i < count; i++) WriteByte(buffer[i]);
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



#endif
