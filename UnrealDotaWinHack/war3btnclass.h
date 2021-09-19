class WarcraftButton
{
	typedef void *( __fastcall * GetBtnAddrGAME )( char * a2 , int  INDEX );
	GetBtnAddrGAME GetBtnAddr_p;


	typedef void *( __fastcall * GetBtnAddrDefoGame )( char * a1 , int INDEX , int a3 , void * a4 );
	GetBtnAddrDefoGame GetBtnAddrDefo;

	bool fistnameset = false;
	char * buttonname;
	int BtnIndex = -1;
	unsigned char btnflag = 0;
	bool defflag = true;

public:


	enum BUTTONBIT : unsigned
	{
		STATE_BUTTON_ENABLED = 0 ,
		STATE_BUTTON_FOCUSED = 1 ,
		STATE_BUTTON_PRESSED = 2 ,
		STATE_BUTTON_MOUSEOVER = 4 ,
		STATE_BUTTON_CHECKED = 5 ,
		STATE_BUTTON_CURSOR = 5
	};

	enum BUTTONBITADD : unsigned
	{
		STATE_BUTTON_INVISIBLE = 0 ,
		/*		STATE_BUTTON_FOCUSED = 1 ,
		STATE_BUTTON_PRESSED = 2 ,
		STATE_BUTTON_MOUSEOVER = 4 ,
		STATE_BUTTON_CHECKED = 5 ,
		STATE_BUTTON_CURSOR = 5*/
	};


	~WarcraftButton( )
	{
		if ( fistnameset )
			delete[ ] buttonname;
	}

	WarcraftButton( const char * name )
	{
		int namesize = strlen( name ) + 1;
		if ( fistnameset )
			delete[ ] buttonname;
		buttonname = new char[ strlen( name ) + 1 ];
		sprintf_s( buttonname , namesize , "%s" , name );
		fistnameset = true;

		if ( IsOk( ) )
		{
			btnflag = GetBtnFlag( );
		}

	}

	WarcraftButton( const char * name , int id )
	{
		int namesize = strlen( name ) + 1;
		if ( fistnameset )
			delete[ ] buttonname;
		buttonname = new char[ strlen( name ) + 1 ];
		sprintf_s( buttonname , namesize , "%s" , name );
		fistnameset = true;

		if ( IsOk( ) )
		{
			if ( id > 0 )
				BtnIndex = id;
			btnflag = GetBtnFlag( );
		}

	}

	void SetButtonName( const char * name )
	{
		int namesize = strlen( name ) + 1;
		if ( fistnameset )
			delete[ ] buttonname;
		buttonname = new char[ strlen( name ) + 1 ];
		sprintf_s( buttonname , namesize , "%s" , name );
		fistnameset = true;
	}

	void SetButtonId( int id )
	{
		BtnIndex = id;
	}

	void UpdateBtnFlag( )
	{
		if ( IsOk( ) )
		{
			btnflag = GetBtnFlag( );
		}
	}

	bool IsOk( )
	{
		if ( GetModuleHandle( "Game.dll" ) )
		{
			if ( fistnameset )
			{
				GetBtnAddrDefo = ( GetBtnAddrDefoGame ) ( ( DWORD ) GetModuleHandle( "Game.dll" ) + 0x5FA830 );
				GetBtnAddr_p = ( GetBtnAddrGAME ) ( ( DWORD ) GetModuleHandle( "Game.dll" ) + 0x5FA970 );
				return GetBtnAddr( ) > 0;
			}
		}
		return false;
	}

	int GetBtnUnknownAddr( )
	{

		if ( !fistnameset )
			return 0;

		if ( !buttonname )
			return 0;

		int tmp = 1;

		if ( BtnIndex != 1 )
			return ( int ) GetBtnAddrDefo( buttonname , BtnIndex , 0 , &tmp );
		else
			return ( int ) GetBtnAddrDefo( buttonname , 0 , 0 , &tmp );
	}

	bool IsBtnEnabled( )
	{
		if ( IsOk( ) )
		{
			return is_btn_bit_set( STATE_BUTTON_ENABLED );
		}
		return false;
	}

	bool IsBtnFocused( )
	{
		if ( IsOk( ) )
		{
			return is_btn_bit_set( STATE_BUTTON_FOCUSED );
		}
		return false;
	}

	bool IsBtnPressed( )
	{
		if ( IsOk( ) )
		{
			return is_btn_bit_set( STATE_BUTTON_PRESSED );
		}
		return false;
	}

	bool IsBtnMouseOver( )
	{
		if ( IsOk( ) )
		{
			return is_btn_bit_set( STATE_BUTTON_MOUSEOVER );
		}
		return false;
	}

	bool IsChecked( )
	{
		if ( IsOk( ) )
		{
			return is_btn_bit_set( STATE_BUTTON_CHECKED );
		}
		return false;
	}

	int GetBtnStringSize( )
	{
		if ( IsOk( ) )
		{
			return *( int* ) ( GetBtnAddr( ) + 0x1E8 );
		}
		return 0;
	}

	bool ReadBtnText( void * outstring , unsigned int outstringsize )
	{
		SIZE_T readbytes;
		if ( ReadProcessMemory( GetCurrentProcess( ) , ( void* ) GetBtnStringAddr( ) , outstring , outstringsize , &readbytes ) && readbytes > 0 )
		{
			return true;
		}

		return false;
	}

	int GetBtnStringAddr( )
	{
		if ( IsOk( ) )
		{
			if ( GetBtnStringSize( ) > 0 )
			{
				int returnaddr = *( int* ) ( GetBtnAddr( ) + 0x1E4 );
				if ( returnaddr < 1000 )
				{
					returnaddr = *( int* ) ( GetBtnAddr( ) + 0x1E8 + 4 );
				}

				if ( returnaddr < 1000 )
				{
					returnaddr = *( int* ) ( GetBtnAddr( ) + 0x1E8 + 8 );
				}

				if ( returnaddr < 1000 )
				{
					returnaddr = 0;
				}

				return returnaddr;
			}
		}
		return 0;
	}

	const char * Author( )
	{
		return "Absol (d3scene.ru)";
	}

	const char * Version( )
	{
		return "3 release d3scene.ru!";
	}

	void SetBtnFlag( unsigned value )
	{
		btnflag = bit_set( btnflag , value );
		WriteBtnFlag( btnflag );
	}
	void ClearBtnFlag( unsigned value )
	{
		btnflag = bit_clear( btnflag , value );
		WriteBtnFlag( btnflag );
	}

	void SetBtnKey( unsigned int key )
	{
		if ( IsOk( ) )
		{
			//0x4c 0x64
			if ( ( *( unsigned int* ) ( GetBtnAddr( ) + 0x4C ) ) < 1000 )
			{
				*( unsigned int* ) ( GetBtnAddr( ) + 0x4C ) = key;
			}
			if ( ( *( unsigned int* ) ( GetBtnAddr( ) + 0x64 ) ) < 1000 )
			{
				*( unsigned int* ) ( GetBtnAddr( ) + 0x64 ) = key;
			}

		}
	}

	void WriteText( const char * text )
	{

		if ( IsOk( ) )
		{
			if ( GetBtnStringAddr( ) < 1000 )
			{
				return;
			}
			unsigned int maxlen = *( int* ) ( GetBtnAddr( ) + 0x1E8 );
			if ( maxlen > 0 )
			{
				unsigned char nullbyte = 0;
				WriteProcessMemory( GetCurrentProcess( ) , ( void * ) ( GetBtnStringAddr( ) ) , text , maxlen , 0 );
				WriteProcessMemory( GetCurrentProcess( ) , ( void * ) ( GetBtnStringAddr( ) + maxlen ) , &nullbyte , 1 , 0 );
			}
		}
	}

	void SetPopupActive( bool active )
	{
		if ( IsOk( ) )
		{
			if ( GetBtnAddr( ) > 1000 )
			{
				*( unsigned char* ) ( 0x1BC + GetBtnAddr( ) ) = active ? 0xAC : 0xAA;
				SetBtnFlag( STATE_BUTTON_ENABLED );
			}

		}


	}

	void WriteTextAddr( int addr )
	{
		if ( IsOk( ) )
		{
			if ( GetBtnStringAddr( ) > 1000 )
			{
				int returnaddr = *( int* ) ( GetBtnAddr( ) + 0x1E4 );
				if ( returnaddr > 1000 )
				{
					*( int* ) ( GetBtnAddr( ) + 0x1E4 ) = addr;
				}

				returnaddr = *( int* ) ( GetBtnAddr( ) + 0x1E8 + 4 );

				if ( returnaddr > 1000 )
				{
					*( int* ) ( GetBtnAddr( ) + 0x1E8 + 4 ) = addr;
				}


				returnaddr = *( int* ) ( GetBtnAddr( ) + 0x1E8 + 8 );

				if ( returnaddr > 1000 )
				{
					*( int* ) ( GetBtnAddr( ) + 0x1E8 + 8 ) = addr;
				}

			}
		}
	}


	void WriteTextSize( int size )
	{
		if ( IsOk( ) )
		{
			*( int* ) ( GetBtnAddr( ) + 0x1E8 ) = size;
		}

	}
	void WriteText( const char * text , bool force )
	{

		if ( IsOk( ) )
		{
			if ( GetBtnStringAddr( ) < 1000 )
			{
				MessageBox( NULL , "no" , "no" , MB_OK );
				return;
			}
			unsigned int maxlen = *( int* ) ( GetBtnAddr( ) + 0x1E8 );
			if ( maxlen > strlen( text ) )
			{
				unsigned char nullbyte = 0;
				WriteProcessMemory( GetCurrentProcess( ) , ( void * ) ( GetBtnStringAddr( ) ) , text , strlen( text ) , 0 );
				WriteProcessMemory( GetCurrentProcess( ) , ( void * ) ( GetBtnStringAddr( ) + strlen( text ) ) , &nullbyte , 1 , 0 );
			}
			else if ( force )
			{
				*( int* ) ( GetBtnAddr( ) + 0x1E8 ) = strlen( text );
				unsigned char nullbyte = 0;
				WriteProcessMemory( GetCurrentProcess( ) , ( void * ) ( GetBtnStringAddr( ) ) , text , strlen( text ) , 0 );
				WriteProcessMemory( GetCurrentProcess( ) , ( void * ) ( GetBtnStringAddr( ) + strlen( text ) ) , &nullbyte , 1 , 0 );
			}
		}
	}

	int GetBtnAddr( )
	{
		if ( !fistnameset )
			return 0;

		if ( !buttonname )
			return 0;

		if ( BtnIndex == -1 )
			return ( int ) GetBtnAddr_p( buttonname , 0 );
		else
			return ( int ) GetBtnAddr_p( buttonname , BtnIndex );
	}

	unsigned char GetBtnFlag( )
	{
		if ( IsOk( ) )
		{
			return *( unsigned char * ) ( GetFlagAddr( ) );
		}
		return 0;
	}

	void WriteBtnFlag( unsigned char flag )
	{
		if ( IsOk( ) )
		{
			*( unsigned char * ) ( GetFlagAddr( ) ) = flag;
		}
	}

	void DumpBtn( const char * filename )
	{
		if ( IsOk( ) && GetBtnAddr( ) > 1000 )
		{
			FILE *fp;
			fopen_s( &fp , filename , "wb" );
			if ( fp )
			{
				unsigned char buffer[ 520 ];
				if ( ReadProcessMemory( GetCurrentProcess( ) , ( void * ) ( GetBtnAddr( ) ) , &buffer , 520 , 0 ) )
				{
					fwrite( buffer , 1 , 520 , fp );
				}
				fclose( fp );
			}
		}
	}

private:


	int GetFlagAddr( )
	{
		return GetBtnAddr( ) + 0x1D4;
	}

	bool is_btn_bit_set( unsigned bitindex )
	{
		unsigned value = GetBtnFlag( );
		return ( value & ( 1 << bitindex ) ) != 0;
	}

	unsigned bit_set( unsigned value , unsigned bitindex )
	{
		return  value | ( 1 << bitindex );
	}

	unsigned bit_clear( unsigned value , unsigned bitindex )
	{
		return  value  & ~( 1 << bitindex );
	}

};