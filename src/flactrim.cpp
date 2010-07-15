/*
 * File:   main.cpp
 * Author: siarhei
 *
 * Created on 5 Январь 2010 г., 19:00
 */

#include <vector>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>
#include "flac_defines.hpp"
#include "Trimmer.hpp"
#include "ByteStream.hpp"
#include <stdio.h>

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
/*{
ByteOBuffer<8> s;
s.Init("/home/miksayer/Files/Music/10.test");
s.WriteByte(0x61);
s.WriteByte(0x99);
s.WriteByte(0x67);
s.WriteByte(0xff);
s.WriteByte(0x01);
s.WriteByte(0x41);
s.WriteByte(0x31);
s.WriteByte(0x29);
s.WriteByte(0xfc);
s.WriteByte(0xc9);
s.WriteByte(0x61);
s.WriteByte(0x99);
s.WriteByte(0x67);
s.WriteByte(0xff);
s.WriteByte(0x01);
s.WriteByte(0x41);
s.WriteByte(0x31);
s.WriteByte(0x29);
s.WriteByte(0xfc);
s.WriteByte(0xc9);
}

{
	ByteIBuffer<8> s;
	s.Init("/home/miksayer/Files/Music/10.test");
		for (int i = 0; i < 12; i++)
		 printf("%x ", s.GetNextByte());
}*/

	Trimmer trimmer("/home/miksayer/Files/Music/4.flac");
	trimmer.CutTrack("/home/miksayer/Files/Music/10.test", 0, 0);

	return (EXIT_SUCCESS);
}

