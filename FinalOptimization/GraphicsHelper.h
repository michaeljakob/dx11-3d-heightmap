#pragma once

#include <sstream>

#ifdef _DEBUG
#define GH_SHOW_CONSOLE
#endif

#ifdef GH_SHOW_CONSOLE
#define GH_LOGGING_ENABLED
#endif

#define GH_PI    (3.14159265358979323846)
#define GH_1BYPI ( 1.0 / GH_PI )

#define GH_DEGTORAD( degree ) ((degree) * (GH_PI / 180.0))
#define GH_RADTODEG( radian ) ((radian) * (180.0 / GH_PI))


template<typename Destination, typename Source>
inline Destination lexical_cast(Source source)
{
	Destination destination = Destination();
	std::stringstream s;
	s << source;
	s >> destination;

	return destination;
}

inline unsigned long getSystemTime() 
{
	SYSTEMTIME time;
	GetSystemTime(&time);
	return time.wSecond * 1000 + time.wMilliseconds;
}