
#include "CommonFuncs.h"
//---------------------------------------------------------------------------
extern "C" int _fltused = 0;

extern "C" __declspec(naked) float _cdecl _CIsqrt(float &Value)
{
 __asm FSQRT
 __asm RET
}
//---------------------------------------------------------------------------
extern "C" __declspec(naked) long _cdecl _ftol2(float &Value)
{
 __asm PUSH EAX
 __asm FISTP DWORD PTR [ESP]
 __asm POP  EAX
 __asm RET
}
//---------------------------------------------------------------------------
#ifdef __CRTDECL

void* __CRTDECL operator new(size_t size){return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);}
//void* __CRTDECL operator new[](size_t width, size_t size){return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(size*width));}
void  __CRTDECL operator delete(void* mem){HeapFree(GetProcessHeap(),0,mem);}
//void  __CRTDECL operator delete[](void* mem){HeapFree(GetProcessHeap(),0,mem);}
//---------------------------------------------------------------------------
/*extern "C" __declspec(naked) unsigned __int64 _cdecl _aullshr(unsigned __int64 Value, unsigned int Shift)  // MS calls this function as _fastcall here!!!!
{
 __asm {
    CMP CL,64
    JNB SHORT RETZERO
    CMP CL,32
    JNB SHORT MORE32
    SHRD EAX,EDX,CL                       
    SHR EDX,CL
    RETN
MORE32:
    MOV EAX,EDX                             
    XOR EDX,EDX                           
    AND CL,0x1F
    SHR EAX,CL
    RETN
RETZERO:
    XOR EAX,EAX
    XOR EDX,EDX                            
    RETN
}
 /*
 if(Shift >= 64)return 0;
 if(Shift >= 32)return ((unsigned int*)&Value)[0] >> (Shift & 0x1F);
 unsigned int Lo = ((unsigned int*)&Value)[0];
 unsigned int Hi = ((unsigned int*)&Value)[1];
 Lo >>= Shift;
 Lo |= (Hi << (32-Shift));
 Hi >>= Shift;
 ((unsigned int*)&Value)[0] = Lo;
 ((unsigned int*)&Value)[1] = Hi;
 return Value;*/
/*}
//---------------------------------------------------------------------------
extern "C" __declspec(naked) unsigned __int64 _cdecl _allmul(unsigned __int64 multiplicand, unsigned __int64 multiplier)
{
 __asm {
    MOV EAX,DWORD PTR [ESP+0x08]
    MOV ECX,DWORD PTR [ESP+0x10]
    OR  ECX,EAX
    MOV ECX,DWORD PTR [ESP+0x0C]
    JNZ SHORT Part2
    MOV EAX,DWORD PTR [ESP+0x04]             
    MUL ECX
    RETN 
Part2:
    PUSH EBX
    MUL ECX
    MOV EBX,EAX
    MOV EAX,DWORD PTR [ESP+0x08]
    MUL DWORD PTR [ESP+0x14]
    ADD EBX,EAX
    MOV EAX,DWORD PTR [ESP+0x08]
    MUL ECX
    ADD EDX,EBX
    POP EBX                                
	RETN
 }
}
//---------------------------------------------------------------------------
*/
//---------------------------------------------------------------------------
extern "C" __declspec(naked) unsigned __int64 _cdecl _aullshr(unsigned __int64 Value, unsigned int Shift)  // MS calls this function as _fastcall !!!!   EDX:EAX, ECX    // ret
{
 __asm
 {
  push ECX
  push EDX
  push EAX
  call ShrULL
  retn
 }
 //return ShrULL(Value, Shift);
}
//---------------------------------------------------------------------------
extern "C" __declspec(naked) unsigned __int64 _cdecl _allshl(unsigned __int64 Value, unsigned int Shift)   // MS calls this function as _fastcall !!!!   EDX:EAX, ECX    // ret
{
 __asm
 {
  push ECX
  push EDX
  push EAX
  call ShlULL
  retn
 }
 //return ShlULL(Value, Shift);
}
//---------------------------------------------------------------------------
extern "C" __declspec(naked) unsigned __int64 _cdecl _allmul(unsigned __int64 multiplicand, unsigned __int64 multiplier)  // MS calls this function as _stdcall !!!! // ret 16 
{
 __asm jmp BinLongUMul
 //return BinLongUMul(multiplicand, multiplier);   // No sign? !!!!!!
}
//---------------------------------------------------------------------------
/*ULONGLONG __stdcall BinLongUDivStub(ULONGLONG Dividend, ULONGLONG Divisor)
{
 return BinLongUDiv(Dividend, Divisor);
}
*/
extern "C" __declspec(naked) unsigned __int64 _cdecl _aulldiv(unsigned __int64 dividend, unsigned __int64 divisor)  // MS calls this function as _stdcall !!!! // ret 16
{
__asm
{
    PUSH EBX
    PUSH ESI
    MOV EAX,DWORD PTR [ESP+0x18]
    OR EAX,EAX
    JNZ Lbl_77CA4682
    MOV ECX,DWORD PTR [ESP+0x14]
    MOV EAX,DWORD PTR [ESP+0x10]
    XOR EDX,EDX                           
    DIV ECX
    MOV EBX,EAX
    MOV EAX,DWORD PTR [ESP+0x0C]
    DIV ECX
    MOV EDX,EBX
    JMP Lbl_77CA46C3
Lbl_77CA4682:
    MOV ECX,EAX
    MOV EBX,DWORD PTR [ESP+0x14]
    MOV EDX,DWORD PTR [ESP+0x10]
    MOV EAX,DWORD PTR [ESP+0x0C]
Lbl_77CA4690:
    SHR ECX,1
    RCR EBX,1
    SHR EDX,1
    RCR EAX,1
    OR ECX,ECX
    JNZ Lbl_77CA4690
    DIV EBX
    MOV ESI,EAX
    MUL DWORD PTR [ESP+0x18]
    MOV ECX,EAX
    MOV EAX,DWORD PTR[ESP+0x14]
    MUL ESI
    ADD EDX,ECX
    JB  Lbl_77CA46BE
    CMP EDX,DWORD PTR [ESP+0x10]
    JA  Lbl_77CA46BE
    JB  Lbl_77CA46BF
    CMP EAX,DWORD PTR [ESP+0x0C]
    JBE Lbl_77CA46BF
Lbl_77CA46BE:
    DEC ESI
Lbl_77CA46BF:
    XOR EDX,EDX                          
    MOV EAX,ESI
Lbl_77CA46C3:
    POP ESI                                
    POP EBX  
    RETN 16                          
}
}


// __asm jmp BinLongUDivStub
 //return BinLongUDiv(dividend, divisor);


//---------------------------------------------------------------------------


extern "C" int __cdecl atexit(void (__cdecl *pf)())
{
 return 0;
}
#endif

#pragma function(memcmp, memset, memcpy, strcmp, strlen)
size_t __cdecl strlen(const char *s)
{
 return STRWORK::swCharsInString((LPSTR)s);
}
int __cdecl strcmp(const char *s1, const char *s2)
{
 return lstrcmp(s1,s2);
} 

void*  __cdecl memmove(void* _Dst, const void* _Src, size_t _Size)
{
 return MEMWORK:: mwMoveMemory(_Dst, (PVOID)_Src, _Size); 
}									
void*  __cdecl memcpy(void* _Dst, const void* _Src, size_t _Size)
{
 return MEMWORK::mwCopyMemory(_Dst, (PVOID)_Src, _Size);
} 
void*  __cdecl memset(void* _Dst, int _Val, size_t _Size)
{
 return  MEMWORK::mwFillMemory(_Dst, _Size, _Val);
}
int    __cdecl memcmp(const void* _Buf1, const void* _Buf2,  size_t _Size)
{
 return MEMWORK::mwCompareMemory((PVOID)_Buf1, (PVOID)_Buf2, _Size);
}

