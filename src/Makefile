CC=g++
CFLAGS=-Wall
CALLFLAGS=-Wall -Wextra -Wcast-qual -Wconversion -Wredundant-decls -Wshadow -Wsign-conversion -Wunreachable-code -Wcast-align -Wformat-nonliteral -Wformat-security -Wformat-y2k -Winit-self -Wlogical-op -Wmissing-format-attribute -Wmissing-include-dirs -Woverlength-strings
LIBS=-lboost_program_options
flactrim: flactrim.cpp defines.hpp BitIStream.cpp BitIStream.hpp BitOStream.cpp BitOStream.hpp
		$(CC) $(CFLAGS) flactrim.cpp BitIStream.cpp BitOStream.cpp -o flactrim -std=c++0x $(LIBS)
		
