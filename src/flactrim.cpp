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


int main(int argc, char** argv)
{
	po::options_description desc("Usage");
	desc.add_options()
		("help", "this help")
		("input-file", po::value<vector<string> >(), "input file");
	po::positional_options_description p;
	p.add("input-file", -1);
	po::variables_map vm;
jhhjh
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
//	while (mbh.IsLastBlock == 0);

//	for (int i = 0; i < 512; i++)
//	{
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

