#pragma once

/* This header contains only auxiliary common functions */

#include <string>

#ifdef _DEBUG
#define assert(x, s)											\
if (!(x)) throw std::exception(('\'' + std::string(s) + '\''	\
+ "\nFUNCTION: " + std::string(__FUNCTION__)					\
+ "\nFILE: "	+ std::string(__FILE__) +  						\
+ "\nLINE: " + std::to_string(__LINE__)).c_str())		

#else

#define assert(x,s) if (!(x))	\
throw std::exception(('\'' + std::string(s) + '\'').c_str())

#endif // DEBUG
