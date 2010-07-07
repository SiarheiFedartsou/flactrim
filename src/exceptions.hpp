#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

class TypeOverflow: public std::exception 
{
public:
	virtual const char* what() const throw()
	{
		return "Type overflow";
	}
};

class UnexpectedEOF : public std::exception 
{
public:
	virtual const char* what() const throw()
	{
		return "Unexpected end of file";
	}

};

class FLACError : public std::exeception
{
public:
	virtual const char* what() const throw()
	{
		return "FLAC Error";
	}
};

class NoFLACFile : FLACError
{
public:
	virtual const char* what() const throw()
	{
		return "No FLAC File";
	}
};



#endif

