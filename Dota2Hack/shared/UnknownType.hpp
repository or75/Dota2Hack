#pragma once

#if defined(__x86_64__) || defined(_WIN64)
#define X64BITS
#endif

typedef unsigned char uint8;
typedef signed char int8;

#if defined( _WIN32 )

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

typedef int64 lint64;
typedef uint64 ulint64;

#ifdef X64BITS
typedef __int64 intp;				// intp is an integer that can accomodate a pointer
typedef unsigned __int64 uintp;		// (ie, sizeof(intp) >= sizeof(int) && sizeof(intp) >= sizeof(void *)
#else
typedef __int32 intp;
typedef unsigned __int32 uintp;
#endif

#else // _WIN32

#ifdef X64BITS
typedef long long intp;
typedef unsigned long long uintp;
#else
typedef int intp;
typedef unsigned int uintp;
#endif

#endif // else _WIN32

struct HSCRIPT__
{
	int unused;
};

class HSCRIPT
{
public:
	HSCRIPT__ * Self;
	HSCRIPT( void* Ptr )
	{
		Self = (HSCRIPT__*)Ptr;
	}
};

typedef intp UtlSymLargeId_t;

#define UTL_INVAL_SYMBOL_LARGE  ((UtlSymLargeId_t)~0)

class CUtlSymbolLarge
{
public:
	// constructor, destructor
	CUtlSymbolLarge()
	{
		u.m_Id = UTL_INVAL_SYMBOL_LARGE;
	}

	CUtlSymbolLarge( UtlSymLargeId_t id )
	{
		u.m_Id = id;
	}
	CUtlSymbolLarge( CUtlSymbolLarge const& sym )
	{
		u.m_Id = sym.u.m_Id;
	}

	// operator=
	CUtlSymbolLarge& operator=( CUtlSymbolLarge const& src )
	{
		u.m_Id = src.u.m_Id;
		return *this;
	}

	// operator==
	bool operator==( CUtlSymbolLarge const& src ) const
	{
		return u.m_Id == src.u.m_Id;
	}

	// operator==
	bool operator==( UtlSymLargeId_t const& src ) const
	{
		return u.m_Id == src;
	}

	// operator==
	bool operator!=( CUtlSymbolLarge const& src ) const
	{
		return u.m_Id != src.u.m_Id;
	}

	// operator==
	bool operator!=( UtlSymLargeId_t const& src ) const
	{
		return u.m_Id != src;
	}

	// Gets at the symbol
	operator UtlSymLargeId_t const( ) const
	{
		return u.m_Id;
	}

	// Gets the string associated with the symbol
	inline const char* String() const
	{
		if ( u.m_Id == UTL_INVAL_SYMBOL_LARGE )
			return "";
		return u.m_pAsString;
	}

	inline bool IsValid() const
	{
		return u.m_Id != UTL_INVAL_SYMBOL_LARGE ? true : false;
	}

private:
	// Disallowed
	CUtlSymbolLarge( const char* pStr );       // they need to go through the table to assign the ptr

	union
	{
		UtlSymLargeId_t m_Id;
		char const *m_pAsString;
	} u;
};

template <unsigned int size = 0x1, class T = BadType>
class UnknownType {
public:
    T& Get() {
        return *(T*)&m_data;
    }

    template <class CustomT>
    CustomT& Get() {
        return *(CustomT*)&m_data;
    }

    operator T&() {
        return Get();
    }

protected:
    char m_data[size];
};

template <unsigned int size = 0x1, template <typename...> class TOuter = BadType, typename... Args>
class UnknownAtomicType {
public:
    TOuter<Args...>& Get() {
        return *(TOuter<Args...>*)&m_data;
    }

    template <typename CustomT>
    CustomT& Get() {
        return *(CustomT*)&m_data;
    }

protected:
    char m_data[size];
};