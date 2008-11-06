// NewPho2NewChewing.cpp : 定義主控台應用程式的進入點
//

#include <string>
#include <fstream>

#include "Pho2Chewing.h"

using namespace std;

const ZuinKeyCompareTable *zuin_imap, *zuin_fmap, *zuin_tmap;
int zuin_inum, zuin_fnum, zuin_tnum;

static ZuinKeyCompareTable *zuin_initial()
{
    static ZuinKeyCompareTable map[] = {
	{"ㄅ","1"}, {"ㄆ","q"}, {"ㄇ","a"}, {"ㄈ","z"}, {"ㄉ","2"}, {"ㄊ","w"},
	{"ㄋ","s"}, {"ㄌ","x"}, {"ㄍ","e"}, {"ㄎ","d"}, {"ㄏ","c"}, {"ㄐ","r"},
	{"ㄑ","f"}, {"ㄒ","v"}, {"ㄓ","5"}, {"ㄔ","t"}, {"ㄕ","g"}, {"ㄖ","b"},
	{"ㄗ","y"}, {"ㄘ","h"},	{"ㄙ","n"}
	};

	zuin_inum = sizeof(map)/sizeof(map[0]);
	return map;
}

static ZuinKeyCompareTable *zuin_final()
{
	static ZuinKeyCompareTable map[] = {
	{"ㄧㄚ","u8"}, {"ㄧㄠ","ul"}, {"ㄧㄝ","u,"}, {"ㄧㄡ","u."}, {"ㄧㄢ","u0"},
	{"ㄧㄣ","up"}, {"ㄧㄤ","u;"}, {"ㄧㄥ","u/"}, {"ㄧㄞ","u9"}, {"ㄧㄛ","ui"},
	{"ㄨㄚ","j8"}, {"ㄨㄛ","ji"}, {"ㄨㄞ","j9"}, {"ㄨㄟ","jo"}, {"ㄨㄢ","j0"},
	{"ㄨㄣ","jp"}, {"ㄨㄤ","j;"}, {"ㄨㄥ","j/"},
	{"ㄩㄝ","m,"}, {"ㄩㄢ","m0"}, {"ㄩㄣ","mp"}, {"ㄩㄥ","m/"},
	{"ㄧ","u"}, {"ㄨ","j"}, {"ㄩ","m"}, {"ㄚ","8"}, {"ㄛ","i"}, {"ㄜ","k"},
	{"ㄝ",","}, {"ㄞ","9"}, {"ㄟ","o"}, {"ㄠ","l"}, {"ㄡ","."}, {"ㄢ","0"},
	{"ㄣ","p"}, {"ㄤ",";"}, {"ㄥ","/"}, {"ㄦ","-"}
	};

	zuin_fnum = sizeof(map)/sizeof(map[0]);
	return map;
}

static ZuinKeyCompareTable *zuin_tone()
{
	static ZuinKeyCompareTable map[] = {
	{"˙","7"}, {"ˊ","6"}, {"ˇ","3"}, {"ˋ","4"}
	};

	zuin_tnum = sizeof(map)/sizeof(map[0]);
	return map;
}

void FreeMap()
{
}

void InitMap()
{
	zuin_imap=zuin_initial();
	zuin_fmap=zuin_final();
	zuin_tmap=zuin_tone();
}

int key_lookup(const ZuinKeyCompareTable *table,
			  int table_size,
			  char *target)
{
	string pstring;
	int	i,j;

	pstring=string(target);
	for ( i = 0; i < table_size; i++ ) {
		j = pstring.find( table[ i ].zuin );
		if (j>=0) {
			return	i;
		}
	}
	return	table_size;
}

int Big52UTF8(string filename)
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

int UTF16LE2UTF8(string filename)
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

void Pos2Begin(string file)
{
	fstream txtfile;
	txtfile.open(file.c_str(), ios_base::in);

	txtfile.seekp(0, ios_base::beg);
}

bool DetectUTF16LEBom(string file)
{
	const char utf16le_bom[] = {'\xff', '\xfe'};
	bool isutf16le = false;

	fstream txtfile;
	txtfile.open(file.c_str(), ios_base::in);

    Pos2Begin(file);
	char buf[2];
	if (txtfile.read(buf, 2)) {
		isutf16le = strncmp(buf, utf16le_bom, 2) == 0;
	}
	txtfile.close();

	Pos2Begin(file);
	return isutf16le;
}

int _tmain(int argc, _TCHAR* argv[])
{
	char phase[100];
	bool isbig5utf16le;
	string filename;
	char *p;

	int i,j;
	uint16 num;

	fstream utf8newphonetic;
	fstream chewingphone;

	cout << " enter the file name = ";
	cin >> filename;

	isbig5utf16le=DetectUTF16LEBom(filename);
	(isbig5utf16le)? (UTF16LE2UTF8(filename)):(Big52UTF8(filename));

	utf8newphonetic.open("~temp1.tmp", ios_base::in);
	chewingphone.open("~temp2.tmp",ios_base::out);

	InitMap();

	while(!utf8newphonetic.getline(phase, 100, '\n').eof())
	{
		p=strtok(phase," ");
		chewingphone << p << " ";

		p=strtok(NULL," ");
		
		while(p != NULL)
		{
			i=key_lookup(zuin_imap, zuin_inum, p);
			if (i != zuin_inum) chewingphone << zuin_imap[i].keysel;
			i=key_lookup(zuin_fmap, zuin_fnum, p);
			if (i != zuin_fnum) chewingphone << zuin_fmap[i].keysel;
			i=key_lookup(zuin_tmap, zuin_tnum, p);
			if (i != zuin_tnum) chewingphone << zuin_tmap[i].keysel;

			chewingphone << " ";
			p=strtok(NULL," ");
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
		p=strtok(phase," ");
		vocabulary=string(p);

		chewingdata.phonenum=int((double) vocabulary.length()/3.0);

		p=strtok(NULL," ");
		
		uhashfile.write((char*) &chewingdata.lastfreq, sizeof(int));
		uhashfile.write((char*) &chewingdata.lasttime, sizeof(int));
		uhashfile.write((char*) &chewingdata.maxfreq, sizeof(int));
		uhashfile.write((char*) &chewingdata.systemfreq, sizeof(int));
		uhashfile.write((char*) &chewingdata.phonenum, sizeof(bool));

		while(p != NULL)
		{
			//KB_DEFAULT=0;
			PhoneFromKey(phoneBuf, p, 0, 1);
			num=UintFromPhone(phoneBuf);
			uhashfile.write((char*) &num, sizeof(uint16));
			p=strtok(NULL," ");
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