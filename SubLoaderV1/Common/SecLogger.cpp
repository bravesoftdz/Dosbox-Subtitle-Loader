
#pragma hdrstop

#include "SecLogger.h"


static CSecLogger Logger;
//====================================================================================
void CSecLogger::SetLogFile(LPSTR Path)
{
 lstrcpy((LPSTR)&this->LogPath,Path);
 DeleteFile(Path);
}
//------------------------------------------------------------------------------------
void CSecLogger::WriteLogFile(LPSTR LogSection, LPSTR ProcName, LPSTR LogMsgFormat, va_list VAList)
{
 if(this->IsValid())
  {
   BYTE NameRecord[1025];
   BYTE Buffer[1025];     //  > 4k creates __chkstk   // 1024 is MAX for wsprintf
   FormatCurDateTime((LPSTR)&NameRecord);
   wvsprintf((LPSTR)&Buffer,LogMsgFormat,VAList);
   if(ProcName && ProcName[0])
    {
     lstrcat((LPSTR)&NameRecord,"__");   // Optimize
     lstrcat((LPSTR)&NameRecord,ProcName);
    }
   if(!LogSection || !LogSection[0])LogSection = "Global";
   WritePrivateProfileString(LogSection,(LPSTR)&NameRecord,(LPSTR)&Buffer,(LPSTR)&this->LogPath);
  }
}
//====================================================================================
void _cdecl SecLogSetFile(LPSTR FilePath)
{
 Logger.SetLogFile(FilePath);
}
//------------------------------------------------------------------------------------
void _cdecl SecLogMsgOut(LPSTR LogSection, LPSTR ProcName, LPSTR LogMsgFormat, ...)   // void (_stdcall *MsgOutProc)(LPSTR)
{
 if(LogMsgFormat && Logger.IsValid())
  {  
   va_list args;
   va_start(args,LogMsgFormat);     
   Logger.WriteLogFile(LogSection,ProcName,LogMsgFormat,args);
   va_end(args);
  }
}
//====================================================================================

