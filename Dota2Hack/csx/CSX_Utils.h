#pragma once

#include "CSX_Engine.h"

namespace CSX
{
	namespace Utils
	{
		MODULEINFO GetModuleInfo( const char* szModule );

		/* Wait dwMsec Load Module */
		bool IsModuleLoad( const char* zModule , DWORD dwMsec = 5000 );

		// convert int to char
		PCHAR itoa( int i );
		// convert char to int
		int atoi( PCHAR pStr1 );
	}
}