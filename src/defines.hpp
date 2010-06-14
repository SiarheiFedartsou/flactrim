#ifndef DEFINES_H
#define DEFINES_H

//#define BITSINBYTE 8

const uint8_t BITSINBYTE = 8;

enum MetaBlockType
{
	STREAMINFO = 0,
	PADDING = 1,
	APPLICATION = 2,
	SEEKTABLE = 3,
	VORBIS_COMMENT = 4,
	CUESHEET = 5
};

#endif
