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
#include <list>
#include <iostream>
#include "BitIStream.hpp"
#include "BitOStream.hpp"

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



int main(int argc, char** argv)
{
	BitIStream bs("/home/miksayer/Files/Music/4.flac");
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
    MetaBlockHeader mbh;
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

	bs.Skip(2);








    //BitOStream bos("/home/miksayer/1.txt");
    return (EXIT_SUCCESS);
}

