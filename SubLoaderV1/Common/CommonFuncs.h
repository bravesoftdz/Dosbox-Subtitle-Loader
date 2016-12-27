#pragma once

#ifndef CommonFuncsH
#define CommonFuncsH

//#define DBGMESSAGES
//---------------------------------------------------------------------------
#include <Windows.h>
#include <tlhelp32.h>   // needed for tool help declarations
//#include <mmsystem.h> 
#include <FormatPE.h>    
#include <MemWork.h>
#include <StrWork.h>
//#include <dsound.h>
//---------------------------------------------------------------------------
#define LOGMSGOUT MessageOut	 
#define HVL volatile              // Mark as 'Must never be removed by optimizer'

//#define sqrt             FPUSqrt32
//#define MWTHISCALL              // Class member function call type (empty for '__thiscall')
//#ifndef DEFCALL
//#define DEFCALL  //__fastcall
//#endif

#define CRC32POLYNOME 0xEDB88320
#define IUNKFLAG      0x4B4E5549
#define DSBFFLAG      0x46425344
#define MAGABYTE      1048576

#define PATHDLMR 0x2F
#define PATHDLML 0x5C
//---------------------------------------------------------------------------
//#define ZeroStruct(a) { FastZeroMemory(&a,sizeof(a)); }
//#define ZeroPointer(a) { FastZeroMemory(a,sizeof(*a)); }
//#define InitPointer(a,b) { a=new b(); ZeroPointer(a); }

//---------------------------------------------------------------------------
#define _REVERSEDW(Value)          (((DWORD)(Value) << 24)|((DWORD)(Value) >> 24)|(((DWORD)(Value) << 8)&0x00FF0000)|(((DWORD)(Value) >> 8)&0x0000FF00))
#define _MINIMAL(ValueA, ValueB) (((ValueA) < (ValueB))?(ValueA):(ValueB))
#define _PERCTOMAX(Perc, Num)         (((Num)*100)/((Perc)?(Perc):1))
#define _NUMTOPERC(Num, MaxVal)       (((Num)*100)/(MaxVal))
#define _PERCTONUM(Perc, MaxVal)      (((Perc)*(MaxVal))/100)
#define _SWAPNUMBERS(A,B)             ((A) ^= (B) ^= (A) ^= (B))
#define _MIN(a, b)                    (((a) < (b)) ? (a) : (b))
#define _MAX(a, b)                    (((a) > (b)) ? (a) : (b))
#define _DELTA(a,b)                   (((a) > (b)) ? ((a) - (b)) : ((b) - (a)))
#define _BYTELIMIT(B)                 ((B) < 0)?(0):(((B) > 255)?(255):(B));
#define _LODWORD(Value)               ((DWORD)(Value))
#define _HIDWORD(Value)               ((DWORD)((Value)>>32))
#define _ALIGN_FRWRD(Value,Alignment) ((((Value)/(Alignment))+((bool)((Value)%(Alignment))))*(Alignment))
#define _ALIGN_BKWRD(Value,Alignment) (((Value)/(Alignment))*(Alignment))

#define HOOKJMPSIZE     5
#define NOP_OPCODE      0x90
#define CALLOFFCODE     0xE8
#define JUMPOFFCODE     0xE9
#define UNSAFELOOP      __asm{xzc: jmp xzc}
#define SAFELOOP(Delay) __asm Label: push Delay      \
                        __asm call dword ptr [Sleep] \
                        __asm jmp Label                                                                                                                      
