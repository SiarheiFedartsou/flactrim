/*
 * File:   main.cpp
 * Author: siarhei
 *
 * Created on 5 Январь 2010 г., 19:00
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include "BitIStream.hpp"
#include "BitOStream.hpp"
#include "flac_defines.hpp"
#include "Trimmer.hpp"
namespace po = boost::program_options;
using namespace std;


uint16_t GetUnencodedSamplesCount(uint8_t frameBitsPerSample, uint8_t siBitsPerSample, uint8_t order)
{
	switch (frameBitsPerSample)
	{
		case 0b000:
			return siBitsPerSample * order;
		case 0b001:
			return 8 * order;
		case 0b010:
			return 12 * order;
		case 0b100:
			return 16 * order;
		case 0b101:
			return 20 * order;
		case 0b110:
			return 24 * order;
		default:
			return 0;
	}
}

uint8_t GetBitsPerSample(uint8_t frameBitsPerSample, uint8_t siBitsPerSample)
{
	switch (frameBitsPerSample)
	{
		case 0b001:
			return 8;
		case 0b010:
			return 12;
		case 0b100:
			return 16;
		case 0b101:
			return 20;
		case 0b110:
			return 24;
		default:
			return siBitsPerSample + 1;
	}
}

uint16_t GetUnencodedSubblockCount(FLACFrameHeader * fh, uint8_t siBlockSize, uint8_t siBitsPerSample)
{
	if (fh->BlockSize == 0b0000) return siBlockSize * GetBitsPerSample(fh->BitsPerSample, siBitsPerSample);
	else if (fh->BlockSize == 0b0001) return 192 * GetBitsPerSample(fh->BitsPerSample, siBitsPerSample);
	else if (0b0010 <= fh->BlockSize && fh->BlockSize <= 0b0101 ) return 576 * (pow(2, fh->BlockSize - 2)) *  GetBitsPerSample(fh->BitsPerSample, siBitsPerSample);
	else if (fh->BlockSize == 0b0110 || fh->BlockSize == 0b0111) return (fh->VariableBlockSize + 1) * GetBitsPerSample(fh->BitsPerSample, siBitsPerSample);
	else return 256 * pow(2, fh->BlockSize - 8) * GetBitsPerSample(fh->BitsPerSample, siBitsPerSample);

}

int main(int argc, char** argv)
{
	po::options_description desc("Usage");
	desc.add_options()
		("help", "this help")
		("input-file", po::value<vector<string> >(), "input file");
	po::positional_options_description p;
	p.add("input-file", -1);
	po::variables_map vm;

	po::store(po::command_line_parser(argc, argv).
          options(desc).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("help")/* || vm.empty()*/)
	{
		cout << desc;
		return 0;
	}

	if (vm.count("input-file"))
	{
		vector<string> input_files = vm["input-file"].as<vector<string> >();
    	cout << "Input files are: ";
		for_each(input_files.begin(), input_files.end(), [](string file) { cout << file; });
		cout << endl;
		return 0;
	}

	BitIStream bs("/home/miksayer/Files/Music/4.flac");
	BitOStream bos("/home/miksayer/Files/Music/10.test");

   /* if (bs.GetString(3) == "ID3")
    {
    	bs.Skip(3);
    	uint32_t id3size = 0;
    	uint32_t b1 = 0, b2 = 0, b3 = 0, b4 = 0;
    	bs.GetInteger(&b1, 8);
    	bs.GetInteger(&b2, 8);
    	bs.GetInteger(&b3, 8);
    	bs.GetInteger(&b4, 8);
    	id3size = (b1 << 21) | (b2 << 14) | (b3 << 7) | b4;
    	bs.Skip(id3size);
    }*/
	uint64_t flac_null = 0;
	uint16_t minBlockSize = 0; //minimal size of block in samples in current stream
	uint16_t maxBlockSize = 0; //maximal size of block in samples in current stream
	uint32_t minFrameSize = 0; //minimal size of frame in bytes in current stream
	uint32_t maxFrameSize = 0; //maximal size of frame in bytes in current stream
	uint32_t samplingHz = 0; //sampling in Hz
	uint8_t channelsCount = 0; //(channels count - 1)
	uint8_t bitsPerSample = 0; //(bits per sample - 1)
	uint64_t streamSamplesCount = 0; //number of samples in the stream
	uint8_t md5[16]; //MD5
	uint8_t * buf;

    if (bs.ReadString(4) != "fLaC")
    {
    	std::cerr << "No FLAC file" << std::endl;
    	return 1;
    }
	bos.WriteString("fLaC");
    FLACMetaBlockHeader mbh;
	mbh.IsLastBlock = false;

	while (!mbh.IsLastBlock)
	{
		ReadMetaBlockHeader(bs, &mbh);
		WriteMetaBlockHeader(bos, &mbh);
		switch (mbh.BlockType)
		{
			case FLAC_META_STREAMINFO:
				bs.ReadInteger(&minBlockSize, 16);
				bs.ReadInteger(&maxBlockSize, 16);
				bs.ReadInteger(&minFrameSize, 24);
				bs.ReadInteger(&maxFrameSize, 24);
				bs.ReadInteger(&samplingHz, 20);
				bs.ReadInteger(&channelsCount, 3);
				bs.ReadInteger(&bitsPerSample, 5);
				bs.ReadInteger(&streamSamplesCount, 36);
				bs.ReadAlignBuffer(md5, 16);

 

				bos.WriteInteger(minBlockSize, 16);
				bos.WriteInteger(maxBlockSize, 16);
				bos.WriteInteger(minFrameSize, 24);
				bos.WriteInteger(maxFrameSize, 24);
				bos.WriteInteger(samplingHz, 20);
				bos.WriteInteger(channelsCount, 3);
				bos.WriteInteger(bitsPerSample, 5);
				bos.WriteInteger(streamSamplesCount, 36);
				bos.WriteAlignBuffer(md5, 16);

			break;
			case FLAC_META_SEEKTABLE:
			case FLAC_META_APPLICATION:
			case FLAC_META_VORBIS_COMMENT:
			case FLAC_META_CUESHEET:
			case FLAC_META_PADDING:
				buf = new uint8_t[mbh.BlockSize];
				bs.ReadAlignBuffer(buf, mbh.BlockSize);
				bos.WriteAlignBuffer(buf, mbh.BlockSize);
				delete[] buf;
			break;

			default:
				cerr << "Warning: unknown block type" << endl;
				buf = new uint8_t[mbh.BlockSize];
				bs.ReadAlignBuffer(buf, mbh.BlockSize);
				bos.WriteAlignBuffer(buf, mbh.BlockSize);
				delete[] buf;
			break;
		}
				
	}
