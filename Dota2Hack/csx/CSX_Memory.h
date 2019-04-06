#pragma once

#include "CSX_Engine.h"

namespace CSX
{
	namespace Memory
	{
		/* IDA Style 00 FF ?? */

		PVOID FindPattern( const char* pPattern , __int3264 dwStart , __int3264 dwEnd , DWORD dwOffset );
		PVOID FindPattern( const char* szModule , const char* pPattern , DWORD dwOffset );
	}
}