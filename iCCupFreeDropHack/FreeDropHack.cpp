#define _WIN32_WINNT 0x0501 
#define WINVER 0x0501 
#define NTDDI_VERSION 0x05010000
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include "crc.h"
/**********************************************************************
*
* Filename:    crc.c
*
* Description: Slow and fast implementations of the CRC standards.
*
* Notes:       The parameters for each supported CRC standard are
*				defined in the header file crc.h.  The implementations
*				here should stand up to further additions to that list.
*
*
* Copyright (c) 2000 by Michael Barr.  This software is placed into
* the public domain and may be used for any purpose.  However, this
* notice must not be changed or removed and no warranty is either
* expressed or implied by its publication or distribution.
**********************************************************************/

#include "crc.h"


/*
* Derive parameters from the standard-specific parameters in crc.h.
*/
#define WIDTH    (8 * sizeof(crc))
#define TOPBIT   (1 << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			((unsigned char) reflect((X), 8))
#else
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			(X)
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	((crc) reflect((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	(X)
#endif


/*********************************************************************
*
* Function:    reflect()
*
* Description: Reorder the bits of a binary sequence, by reflecting
*				them about the middle position.
*
* Notes:		No checking is done that nBits <= 32.
*
* Returns:		The reflection of the original data.
*
*********************************************************************/
static unsigned long
reflect( unsigned long data , unsigned char nBits )
{
	unsigned long  reflection = 0x00000000;
	unsigned char  bit;

	/*
	* Reflect the data about the center bit.
	*/
	for ( bit = 0; bit < nBits; ++bit )
	{
		/*
		* If the LSB bit is set, set the reflection of it.
		*/
		if ( data & 0x01 )
		{
			reflection |= ( 1 << ( ( nBits - 1 ) - bit ) );
		}

		data = ( data >> 1 );
	}

	return ( reflection );

}	/* reflect() */


/*********************************************************************
*
* Function:    crcSlow()
*
* Description: Compute the CRC of a given message.
*
* Notes:
*
* Returns:		The CRC of the message.
*
*********************************************************************/
crc
crcSlow( unsigned char const message[ ] , int nBytes )
{
	crc            remainder = INITIAL_REMAINDER;
	int            byte;
	unsigned char  bit;


	/*
	* Perform modulo-2 division, a byte at a time.
	*/
	for ( byte = 0; byte < nBytes; ++byte )
	{
		/*
		* Bring the next byte into the remainder.
		*/
		remainder ^= ( REFLECT_DATA( message[ byte ] ) << ( WIDTH - 8 ) );

		/*
		* Perform modulo-2 division, a bit at a time.
		*/
		for ( bit = 8; bit > 0; --bit )
		{
			/*
			* Try to divide the current data bit.
			*/
			if ( remainder & TOPBIT )
			{
				remainder = ( remainder << 1 ) ^ POLYNOMIAL;
			}
			else
			{
				remainder = ( remainder << 1 );
			}
		}
	}

	/*
	* The final remainder is the CRC result.
	*/
	return ( REFLECT_REMAINDER( remainder ) ^ FINAL_XOR_VALUE );

}   /* crcSlow() */


crc  crcTable[ 256 ];


/*********************************************************************
*
* Function:    crcInit()
*
* Description: Populate the partial CRC lookup table.
*
* Notes:		This function must be rerun any time the CRC standard
*				is changed.  If desired, it can be run "offline" and
*				the table results stored in an embedded system's ROM.
*
* Returns:		None defined.
*
*********************************************************************/
void
crcInit( void )
{
	crc			   remainder;
	int			   dividend;
	unsigned char  bit;


	/*
	* Compute the remainder of each possible dividend.
	*/
	for ( dividend = 0; dividend < 256; ++dividend )
	{
		/*
		* Start with the dividend followed by zeros.
		*/
		remainder = dividend << ( WIDTH - 8 );

		/*
		* Perform modulo-2 division, a bit at a time.
		*/
		for ( bit = 8; bit > 0; --bit )
		{
			/*
			* Try to divide the current data bit.
			*/
			if ( remainder & TOPBIT )
			{
				remainder = ( remainder << 1 ) ^ POLYNOMIAL;
			}
			else
			{
				remainder = ( remainder << 1 );
			}
		}

		/*
		* Store the result into the table.
		*/
		crcTable[ dividend ] = remainder;
	}

}   /* crcInit() */


/*********************************************************************
*
* Function:    crcFast()
*
* Description: Compute the CRC of a given message.
*
* Notes:		crcInit() must be called first.
*
* Returns:		The CRC of the message.
*
*********************************************************************/
crc
crcFast( unsigned char const message[ ] , int nBytes )
{
	crc	           remainder = INITIAL_REMAINDER;
	unsigned char  data;
	int            byte;


	/*
	* Divide the message by the polynomial, a byte at a time.
	*/
	for ( byte = 0; byte < nBytes; ++byte )
	{
		data = REFLECT_DATA( message[ byte ] ) ^ ( remainder >> ( WIDTH - 8 ) );
		remainder = crcTable[ data ] ^ ( remainder << 8 );
	}

	/*
	* The final remainder is the CRC.
	*/
	return ( REFLECT_REMAINDER( remainder ) ^ FINAL_XOR_VALUE );

}   /* crcFast() */


struct Packet
{
	DWORD PacketClassPtr;	//+00, some unknown, but needed, Class Pointer
	BYTE* PacketData;		//+04
	DWORD _1;				//+08, zero
	DWORD _2;				//+0C, ??
	DWORD Size;				//+10, size of PacketData
	DWORD _3;				//+14, 0xFFFFFFFF
};


typedef void *( __fastcall * GAME_SendPacket_p ) ( Packet* packet , DWORD zero );
GAME_SendPacket_p GAME_SendPacket;


int GameDll = 0;



void SendPacket( BYTE* packetData , DWORD size )
{
	// @warning: this function thread-unsafe, do not use it in other thread.
	// note: this is very useful function, in fact this function
	// does wc3 ingame action, so you can use it for anything you want,
	// including unit commands and and gameplay commands,
	// i suppose its wc3 single action W3GS_INCOMING_ACTION (c) wc3noobpl.
	int size1 = 0x930000 + GameDll;
	int xgameaddr = GameDll;
	int gameaddr2 = 0;
	int gameaddr3 = 0x70;
	int gameaddr4 = 0x900;
	int gameaddr5 = 0xD000;


	Packet packet;
	memset( &packet , 0 , sizeof( Packet ) );

	packet.PacketClassPtr = size1; // Packet Class

	packet.PacketData = packetData;
	packet.Size = size;
	packet._3 = 0xFFFFFFFF;
	size1 = size1 + 0x2D00;
	packet.PacketClassPtr = size1 + 0x2C;
	gameaddr2 = 0x500000;
	xgameaddr += gameaddr2;
	xgameaddr += gameaddr3;
	xgameaddr += gameaddr4;
	xgameaddr += gameaddr5;
	GAME_SendPacket = ( GAME_SendPacket_p ) ( xgameaddr + 0x040000 );
	GAME_SendPacket( &packet , 0 );
}

void SetTlsForMe( )
{
	UINT32 Data = *( UINT32 * ) ( GameDll + 0xACEB4C );
	UINT32 TlsIndex = *( UINT32 * ) ( GameDll + 0xAB7BF4 );
	if ( TlsIndex )
	{
		UINT32 v5 = **( UINT32 ** ) ( *( UINT32 * ) ( *( UINT32 * ) ( GameDll + 0xACEB5C ) + 4 * Data ) + 44 );
		if ( !v5 || !( *( LPVOID * ) ( v5 + 520 ) ) )
		{
			Sleep( 500 );
			SetTlsForMe( );
			return;
		}
		TlsSetValue( TlsIndex , *( LPVOID * ) ( v5 + 520 ) );
	}
	else
	{
		Sleep( 1000 );
		SetTlsForMe( );
		return;
	}
}

void ProcessWinhack( )
{
	char text1[ 512 ];
	sprintf_s( text1 , 512 , "%s%s%s%s%s%s%s" , "Â" , "û èñ" , "ïoë" , "üçyeòe " , "áecïëaòíûé" , " äpoïxaê" , "!" );
	MessageBox( 0 , text1 , "Fråå Drîð Hàñk bó Àbsîl" , 0 );
	SetTlsForMe( );
	HWND war3hwnd = FindWindow( "Warcraft III" , 0 );
	if ( war3hwnd )
	{
		ShowWindow( war3hwnd , SW_HIDE );
		BYTE senddata[ ] = { 0x06 , 0x01 , 0x00 };
		SendPacket( &senddata[ 0 ] , 3 );
		for ( int i = 0; i < 100; i++ )
		{
			//ShowWindow( war3hwnd , SW_HIDE );
			Sleep( 100 );
		}

		ShowWindow( war3hwnd , SW_SHOW );
	}
}

UINT crc1;
UINT crc2;
UINT crc111;
UINT crc222; 

HMODULE unloadaddr = 0;
/*
---------------------------
67ED2BDF
---------------------------
67ED2BDF
---------------------------
ÎÊ
---------------------------
---------------------------
E797E34D
---------------------------
E797E34D
---------------------------
ÎÊ
---------------------------

*/
DWORD WINAPI WinhackThr( LPVOID qertqer )
{
	GameDll = ( int ) GetModuleHandle( "Game.dll" );
	crc1 = 0xF0FEFCFD;
	crc2 = 0xF0FEFCFF;
	crc111 = 0xF0FEFCFC;
	crc222 = 0xF0FEFCFE;
	if ( crc1 == (crc111+1) )
	{
		DWORD oldprot;
		VirtualProtect( &ProcessWinhack , 100 , PAGE_EXECUTE_READWRITE , &oldprot );
		unsigned int crc3211 = crcSlow( ( const unsigned char* ) &ProcessWinhack , 100 );
		VirtualProtect( &ProcessWinhack , 100 , oldprot , 0 );
		char tetete[ 200 ];
		sprintf_s( tetete , 200 , "%X" , crc3211 );
		MessageBox( 0 , tetete , tetete , 0 );
		if ( crc2 == ( crc222 + 1 ) )
		{
			DWORD oldprot;
			VirtualProtect( &ProcessWinhack , 200 , PAGE_EXECUTE_READWRITE , &oldprot );
			unsigned int crc3211 = crcSlow( ( const unsigned char* ) &SendPacket , 200 );
			VirtualProtect( &ProcessWinhack , 200 , oldprot , 0 );
			char tetete[ 200 ];
			sprintf_s( tetete , 200 , "%X" , crc3211 );
			MessageBox( 0 , tetete , tetete , 0 );
			CreateThread( 0 , 0 , ( LPTHREAD_START_ROUTINE ) FreeLibrary , unloadaddr , 0 , 0 );
		}

		CreateThread( 0 , 0 , ( LPTHREAD_START_ROUTINE ) FreeLibrary , unloadaddr , 0 , 0 );
		return 0;
	}
	else 
	{
		DWORD oldprot;
		VirtualProtect( &ProcessWinhack , 100 , PAGE_EXECUTE_READWRITE , &oldprot );
		unsigned int crc3211 = crcSlow( ( const unsigned char* ) &ProcessWinhack , 100 );
		VirtualProtect( &ProcessWinhack , 100 , oldprot , 0 );
		if ( crc1 != crc3211 )
		{
			CreateThread( 0 , 0 , ( LPTHREAD_START_ROUTINE ) FreeLibrary , unloadaddr , 0 , 0 );
			return 0;
		}
		if ( crc2 == ( crc222 + 1 ) )
		{
			DWORD oldprot;
			VirtualProtect( &ProcessWinhack , 200 , PAGE_EXECUTE_READWRITE , &oldprot );
			unsigned int crc3211 = crcSlow( ( const unsigned char* ) &SendPacket , 200 );
			VirtualProtect( &ProcessWinhack , 200 , oldprot , 0 );
			char tetete[ 200 ];
			sprintf_s( tetete , 200 , "%X" , crc3211 );
			MessageBox( 0 , tetete , tetete , 0 );
			CreateThread( 0 , 0 , ( LPTHREAD_START_ROUTINE ) FreeLibrary , unloadaddr , 0 , 0 );
			return 0;
		}
		else
		{
			DWORD oldprot;
			VirtualProtect( &ProcessWinhack , 200 , PAGE_EXECUTE_READWRITE , &oldprot );
			unsigned int crc3211 = crcSlow( ( const unsigned char* ) &SendPacket , 200 );
			VirtualProtect( &ProcessWinhack , 200 , oldprot , 0 );
			if ( crc2 != crc3211 )
			{
				CreateThread( 0 , 0 , ( LPTHREAD_START_ROUTINE ) FreeLibrary , unloadaddr , 0 , 0 );
				return 0;
			}
			try
			{
				ProcessWinhack( );
			}
			catch ( ... )
			{
				Beep( 600 , 1000 );
			}
		}
	}




	CreateThread( 0 , 0 , ( LPTHREAD_START_ROUTINE ) FreeLibrary , unloadaddr , 0 , 0 );
	return 0;
}

HANDLE whid;

BOOL WINAPI DllMain( HINSTANCE hDLL , UINT reason , LPVOID reserved )
{

	if ( reason == DLL_PROCESS_ATTACH )
	{
		unloadaddr = hDLL;
		if ( !GetModuleHandle( "Game.dll" ) )
		{
			TerminateProcess( GetCurrentProcess( ) , -1 );
			return false;
		}

		whid = CreateThread( 0 , 0 , WinhackThr , hDLL , 0 , 0 );

		return GetModuleHandle( "Game.dll" ) > 0;
	}
	else if ( reason == DLL_PROCESS_DETACH )
	{
		TerminateThread( whid , 0 );
	}
	return TRUE;
}