//	while (mbh.IsLastBlock == 0);

//	for (int i = 0; i < 512; i++)
//	{
		FLACFrameHeader fh;
		ReadFrameHeader(bs, &fh);
		WriteFrameHeader(bos, &fh);

		for (int j = 0; j <= channelsCount; j++)
		{
			uint16_t constant_sample = 0;
			uint8_t * buf;
			uint8_t linearPredictor = 0;
			int8_t spredictorShift = 0;
			uint8_t predictorShift = 0;
			uint64_t n = 0;
			uint16_t unencPredCoeffs = 0;
			FLACSubframeHeader sfh;
			ReadSubframeHeader(bs, &sfh);
		//	cout << (int)sfh.type << endl;
		//	cout << (int)sfh.order << endl;
		//	cout << sfh.waste << endl;
			WriteSubframeHeader(bos, &sfh);

		//	cout << sfh.type << endl;

			switch (sfh.type)
			{
				case FLAC_SF_CONSTANT:
					bs.ReadInteger(&constant_sample, 16);
					bos.WriteInteger(constant_sample, 16);
				break;
				case FLAC_SF_LPC:
					n = GetUnencodedSamplesCount(fh.BitsPerSample, bitsPerSample, sfh.order);
					buf = new uint8_t[n / BITSINBYTE];
					bs.ReadBuffer(buf, n / BITSINBYTE);
					bos.WriteBuffer(buf, n / BITSINBYTE);
					delete[] buf;
					bs.ReadInteger(&linearPredictor, 4);
					bos.WriteInteger(linearPredictor, 4);
					linearPredictor++;
				//	std::cout << linearPredictor << endl;
					bs.ReadInteger(&predictorShift, 5);
					bos.WriteInteger(predictorShift, 5);
				//	std::cout << (int)sfh.order * linearPredictor << endl;
					std::cout <<(unsigned int) sfh.order << endl;
std::cout << (unsigned int)linearPredictor << endl;
					if (linearPredictor = 0b1111) cout << "e" << endl;
					n = sfh.order * linearPredictor;
					bs.ReadInteger(&unencPredCoeffs, n);
					bos.WriteInteger(unencPredCoeffs, n);
				break;
				case FLAC_SF_VERBATIM:
					n = GetUnencodedSubblockCount(&fh, maxBlockSize, bitsPerSample);
					buf = new uint8_t[n / BITSINBYTE];
					bs.ReadBuffer(buf, n / BITSINBYTE);
					bos.WriteBuffer(buf, n / BITSINBYTE);
					delete[] buf;
				break;
				case FLAC_SF_FIXED:
				break;
				default:
				break;
			}
		}
//	}
//	bos.AlignByte();

	uint16_t crc16;
	bs.ReadInteger(&crc16, 16);
	bos.WriteInteger(crc16, 16);

	return (EXIT_SUCCESS);
}

