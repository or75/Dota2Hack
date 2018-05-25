#include "DllMain.h"

#include "shared/!GlobalTypes_enums.hpp"
#include "shared/client.dll_classes.hpp"

string BaseDir = "";
string LogFile = "";

#define MAX_ENTITIES_IN_LIST 512
#define MAX_ENTITY_LISTS 64
#define MAX_TOTAL_ENTITIES MAX_ENTITIES_IN_LIST * MAX_ENTITY_LISTS

void AddLog( const char * format , ... )
{
	char pszMessage[1024] = { 0 };
	va_list args;
	va_start( args , format );
	vsprintf_s( pszMessage , format , args );
	CSX::Log::AddLog( pszMessage );
	va_end( args );
}

class CEntityIdentities
{
public:
	CEntityIdentity m_pIdentities[MAX_ENTITIES_IN_LIST];
};

class EntityIdentityList
{
public:
	CEntityIdentities* m_pIdentityList;
};

class CGameEntitySystem
{
public:
	C_BaseEntity* GetBaseEntity( int Index )
	{
		C_BaseEntity* ret = nullptr;

		if ( Index >= -1 && Index <= MAX_TOTAL_ENTITIES )
		{
			if ( ( Index >> 9 ) < MAX_ENTITY_LISTS )
			{
				auto pEntityIdentities = ( &m_pEntityList[( Index >> 9 ) + 1] )->m_pIdentityList;

				if ( pEntityIdentities )
				{
					auto pEntityIdentity = &pEntityIdentities->m_pIdentities[Index & ( MAX_ENTITIES_IN_LIST - 1 )];

					if ( pEntityIdentity )
					{
						int EntityIndex = ( pEntityIdentity->Index & ( MAX_ENTITIES_IN_LIST - 1 ) );

						if ( EntityIndex == Index )
						{
							ret = *(C_BaseEntity**)pEntityIdentity;
						}
					}
				}
			}
		}

		return ret;
	}
	int GetHighestEntityIndex()
	{
		return *(PINT)( (DWORD_PTR)this + 0x12040 );
	}
private:
	void* unknown1;
	void* unknown2;
	EntityIdentityList m_pEntityList[MAX_ENTITY_LISTS];
};

CGameEntitySystem* g_pGameEntitySystem = nullptr;

typedef bool ( __fastcall* DoIncludeScriptPtr )( const char *pszScriptName , HSCRIPT hScope );

DWORD WINAPI CheatEntry( LPVOID lpThreadParameter )
{
	AddLog( "[+] CheatEntry\n" );
	
	DWORD_PTR dwGameEntitySystemPattern = (DWORD_PTR)CSX::Memory::FindPattern( XorStr( "client.dll" ) , XorStr( "48 8B 0D ? ? ? ? 33 FF 8B 81 40 20 01 00 FF C0" ) , 0 );
	DWORD_PTR dwIScriptManagerPattern = (DWORD_PTR)CSX::Memory::FindPattern( XorStr( "vscript.dll" ) , XorStr( "48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 5C 24 08" ) , 0 );

	AddLog( "[+] dwGameEntitySystemPattern: %p\n" , dwGameEntitySystemPattern );
	AddLog( "[+] dwIScriptManagerPattern: %p\n" , dwIScriptManagerPattern );

	DoIncludeScriptPtr DoIncludeScript = (DoIncludeScriptPtr)CSX::Memory::FindPattern(
		XorStr( "client.dll" ) , 
		XorStr( "40 57 48 ? ? ? ? ? ? 48 83 3D 07 0B 78 01 00 48 8B F9 0F 84" ) 
		, 0 );

	if ( dwGameEntitySystemPattern && dwIScriptManagerPattern )
	{
		DWORD_PTR dwGameEntitySystem = *(PDWORD_PTR)( ( dwGameEntitySystemPattern + 7 ) + *(PDWORD)( dwGameEntitySystemPattern + 3 ) );

		g_pGameEntitySystem = (CGameEntitySystem*)dwGameEntitySystem;

		AddLog( "[+] g_pGameEntitySystem: %p\n" , g_pGameEntitySystem );

		int EntityCount = g_pGameEntitySystem->GetHighestEntityIndex() + 1;

		AddLog( "[+] GetHighestEntityIndex: %i\n" , EntityCount );

		if ( EntityCount )
		{
			for ( int EntityIndex = 0; EntityIndex < g_pGameEntitySystem->GetHighestEntityIndex() + 1; EntityIndex++ )
			{
				auto pEntity = g_pGameEntitySystem->GetBaseEntity( EntityIndex );

				if ( pEntity )
				{
					const char* EntityClass = pEntity->Schema_DynamicBinding()->GetBindingName();

					AddLog( "[+] pEntity: %p , %i , %s\n" , pEntity , EntityIndex , EntityClass );
				}
			}
		}
	}

	if ( DoIncludeScript )
	{
		AddLog( "[+] DoIncludeScript: %p\n" , DoIncludeScript );
		
		MessageBoxA( 0 , "ok to include" , "success" , MB_ICONINFORMATION );
		
		DoIncludeScript( "cheat" , 0 );
	}

	return 0;
}

BOOL WINAPI DllMain( HINSTANCE hinstDLL , DWORD fdwReason , LPVOID lpReserved )
{
	if ( fdwReason == DLL_PROCESS_ATTACH )
	{
		char szDllFile[MAX_PATH] = { 0 };
		GetModuleFileNameA( hinstDLL , szDllFile , MAX_PATH );

		BaseDir = szDllFile;
		BaseDir = BaseDir.substr( 0 , BaseDir.find_last_of( "\\/" ) );

		memset( szDllFile , 0 , MAX_PATH );

		BaseDir += '\\';

		LogFile = BaseDir + "dota2.log";

		CSX::Log::SetLogFile( LogFile.c_str() );

		CreateThread( 0 , 0 , CheatEntry , lpReserved , 0 , 0 );
		return TRUE;
	}

	return FALSE;
}