//---------------------------------------------------------------------------
__declspec(naked)long _stdcall FPURound32(float Value)
{  // Used Predefined Rounding Mode
 __asm FLD   DWORD PTR [ESP+4]
 __asm FISTP DWORD PTR [ESP+4]
 __asm MOV   EAX,[ESP+4]
 __asm RET 4
}
//---------------------------------------------------------------------------
__declspec(naked)float _stdcall FPUSqrt32(float Value)
{  // Float results returned in FPU stack
 __asm FLD   DWORD PTR [ESP+4]
 __asm FSQRT
 __asm RET 4
}
//---------------------------------------------------------------------------
__declspec(naked)float _stdcall FPURem32(float ValueA, float ValueB) // A%B
{  // Float results returned in FPU stack
 __asm FLD   DWORD PTR [ESP+8]    // ValueB
 __asm FLD   DWORD PTR [ESP+4]    // ValueA 
 __asm FPREM
 __asm FSTP  DWORD PTR [ESP+4]    // Round result
 __asm MOV   EAX,[ESP+4]
 __asm FSTP  DWORD PTR [ESP+4]    // Round result 
 __asm MOV   [ESP+4],EAX
 __asm FLD   DWORD PTR [ESP+4]
 __asm RET 8
}
//---------------------------------------------------------------------------



