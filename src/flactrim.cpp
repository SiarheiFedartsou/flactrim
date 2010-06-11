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
    if (bs.ReadString(4) != "fLaC")
    {
    	std::cerr << "No FLAC file" << std::endl;
    	return 1;
    }
	bos.WriteString("fLaC");
    MetaBlockHeader mbh;
    ReadMetaBlockHeader(bs, &mbh);
	do 
	{
		WriteMetaBlockHeader(bos, &mbh);
		ReadMetaBlockHeader(bs, &mbh);
	}
	while (mbh.IsLastBlock == 0);
    /*std::cout << "Is last: " << mbh.IsLastBlock << std::endl;
    std::cout << "Type: " << mbh.BlockType << std::endl;
    std::cout << "Size: " << mbh.BlockSize << std::endl;
    bs.Skip(mbh.BlockSize);
    ReadMetaBlockHeader(bs, &mbh);
    std::cout << "Is last: " << mbh.IsLastBlock << std::endl;
    std::cout << "Type: " << mbh.BlockType << std::endl;
    std::cout << "Size: " << mbh.BlockSize << std::endl;
    bs.Skip(mbh.BlockSize);
	ReadMetaBlockHeader(bs, &mbh);
    std::cout << "Is last: " << mbh.IsLastBlock << std::endl;
    std::cout << "Type: " << mbh.BlockType << std::endl;
    std::cout << "Size: " << mbh.BlockSize << std::endl;
    bs.Skip(mbh.BlockSize);
    ReadMetaBlockHeader(bs, &mbh);
    std::cout << "Is last: " << mbh.IsLastBlock << std::endl;
    std::cout << "Type: " << mbh.BlockType << std::endl;
    std::cout << "Size: " << mbh.BlockSize << std::endl;
    bs.Skip(mbh.BlockSize);
    uint16_t code = 0;
    bs.ReadInteger(&code, 14);
    std::cout << code << std::endl;
	uint8_t v0 = 0;
	bs.ReadInteger(&v0, 2);
	std::cout << ((v0 == 0) ? "0" : "1") << std::endl;
	v0 = 0;
	bs.ReadInteger(&v0, 4);
	std::cout << (int)v0 << std::endl;
	v0 = 0;
	bs.ReadInteger(&v0, 4);
	std::cout <<  (int)v0 << std::endl;

	v0 = 0;
	bs.ReadInteger(&v0, 4);
	std::cout <<  (int)v0 << std::endl;


	v0 = 0;
	bs.ReadInteger(&v0, 3);
	std::cout <<  (int)v0 << std::endl;


	v0 = 0;
	bs.ReadInteger(&v0, 1);
	std::cout <<  (int)v0 << std::endl;

	bs.Skip(4);

	v0 = 0;
	bs.ReadInteger(&v0, 8);
	std::cout <<  (int)v0 << std::endl;

	v0 = 0;
	bs.ReadInteger(&v0, 1);
	std::cout <<  (int)v0 << std::endl;

	v0 = 0;
	bs.ReadInteger(&v0, 6);
	std::cout <<  (int)v0 << std::endl;

	v0 = 0;
	bs.ReadInteger(&v0, 1);
	std::cout <<  (int)v0 << std::endl;

	bs.Skip(2);*/







    //BitOStream bos("/home/miksayer/1.txt");
    return (EXIT_SUCCESS);
}

