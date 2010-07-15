#ifndef FLAC_DEFINES_HPP
#define FLAC_DEFINES_HPP

/*struct Sampling
{
	enum 
	{
		GET_FROM_STREAMINFO = 0b0000,
		KHZ8 = 0b0100,
		KHZ16 = 0b0101,
		KHZ2205 = 0b0110,
		KHZ24 = 0b0111,
		KHZ32 = 0b1000,
		KHZ441 = 0b1001,
		KHZ48 = 0b1010,
		KHZ96 = 0b1011,
		GET_8KHZ = 0b1100,
		GET_16HZ = 0b1101,
		GET_16DHZ = 0b1110
	};
	uint8_t code;
	
}*/


enum FLACMetaBlockType
{
	FLAC_META_STREAMINFO = 0,
	FLAC_META_PADDING = 1,
	FLAC_META_APPLICATION = 2,
	FLAC_META_SEEKTABLE = 3,
	FLAC_META_VORBIS_COMMENT = 4,
	FLAC_META_CUESHEET = 5,
	FLAC_META_PICTURE = 6
};


struct FLACMetaBlockHeader
{
	bool IsLastBlock;
	uint32_t  BlockType;
	uint32_t BlockSize;
};

struct FLACMetaStreamInfo 
{
	uint16_t MinBlockSize, MaxBlockSize;
	uint32_t MinFrameSize, MaxFrameSize;
	uint32_t Sampling;
	uint8_t ChannelsCount;
	uint8_t BitsPerSample;
	uint64_t StreamSamplesCount;
	uint8_t md5[16];
};

struct FLACFrameHeader
{
	uint16_t SyncCode;
	uint8_t Zero;
	uint8_t Zero2;
	uint8_t BlockSize;
	uint8_t Sampling;
	uint8_t ChannelsLocation;
	uint8_t BitsPerSample;
	uint8_t ControlSum;
	uint16_t VariableBlockSize;
	uint16_t VariableSampling;
	bool IsVariableBlockSize;
	bool IsVariableSampling;
	uint64_t UTF8Num;
};

enum FLACSubframeType
{
	FLAC_SF_CONSTANT = 0,
	FLAC_SF_FIXED = 1,
	FLAC_SF_LPC = 2,
	FLAC_SF_VERBATIM = 3
};


struct FLACSubframeHeader
{
	FLACSubframeType Type;
	uint64_t Waste;
	uint8_t Order;
	uint8_t Zero;
	bool IsWasted;
};

enum FLACCResidualCodingMethod
{
	RICE = 0,
	RICE2 = 1
};

#endif
