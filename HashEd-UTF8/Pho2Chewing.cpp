// NewPho2NewChewing.cpp : 定義主控台應用程式的進入點
//

#include <string>
#include <fstream>

#include "Pho2Chewing.h"

using namespace std;

const zuintable *zuin_imap, *zuin_fmap, *zuin_tmap;
const zuintable *key_imap, *key_fmap, *key_tmap;
int zuin_inum, zuin_fnum, zuin_tnum;
int key_inum, key_fnum, key_tnum;

static zuintable *zuin_initial()
{
    static zuintable map[] = {
	"ㄅ", "ㄆ", "ㄇ", "ㄈ", "ㄉ", "ㄊ", "ㄋ", "ㄌ", "ㄍ", "ㄎ",
	"ㄏ", "ㄐ", "ㄑ", "ㄒ", "ㄓ", "ㄔ", "ㄕ", "ㄖ", "ㄗ", "ㄘ",
	"ㄙ"};
	zuin_inum = sizeof(map)/sizeof(map[0]);
	return map;
}

static zuintable *zuin_final()
{
	static zuintable map[] = {
	"ㄧㄚ", "ㄧㄠ", "ㄧㄝ", "ㄧㄡ", "ㄧㄢ", "ㄧㄣ", "ㄧㄤ", "ㄧㄥ","ㄧㄞ","ㄧㄛ",
	"ㄨㄚ", "ㄨㄛ", "ㄨㄞ", "ㄨㄟ", "ㄨㄢ", "ㄨㄣ", "ㄨㄤ", "ㄨㄥ",
	"ㄩㄝ", "ㄩㄢ", "ㄩㄣ", "ㄩㄥ",
	"ㄧ", "ㄨ", "ㄩ", "ㄚ", "ㄛ", "ㄜ", "ㄝ", "ㄞ", "ㄟ", "ㄠ",
	"ㄡ", "ㄢ", "ㄣ", "ㄤ", "ㄥ", "ㄦ"};
	zuin_fnum = sizeof(map)/sizeof(map[0]);
	return map;
}

static zuintable *zuin_tone()
{
	static zuintable map[] = {"˙", "ˊ", "ˇ", "ˋ"};
	zuin_tnum = sizeof(map)/sizeof(map[0]);
	return map;
}

static zuintable *key_initial()
{
    static zuintable map[] = {
	"1", "q", "a", "z", "2", "w", "s", "x", "e", "d",
	"c", "r", "f", "v", "5", "t", "g", "b", "y", "h",
	"n"};
	key_inum = sizeof(map)/sizeof(map[0]);
	return map;
}

static zuintable *key_final()
{
	static zuintable map[] = {
	"u8", "ul", "u,", "u.", "u0", "up", "u;", "u/","u9","ui",
    "j8", "ji", "j9", "jo", "j0", "jp", "j;", "j/",
    "m,", "m0", "mp", "m/",
    "u", "j", "m", "8", "i", "k", ",", "9", "o", "l",
    ".", "0", "p", ";", "/", "-"};
	key_fnum = sizeof(map)/sizeof(map[0]);
	return map;
}

static zuintable *key_tone()
{
	static zuintable map[] = {"7", "6", "3", "4"};
	zuin_tnum = sizeof(map)/sizeof(map[0]);
	return map;
}

void FreeMap()
{}

void InitMap()
{
	zuin_imap=zuin_initial();
	zuin_fmap=zuin_final();
	zuin_tmap=zuin_tone();
	key_imap=key_initial();
	key_fmap=key_final();
	key_tmap=key_tone();
}

