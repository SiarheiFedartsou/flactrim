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

namespace po = boost::program_options;
using namespace std;

struct MetaBlockHeader
{
	uint32_t IsLastBlock;
	uint32_t  BlockType;
	uint32_t BlockSize;
};

void ReadMetaBlockHeader(BitIStream& bs, MetaBlockHeader * mbh)
{
	bs.ReadInteger(&mbh->IsLastBlock, 1);
	bs.ReadInteger(&mbh->BlockType, 7);
	bs.ReadInteger(&mbh->BlockSize, 24);
}

void WriteMetaBlockHeader(BitOStream& bs, MetaBlockHeader * mbh)
{
	bs.WriteInteger(mbh->IsLastBlock, 1);
	bs.WriteInteger(mbh->BlockType, 7);
	bs.WriteInteger(mbh->BlockSize, 24);

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

//	bos.WriteInteger(1, 1);
//	bos.WriteInteger(103, 7);

	//std::cout << bs.PeekString(4) << std::endl;
	//std::cout << bs.ReadString(4) << std::endl;

	//BitOStream bs2("/home/miksayer/Files/Music/10.test");
	//bs2.WriteString("dfgfgfgfgfgfgdtret");
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
    MetaBlockHeader mbh;
    ReadMetaBlockHeader(bs, &mbh);

//streamSamplesCount = 494655;
//bos.WriteInteger(streamSamplesCount, 36);
//bos.WriteInteger(0, 4);

	do 
	{
		WriteMetaBlockHeader(bos, &mbh);
		switch (mbh.BlockType)
		{
			case STREAMINFO:
				bs.ReadInteger(&minBlockSize, 16);
				bs.ReadInteger(&maxBlockSize, 16);
				bs.ReadInteger(&minFrameSize, 24);
				bs.ReadInteger(&maxFrameSize, 24);
				bs.ReadInteger(&samplingHz, 20);
				bs.ReadInteger(&channelsCount, 3);
				bs.ReadInteger(&bitsPerSample, 5);
				bs.ReadInteger(&streamSamplesCount, 36);
			//	cout << streamSamplesCount << endl;
				bs.ReadBuffer(md5, 16);



				bos.WriteInteger(minBlockSize, 16);
				bos.WriteInteger(maxBlockSize, 16);
				bos.WriteInteger(minFrameSize, 24);
				bos.WriteInteger(maxFrameSize, 24);
				bos.WriteInteger(samplingHz, 20);
				bos.WriteInteger(channelsCount, 3);
				bos.WriteInteger(bitsPerSample, 5);
				bos.WriteInteger(streamSamplesCount, 36);
				bos.WriteBuffer(md5, 16);

			break;

			case PADDING:
				buf = new uint8_t[mbh.BlockSize];
				bs.ReadBuffer(buf, mbh.BlockSize);
				bos.WriteBuffer(buf, mbh.BlockSize);
				delete[] buf;
			break;

		/*	case APPLICATION:

		//	break;
			
			case SEEKTABLE:

		//	break;

			case VORBIS_COMMENT:
			
		//	break;

			case CUESHEET:

	//		break;*/

			default:
				buf = new uint8_t[mbh.BlockSize];
				bs.ReadBuffer(buf, mbh.BlockSize);
				bos.WriteBuffer(buf, mbh.BlockSize);
				delete[] buf;
			break;
		}
		ReadMetaBlockHeader(bs, &mbh);
		
	}
	while (mbh.IsLastBlock == 0);
    //BitOStream bos("/home/miksayer/1.txt");
    return (EXIT_SUCCESS);
}

