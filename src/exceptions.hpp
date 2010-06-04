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

#endif