__declspec(naked)long _stdcall FPUDiv32(float ValueA, float ValueB) // A/B
{  // Float results returned in FPU stack
 __asm FLD   DWORD PTR [ESP+4]    // ValueA
 __asm FDIV  DWORD PTR [ESP+8]
 __asm FISTP DWORD PTR [ESP+4]    // Round result
 __asm MOV   EAX,[ESP+4]
 __asm RET 8
}
//---------------------------------------------------------------------------
__declspec(naked)float _stdcall FPUSinRad32(float Angle)
{  // Float results returned in FPU stack
 __asm FLD   DWORD PTR [ESP+4]
 __asm FSIN
 __asm RET 4
}
//---------------------------------------------------------------------------
__declspec(naked)float _stdcall FPUSinDeg32(float Angle)
{  // Float results returned in FPU stack
 __asm FLD   DWORD PTR [ESP+4]
 __asm MOV   DWORD PTR [ESP+4],180
 __asm FIDIV [ESP+4]
 __asm FLDPI
 __asm FMULP st(1),st
 __asm FSIN
 __asm RET 4
}
//---------------------------------------------------------------------------
__declspec(naked)float _stdcall FPUCosRad32(float Angle)
{  // Float results returned in FPU stack
 __asm FLD   DWORD PTR [ESP+4]
 __asm FCOS
 __asm RET 4
}
//---------------------------------------------------------------------------
__declspec(naked)float _stdcall FPUCosDeg32(float Angle)
{  // Float results returned in FPU stack
 __asm FLD   DWORD PTR [ESP+4]
 __asm MOV   DWORD PTR [ESP+4],180
 __asm FIDIV [ESP+4]
 __asm FLDPI
 __asm FMULP st(1),st
 __asm FCOS
 __asm RET 4
}
//---------------------------------------------------------------------------
__declspec(naked)__int64 _stdcall FPUSinCosRad32(float Angle)
{   // [0] - sine; [1] - cosine
 __asm FLD   DWORD PTR [ESP+4]
 __asm FSINCOS
 __asm FSTP  DWORD PTR [ESP+4]   // COS
 __asm MOV   EDX,[ESP+4]
 __asm FSTP  DWORD PTR [ESP+4]   // SIN
 __asm MOV   EAX,[ESP+4]
 __asm RET 4
}
//---------------------------------------------------------------------------
__declspec(naked)__int64 _stdcall FPUSinCosDeg32(float Angle)
{   // [0] - sine; [1] - cosine
 __asm FLD   DWORD PTR [ESP+4]
 __asm MOV   DWORD PTR [ESP+4],180
 __asm FIDIV [ESP+4]
 __asm FLDPI
 __asm FMULP st(1),st
 __asm FSINCOS
 __asm FSTP  DWORD PTR [ESP+4]   // COS
 __asm MOV   EDX,[ESP+4]
 __asm FSTP  DWORD PTR [ESP+4]   // SIN
 __asm MOV   EAX,[ESP+4]
 __asm RET 4
}
//---------------------------------------------------------------------------
/*void DEFCALL WriteLog(LPSTR Message, LPSTR LogFile)
{
 static HANDLE hLofFile = INVALID_HANDLE_VALUE;

 if(LogFile)
  {
   if(hLofFile != INVALID_HANDLE_VALUE)
  
  
  }

 if(Message && LogFile){CloseHandle(hLofFile);hLofFile = INVALID_HANDLE_VALUE;}
  else FlushFileBuffers(hLofFile);
}  */
//---------------------------------------------------------------------------
LPSTR _stdcall TrimToFilePath(LPSTR FullPath)
{
 for(int ctr=(lstrlen(FullPath)-1);ctr > 0;ctr--){if((FullPath[ctr] == PATHDLML)||(FullPath[ctr] == PATHDLMR)){FullPath[ctr+1] = 0;break;}}    
 return FullPath;
}
//---------------------------------------------------------------------------
void _stdcall ConMessageOut(LPSTR Message, DWORD TxtAttr)
{
 DWORD  Result;
 HANDLE hConOutput;

 hConOutput = GetStdHandle(STD_OUTPUT_HANDLE);
 if(TxtAttr)SetConsoleTextAttribute(hConOutput,TxtAttr);
 WriteConsole(hConOutput,Message,lstrlen(Message),&Result,NULL);
 WriteConsole(hConOutput,"\n\r",2,&Result,NULL);
}	
//---------------------------------------------------------------------------
// 'naked' used bacause compiler inserts __chkstk
__declspec(naked) void _cdecl MessageOut(LPSTR Message, void (_stdcall *MsgOutProc)(LPSTR), ...)
{
 __asm push EBP
 __asm mov  EBP, ESP
 if(Message && MsgOutProc)
  {
   PBYTE Buffer[1024];   
   va_list args;
   va_start(args,MsgOutProc);
   wvsprintf((LPSTR)&Buffer,Message,args);
   va_end(args);
   MsgOutProc((LPSTR)&Buffer);
  }
 __asm mov ESP, EBP
 __asm POP EBP
 __asm retn 
}
//---------------------------------------------------------------------------
__declspec(naked) void _cdecl ConMessageOutFmt(LPSTR Message, DWORD TxtAttr, ...)
{
 __asm
  {
   pop  EAX       // Return address
   pop  ECX       // Message
   pop  EDX       // TxtAttr
   push ECX       // Message
   lea  ECX,[ESP-2048]       // Not safe - better use HeapAlloc
   push ECX       // Buffer
   push EBX       // Save EBX
   mov  EBX,ESP
   mov  ESP,ECX   // Allocate bufer in stack
   push ESI       // Save ESI
   push EDI       // Save EDI
   push EAX       // Save Return address
   push EDX       // TxtAttr
   push ECX       // Buffer
   mov  EDI,ESP
   mov  ESP,EBX   // Restore stack for VPar CALL
   pop  EBX       // Restore original EBX
   mov  ESI,ESP
   call DWORD PTR [wsprintfA]  // Always use this type of calls for ALL imported functions!
   mov  ESP,EDI
   call ConMessageOut
   mov  ECX,ESI
   pop  EDX
   pop  EDI
   pop  ESI
   mov  ESP,ECX
   jmp  EDX        // EXIT
  }
}
//---------------------------------------------------------------------------
__declspec(naked) void _cdecl MessageOutFmt(LPSTR Message, PVOID MsgOutProc, ...)
{
 __asm
  {
   pop  EAX       // Return address
   pop  ECX       // Message
   pop  EDX       // MsgOutProc
   push ECX       // Message
   lea  ECX,[ESP-2048]       // Not safe - better use HeapAlloc
   push ECX       // Buffer
   push EBX       // Save EBX
   mov  EBX,ESP
   mov  ESP,ECX   // Allocate bufer in stack
   push ESI       // Save ESI
   push EDI       // Save EDI
   push EAX       // Save Return address
   push ECX       // Buffer
   push EDX       // MsgOutProc
   mov  EDI,ESP
   mov  ESP,EBX   // Restore stack for VPar CALL
   pop  EBX       // Restore original EBX
   mov  ESI,ESP
   call DWORD PTR [wsprintfA]  // Always use this type of calls for ALL imported functions!
   mov  ESP,EDI
   pop  EAX
   call EAX		  // Proc: void _stdcall (LPSTR Message)
   mov  ECX,ESI
   pop  EDX
   pop  EDI
   pop  ESI
   mov  ESP,ECX
   jmp  EDX        // EXIT
  }
}
//---------------------------------------------------------------------------
long _stdcall CharToHex(BYTE CharValue) 
{
 if((CharValue >= 0x30)&&(CharValue <= 0x39))return (CharValue - 0x30);		 // 0 - 9
 if((CharValue >= 0x41)&&(CharValue <= 0x46))return (CharValue - (0x41-10)); // A - F
 if((CharValue >= 0x61)&&(CharValue <= 0x66))return (CharValue - (0x41-10)); // a - f
 return -1;
}
//---------------------------------------------------------------------------
DWORD _stdcall HexStrToDW(LPSTR String, UINT Len=0)   // Fast, but do not safe
{
 long  StrLength = 0;
 DWORD Result    = 0;
 DWORD DgtPow    = 1;
 BYTE  Symbol;

 for(int ctr=0;(CharToHex(String[ctr]) >= 0)&&(!Len||(ctr < Len));ctr++)StrLength++;     // Break on any non hex digit
 for(long ctr=1;(ctr<=8)&&(ctr<=StrLength);ctr++)
  {
   Symbol = (String[StrLength-ctr]-0x30);
   if(Symbol > 9)Symbol  -= 7;
   if(Symbol > 15)Symbol -= 32;
   Result += (DgtPow*Symbol);
   DgtPow  = 1;
   for(long num = 0;num < ctr;num++)DgtPow = DgtPow*16;
  }
 return Result;
}
//---------------------------------------------------------------------------
DWORD _stdcall DecStrToDW(LPSTR String)   // Fast, but do not safe
{
 long  StrLength = 0;
 DWORD Result    = 0;
 DWORD DgtPow    = 1;
 BYTE  Symbol;

 for(int ctr=0;((BYTE)(String[ctr]-0x30)) <= 9;ctr++)StrLength++;     // Break on any non digit
 for(long ctr=1;ctr<=StrLength;ctr++)
  {
   Symbol  = (String[StrLength-ctr]-0x30);
   Result += (DgtPow*Symbol);
   DgtPow  = 1;
   for(long num = 0;num < ctr;num++)DgtPow = DgtPow*10;
  }            
 return Result;
}
//---------------------------------------------------------------------------
// 18446744073709551615 (0xFFFFFFFFFFFFFFFF)
QWORD _stdcall DecStrToDW64(LPSTR String)   // Fast, but do not safe
{
 long  StrLength = 0;
 QWORD Result    = 0;
 QWORD DgtPow    = 1;
 BYTE  Symbol;

 for(int ctr=0;((BYTE)(String[ctr]-0x30)) <= 19;ctr++)StrLength++;     // Break on any non digit
 for(long ctr=1;ctr<=StrLength;ctr++)
  {
   Symbol  = (String[StrLength-ctr]-0x30);
   Result += DgtPow*Symbol;
   DgtPow  = 1;
   for(long num = 0;num < ctr;num++)DgtPow = DgtPow*10;
  } 
 return Result;
}
//---------------------------------------------------------------------------
BOOL _stdcall SetWinConsoleSizes(DWORD WndWidth, DWORD WndHeight, DWORD BufWidth, DWORD BufHeight)
{
 HANDLE hConOutput;  
 CONSOLE_SCREEN_BUFFER_INFO ConBufInfo;

 hConOutput = GetStdHandle(STD_OUTPUT_HANDLE);
 if(!GetConsoleScreenBufferInfo(hConOutput,&ConBufInfo))return false;
 // 
 // Need do some a VERY HARD calculations !!!
 //
 
 ConBufInfo.dwSize.X = BufWidth;
 ConBufInfo.dwSize.Y = BufHeight;
  
 if(!SetConsoleWindowInfo(hConOutput,true,&ConBufInfo.srWindow))return false;
 if(!SetConsoleScreenBufferSize(hConOutput,ConBufInfo.dwSize))return false; 
 return true;
}
//===========================================================================
//                           STRING FUNCTIONS
//
int _stdcall CharPosFromLeft(LPSTR String, char Letter, int Index, int Offset)
{
 int Counter = 0;
 int Num     = 0;

 if(Offset > lstrlen(String))return -1;
 if(Index  < 0)Index = -Index;    // For all possible index manipulations
 if(Index == 0)Index = 1;
 Counter += Offset;
 for(;;)
  {
   if(String[Counter] == Letter)Num++;
   if(Num == Index)return Counter;
   if(String[Counter] == 0)return -1;
   Counter++;
  }
}
//---------------------------------------------------------------------------
int _stdcall CharPosFromRight(LPSTR String, char Letter, int Index, int Offset)
{
 int Counter = lstrlen(String)-1;
 int Num     = 0;

 if(Offset > Counter)return -1;
 if(Index  < 0)Index = -Index;    // For all possible index manipulations
 if(Index == 0)Index = 1;
 Counter -= Offset;
 for(;;)
  {
   if(String[Counter] == Letter)Num++;
   if(Num == Index)return Counter;
   if(Counter == 0)return -1;
   Counter--;
  }
}
//---------------------------------------------------------------------------
int _stdcall GetCharCount(LPSTR String, char Par)
{
 int Counter = 0;
 int Num     = 0;

 for(;;)
  {
   if(String[Counter] == Par)Num++;
   if(String[Counter] == 0)return Num;
   Counter++;
  }
}
//---------------------------------------------------------------------------
BOOL _stdcall IsUnicodeString(PVOID String)
{
 if((((BYTE*)String)[0]!=0)&&(((BYTE*)String)[1]==0)&&(((BYTE*)String)[2]!=0)&&(((BYTE*)String)[3]==0))return true;
 return false;
}
//---------------------------------------------------------------------------
DWORD _stdcall WriteLocalProtectedMemory(PVOID Address, PVOID Data, DWORD DataSize, bool RestoreProt)
{
 DWORD                    Result;
 DWORD                    Offset;
 DWORD                    PrevProt;
 DWORD                    ProtSize;
 DWORD                    BlockSize;
 PVOID                    CurProtBase;
 MEMORY_BASIC_INFORMATION MemInf;

 Offset      = 0;
 ProtSize    = 0;
 BlockSize   = DataSize;
 while(BlockSize)               // WARNING  BlockSize must be COUNTED TO ZERO !!!
  {
   CurProtBase = &((BYTE*)Address)[Offset];
   if(!VirtualQuery(CurProtBase,&MemInf,sizeof(MEMORY_BASIC_INFORMATION)))break;    // Rounded to begin of page
   if(MemInf.RegionSize > BlockSize)ProtSize = BlockSize;   // No Protection loop
	 else ProtSize = MemInf.RegionSize;
   if((MemInf.Protect==PAGE_READWRITE)||(MemInf.Protect==PAGE_EXECUTE_READWRITE)) // WRITECOPY  changed to READWRITE by writeprocessmemory - DO NOT ALLOW THIS !!!
	 {
	  FastMoveMemory(CurProtBase,&((BYTE*)Data)[Offset],ProtSize);
	  Result = ProtSize;
	 }
	  else
	   {
		if(!VirtualProtect(CurProtBase,ProtSize,PAGE_EXECUTE_READWRITE,&PrevProt))break;   // Allow writing
		FastMoveMemory(CurProtBase,&((BYTE*)Data)[Offset],ProtSize);
		Result = ProtSize;
		if(RestoreProt){if(!VirtualProtect(CurProtBase,ProtSize,PrevProt,&PrevProt))break;}  // Restore protection
	   }
   if(Result  != ProtSize)break;
   Offset     += ProtSize;
   BlockSize  -= ProtSize;
  }
 return (DataSize-BlockSize); // Bytes written
}
//===========================================================================	   
DWORD _stdcall WriteProtectedMemory(DWORD ProcessID, HANDLE hProcess, PVOID Address, PVOID Data, DWORD DataSize, bool RestoreProt)
{
 bool                     CurProc;
 bool                     CloseP;
 DWORD                    Result;
 DWORD                    Offset;
 DWORD                    PrevProt;
 DWORD                    ProtSize;
 DWORD                    BlockSize;
 PVOID                    CurProtBase;
 MEMORY_BASIC_INFORMATION MemInf;

 CloseP      = 0;
 Offset      = 0;
 CurProc     = 0;
 ProtSize    = 0;
 BlockSize   = DataSize;
 if(!hProcess){hProcess = OpenProcess(PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION|PROCESS_QUERY_INFORMATION,false,ProcessID);CloseP=true;}
 if(hProcess == GetCurrentProcess())CurProc = true;
 while(BlockSize)               // WARNING  BlockSize must be COUNTED TO ZERO !!!
  {
   CurProtBase = &((BYTE*)Address)[Offset];
   if(!VirtualQueryEx(hProcess,CurProtBase,&MemInf,sizeof(MEMORY_BASIC_INFORMATION)))break;    // Rounded to begin of page
   if(MemInf.RegionSize > BlockSize)ProtSize = BlockSize;   // No Protection loop
	 else ProtSize = MemInf.RegionSize;
   if((MemInf.Protect==PAGE_READWRITE)||(MemInf.Protect==PAGE_EXECUTE_READWRITE)) // WRITECOPY  changed to READWRITE by writeprocessmemory - DO NOT ALLOW THIS !!!
	 {
	  if(CurProc){FastMoveMemory(CurProtBase,&((BYTE*)Data)[Offset],ProtSize);Result = ProtSize;}
		else {if(!WriteProcessMemory(hProcess,CurProtBase,&((BYTE*)Data)[Offset],ProtSize,&Result))break;} // Size of Type VOID do not converted to DWORD - compatible with WIN32 And WIN64
	 }
	  else
	   {
		if(!VirtualProtectEx(hProcess,CurProtBase,ProtSize,PAGE_EXECUTE_READWRITE,&PrevProt))break;   // Allow writing
		if(CurProc){FastMoveMemory(CurProtBase,&((BYTE*)Data)[Offset],ProtSize);Result = ProtSize;}
		  else {if(!WriteProcessMemory(hProcess,CurProtBase,&((BYTE*)Data)[Offset],ProtSize,&Result))break;} // Size of Type VOID do not converted to DWORD - compatible with WIN32 And WIN64
		if(RestoreProt){if(!VirtualProtectEx(hProcess,CurProtBase,ProtSize,PrevProt,&PrevProt))break;}  // Restore protection
	   }
   if(Result  != ProtSize)break;
   Offset     += ProtSize;
   BlockSize  -= ProtSize;
  }
 if(CloseP)CloseHandle(hProcess);
 return (DataSize-BlockSize); // Bytes written
}
//===========================================================================	   
//   Signature Format: '123464??????5636'
//
PVOID _stdcall FindMemSignatureLocal(PVOID Address, LPSTR Signature)
{
 DWORD BlockSize;
 DWORD SigLen;
 PBYTE BytesBuf;
 MEMORY_BASIC_INFORMATION MemInf;

 if((SigLen = lstrlen(Signature)) < 2)return NULL;
 SigLen = (SigLen/2);
 while(VirtualQuery(Address,&MemInf, sizeof(MEMORY_BASIC_INFORMATION)))
  {
   BytesBuf  = (PBYTE)Address;
   Address   = &((BYTE*)Address)[MemInf.RegionSize]; 
   BlockSize = MemInf.RegionSize;
   if((!(MemInf.State & MEM_COMMIT))||(MemInf.State & PAGE_NOACCESS)||(MemInf.State & PAGE_GUARD))continue;  	
   while (BlockSize >= SigLen)
   	{				  	 	 	    
	 if(IsMemSignatureMatch(BytesBuf,Signature,SigLen))return (PVOID)BytesBuf;		
	 BytesBuf++;
	 BlockSize--;
	}
  }
 return NULL;
}
//===========================================================================
// SigLen - number of bytes in signature
BOOL _stdcall IsMemSignatureMatch(PVOID Address, LPSTR Signature, int SigLen)
{
 BYTE  Value   = NULL;
 long  ValueH  = NULL;
 long  ValueL  = NULL;
 int   Counter = 0;
 int   Offset  = 0; 
 int   SigMult = 1;

 if(!Signature || (SigLen <= 0))return FALSE;
 if('R'==(BYTE)Signature[0]){Signature++;Offset--;SigMult=-1;}
 // Scan by Half byte
 while(Signature[0] && !Value)
  {
   while(Signature[0] == 0x20)Signature++;   // Skip spaces
   if(Signature[0] == '*')
    {
     Signature++;
     Counter = DecStrToDW(Signature);
     while(Signature[0] != '*')Signature++;
     Signature++;
     Offset += (Counter*SigMult);   // Skip N bytes
     continue;
    }
       
   Value  = ((PBYTE)Address)[Offset];
   ValueH = CharToHex(Signature[0]);
   ValueL = CharToHex(Signature[1]);
   if(ValueH < 0)ValueH = (Value >> 4);
   if(ValueL < 0)ValueL = (Value & 0x0F);
   Value  = (((ValueH << 4) | ValueL) ^ Value);
   Signature += 2;
   Offset += SigMult;
  }
 if(!Value && !Signature[0])return TRUE;
 return FALSE;
}
//===========================================================================	   
PVOID _stdcall FindProcEntry(HMODULE Module, LPSTR ProcName)
{
 DWORD          Value;
 LPSTR          FuncName;
 DOS_HEADER     *DosHdr;
 WIN_HEADER     *WinHdr;
 EXPORT_DIR     *Export;
 DATA_DIRECTORY *ExportDir;

 DosHdr    = (DOS_HEADER*)Module;
 WinHdr    = (WIN_HEADER*)&((BYTE*)Module)[DosHdr->OffsetHeaderPE];
 ExportDir = &WinHdr->OptionalHeader.DataDirectories.ExportTable;
 Export    = (EXPORT_DIR*)&((BYTE*)Module)[ExportDir->DirectoryRVA];
 
 for(DWORD ctr=0;ctr < Export->NamePointersNumber;ctr++)
  {
   FuncName = (LPSTR)&((BYTE*)Module)[(((DWORD*)&((BYTE*)Module)[Export->NamePointersRVA])[ctr])]; 
   if(lstrcmp(FuncName,ProcName) == 0)
    {     
	 Value = ((WORD*)&((BYTE*)Module)[Export->OrdinalTableRVA])[ctr];
	 return &((DWORD*)&((BYTE*)Module)[Export->AddressTableRVA])[Value];
    }  
  }
 return NULL;
}
//===========================================================================	   
PVOID _stdcall FindProcAddress(HMODULE Module, LPSTR ProcName)
{
 return  &((BYTE*)Module)[(((DWORD*)FindProcEntry(Module,ProcName))[0])];    
}
//===========================================================================	   
//  CmdAddr [ESP+4]
//  CmdLen  [ESP+8]
//  JmpAddr [ESP+12]
//
_declspec( naked )DWORD _stdcall AddrToRelAddr(PVOID CmdAddr, DWORD CmdLen, PVOID TgtAddr)
{
 __asm
 {
  MOV  EAX,[ESP+4]
  ADD  EAX,[ESP+8]
  SUB  EAX,[ESP+12]
  NEG  EAX
  RET 12
 } 
}
//===========================================================================
//  CmdAddr   [ESP+4]
//  CmdLen    [ESP+8]
//  JmpOffset [ESP+12]
//
// WRONG !!!!!!! How to know difference bet. 8-bit negative and 32-bit positive
_declspec( naked )PVOID _stdcall RelAddrToAddr(PVOID CmdAddr, DWORD CmdLen, DWORD TgtOffset)
{
 __asm
 {
  MOV   EAX,[ESP+12]
  CMP   EAX,0x000000FF
  MOVSX EAX,AL
  JBE   RATA1         // ; Rel8
  MOV   EAX,[ESP+12]
  CMP   EAX,0x0000FFFF
  MOVSX EAX,AX
  JBE   RATA1         // ; Rel16
  MOV   EAX,[ESP+12]  // ; Rel32
RATA1:
  ADD   EAX,[ESP+8]
  ADD   EAX,[ESP+4]
  RET   12
 }
}
//---------------------------------------------------------------------------
// Suspicious... Taked from 'WinApiOverride32' source
WORD _stdcall ComputeCRC16(PVOID Buffer, DWORD BufferSize)
{
 if(!Buffer || !BufferSize)return 0;
 DWORD Checksum = 0;
 for(DWORD ctr=0;ctr < (BufferSize/2);ctr++)Checksum += ((PWORD)Buffer)[ctr];  
 if(BufferSize % 2)Checksum += ((PBYTE)Buffer)[BufferSize-1];
 Checksum  = (Checksum >> 16) + (Checksum & 0xffff);
 Checksum += (Checksum >> 16);
 Checksum =~ Checksum;
 return Checksum;
}
//---------------------------------------------------------------------------
// Something from USB spec.
DWORD _stdcall ComputeCRC32(PVOID Buffer, DWORD BufferSize)
{
 if(!Buffer || !BufferSize)return 0;
 DWORD Checksum = 0xFFFFFFFF;
 for(int bctr=0;bctr < BufferSize;bctr++)
  {
   Checksum ^= ((PBYTE)Buffer)[bctr];
   for(int ctr=0;ctr < 8;ctr++)Checksum = ((Checksum >> 1) ^ (CRC32POLYNOME & ~((Checksum & 1) - 1))); // {if((Checksum = Checksum >> 1) & 1)Checksum ^= 0xA001A001;}
  }
 return Checksum;
}
//---------------------------------------------------------------------------
BOOL _stdcall IsKeyCombinationPressed(DWORD Combination)
{
 for(DWORD ctr=4;ctr > 0;ctr--)
  {
   if(BYTE KeyCode = (Combination & 0x000000FF))
	{
     WORD KeyState = GetAsyncKeyState(KeyCode);	// 1 - key is DOWN; 0 - key is UP
	 if(!(KeyState & 0x8000))return FALSE;  // If one of keys in combination is up - no combination pressed 
	}
   Combination = (Combination >> 8);
  }
 return TRUE;
}
//---------------------------------------------------------------------------
DWORD _stdcall ApplyPatchLocal(PVOID PatchAddr, PVOID PatchData, DWORD PatchSize, LPSTR Signature)
{
 // Uncomment to take a REVERSE signatures
 //BYTE Buffer[256];
 //wsprintf((LPSTR)&Buffer,"R %08X: %02X%02X%02X%02X%02X%02X%02X%02X%02X%02X",(DWORD)PatchAddr,((PBYTE)PatchAddr)[-1],((PBYTE)PatchAddr)[-2],((PBYTE)PatchAddr)[-3],((PBYTE)PatchAddr)[-4],((PBYTE)PatchAddr)[-5],((PBYTE)PatchAddr)[-6],((PBYTE)PatchAddr)[-7],((PBYTE)PatchAddr)[-8],((PBYTE)PatchAddr)[-9],((PBYTE)PatchAddr)[-10]);
 //OutputDebugString((LPSTR)&Buffer);

 if(IsBadReadPtr(PatchAddr,PatchSize))return 1;
 if(IsBadReadPtr(PatchData,PatchSize))return 2;
 if(Signature && !IsMemSignatureMatch(PatchAddr,Signature,(lstrlen(Signature)/2)))return 3;  //Signature not match
 if(!WriteLocalProtectedMemory(PatchAddr, PatchData, PatchSize, TRUE))return 4;   // Patching failed
 return 0;
}
//---------------------------------------------------------------------------
DWORD _stdcall SetFunctionPatch(HMODULE Module, DWORD Offset, PVOID FuncAddr, BYTE CmdCode, DWORD AddNOP, LPSTR Signature)
{
 PVOID PatchAddr;
 DWORD PatchLen;
 DWORD Address;
 DWORD Index = 0;
 BYTE  PatchData[128];

 FastFillMemory(&PatchData,sizeof(PatchData),0x90); // Fill with NOPs
 PatchAddr = &((BYTE*)Module)[Offset];
 switch(CmdCode)
  {
   case 0xE8:    // Call
   case 0xE9:    // Jmp
     Address      = AddrToRelAddr(PatchAddr, 5, FuncAddr);     
     PatchLen     = 5;
     PatchData[Index] = CmdCode;
     Index++;
     break;
   case 0xB8:    // mov EAX,NNNN
   case 0x68:    // push DWORD
   case 0xA1:    // mov EAX,[NNNN]  
   case 0xA3:    // mov [NNNN],EAX
     Address      = (DWORD)FuncAddr;     
     PatchLen     = 5;
     PatchData[Index] = CmdCode;
     Index++;
     break;	 
   case 0x6A:    // push BYTE
     Address      = (DWORD)FuncAddr;     
     PatchLen     = 2;
     PatchData[Index] = CmdCode;
     Index++;
     break;	
   case 0x90:    // nop
     Address      = 0x90909090;     
	 PatchLen     = 1;
     PatchData[Index] = CmdCode;
     Index++;
     break;	
   case 0xEB:    // jmp rel BYTE
     PatchLen     = 1;
     PatchData[Index] = CmdCode;
	 //if(FuncAddr){}  // convert addr to rel
     Index++;
     break;	
  }
         
 ((PDWORD)&PatchData[Index])[0] = Address;  
 return ApplyPatchLocal(PatchAddr, &PatchData, (PatchLen+AddNOP), Signature);
}
//====================================================================================
LPSTR _stdcall FormatCurDateTime(LPSTR DateTimeStr)
{
 static BYTE MsgNum = 0;
 SYSTEMTIME  CurDateTime;
   
 GetLocalTime(&CurDateTime);
 wsprintf(DateTimeStr,"%.2u__%.3u:%.2u:%.2u:%.2u_%.2u.%.2u.%.4u",MsgNum,CurDateTime.wMilliseconds,CurDateTime.wSecond,CurDateTime.wMinute,CurDateTime.wHour,CurDateTime.wDay,CurDateTime.wMonth,CurDateTime.wYear);                                   
 MsgNum++;
 if(MsgNum > 99)MsgNum = 0;
 return DateTimeStr;
}
//====================================================================================
PVOID _stdcall FindLocalModule(PVOID *ModuleBase, DWORD *ModuleSize)
{
 DWORD Type     = NULL;
 DWORD Size     = NULL;
 PVOID Base     = NULL;
 PVOID BaseAddr = (*ModuleBase);
 MEMORY_BASIC_INFORMATION MemInf;

 // Set initial region by allocation
 while(VirtualQuery(BaseAddr,&MemInf,sizeof(MEMORY_BASIC_INFORMATION)))
  {
   BaseAddr = (MemInf.AllocationBase)?(MemInf.AllocationBase):(MemInf.BaseAddress);     // Initial base
   Base     = &((BYTE*)BaseAddr)[MemInf.RegionSize];   // Next region
   Size     = MemInf.RegionSize;
   Type     = MemInf.Type;
   while(VirtualQuery(Base,&MemInf,sizeof(MEMORY_BASIC_INFORMATION)))
	{
	 if(MemInf.AllocationBase != BaseAddr)break;      // End of allocated region
     Base  = &((BYTE*)Base)[MemInf.RegionSize];   
     Size += MemInf.RegionSize;      
     Type  = MemInf.Type;
    }
   if(Type == MEM_IMAGE)
    {
     (*ModuleBase) = Base;  // Set next region base
     if(ModuleSize)(*ModuleSize) = Size;  // Set current module size
     return BaseAddr;    // Return module base
    }
   BaseAddr = &((BYTE*)BaseAddr)[Size]; 
  }
 return NULL;
}
//===========================================================================	   
HMODULE _stdcall GetOwnerModule(PVOID Address)
{
 MEMORY_BASIC_INFORMATION MemInf;

 if(!VirtualQuery(Address,&MemInf,sizeof(MEMORY_BASIC_INFORMATION)))return NULL;
 if(MemInf.Type != MEM_IMAGE)return NULL;
 return (HMODULE)MemInf.AllocationBase;
}
//===========================================================================	   
DWORD _stdcall UpdateProcessImports(PVOID OldProcAddr, PVOID NewProcAddr, BOOL OnlyLocalModules)
{
 PVOID BaseAddr    = NULL;
 PVOID ModuleAddr  = NULL;
 PVOID OwnerModule = NULL;
 PDWORD         Table;
 DOS_HEADER     *DosHdr;
 WIN_HEADER     *WinHdr;
 IMPORT_DESC    *Import;
 DATA_DIRECTORY *ImportDir; 
 BYTE           LocalPath[MAX_PATH];
 BYTE           ModulePath[MAX_PATH];
     
 OwnerModule = (PVOID)GetOwnerModule(&UpdateProcessImports); 
 GetModuleFileName((HMODULE)OwnerModule,(LPSTR)&LocalPath,sizeof(LocalPath));
 LocalPath[CharPosFromRight((LPSTR)&LocalPath, '\\', 0, 0)+1] = 0;
 DBGMSGOUT("UpdateProcessImports -> Current directory: '%s'.",FOREGROUND_GREEN,(LPSTR)&LocalPath);

 while((ModuleAddr = FindLocalModule(&BaseAddr, NULL)))
  {  
   GetModuleFileName((HMODULE)ModuleAddr,(LPSTR)&ModulePath,sizeof(ModulePath));
   DBGMSGOUT("UpdateProcessImports -> Found module '%s' at %08X.",FOREGROUND_GREEN,(LPSTR)&ModulePath,(DWORD)ModuleAddr); 
   ModulePath[CharPosFromRight((LPSTR)&ModulePath, '\\', 0, 0)+1] = 0;
   if(OnlyLocalModules && (lstrcmpi((LPSTR)&ModulePath,(LPSTR)&LocalPath) != 0))continue;
   if(ModuleAddr == OwnerModule)continue;    // Do not hook this CRKLIB
   if(!IsValidPEHeader(ModuleAddr))continue; // If some strange module
     
   DosHdr    = (DOS_HEADER*)ModuleAddr;
   WinHdr    = (WIN_HEADER*)&((BYTE*)ModuleAddr)[DosHdr->OffsetHeaderPE];
   ImportDir = &WinHdr->OptionalHeader.DataDirectories.ImportTable;
   if(!ImportDir->DirectoryRVA)continue;
   Import    = (IMPORT_DESC*)&((BYTE*)ModuleAddr)[ImportDir->DirectoryRVA];
    
   for(DWORD tctr=0;Import[tctr].AddressTabRVA;tctr++)
    {
     Table = (PDWORD)&((BYTE*)ModuleAddr)[Import[tctr].AddressTabRVA];
     for(DWORD actr=0;Table[actr];actr++)
      {
       if(Table[actr] == (DWORD)OldProcAddr)
        {
         if(WriteLocalProtectedMemory(&Table[actr], &NewProcAddr, 4, TRUE))DBGMSGOUT("UpdateProcessImports -> Import replaced at %08X.", FOREGROUND_GREEN,(DWORD)&Table[actr]);          
           else DBGMSGOUT("UpdateProcessImports -> Replacing import at %08X - FAILED!.",FOREGROUND_RED,(DWORD)&Table[actr]); 			  
        } 
      } 
    }
  }
 DBGMSGOUT("UpdateProcessImports -> Finished.",FOREGROUND_YELLOW); 
 return 0;
}
//===========================================================================
DWORD _stdcall UpdateModuleImports(PVOID OldProcAddr, PVOID NewProcAddr, HMODULE hTgtModule)
{
 PVOID ModuleAddr  = NULL;
 PVOID OwnerModule = NULL;
 PDWORD         Table;
 DOS_HEADER     *DosHdr;
 WIN_HEADER     *WinHdr;
 IMPORT_DESC    *Import;
 DATA_DIRECTORY *ImportDir; 
 BYTE           ModulePath[MAX_PATH];
     
 ModuleAddr  = (PVOID)hTgtModule;    
 OwnerModule = (PVOID)GetOwnerModule(&UpdateProcessImports); 
 GetModuleFileName((HMODULE)ModuleAddr,(LPSTR)&ModulePath,sizeof(ModulePath));
 DBGMSGOUT("UpdateProcessImports -> For module '%s' at %08X.",FOREGROUND_GREEN,(LPSTR)&ModulePath,(DWORD)ModuleAddr); 
 if(ModuleAddr == OwnerModule)return 1;    // Do not hook this CRKLIB
 if(!IsValidPEHeader(ModuleAddr))return 2; // If some strange module
     
 DosHdr    = (DOS_HEADER*)ModuleAddr;
 WinHdr    = (WIN_HEADER*)&((BYTE*)ModuleAddr)[DosHdr->OffsetHeaderPE];
 ImportDir = &WinHdr->OptionalHeader.DataDirectories.ImportTable;
 if(!ImportDir->DirectoryRVA)return 3;
 Import    = (IMPORT_DESC*)&((BYTE*)ModuleAddr)[ImportDir->DirectoryRVA];
    
 for(DWORD tctr=0;Import[tctr].AddressTabRVA;tctr++)
  {
   Table = (PDWORD)&((BYTE*)ModuleAddr)[Import[tctr].AddressTabRVA];
   for(DWORD actr=0;Table[actr];actr++)
    {
     if(Table[actr] == (DWORD)OldProcAddr)
      {
       if(WriteLocalProtectedMemory(&Table[actr], &NewProcAddr, 4, TRUE))DBGMSGOUT("UpdateProcessImports -> Import replaced at %08X.", FOREGROUND_GREEN,(DWORD)&Table[actr]);          
         else DBGMSGOUT("UpdateProcessImports -> Replacing import at %08X - FAILED!.",FOREGROUND_RED,(DWORD)&Table[actr]); 			  
      } 
    } 
  }
 DBGMSGOUT("UpdateProcessImports -> Finished.",FOREGROUND_YELLOW); 
 return 0;
}
//===========================================================================	   
BOOL _stdcall SetDbgFlag(BOOL Flag)
{
 BOOL  OldFlag;
 PVOID Address;

 __asm { MOV EAX,DWORD PTR FS:[0x18] 
         MOV Address,EAX }   
 DBGMSGOUT("SetDbgFlag::Value of 'FS:[18]' = %08X.", FOREGROUND_GREEN,(DWORD)Address);           
 if(Address)
  {
   Address = (PVOID)((DWORD*)Address)[12];
   DBGMSGOUT("SetDbgFlag::Value of '(FS:[18])+30' = %08X.", FOREGROUND_GREEN,(DWORD)Address);           
   if(Address)
    {
     OldFlag = ((BYTE*)Address)[2];
     ((BYTE*)Address)[2] = Flag;
     DBGMSGOUT("SetDbgFlag::Old DBG flag value = %02X.", FOREGROUND_GREEN,OldFlag);           
    } 
  }
 return OldFlag;
}
//====================================================================================
int _stdcall GetDesktopRefreshRate(void)
{
 HDC hDCScreen = GetDC(NULL);
 int Refresh   = GetDeviceCaps(hDCScreen, VREFRESH);
 ReleaseDC(NULL, hDCScreen);
 return Refresh;
}
//====================================================================================
LPSTR _stdcall ConvertToHexDW(DWORD Value, DWORD MaxDigits, LPSTR Number, bool UpCase)
{
 char  HexNums[] = "0123456789ABCDEF0123456789abcdef";
 DWORD DgCnt;
 DWORD Case;

 for(int ctr=0;ctr<MaxDigits;ctr++){Number[ctr]='0';Number[ctr+1]=0;}
 if(UpCase)Case = 0;
   else Case = 16;
 for(DgCnt = 8;DgCnt > 0;DgCnt--)
  {
   ((char *)Number)[DgCnt-1] = HexNums[((Value & 0x0000000F)+Case)];
   Value = Value >> 4;
  }
 for(DgCnt = 0;Number[DgCnt] == '0';DgCnt++);
 if((MaxDigits > 8) || (MaxDigits == 0))MaxDigits = 8;
 if((8-DgCnt) < MaxDigits)DgCnt = 8 - MaxDigits;
 return (Number + DgCnt);
}
//---------------------------------------------------------------------------
LPSTR _stdcall ConvertToDecDW(DWORD Value, LPSTR Number)
{
 char  DecNums[] = "0123456789";
 int   DgCnt = 0;
 DWORD TmpValue;
 DWORD Divid;

 Divid  = 1000000000;
 for(DgCnt = 0;DgCnt < 10;DgCnt++)
  {
   TmpValue = Value / Divid;
   Number[DgCnt] = DecNums[TmpValue];
   Value -= TmpValue * Divid;
   Divid  = Divid / 10;
  }
 Number[DgCnt] = 0;
 for(DgCnt = 0;(DgCnt < 9) && (Number[DgCnt] == '0');DgCnt++);
 return (LPSTR)(((DWORD)Number) + DgCnt);
}
//---------------------------------------------------------------------------
BOOL _stdcall IsNumberInRange(long Number, long TresholdA, long TresholdB)
{
 long Min, Max;
 
 if(TresholdA < TresholdB){Min=TresholdA;Max=TresholdB;}
   else {Min=TresholdB;Max=TresholdA;}
 return ((Number >= Min)&&(Number <= Max));
}
//---------------------------------------------------------------------------
long  _stdcall GetProcessPathByID(DWORD ProcessID, LPSTR PathBuffer, long BufferLngth)
{
 PROCESSENTRY32 pent32;
 MODULEENTRY32  ment32;
 HANDLE         hProcessSnap;
 HANDLE         hModulesSnap;

 PathBuffer[0] = 0;
 hProcessSnap  = INVALID_HANDLE_VALUE;
 hModulesSnap  = INVALID_HANDLE_VALUE;
 ment32.dwSize = sizeof(MODULEENTRY32);
 pent32.dwSize = sizeof(PROCESSENTRY32);
 hProcessSnap  = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
 if(Process32First(hProcessSnap, &pent32))
  {
   do
	{
	 if(pent32.th32ProcessID == ProcessID)
	  {
	   hModulesSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pent32.th32ProcessID);
	   if(Module32First(hModulesSnap, &ment32))
		{
         do
          {
           if(lstrcmpi(pent32.szExeFile, ment32.szModule)==0)
            {
             lstrcpyn(PathBuffer, ment32.szExePath,BufferLngth);
             CloseHandle(hModulesSnap);
             return lstrlen(PathBuffer)+1;  // Excess calculations !!!
            }
          }
		   while(Module32Next(hModulesSnap, &ment32));
		}
       CloseHandle(hModulesSnap);
       break;
	  }
	}
   while(Process32Next(hProcessSnap, &pent32));
  }
 CloseHandle(hProcessSnap);
 return 0;
}
//---------------------------------------------------------------------------
long _stdcall GetProcessPathByHandle(HANDLE hProcess, LPSTR PathBuffer, long BufferLngth)
{
 PVOID         GetProcInfo;
 PEBL          ProcBlock;
 PROCPARAMS    ProcPars;
 PROCBASICINFO ProcInfo;
 wchar_t       UImagePath[MAX_PATH];      // Better allocate buffer in heap

 FastZeroMemory(&ProcPars,sizeof(ProcPars));
 FastZeroMemory(&ProcInfo,sizeof(ProcInfo));
 FastZeroMemory(&ProcBlock,sizeof(ProcBlock));
 FastZeroMemory(&UImagePath,sizeof(UImagePath));
 if( (GetProcInfo = GetProcAddress(GetModuleHandle("ntdll.dll"),"NtQueryInformationProcess")) &&
     !((DWORD (_stdcall *)(HANDLE,DWORD,PVOID,DWORD,PDWORD))GetProcInfo) (hProcess, 0, &ProcInfo, sizeof(ProcInfo), NULL) &&
     ReadProcessMemory(hProcess, ProcInfo.PebBaseAddress,&ProcBlock,sizeof(ProcBlock),NULL) &&
     ReadProcessMemory(hProcess, ProcBlock.ProcessParameters,&ProcPars,sizeof(ProcPars),NULL) &&
     ReadProcessMemory(hProcess, ProcPars.ApplicationName.Buffer,&UImagePath,ProcPars.ApplicationName.Length,NULL))
           return WideCharToMultiByte(CP_ACP,0,(PWSTR)&UImagePath,-1,PathBuffer,BufferLngth,NULL,NULL);
 return GetProcessPathByID(GetProcessId(hProcess), PathBuffer, BufferLngth);
}
//---------------------------------------------------------------------------
bool _stdcall SaveINIValueInt(LPSTR SectionName, LPSTR ValueName, int Value, LPSTR FileName)
{
 BYTE Buffer[128];
 wsprintf((LPSTR)&Buffer,"%i",Value);
 return WritePrivateProfileString(SectionName,ValueName,(LPSTR)&Buffer,FileName);
}
//---------------------------------------------------------------------------
int _stdcall RefreshINIValueInt(LPSTR SectionName, LPSTR ValueName, int Default, LPSTR FileName)
{
 BYTE Buffer[128];
 int  Result = GetPrivateProfileInt(SectionName,ValueName,Default,FileName);
 wsprintf((LPSTR)&Buffer,"%i",Result);
 WritePrivateProfileString(SectionName,ValueName,(LPSTR)&Buffer,FileName);
 return Result;
}
//---------------------------------------------------------------------------
int _stdcall RefreshINIValueStr(LPSTR SectionName, LPSTR ValueName, LPSTR Default, LPSTR RetString, DWORD Size, LPSTR FileName)
{
 int  Result = GetPrivateProfileString(SectionName, ValueName, Default, RetString, Size, FileName);
 WritePrivateProfileString(SectionName,ValueName,RetString,FileName);
 return Result;
}
//---------------------------------------------------------------------------
bool _stdcall FindFile(LPSTR FilePath, LPSTR OutBuffer)
{
 int    Index = -1;
 DWORD  Attrs;
 HANDLE hSearch;
 WIN32_FIND_DATA fdat;

 if((hSearch = FindFirstFile(FilePath,&fdat)) == INVALID_HANDLE_VALUE)return false;
 if(!OutBuffer)OutBuffer = FilePath; 
   else lstrcpy(OutBuffer, FilePath);
 FindClose(hSearch);  
 for(int ctr=0;OutBuffer[ctr];ctr++){if((OutBuffer[ctr] == PATHDLML)||(OutBuffer[ctr] == PATHDLMR))Index=ctr;}
 if(Index >= 0)OutBuffer[Index+1] = 0;
 lstrcat(OutBuffer, (LPSTR)&fdat.cFileName); 
 Attrs = GetFileAttributes(OutBuffer);
 if((Attrs != INVALID_FILE_ATTRIBUTES)&&!(Attrs & FILE_ATTRIBUTE_DIRECTORY))return true;  
 return false;
}
//---------------------------------------------------------------------------
/*bool _stdcall IsValidDSBuffer(LPDIRECTSOUNDBUFFER pDSBuffer)
{
 return (!IsBadReadPtr(pDSBuffer,32)&&!IsBadReadPtr((PVOID)((PDWORD)pDSBuffer)[0],4)&&(((PDWORD)pDSBuffer)[2] == IUNKFLAG)&&(((PDWORD)pDSBuffer)[6] == DSBFFLAG));
}*/
//------------------------------------------------------------------------------------
DWORD _stdcall GetFileNameByHandle(HANDLE hFile, LPSTR Name)
{
 PVOID  Proc;
 DWORD  Result;
 PWSTR  NamePtr;
 BYTE   Status[8];
 struct UNI_NAME
  {
   DWORD   Length;
   wchar_t Data[MAX_PATH];
  }String;

 if(Name)Name[0] = 0;
 if(!(Proc = GetProcAddress(GetModuleHandle("ntdll.dll"),"NtQueryInformationFile")))return -1;
 Result = ((DWORD (_stdcall *)(PVOID,PVOID,PVOID,DWORD,DWORD))Proc)(hFile,(PVOID)&Status,(PVOID)&String,sizeof(String),9); // FILE_NAME_INFORMATION  // Only path, no drive letter
 if(Result)return Result;
 Result  = (String.Length/sizeof(wchar_t));
 NamePtr = (PWSTR)&String.Data;
 NamePtr[Result] = 0;
 for(int ctr=Result-1;ctr >= 0;ctr--)if((String.Data[ctr]==PATHDLMR)||(String.Data[ctr]==PATHDLML)){NamePtr = (PWSTR)&String.Data[ctr+1];break;}
 WideCharToMultiByte(CP_ACP,0,NamePtr,-1,Name,MAX_PATH,NULL,NULL);
 return 0;
}
//---------------------------------------------------------------------------
DWORD _stdcall SetProcRedirection(PVOID Address, PVOID TargetAddr, LPSTR Signature)
{
 BYTE CodePatch[8];

 CodePatch[0] = JUMPOFFCODE;
 ((PDWORD)&CodePatch[1])[0] = AddrToRelAddr(Address, HOOKJMPSIZE, TargetAddr);
 return ApplyPatchLocal(Address, &CodePatch, HOOKJMPSIZE, Signature);
}
//---------------------------------------------------------------------------
DWORD _stdcall GetRealModuleSize(HANDLE hProcess, DWORD ModuleBase)
{
 MEMORY_BASIC_INFORMATION meminfo;
 DWORD                    ModuleAddr;
 DWORD                    ModuleSize;

 ModuleSize = 0;
 ModuleAddr = ModuleBase;
 FastZeroMemory(&meminfo, sizeof(MEMORY_BASIC_INFORMATION));
 while(VirtualQueryEx(hProcess, (LPCVOID)ModuleAddr, &meminfo, sizeof(MEMORY_BASIC_INFORMATION)))
  {
   ModuleAddr += meminfo.RegionSize;
   if((DWORD)meminfo.AllocationBase == ModuleBase)ModuleSize += meminfo.RegionSize;
	 else break;
  }
 return ModuleSize;
}
//---------------------------------------------------------------------------
UINT _stdcall TrimFilePath(LPSTR FullPath)
{
 int ctr = lstrlen(FullPath)-1;
 for(;ctr > 0;ctr--){if((FullPath[ctr] == PATHDLMR)||(FullPath[ctr] == PATHDLML)){FullPath[ctr+1] = 0;return ctr+1;}}  
 return 0;
}
//---------------------------------------------------------------------------
LPSTR _stdcall GetFileName(LPSTR FullPath)
{
 int ctr = lstrlen(FullPath)-1;
 for(;ctr > 0;ctr--){if((FullPath[ctr] == PATHDLMR)||(FullPath[ctr] == PATHDLML))return (LPSTR)&FullPath[ctr+1];}  
 return FullPath;
}
//---------------------------------------------------------------------------
PWSTR _stdcall GetFileName(PWSTR FullPath)
{
 int ctr = lstrlenW(FullPath)-1;
 for(;ctr > 0;ctr--){if((FullPath[ctr] == PATHDLMR)||(FullPath[ctr] == PATHDLML))return (PWSTR)&FullPath[ctr+1];}  
 return FullPath;
}
//---------------------------------------------------------------------------
LPSTR _stdcall GetFileExt(LPSTR FullPath)
{
 int ctr = lstrlen(FullPath)-1;
 for(;ctr > 0;ctr--){if(FullPath[ctr] == '.')return (LPSTR)&FullPath[ctr+1];}  
 return FullPath;
}
//---------------------------------------------------------------------------
PWSTR _stdcall GetFileExt(PWSTR FullPath)
{
 int ctr = lstrlenW(FullPath)-1;
 for(;ctr > 0;ctr--){if(FullPath[ctr] == '.')return (PWSTR)&FullPath[ctr+1];}  
 return FullPath;
}
//---------------------------------------------------------------------------
BOOL _stdcall ForceProcessSingleCore(HANDLE hProcess)
{
 DWORD ProcAffMask = 0;
 DWORD SystAffMask = 0;

 if(!GetProcessAffinityMask(hProcess,&ProcAffMask,&SystAffMask))return false;
 if(SystAffMask & 0xFFFFFFFE)  // Zero bit is always set for first core
  {
   for(UINT ctr=31;ctr > 0;ctr--) // Leave first core for something else and find next
	{
	 if(SystAffMask >> ctr)return SetProcessAffinityMask(hProcess, (((DWORD)1) << ctr));
	}
  }
 return false;
}
//---------------------------------------------------------------------------
int _stdcall ConvertFromUtf8(LPSTR DstStr, LPSTR SrcStr, UINT DstSize)
{
 WCHAR Buffer[1024];
 Buffer[0] = 0;
 MultiByteToWideChar(CP_UTF8,0,SrcStr,-1,(PWSTR)&Buffer,(sizeof(Buffer)/2));
 int len = WideCharToMultiByte(CP_ACP,0,(PWSTR)&Buffer,-1,DstStr,DstSize,NULL,NULL); 
 return len;
}
//---------------------------------------------------------------------------
int _stdcall ConvertToUtf8(LPSTR DstStr, LPSTR SrcStr, UINT DstSize)
{
 WCHAR Buffer[1024];
 Buffer[0] = 0;
 MultiByteToWideChar(CP_ACP,0,SrcStr,-1,(PWSTR)&Buffer,(sizeof(Buffer)/2));
 int len = WideCharToMultiByte(CP_UTF8,0,(PWSTR)&Buffer,-1,DstStr,DstSize,NULL,NULL);
 return len;
}
//---------------------------------------------------------------------------
ULONGLONG __stdcall ShlULL(ULONGLONG Value, LONG Shift)
{
 if(!Shift)return Value;
 if(Shift >= 64)return 0;
 if(Shift >= 32)
  {
   ((ULARGE_INTEGER*)&Value)->HighPart = ((ULARGE_INTEGER*)&Value)->LowPart << (0x1F & Shift);
   ((ULARGE_INTEGER*)&Value)->LowPart  = 0;
  }
   else
	{
	 ((ULARGE_INTEGER*)&Value)->HighPart  = (((ULARGE_INTEGER*)&Value)->LowPart >> (32 - Shift)) | (((ULARGE_INTEGER*)&Value)->HighPart << Shift);
	 ((ULARGE_INTEGER*)&Value)->LowPart <<= Shift;
	}
 return Value;
}
//---------------------------------------------------------------------------
ULONGLONG __stdcall ShrULL(ULONGLONG Value, LONG Shift)
{
 if(!Shift)return Value;
 if(Shift >= 64)return 0;
 if(Shift >= 32)return ((ULARGE_INTEGER*)&Value)->HighPart >> (0x1F & Shift);
 ((ULARGE_INTEGER*)&Value)->LowPart    = (((ULARGE_INTEGER*)&Value)->HighPart << (32 - Shift)) | (((ULARGE_INTEGER*)&Value)->LowPart >> Shift);
 ((ULARGE_INTEGER*)&Value)->HighPart >>= Shift;
 return Value;
}
//---------------------------------------------------------------------------
/*ULONGLONG __stdcall BinLongUDiv(ULONGLONG Dividend, ULONGLONG Divisor, ULONGLONG *Rem)
{
 if(!Divisor)return 0;   // Dividend;   // Or return 0 ?
 ULONGLONG Quotient, Remainder;
 Quotient = Remainder = 0;
 long idx = 0;

 // Get highest bit index of Dividend  (Use an Intrinsics?)
 ULARGE_INTEGER Value;
 Value.QuadPart = Dividend;
 if(!Value.HighPart){idx += 32;Value.HighPart = Value.LowPart;Value.LowPart = 0;}  // High 32 bits are zero
 ULONG PrvVal = Value.HighPart;
 if(!(PrvVal >> 16)){idx += 16;Value.HighPart <<= 16;}   // High 16 bits are zero
 if(!(PrvVal >>  8)){idx +=  8;Value.HighPart <<=  8;}   // High 8  bits are zero
 if(!(PrvVal >>  4)){idx +=  4;Value.HighPart <<=  4;}   // High 4  bits are zero
 if(!(PrvVal >>  2)){idx +=  2;Value.HighPart <<=  2;}   // High 2  bits are zero
 Dividend = Value.QuadPart;

 for(;idx < (sizeof(ULONGLONG)*8);idx++)  // Set initial idx to result of BSR (Bit Scan Reverse) for Dividend
  {
   Quotient  <<= 1;
   Remainder <<= 1;
   if(Dividend & ~(((ULONGLONG)(-1))/2))Remainder |= 1; // Dividend & 0x8000000000000000
   Dividend  <<= 1;
   if(Remainder >= Divisor)
	{
	 Remainder -= Divisor;
	 Quotient  |= 1;
	}
  }
 if(Rem)*Rem = Remainder;
 return Quotient;
}*/
//---------------------------------------------------------------------------
ULONGLONG __stdcall BinLongUMul(ULONGLONG Multiplicand, ULONGLONG Multiplier)
{
 ULONGLONG Summ = 0;
 for(long idx=0;Multiplier && (idx < (sizeof(ULONGLONG)*8));idx++)
  {
   if(Multiplier & 1)Summ += Multiplicand;
   Multiplicand <<= 1;
   Multiplier   >>= 1;
  }
 return Summ;
}
//---------------------------------------------------------------------------
void _stdcall CreateDirectoryPath(LPSTR Path)
{
 BYTE FullPath[MAX_PATH];

 lstrcpy((LPSTR)&FullPath, Path);
 for(int Count=0;FullPath[Count] != 0;Count++)
  {
   if((FullPath[Count]==PATHDLML)||(FullPath[Count]==PATHDLMR))
    {
     FullPath[Count] = 0;
     CreateDirectory((LPSTR)&FullPath, NULL); // Faster Always create or Test it`s existance first ?
     FullPath[Count] = PATHDLML;
    } 
  }
}
//---------------------------------------------------------------------------