//---------------------------------------------------------------------------
#define APIWRAPPER(LibPathName,NameAPI) extern "C" _declspec(dllexport) _declspec(naked) void __cdecl NameAPI(void) \
{ \
 static void* Address; \
 if(!Address)Address = GetProcAddress(LoadLibrary(LibPathName),#NameAPI); \
 __asm mov EAX, [Address] \
 __asm jmp EAX \
}
//------------------------------------------------------------------------------------------------------------------------
#undef PUNICODE_STRING
struct UNICODE_STRING 
{
 USHORT Length;
 USHORT MaximumLength;
 PWSTR  Buffer;
}; 
typedef	UNICODE_STRING *PUNICODE_STRING;
//---------------------------------------------------------------------------
template<class N, class M> __inline M NumToPerc(N Num, M MaxVal){return (((Num)*100)/(MaxVal));}
template<class P, class M> __inline M PercToNum(P Per, M MaxVal){return (((Per)*(MaxVal))/100);}                         
template<class T> __inline T    Min(T ValueA, T ValueB){return (ValueA > ValueB)?(ValueB):(ValueA);} 
template<class T> __inline T    Max(T ValueA, T ValueB){return (ValueA > ValueB)?(ValueA):(ValueB);}                      
template<class T> __inline bool IsInRange(T Value, T Minv, T Maxv){return ((Value >= Min(Minv,Maxv))&&(Value <= Max(Minv,Maxv)));}
                                   
template<class R, class T> __inline R GetAddress(T Src){return reinterpret_cast<R>(reinterpret_cast<void* &>(Src));}
template<class T> __inline void ZeroStruct(T &Stru){FastZeroMemory(&Stru,sizeof(Stru));}  
template<class T> __inline void ZeroPointer(T Stru){FastZeroMemory(Stru,sizeof(*Stru));}
template<class T> __inline T*   AddressOf(T Src){return reinterpret_cast<T*>(reinterpret_cast<void* &>(Src));}
template<class T> __inline int  SetAddressInVFT(PVOID VFT, int Index, T Proc, LPSTR Signature=NULL){PVOID Addr=reinterpret_cast<PVOID>(reinterpret_cast<void* &>(Proc));return ApplyPatchLocal(&((PVOID*)VFT)[Index], &Addr, sizeof(PVOID), Signature);}
template<class T> __inline bool SetPtrAddress(PVOID Src, T &Dst, LPSTR Signature=NULL)
{
 if(IsMemSignatureMatch(Src, Signature)){Dst = reinterpret_cast<T>(Src);return false;}
   else return true;
}

__inline void __cdecl SetAddress(PVOID SrcAddr,...){((PVOID*)(((PVOID*)&SrcAddr)[1]))[0] = SrcAddr;} 
    

#define FOREGROUND_YELLOW (FOREGROUND_RED|FOREGROUND_GREEN)
#define _MAKEVKEYCOMB(VKeyA,VKeyB,VKeyC,VKeyD) ((((DWORD)((BYTE)(VKeyA)))<<24)|(((DWORD)((BYTE)(VKeyB)))<<16)|(((DWORD)((BYTE)(VKeyC)))<<8)|((DWORD)((BYTE)(VKeyD))))
//---------------------------------------------------------------------------
//#define LOGMSGOUT	MessageOutFmt	 //(!LogFilePath[0])?(0):MessageOutFmt

#ifdef  DBGMESSAGES
  #define MESSAGEOUT    ConMessageOut
  #define MESSAGEOUTFMT ConMessageOutFmt
  #define INITDBGMSGOUT AllocConsole();
#else
  #define MESSAGEOUT(Message, TxtAttr)    
  #define MESSAGEOUTFMT(Message, TxtAttr, ...)
  #define INITDBGMSGOUT ;
#endif

#define DBGMSGOUT MESSAGEOUTFMT
//---------------------------------------------------------------------------
#define SETANYADDRESS(DstAddr,TakeFrom) __asm push EAX         \
                                        __asm mov EAX,TakeFrom \
                                        __asm mov DstAddr,EAX  \
                                        __asm pop EAX
//---------------------------------------------------------------------------
#define TAKEANYADDRESS(DstAddr,TakeFrom) __asm push EAX         \
                                         __asm lea EAX,TakeFrom \
                                         __asm mov DstAddr,EAX  \
                                         __asm pop EAX
//---------------------------------------------------------------------------
// Compiler always generates Stack Frame Pointers for member functions - used 'pop EBP' to restore it
// How to force the compiler do not make stack frame pointer ?
// How to force the compiler do not push ECX ?
// For __thiscall
#define JUMPEXTMEMBER(Base,Offset) __asm pop  ECX                \
                                   __asm pop  EBP                \
                                   __asm mov  EAX, Base          \
                                   __asm lea  EAX, [EAX+Offset]  \
	                            __asm jmp  EAX
//---------------------------------------------------------------------------
// Compiler always generates Stack Frame Pointers for member functions - used 'pop EBP' to restore it
// How to force the compiler do not make stack frame pointer ?
// How to force the compiler do not push ECX ?
// For __thiscall
#define JUMPVFTMEMBER(Index) __asm pop  ECX           \
                             __asm pop  EBP           \
                             __asm mov  EAX, [ECX]    \
                             __asm add  EAX, Index*4  \
                             __asm jmp  [EAX] 
//---------------------------------------------------------------------------
#ifdef __CRTDECL
void* __CRTDECL operator new(size_t size);
//void* __CRTDECL operator new[](size_t width, size_t size);
void  __CRTDECL operator delete(void* mem);
//void  __CRTDECL operator delete[](void* mem);


#endif

void*  __cdecl memmove(void* _Dst, const void* _Src, size_t _Size);
void*  __cdecl memcpy(void* _Dst, const void* _Src, size_t _Size);
void*  __cdecl memset(void* _Dst, int _Val, size_t _Size);
int    __cdecl memcmp(const void* _Buf1, const void* _Buf2,  size_t _Size);

//---------------------------------------------------------------------------

typedef unsigned __int64 QWORD;
typedef QWORD* PQWORD;

void _cdecl MessageOut(LPSTR Message, void (_stdcall *MsgOutProc)(LPSTR), ...);
//---------------------------------------------------------------------------
struct UNISTRING
{
 USHORT Length;
 USHORT MaximumLength;
 PWSTR  Buffer;
};

struct PROCPARAMS
{
 ULONG     AllocationSize;
 ULONG     ActualSize;
 ULONG     Flags;
 ULONG     Unknown1;
 ULONG     Unknown2;
 ULONG     Unknown3;
 HANDLE    InputHandle;
 HANDLE    OutputHandle;
 HANDLE    ErrorHandle;
 UNISTRING CurrentDirectory;
 HANDLE    CurrentDir;
 UNISTRING SearchPaths;
 UNISTRING ApplicationName;
 UNISTRING CommandLine;
 PVOID     EnvironmentBlock;
 ULONG     Unknown[9];
 UNISTRING Unknown4;
 UNISTRING Unknown5;
 UNISTRING Unknown6;
 UNISTRING Unknown7;
};

struct PEBL
{
 ULONG       AllocationSize;
 ULONG       Unknown1;
 HANDLE      ProcessInstance;
 PVOID       DllList;
 PROCPARAMS* ProcessParameters;
 ULONG       Unknown2;
 HANDLE      DefaultHeap;
};

struct PROCBASICINFO
{
 PVOID  Reserved1;
 PEBL*  PebBaseAddress;
 PVOID  Reserved2[2];
 ULONG* UniqueProcessId;
 PVOID  Reserved3;
};
//---------------------------------------------------------------------------
#pragma pack(push,1)
struct HDE_STRUCT
{
 BYTE  Len;                // length of command
 BYTE  P_rep;              // rep/repnz/.. prefix: 0xF2 or 0xF3
 BYTE  P_lock;             // lock prefix 0xF0
 BYTE  P_seg;              // segment prefix: 0x2E, 0x36, 0x3E, 0x26, 0x64, 0x65
 BYTE  P_66;               // prefix 0x66
 BYTE  P_67;               // prefix 0x67
 BYTE  Opcode;             // opcode
 BYTE  Opcode2;            // second opcode, if first opcode equal 0x0F
 BYTE  Modrm;              // ModR/M byte
 BYTE  Modrm_mod;          //   - mod byte of ModR/M
 BYTE  Modrm_reg;          //   - reg byte of ModR/M
 BYTE  Modrm_rm;           //   - r/m byte of ModR/M
 BYTE  Sib;                // SIB byte
 BYTE  Sib_scale;          //   - scale (ss) byte of SIB
 BYTE  Sib_index;          //   - index byte of SIB
 BYTE  Sib_base;           //   - base byte of SIB
 BYTE  Imm8;               // immediate imm8
 WORD  Imm16;              // immediate imm16
 DWORD Imm32;              // immediate imm32
 BYTE  Disp8;              // displacement disp8
 WORD  Disp16;             // displacement disp16, if prefix 0x67 exist
 DWORD Disp32;             // displacement disp32
 BYTE  Rel8;               // relative address rel8
 WORD  Rel16;              // relative address rel16, if prefix 0x66 exist
 DWORD Rel32;              // relative address rel32
 BYTE  Immpresent;		   // It`s need, because value can be '0'
 BYTE  Dispresent;		   // It`s need, because value can be '0'
 BYTE  Relpresent;		   // It`s need, because value can be '0'
};
#pragma pack(pop)
extern "C" void  _stdcall  hde_disasm(PVOID Address, HDE_STRUCT *HdeInfo);
//---------------------------------------------------------------------------
typedef long (_stdcall *LDR_LOAD_DLL)(PWCHAR PathToFile, ULONG Flags, PUNICODE_STRING ModuleFileName, PHANDLE ModuleHandle);
													 
void    _stdcall ConMessageOut(LPSTR Message, DWORD TxtAttr);
void    _cdecl   ConMessageOutFmt(LPSTR Message, DWORD TxtAttr, ...);
void    _cdecl   MessageOutFmt(LPSTR Message, PVOID MsgOutProc, ...);
DWORD   _stdcall HexStrToDW(LPSTR String, UINT Len);   // Fast, but do not safe
DWORD   _stdcall DecStrToDW(LPSTR String);
QWORD   _stdcall DecStrToDW64(LPSTR String);
DWORD   _stdcall ComputeCRC32(PVOID Buffer, DWORD BufferSize);
WORD    _stdcall ComputeCRC16(PVOID Buffer, DWORD BufferSize);
long    _stdcall CharToHex(BYTE CharValue); 
PVOID   _stdcall FindProcEntry(HMODULE Module, LPSTR ProcName);
PVOID   _stdcall FindProcAddress(HMODULE Module, LPSTR ProcName);
PVOID   _stdcall FindMemSignatureLocal(PVOID Address, LPSTR Signature);
BOOL    _stdcall IsNumberInRange(long Number, long TresholdA, long TresholdB);
BOOL    _stdcall IsKeyCombinationPressed(DWORD Combination);
BOOL    _stdcall IsMemSignatureMatch(PVOID Address, LPSTR Signature, int SigLen=-1);
BOOL    _stdcall SetWinConsoleSizes(DWORD WndWidth, DWORD WndHeight, DWORD BufWidth, DWORD BufHeight);
BOOL    _stdcall IsUnicodeString(PVOID String);
int     _stdcall GetCharCount(LPSTR String, char Par);
int     _stdcall CharPosFromLeft(LPSTR String, char Letter, int Index, int Offset);
int     _stdcall CharPosFromRight(LPSTR String, char Letter, int Index, int Offset);
DWORD   _stdcall WriteLocalProtectedMemory(PVOID Address, PVOID Data, DWORD DataSize, bool RestoreProt);
DWORD   _stdcall WriteProtectedMemory(DWORD ProcessID, HANDLE hProcess, PVOID Address, PVOID Data, DWORD DataSize, bool RestoreProt);
DWORD   _stdcall ApplyPatchLocal(PVOID PatchAddr, PVOID PatchData, DWORD PatchSize, LPSTR Signature);
DWORD   _stdcall AddrToRelAddr(PVOID CmdAddr, DWORD CmdLen, PVOID TgtAddr);
PVOID   _stdcall RelAddrToAddr(PVOID CmdAddr, DWORD CmdLen, DWORD TgtOffset);
DWORD   _stdcall SetFunctionPatch(HMODULE Module, DWORD Offset, PVOID FuncAddr, BYTE CmdCode, DWORD AddNOP, LPSTR Signature);
LPSTR   _stdcall FormatCurDateTime(LPSTR DateTimeStr);
HMODULE _stdcall GetOwnerModule(PVOID Address);
DWORD   _stdcall UpdateProcessImports(PVOID OldProcAddr, PVOID NewProcAddr, BOOL OnlyLocalModules);
DWORD   _stdcall UpdateModuleImports(PVOID OldProcAddr, PVOID NewProcAddr, HMODULE hTgtModule);
PVOID   _stdcall FindLocalModule(PVOID *ModuleBase, DWORD *ModuleSize);
BOOL    _stdcall SetDbgFlag(BOOL Flag);
int     _stdcall GetDesktopRefreshRate(void);
LPSTR   _stdcall ConvertToHexDW(DWORD Value, DWORD MaxDigits, LPSTR Number, bool UpCase);
LPSTR   _stdcall ConvertToDecDW(DWORD Value, LPSTR Number);
long    _stdcall GetProcessPathByID(DWORD ProcessID, LPSTR PathBuffer, long BufferLngth);
long    _stdcall GetProcessPathByHandle(HANDLE hProcess, LPSTR PathBuffer, long BufferLngth);
LPSTR   _stdcall TrimToFilePath(LPSTR FullPath);
bool    _stdcall SaveINIValueInt(LPSTR SectionName, LPSTR ValueName, int Value, LPSTR FileName);
int     _stdcall RefreshINIValueInt(LPSTR SectionName, LPSTR ValueName, int Default, LPSTR FileName);
int     _stdcall RefreshINIValueStr(LPSTR SectionName, LPSTR ValueName, LPSTR Default, LPSTR RetString, DWORD Size, LPSTR FileName);
bool    _stdcall FindFile(LPSTR FilePath, LPSTR OutBuffer=NULL);
//bool    _stdcall IsValidDSBuffer(LPDIRECTSOUNDBUFFER pDSBuffer);
DWORD   _stdcall GetFileNameByHandle(HANDLE hFile, LPSTR Name);
DWORD   _stdcall SetProcRedirection(PVOID Address, PVOID TargetAddr, LPSTR Signature=NULL);
DWORD   _stdcall GetRealModuleSize(HANDLE hProcess, DWORD ModuleBase);
BOOL    _stdcall ForceProcessSingleCore(HANDLE hProcess);
int     _stdcall ConvertFromUtf8(LPSTR DstStr, LPSTR SrcStr, UINT DstSize);
int     _stdcall ConvertToUtf8(LPSTR DstStr, LPSTR SrcStr, UINT DstSize);
void    _stdcall CreateDirectoryPath(LPSTR Path);

ULONGLONG __stdcall ShlULL(ULONGLONG Value, LONG Shift);
ULONGLONG __stdcall ShrULL(ULONGLONG Value, LONG Shift);
ULONGLONG __stdcall BinLongUMul(ULONGLONG Multiplicand, ULONGLONG Multiplier);
ULONGLONG __stdcall BinLongUDiv(ULONGLONG Dividend, ULONGLONG Divisor, ULONGLONG *Rem=NULL);



UINT  _stdcall TrimFilePath(LPSTR FullPath);
LPSTR _stdcall GetFileName(LPSTR FullPath);
LPSTR _stdcall GetFileExt(LPSTR FullPath);
PWSTR _stdcall GetFileExt(PWSTR FullPath);
PWSTR _stdcall GetFileName(PWSTR FullPath);


float _stdcall FPUSqrt32(float Value);
long  _stdcall FPURound32(float Value);
long  _stdcall FPUDiv32(float ValueA, float ValueB);
float _stdcall FPURem32(float ValueA, float ValueB);
float _stdcall FPUSinRad32(float Angle);
float _stdcall FPUSinDeg32(float Angle);
float _stdcall FPUCosRad32(float Angle);
float _stdcall FPUCosDeg32(float Angle);
__int64 _stdcall FPUSinCosRad32(float Angle);
__int64 _stdcall FPUSinCosDeg32(float Angle);
 
//-------------------------------------------------------------------------
template<typename T> double _stdcall tiny_strtod(const T* s, T **endp)
{
    const T* p = (T*)s;
    double res = 0;
    int negative = 0;
    unsigned int val;

    // skip initial whitespaces
    while (*p && *p <= ' ') ++p;

    if (p[0] == 0)
    {
        if (endp) *endp = (T*)p;
        return 0.0;
    }

    // check sign
    if (p[0] == '+' || p[0] == '-')
    {
        negative = p[0] == '-' ? 1 : 0;
        ++p;
    }

    // get non-fractional part
    for ( ; (val = p[0] - '0') < 10; ++p) res = 10.0*res + val;

    // get fractional part
    if (p[0] == '.')
    {
        double factor = 0.1;

        for (++p; (val = p[0] - '0') < 10; ++p)
        {
            res += val*factor;
            factor *= 0.1;
        }
    }

    // get exponent
    if (p[0] == 'e' || p[0] == 'E')
    {
        int exponent = 0;
        double factor = 10.0;

        ++p;

        // check sign
        if (p[0] == '+' || p[0] == '-')
        {
            if (p[0] == '-') factor = 0.1;
            ++p;
        }

        // get exponent value
        for ( ; (val = p[0] - '0') < 10; ++p) exponent = 10*exponent + val;

        // perform exponentiation
        for ( ; exponent; exponent >>= 1)
        {
            if (exponent & 0x01) res *= factor;
            // TODO: possible overflow?
            factor *= factor;
        }
    }

    if (endp) *endp = (T*)p;

    return negative ? -res : res;
}
//-------------------------------------------------------------------------
template<typename T> long _stdcall tiny_strtol(const T* s, T** endp, int base)
{
    const T* p = (T*)s;
    unsigned long res = 0;
    int negative = 0;

    // skip initial whitespaces
    while (*p && *p <= ' ') ++p;

    if (p[0] == 0)
    {
        if (endp) *endp = (T*)p;
        return 0;
    }

    // check sign
    if ((p[0] == '+') || (p[0] == '-'))
    {
        negative = p[0] == '-' ? 1 : 0;
        ++p;
    }

    // figure out base if not given
    if (base == 0)
    {
        if (p[0] != '0')
        {
            base = 10;
        } else {
            if ((p[1] == 'x') || (p[1] == 'X'))
            {
                base = 16;
                p += 2;
            } else {
                base = 8;
                ++p;
            }
        }

    } else {

        // 0x/0X is allowed for hex even when base is given
        if ((base == 16) && (p[0] == '0'))
        {
            if ((p[1] == 'x') || (p[1] == 'X'))
            {
                p += 2;
            }
        }
    }

    for ( ; p[0]; ++p)
    {
        unsigned long val = p[0];

        if (val >= 'a') val -= 'a' - 'A';

        if (val >= 'A')
        {
            val = val - 'A' + 10;
        } else {
            if (val < '0' || val > '9') break;
            val = val - '0';
        }

        if (val >= (unsigned long)base) break;

        res = res*(unsigned long)base + val;
    }

    if (endp) *endp = (T*)p;

    return negative ? -((long)res) : (long)res;
}
//=============================================================================================================
template<typename T> void _stdcall HookEntryVFT(UINT Index, PVOID ClassPtr, PVOID* OrigAddr, T HookProc)
{
 PVOID  Proc = GetAddress<PVOID>(HookProc);  //  (&HookIDirect3D9::HookCreateDevice);
 if((*((PVOID**)ClassPtr))[Index] != Proc)
  {
   *OrigAddr = (*((PVOID**)ClassPtr))[Index]; 
   WriteLocalProtectedMemory(&(*((PVOID**)ClassPtr))[Index], &Proc, sizeof(PVOID), TRUE);
  }
}
//=============================================================================================================
//
//                                     HOOKS IMPORT TABLE OF MODULES
//
//-------------------------------------------------------------------------------------------------------------
struct IMPORTHOOK
{
 PVOID   HookAddress;
 PVOID   OriginalAddr;
 HMODULE hCurModule;
 //---------------------------------------
 DWORD _stdcall SetHook(PVOID HookProc, LPSTR ModuleName, LPSTR ProcName, HMODULE hTargetMod, BOOL LoadIfNotExist)
  {
   HMODULE Module;
 
   DBGMSGOUT("IMPORTHOOK::SetImportHook -> Trying to set export hook: %s::%s to %08X.",FOREGROUND_GREEN,ModuleName,ProcName,(DWORD)HookProc);
   HookAddress  = NULL;
   OriginalAddr = NULL;
   Module       = GetModuleHandle(ModuleName);
   if(!Module && LoadIfNotExist)Module = LoadLibrary(ModuleName);
   if(!Module)return 1;
   
   hCurModule   = hTargetMod;
   OriginalAddr = FindProcAddress(Module, ProcName);
   if(!OriginalAddr)return 2;
   HookAddress  = HookProc;
   return UpdateModuleImports(OriginalAddr, HookProc, hCurModule);  
  }
 //---------------------------------------  
 DWORD _stdcall RemoveHook(void)
  {
   if(!HookAddress || !OriginalAddr)return 1;
   DBGMSGOUT("IMPORTHOOK::Restoring...",FOREGROUND_GREEN); 
   return UpdateModuleImports(HookAddress, OriginalAddr, hCurModule);  
  }
};
//=============================================================================================================
//
//                                        HOOKS EXPORT TABLE OF MODULES
//
//-------------------------------------------------------------------------------------------------------------
struct EXPORTHOOK
{
 PVOID EntryAddr;
 PVOID ProcAddr;
 PVOID HookAddr;
 BOOL  OnlyLocal;
 //---------------------------------------
 DWORD _stdcall SetHook(PVOID HookProc, LPSTR ModuleName, LPSTR ProcName, BOOL LoadIfNotExist, BOOL HookOnlyLocal)
  {
   HMODULE Module;
 
   DBGMSGOUT("EXPORTHOOK::SetExportHook -> Trying to set export hook: %s::%s to %08X.",FOREGROUND_GREEN,ModuleName,ProcName,(DWORD)HookProc);
   EntryAddr = NULL;
   ProcAddr  = NULL;
   HookAddr  = NULL;
   OnlyLocal = HookOnlyLocal;
   Module    = GetModuleHandle(ModuleName);
   if(!Module && LoadIfNotExist)Module = LoadLibrary(ModuleName);
   if(!Module)return 1;
   
   EntryAddr = FindProcEntry(Module, ProcName);
   if(!EntryAddr)return 2;
   
   ProcAddr  = &((BYTE*)Module)[(((DWORD*)EntryAddr)[0])];
   if(!ProcAddr)return 3;
   
   if(WriteLocalProtectedMemory(EntryAddr, &HookProc, 4, TRUE))DBGMSGOUT("EXPORTHOOK::Export %s::%s replaced to %08X.", FOREGROUND_GREEN,ModuleName,ProcName,(DWORD)HookProc);          
     else DBGMSGOUT("EXPORTHOOK::Replacing export %s::%s FAILED!.",FOREGROUND_RED,ModuleName,ProcName); 			
    
   DBGMSGOUT("EXPORTHOOK::SetExportHook -> Hook is set for ET_ENTRY=%08X, PROC=%08X.",FOREGROUND_GREEN,(DWORD)EntryAddr,(DWORD)ProcAddr);   
   HookAddr = HookProc; 
   return UpdateProcessImports(ProcAddr, HookAddr, HookOnlyLocal);
  } 
 //---------------------------------------  
 DWORD _stdcall RemoveHook(void)
  {
   if(!EntryAddr || !ProcAddr || !HookAddr)return 1;
   if(WriteLocalProtectedMemory(EntryAddr, &ProcAddr, 4, TRUE))DBGMSGOUT("EXPORTHOOK::Export restored at %08X.", FOREGROUND_GREEN,(DWORD)ProcAddr);          
     else DBGMSGOUT("EXPORTHOOK::Export restoring FAILED!.",FOREGROUND_RED); 			 
   return UpdateProcessImports(HookAddr, ProcAddr, OnlyLocal);
  }
};
//=============================================================================================================
//
//
//-------------------------------------------------------------------------------------------------------------
// !!! MUST BE SET ONLY ON BEGIN OF A FUNCTION (Checks return address to detect a caller module) !!!
// HookProc do not called, when call to function comes from Module, setted the hook
//
//               PUSHAD
//               MOV EAX, [ESP+20]
//               MOV ECX, 0   // Hooker Module Begin Addr
//               MOV EDX, 0   // Hooker Module End Addr
//               CMP EAX, ECX
//               JB SHORT GoToHookProc
//               CMP EAX, EDX
//               JA SHORT GoToHookProc
//               POPAD
//               NOP         // From here begins original code
//               NOP
//               NOP
//               NOP
//               NOP
//               NOP
//               NOP
//               NOP
//               NOP
//               NOP
//               JMP AfterReplacedCode
//GoToHookProc:  POPAD
//               JMP HookProcAddr
//
struct CODEHOOK
{
 PVOID ContAddr;         // To where return from 'HookCall' (rigth after replaced code)
 PVOID ProcAddr;         // Where must be placed 'JMP' to 'HookCall'
 PVOID HookAddr;         // Addr of a hook proc, called from 'HookCall'
 PVOID JumpAddr;         // Where really was placed 'JMP' to 'HookCall'
 DWORD BytesTaked;       // Taked from target code
 BYTE  HookCall[64];
 //---------------------------------------

 DWORD _stdcall SetHookToAddress(PVOID AddressToHook, PVOID HookProc)
  {
   DWORD      Value;
   DWORD      Offset;
   PVOID      Address;
   BYTE       CodePatch[9];
   BYTE       CodeBlock[] = {0x60,0x8B,0x44,0x24,0x20,0xB9,0x00,0x00,0x00,0x00,0xBA,0x00,0x00,0x00,0x00,0x3B,0xC1,0x72,0x14,0x3B,0xC2,0x77,0x10,0x61,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0xE9,0x5F,0xDB,0x3E,0xFF,0x61,0xE9,0x59,0xDB,0x3E,0xFF};
   HDE_STRUCT HdeInfo;
 
   Offset   = 0;
   ContAddr = NULL;
   JumpAddr = NULL;   
   HookAddr = HookProc;
   ProcAddr = AddressToHook;
   do
    {
     hde_disasm(&((BYTE*)AddressToHook)[Offset],&HdeInfo);
     if(HdeInfo.Relpresent)
      {   
       // WARNING: Possible wrong hook - jump followed     
       AddressToHook = RelAddrToAddr(&((BYTE*)AddressToHook)[Offset], HdeInfo.Len, (HdeInfo.Rel8+HdeInfo.Rel16+HdeInfo.Rel32));
	   Offset = 0;  
      }
       else Offset += HdeInfo.Len;
    }
     while(Offset < HOOKJMPSIZE);
  
   BytesTaked = Offset;
   JumpAddr   = AddressToHook;
   ContAddr   = &((BYTE*)AddressToHook)[Offset];    
   FastMoveMemory(&HookCall,&CodeBlock,sizeof(CodeBlock));    
   FastMoveMemory(&HookCall[24],AddressToHook,Offset);	  // Save Previous Code
  
   Value        = AddrToRelAddr(AddressToHook, HOOKJMPSIZE, &HookCall); 
   CodePatch[0] = JUMPOFFCODE; 
   CodePatch[1] = ((BYTE*)&Value)[0]; 
   CodePatch[2] = ((BYTE*)&Value)[1]; 
   CodePatch[3] = ((BYTE*)&Value)[2]; 
   CodePatch[4] = ((BYTE*)&Value)[3]; 
   
   Value        = AddrToRelAddr(&HookCall[34], HOOKJMPSIZE, ContAddr); 
   HookCall[35] = ((BYTE*)&Value)[0];
   HookCall[36] = ((BYTE*)&Value)[1];          
   HookCall[37] = ((BYTE*)&Value)[2];
   HookCall[38] = ((BYTE*)&Value)[3];              
 
   Value        = AddrToRelAddr(&HookCall[40], HOOKJMPSIZE, HookProc); 
   HookCall[41] = ((BYTE*)&Value)[0];
   HookCall[42] = ((BYTE*)&Value)[1];          
   HookCall[43] = ((BYTE*)&Value)[2];
   HookCall[44] = ((BYTE*)&Value)[3];              

   Address      = GetOwnerModule(&WriteLocalProtectedMemory); 
   HookCall[6]  = ((BYTE*)&Address)[0];
   HookCall[7]  = ((BYTE*)&Address)[1];          
   HookCall[8]  = ((BYTE*)&Address)[2];
   HookCall[9]  = ((BYTE*)&Address)[3];              
  
   Value = (DWORD)FindLocalModule(&Address, &Value)+Value;    
   HookCall[11] = ((BYTE*)&Value)[0];
   HookCall[12] = ((BYTE*)&Value)[1];          
   HookCall[13] = ((BYTE*)&Value)[2];
   HookCall[14] = ((BYTE*)&Value)[3];              

   if(WriteLocalProtectedMemory(AddressToHook, &CodePatch, HOOKJMPSIZE, TRUE))DBGMSGOUT("CODEHOOK::Hook setted to %08X.", FOREGROUND_GREEN,(DWORD)AddressToHook);          
     else DBGMSGOUT("CODEHOOK::Replacing code FAILED!.",FOREGROUND_RED); 			

 return Offset;
}
//---------------------------------------------------------------------------
 DWORD _stdcall SetHook(PVOID HookProc, LPSTR ModuleName, LPSTR ProcName, BOOL LoadIfNotExist)
  {
   HMODULE Module = GetModuleHandle(ModuleName);  
   if(!Module && LoadIfNotExist)Module = LoadLibrary(ModuleName);
   if(!Module)return 1;
   return this->SetHook(HookProc,Module,ProcName,LoadIfNotExist);  
  }
 DWORD _stdcall SetHook(PVOID HookProc, HMODULE Module, LPSTR ProcName, BOOL LoadIfNotExist)
  {
   PVOID   Address;
   
   DBGMSGOUT("CODEHOOK::SetCodeHook -> Trying to set code hook: %s::%s to %08X.",FOREGROUND_GREEN,ModuleName,ProcName,(DWORD)HookProc);
   ContAddr = NULL;
   ProcAddr = NULL;
   HookAddr = NULL;
   JumpAddr = NULL; 
  
   Address = FindProcAddress(Module, ProcName);
   if(!Address)return 2;
  
   if(!SetHookToAddress(Address, HookProc))return 3;
   return 0;
  }

//---------------------------------------------------------------------------
 DWORD _stdcall RemoveHook(void)
  {
   if(!JumpAddr || !ProcAddr || !HookAddr)return 1;
   if(WriteLocalProtectedMemory(JumpAddr, &HookCall[24], BytesTaked, TRUE))DBGMSGOUT("CODEHOOK::Original code restored at %08X.", FOREGROUND_GREEN,(DWORD)JumpAddr);          
     else DBGMSGOUT("CODEHOOK::Restoring code FAILED!",FOREGROUND_RED); 			
   return 0;
  }
//---------------------------------------------------------------------------
 BOOL _stdcall IsHookValid(void)
  {
   if(!JumpAddr || !ProcAddr || !HookAddr)return 0; 
   DWORD Value       = AddrToRelAddr(JumpAddr, HOOKJMPSIZE, &HookCall); 
   BYTE  CodePatch[] = {JUMPOFFCODE,((BYTE*)&Value)[0],((BYTE*)&Value)[1],((BYTE*)&Value)[2],((BYTE*)&Value)[3]};
   if(FastCompMemory(&CodePatch,JumpAddr,HOOKJMPSIZE))return 0;
   return 1;
  }
};
//===========================================================================
// !!! Calls to code from 'Hooker Module' will be also intercepted !!!
//                   !!! THREAD SAFE HOOK !!! 
//
// Stack ESP+NN - Used space
// Stack ESP-NN - Unused space
// 60 9C FF 35 00 00 40 00 E8 C8 00 00 00 5A 8B 4C 24 10 89 51 B0 84 C0 75 06 9D 61 FF 64 24 B0 9D 61
//---------------------------------------------------------------------------
struct HOOKREGS
{
 DWORD Reg_EDI;
 DWORD Reg_ESI;
 DWORD Reg_EBP;
 DWORD Reg_ESP;
 DWORD Reg_EBX;
 DWORD Reg_EDX;
 DWORD Reg_ECX;
 DWORD Reg_EAX;
 DWORD Reg_EFL; 
 DWORD Reg_EIP;
};
class CODEHOOK32
{
#pragma pack(push,1)
 struct FHOOKCALL
  {
   BYTE  PUSHFD_9C;                        // pushfd
   BYTE  PUSHAD_60;                        // pushad                      // Stored ESP is not changed
   BYTE  CALL_E8;   DWORD HookProcRel;     // call   HookProc
   BYTE  POPAD_61;                         // popad
   BYTE  POPFD_9D;                         // popfd
   BYTE  OldCode[64];        // Original code, modified if needed to make it work here
  };
 struct CHOOKCALL
  {
   BYTE  PUSHAD_60;                        // pushad                      // Stored ESP is not changed
   BYTE  PUSHFD_9C;                        // pushfd
   WORD  PUSH_FF35; DWORD RetAddr;         // push   [0x00000000]         // RETURN ADDRESS [EIP](Allowed to modify as argument)
   BYTE  CALL_E8;   DWORD HookProcRel;     // call   HookProc
   BYTE  POP_EDX_5A;                       // pop  EDX                    // Get return address (possibly modifyed)
   DWORD MOV_ECX_8B4C2410;                 // mov  ECX, [ESP+0x0010]      // Get stored ESP (possibly modifyed)
   BYTE  MOV_StackRet_89; WORD _EDX_51B0;  // mov  [ECX-0x0050], EDX      // Write ret address to unused stack slot (For possibly modifyed ESP)
   WORD  TEST_AL_AL_84C0;                  // test AL, AL
   WORD  JNZ_7506;                         // jnz OldCodeStart
   BYTE  POPFD_9D;                         // popfd
   BYTE  POPAD_61;                         // popad
   DWORD JMP_FF6424B0;                     // jmp [ESP-0x0050]            // Unused stack slot   [FF 64 24 B0]
   BYTE  POPFD__9D;                        // OldCodeStart: popfd
   BYTE  POPAD__61;                        //               popad
   BYTE  OldCode[64];        // Original code, modified if needed to make it work here
  };
#pragma pack(pop)
 //---------------------------------------
public:
 typedef void (_cdecl *FUNCHOOKPROC32)(HVL HOOKREGS Registers,...);   // Check registers order !

 typedef BOOL (_cdecl *CODEHOOKPROC32)(HVL DWORD Reg_EIP,HVL DWORD Reg_EFL,HVL DWORD Reg_EDI,HVL DWORD Reg_ESI,HVL DWORD Reg_EBP,HVL DWORD Reg_ESP,HVL DWORD Reg_EBX,HVL DWORD Reg_EDX,HVL DWORD Reg_ECX,HVL DWORD Reg_EAX,...);    // Return 'TRUE' to execute original code

 PVOID JumpAddr;          // Where really was placed 'JMP' to 'CHookStub'
 PVOID ContAddr;          // To where return from 'CHookStub' (rigth after replaced code)
 DWORD BytesTaked;        // Taked from target code
 PVOID HookAddr;          // Addr of a hook proc, called from 'CHookStub'

 //--------------------------------------- 
 DWORD _stdcall RemoveHook(void)
  {
   if(!JumpAddr || !TargAddr || !HookAddr)return 1;
   if(WriteLocalProtectedMemory(JumpAddr, &OriginalCode, BytesTaked, TRUE))//DBGMSGOUT("CODEHOOK32::Original code restored at %08X.", FOREGROUND_GREEN,(DWORD)JumpAddr);
    // else DBGMSGOUT("CODEHOOK32::Restoring code FAILED!",FOREGROUND_RED);
   return 0;
  }
 //---------------------------------------
 DWORD _stdcall SetCodeHook(PVOID AddressToHook, CODEHOOKPROC32 HookProc, LPSTR Signature=NULL)
  {
   DWORD Result; 
   DWORD OCBytes;
   BYTE  CodePatch[32];

   if(IsBadReadPtr(AddressToHook, HOOKJMPSIZE))return 9;
   CHookStub.PUSHAD_60        = 0x60;
   CHookStub.PUSHFD_9C        = 0x9C;
   CHookStub.PUSH_FF35        = 0x35FF;
   CHookStub.CALL_E8          = 0xE8;
   CHookStub.POP_EDX_5A       = 0x5A;
   CHookStub.MOV_ECX_8B4C2410 = 0x10244C8B;
   CHookStub.MOV_StackRet_89  = 0x89;
   CHookStub._EDX_51B0        = 0xB051;
   CHookStub.TEST_AL_AL_84C0  = 0xC084;
   CHookStub.JNZ_7506         = 0x0675;
   CHookStub.POPFD_9D         = 0x9D;
   CHookStub.POPAD_61         = 0x61;
   CHookStub.JMP_FF6424B0     = 0xB02464FF;
   CHookStub.POPFD__9D        = 0x9D;
   CHookStub.POPAD__61        = 0x61;
   
   OCBytes = PrepareCodeHook(AddressToHook, HOOKJMPSIZE, (PBYTE)&CHookStub.OldCode);
   FastFillMemory(&CodePatch, BytesTaked, 0x90);    // Watch for the buffer overflow !
   HookAddr                   = HookProc;
   CodePatch[0]               = JUMPOFFCODE;
   CHookStub.RetAddr          = (DWORD)&ContAddr;
   CHookStub.HookProcRel      = AddrToRelAddr(&CHookStub.CALL_E8, HOOKJMPSIZE, HookProc);  
   ((PDWORD)&CodePatch[1])[0] = AddrToRelAddr(JumpAddr, HOOKJMPSIZE, &CHookStub);
   if(!StackMod)((PDWORD)&CHookStub.OldCode[OCBytes])[0] = CHookStub.JMP_FF6424B0;          // Write returning jump
     else
      {
       CHookStub.OldCode[OCBytes] = JUMPOFFCODE;
       ((PDWORD)&CHookStub.OldCode[OCBytes+1])[0] = AddrToRelAddr(&CHookStub.OldCode[OCBytes], HOOKJMPSIZE, ContAddr);
      }

   /*
   if(WriteLocalProtectedMemory(AddressToHook, &CodePatch, HOOKJMPSIZE, TRUE))//DBGMSGOUT("CODEHOOK32::Hook placed at %08X.", FOREGROUND_GREEN,(DWORD)AddressToHook);
   //  else DBGMSGOUT("CODEHOOK32::Replacing code FAILED!.",FOREGROUND_RED);
      */
   if(((PBYTE)JumpAddr)[0]==CodePatch[0])Signature=NULL;     // Don`t check signature if address are possibly already patched
   Result = ApplyPatchLocal(JumpAddr, &CodePatch, BytesTaked, Signature);     // !!!!!!!!!!!!! BytesTaked;   
   if(Result == 0)Busy = TRUE;
   return Result;
  }
 //---------------------------------------
 DWORD _stdcall SetFuncHook(PVOID AddressToHook, FUNCHOOKPROC32 HookProc, LPSTR Signature=NULL)
  {
   DWORD Result; 
   DWORD OCBytes;
   BYTE  CodePatch[32];

   if(IsBadReadPtr(AddressToHook, HOOKJMPSIZE))return 9;
   FHookStub.PUSHFD_9C = 0x9C;
   FHookStub.PUSHAD_60 = 0x60;
   FHookStub.CALL_E8   = 0xE8;
   FHookStub.POPAD_61  = 0x61;
   FHookStub.POPFD_9D  = 0x9D;
   
   OCBytes = PrepareCodeHook(AddressToHook, HOOKJMPSIZE, (PBYTE)&FHookStub.OldCode);
   FastFillMemory(&CodePatch, BytesTaked, 0x90);    // Watch for the buffer overflow !
   HookAddr     = HookProc;
   CodePatch[0] = JUMPOFFCODE;
   ((PDWORD)&CodePatch[1])[0] = AddrToRelAddr(JumpAddr, HOOKJMPSIZE, &FHookStub);
   FHookStub.HookProcRel      = AddrToRelAddr(&FHookStub.CALL_E8, HOOKJMPSIZE, HookProc);
   FHookStub.OldCode[OCBytes] = JUMPOFFCODE;
   ((PDWORD)&FHookStub.OldCode[OCBytes+1])[0] = AddrToRelAddr(&FHookStub.OldCode[OCBytes], HOOKJMPSIZE, ContAddr);
      
   /*
   if(WriteLocalProtectedMemory(AddressToHook, &CodePatch, HOOKJMPSIZE, TRUE))//DBGMSGOUT("CODEHOOK32::Hook placed at %08X.", FOREGROUND_GREEN,(DWORD)AddressToHook);
   //  else DBGMSGOUT("CODEHOOK32::Replacing code FAILED!.",FOREGROUND_RED);
      */
   if(((PBYTE)JumpAddr)[0]==CodePatch[0])Signature=NULL;     // Don`t check signature if address are possibly already patched    
   Result = ApplyPatchLocal(JumpAddr, &CodePatch, BytesTaked, Signature);     // !!!!!!!!!!!!! BytesTaked;   
   if(Result == 0)Busy = TRUE;
   return Result;
  }
 
 //---------------------------------------
 // TODO: Rework this function !
 DWORD _stdcall SetCmdValue(PVOID CmdAddress, PVOID Value, LPSTR Signature=NULL)
  {
   DWORD Result = 3;
   HDE_STRUCT HdeInfo;
   BYTE CodePatch[32];

   if(IsBadReadPtr(CmdAddress, HOOKJMPSIZE))return 9;
   hde_disasm(CmdAddress, &HdeInfo);
   FastMoveMemory(&CodePatch, CmdAddress, HdeInfo.Len);
   FastMoveMemory(&OriginalCode, CmdAddress, HdeInfo.Len); 
   if(HdeInfo.Relpresent)
    {  
     ((PDWORD)&CodePatch[1])[0] = AddrToRelAddr(CmdAddress, HdeInfo.Len, Value);
     Result = ApplyPatchLocal(CmdAddress, &CodePatch, HdeInfo.Len, Signature);
    }
   if(HdeInfo.Immpresent)
    {
     ((PDWORD)&CodePatch[1])[0] = reinterpret_cast<DWORD>(Value);
     Result = ApplyPatchLocal(CmdAddress, &CodePatch, HdeInfo.Len, Signature);
    }  
   if(Result == 0)Busy = TRUE;
   JumpAddr   = CmdAddress;
   BytesTaked = HdeInfo.Len;
   return Result;
  } 
 //---------------------------------------
 DWORD _stdcall SetCallHook(PVOID CmdAddress, PVOID Value, LPSTR Signature=NULL)
  {
   DWORD Result = 3;
   HDE_STRUCT HdeInfo;
   BYTE CodePatch[32];

   if(IsBadReadPtr(CmdAddress, HOOKJMPSIZE))return 9;
   hde_disasm(CmdAddress, &HdeInfo);
   FastFillMemory(&CodePatch,HdeInfo.Len,NOP_OPCODE);
   FastMoveMemory(&OriginalCode, CmdAddress, HdeInfo.Len); 
  
   CodePatch[0] = CALLOFFCODE;
   ((PDWORD)&CodePatch[1])[0] = AddrToRelAddr(CmdAddress, HOOKJMPSIZE, Value);
   Result = ApplyPatchLocal(CmdAddress, &CodePatch, HdeInfo.Len, Signature);
    
   if(Result == 0)Busy = TRUE;
   JumpAddr   = CmdAddress;
   BytesTaked = HdeInfo.Len;
   return Result;
  } 
 //---------------------------------------
  
  
  
private: 
 BOOL      Busy;
 BOOL      StackMod;          // Replaced code modifies the stack - Changing EIP will no effect when executing replaced code
 PVOID     TargAddr;          // Where must be placed 'JMP' to 'CHookStub'
 CHOOKCALL CHookStub;         // Code of a hook stub
 FHOOKCALL FHookStub;         // Code of a hook stub
 BYTE      OriginalCode[64];  // Original, don`t modified code
 //---------------------------------------

 int _stdcall PrepareCodeHook(PVOID AddressToHook, DWORD HookCmdSize, PBYTE ModOldCode)
  {
   DWORD      OCBytes;
   HDE_STRUCT HdeInfo;

   OCBytes    = 0;
   ContAddr   = NULL;
   JumpAddr   = NULL;
   TargAddr   = AddressToHook;
   BytesTaked = 0;
  
   do
    {
     hde_disasm(&((BYTE*)AddressToHook)[BytesTaked],&HdeInfo);
     FastMoveMemory(&ModOldCode[OCBytes], &((BYTE*)AddressToHook)[BytesTaked], HdeInfo.Len);	  // Save Previous Code  
     // Call Rel32
     if((HdeInfo.Opcode == 0xE8))
      {
       StackMod = true; //For any dangerous stack modification
       ((PDWORD)&ModOldCode[OCBytes+1])[0] = AddrToRelAddr(&ModOldCode[OCBytes], 5, RelAddrToAddr(&((BYTE*)AddressToHook)[BytesTaked], HdeInfo.Len, (HdeInfo.Rel8+HdeInfo.Rel16+HdeInfo.Rel32)));
      }
     // Conditional jumps 
     if((HdeInfo.Opcode == 0x0F)&&((HdeInfo.Opcode2 >= 0x80)&&(HdeInfo.Opcode2 <= 0x8F))){((PDWORD)&ModOldCode[OCBytes+2])[0] = AddrToRelAddr(&ModOldCode[OCBytes], 6, &((BYTE*)AddressToHook)[BytesTaked+HdeInfo.Len+HdeInfo.Rel32]);}
     if((HdeInfo.Opcode >= 0x70)&&(HdeInfo.Opcode <= 0x7F))
      {
       ModOldCode[OCBytes]   = 0x0F;   // Prefix
       ModOldCode[OCBytes+1] = (HdeInfo.Opcode+0x10);
       ((PDWORD)&ModOldCode[OCBytes+2])[0] = AddrToRelAddr(&ModOldCode[OCBytes], 6, RelAddrToAddr(&((BYTE*)AddressToHook)[BytesTaked], HdeInfo.Len, (HdeInfo.Rel8+HdeInfo.Rel16+HdeInfo.Rel32)));
       OCBytes += 4;  // For long jump size
      }
     /*if(HdeInfo.Relpresent)
      {
       // WARNING: Possible wrong hook - jump followed
       AddressToHook = RelAddrToAddr(&((BYTE*)AddressToHook)[BytesTaked], HdeInfo.Len, (HdeInfo.Rel8+HdeInfo.Rel16+HdeInfo.Rel32));
	   BytesTaked = 0;
      }
       else*/ BytesTaked += HdeInfo.Len;
       OCBytes += HdeInfo.Len;
    }
     while(BytesTaked < HookCmdSize);

   JumpAddr = AddressToHook;
   ContAddr = &((BYTE*)AddressToHook)[BytesTaked];
   FastMoveMemory(&OriginalCode, AddressToHook, BytesTaked);	      // Save Previous Code
   return OCBytes;
  }
 //---------------------------------------
};
//===========================================================================


//===========================================================================
#endif
