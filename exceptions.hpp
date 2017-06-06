#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <exception>

class readError : public std::runtime_error {
	public:
		virtual const char* what() const throw() {
			return "Fatal error reading definition files.";
		}

};

#endif
