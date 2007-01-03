#ifndef _getsysver
#ifdef _cplusplus
extern "C" {
#endif
DLL_DEF SCR_INT CALL_CONV GetSysVersion();

#ifdef _cplusplus
}
#endif


#define OSVerWin3x		0	//	Win 3x or lower
#define OSVerWin95		1	// Win95
#define OSVerWin98		2	// Win98
#define OSVerWinNT3x	3	// NT 3.x
#define OSVerWinNT4x	4	// NT 4.x
#define OSVerWin2000	5	// Win2000
#endif
