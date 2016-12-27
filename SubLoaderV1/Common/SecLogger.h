//---------------------------------------------------------------------------

#ifndef SecLoggerH
#define SecLoggerH

#include "CommonFuncs.h"
//---------------------------------------------------------------------------
//void realdprintf (char const *file, int line, char const *fmt, ...){}
//#define dprintf(...) realdprintf(__FILE__, __LINE__, __VA_ARGS__)
// EXAMPLE: dprintf("d",2,"t",2,7,"hello");

// If need print a string by ptr, use LOGMSG("%s",Ptr);


// Log message to spefified section and with custom proc name
#define MSGPLOG(sec,prc,msg,...) SecLogMsgOut
// Log message to spefified section
#define MSGSLOG(sec,msg,...) SecLogMsgOut(sec,NULL,msg,__VA_ARGS__)
// Log message to default section
#define MSGLOG(msg,...) SecLogMsgOut(NULL,NULL,msg,__VA_ARGS__)


#if _DEBUG
// Log debug message to spefified section and with custom proc name
#define DBGPLOG(sec,prc,msg,...) SecLogMsgOut
// Log debug message to spefified section
#define DBGSLOG(sec,msg,...) SecLogMsgOut(sec,__FUNCTION__,msg,__VA_ARGS__)
// Log debug message to default section
#define DBGLOG(msg,...) SecLogMsgOut(NULL,__FUNCTION__,msg,__VA_ARGS__)
#else
#pragma warning(disable:4002)
#define DBGPLOG(...)
#define DBGSLOG(...)
#define DBGLOG(...)
#endif

class CSecLogger
{
 BYTE LogPath[MAX_PATH];
public:
 bool IsValid(void){return this->LogPath[0];}
 void SetLogFile(LPSTR Path);
 void WriteLogFile(LPSTR LogSection, LPSTR ProcName, LPSTR LogMsgFormat, va_list VAList);
};
//-----------------------------------------------------------------------------

void _cdecl SecLogSetFile(LPSTR FilePath);
void _cdecl SecLogMsgOut(LPSTR LogSection, LPSTR ProcName, LPSTR LogMsgFormat, ...);
//---------------------------------------------------------------------------
#endif