int big52utf8(string filename)
{
	HANDLE hHashFile = CreateFile( filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( hHashFile == INVALID_HANDLE_VALUE )
		return 0; /* absence of hash.dat simply means "nothing", 
		             and should not rise an error to NSIS. */

	DWORD size = GetFileSize( hHashFile, NULL );
	DWORD buflen = (size *3/2) +1;
	char *buf = new char[ buflen ];
	if( buf )	{
		if( ReadFile( hHashFile, buf, size, &size, NULL ) )	{
			CloseHandle( hHashFile );
			DWORD usize = MultiByteToWideChar( 950, 0, buf, size + 1, NULL, 0 );
			wchar_t* ubuf = new wchar_t[usize];
			if( ubuf )	{
				usize = MultiByteToWideChar( 950, 0, buf, size + 1, ubuf, usize );
				size = WideCharToMultiByte( CP_UTF8, 0, ubuf, usize, NULL, 0, NULL, NULL );
				if ( buf!=NULL ) {
					delete [] buf;
					buf = new char[size+1];
				}
				size = WideCharToMultiByte( CP_UTF8, 0, ubuf, usize, buf, size, NULL, NULL );
				hHashFile = CreateFile( "~temp1.tmp", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL );
				if( hHashFile != INVALID_HANDLE_VALUE )	{
					WriteFile( hHashFile, buf, size, &size, NULL );
					CloseHandle( hHashFile );
				}
				delete []ubuf;
			}
		}
		delete []buf;
	}
	return 0;
}

int utf16le2utf8(string filename)
{
	HANDLE hHashFile = CreateFile( filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
	if( hHashFile == INVALID_HANDLE_VALUE )
		return 0; /* absence of hash.dat simply means "nothing", 
		             and should not rise an error to NSIS. */

	DWORD size = GetFileSize( hHashFile, NULL );
	DWORD buflen = (size *3) +1;
	wchar_t *buf = new wchar_t[ buflen ];
	if( buf )	{
		if( ReadFile( hHashFile, buf, size, &size, NULL ) )	{
			CloseHandle( hHashFile );
			DWORD usize = WideCharToMultiByte( CP_UTF8, 0, buf, size, NULL, 0, NULL, NULL );
			char* ubuf = new char[usize];
			if( ubuf )	{
				size = WideCharToMultiByte( CP_UTF8, 0, buf, size/2, ubuf, usize, NULL, NULL );
				hHashFile = CreateFile( "~temp1.tmp", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL );
				if( hHashFile != INVALID_HANDLE_VALUE )	{
					WriteFile( hHashFile, ubuf, size, &size, NULL );
					CloseHandle( hHashFile );
				}
				delete []ubuf;
			}
		}
	delete []buf;
	}
	return 0;
}

void postobegin(string file)
{
	fstream txtfile;
	txtfile.open(file.c_str(), ios_base::in);

	txtfile.seekp(0, ios_base::beg);
}

bool utf16lebom(string file)
{
	const char utf16le_bom[] = {'\xff', '\xfe'};
	bool isutf16le = false;

	fstream txtfile;
	txtfile.open(file.c_str(), ios_base::in);

    postobegin(file);
	char buf[2];
	if (txtfile.read(buf, 2)) {
		isutf16le = strncmp(buf, utf16le_bom, 2) == 0;
	}
	txtfile.close();

	postobegin(file);
	return isutf16le;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char phase[100];
	bool isbig5utf16le;
	string filename, seach;
	char *data;

	int i,j;
	uint16 num;

	fstream utf8newphonetic;
	fstream chewingphone;

	cout << " enter the file name = ";
	cin >> filename;

	isbig5utf16le=utf16lebom(filename);
	(isbig5utf16le)? (utf16le2utf8(filename)):(big52utf8(filename));

	utf8newphonetic.open("~temp1.tmp", ios_base::in);
	chewingphone.open("~temp2.tmp",ios_base::out);

	InitMap();

	while(!utf8newphonetic.getline(phase, 100, '\n').eof())
	{
		data=strtok(phase," ");
		chewingphone << data << " ";

		data=strtok(NULL," ");
		
		while(data != NULL)
		{
			seach=string(data);
			for(i=0; i<zuin_inum; i++)
			{
				j=seach.find(zuin_imap[i].zuin);
				if(j>=0)	
				{
					chewingphone << key_imap[i].zuin;
					break;
				}
			}

			for(i=0; i<zuin_fnum; i++)
			{
				j=seach.find(zuin_fmap[i].zuin);
				if(j>=0)	
				{
					chewingphone << key_fmap[i].zuin;
					break;
				}
			}
			for(i=0; i<zuin_tnum; i++)
			{
				j=seach.find(zuin_tmap[i].zuin);
				if(j>=0)	
				{
					chewingphone << key_tmap[i].zuin;
					break;
				}
			}
			chewingphone << " ";
			data=strtok(NULL," ");
		};
	chewingphone << "\n";
	};
	utf8newphonetic.close();
	chewingphone.close();
/////////////////////////////////////////////////////////////////
	bool k=0;
	char phoneBuf[128];

	string uhashname, vocabulary;
	chewing chewingdata;

	fstream chewingfone;
	fstream uhashfile;
	
	chewingfone.open("~temp2.tmp",ios_base::in);
	cout << "enter file name of uhash = ";
	cin >> uhashname;
	uhashfile.open(uhashname.c_str(), ios_base::out|ios_base::app|ios_base::binary);
	if(!uhashfile) 
	{
		cerr << "ERROR!" << endl;
		return 0;
	}

	chewingdata.lastfreq=1;
	chewingdata.lasttime=0;
	chewingdata.maxfreq=1;
	chewingdata.systemfreq=1;

	//if utf16le -> utf8, utf8 file has BOM, seek 3 bits to 避開 BOM
	if (isbig5utf16le) chewingfone.seekg(3, ios_base::beg);

	while(!chewingfone.getline(phase, 100, '\n').eof())
	{
		data=strtok(phase," ");
		vocabulary=data;

		chewingdata.phonenum=int((double) vocabulary.length()/3.0);

		data=strtok(NULL," ");
		
		uhashfile.write((char*) &chewingdata.lastfreq, sizeof(int));
		uhashfile.write((char*) &chewingdata.lasttime, sizeof(int));
		uhashfile.write((char*) &chewingdata.maxfreq, sizeof(int));
		uhashfile.write((char*) &chewingdata.systemfreq, sizeof(int));
		uhashfile.write((char*) &chewingdata.phonenum, sizeof(bool));

		while(data != NULL)
		{
			//KB_DEFAULT=0;
			PhoneFromKey(phoneBuf, data, 0, 1);
			num=UintFromPhone(phoneBuf);
			uhashfile.write((char*) &num, sizeof(uint16));
			data=strtok(NULL," ");
		};
		chewingdata.bits=3*chewingdata.phonenum;
		uhashfile.write((char*) &chewingdata.bits, sizeof(bool));
		uhashfile << vocabulary;

		/* bytes = chewingdata.lastfreq + chewingdata.lasttime + chewingdata.maxfreq + 
		chewingdata.systemfreq + chewingdata.phonenum + zuinserial(unsigned integer = 2 bytes
		have chewingdata.phonenum s zuinserial) + words(in utf8, word = 3 bytes, 
		have chewingdata.phonenum s words);*/
		i=4*4+1+2*chewingdata.phonenum+1+3*chewingdata.phonenum;
		for(j=0;j<(125-i);j++)
		{
			uhashfile.write((char*) &k, sizeof(bool));
		}
	};
	chewingfone.close();
	uhashfile.close();
/////////////////////////////////////////////////////

	return 0;
}
