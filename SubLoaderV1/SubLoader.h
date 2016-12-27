// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CRKLIB_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CRKLIB_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//====================================================================================

//====================================================================================
#pragma once
//#define _WIN32
//#undef  _NO_COM
#define WIN32_LEAN_AND_MEAN		 // Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT  0x0500     // For CoInitializeEx support

// Windows Header Files:
#include <windows.h>
#include "d3d9.h"
#include "ddraw.h"
#include "CommonFuncs.h"
#include "SubList.h"
//====================================================================================
#define SUBSEXT L".subs"
#define CFGFILE "SubsPlugin.ini"
#define LOGFILE "SubsPlugin.log"
#define CFGSECNAME "Parameters"

#define LOGMSG(msg,...) LogProc(__FUNCTION__,msg,__VA_ARGS__)
//====================================================================================
#define CALL_VDESTR(Class) __asm { __asm lea  ECX, Class  \
                                   __asm mov  EAX, [ECX]  \
                                   __asm call [EAX]       }
     
//====================================================================================
#define APIWRAPPER(LibPathName,NameAPI) extern "C" _declspec(dllexport) _declspec(naked) void __cdecl NameAPI(void) \
{ \
 static void* Address; \
 if(!Address)Address = GetProcAddress(LoadLibrary(LibPathName),#NameAPI); \
 __asm mov EAX, [Address] \
 __asm jmp EAX \
}
//====================================================================================
#define ADDROFFSET(addr,offset) ((addr)+(offset))

#ifndef _REV_DW
#define _REV_DW(Value)    (((DWORD)(Value) << 24)|((DWORD)(Value) >> 24)|(((DWORD)(Value) << 8)&0x00FF0000)|(((DWORD)(Value) >> 8)&0x0000FF00))
#endif
//---------------------------------------------------------------------------
// Compiler always generates Stack Frame Pointers for member functions - used 'pop EBP' to restore it
// How to force the compiler do not make stack frame pointer ?
// How to force the compiler do not push ECX ?
// For __thiscall
#ifndef JUMPVFTMEMBER
#define JUMPVFTMEMBER(Index) __asm pop  ECX           \
                             __asm pop  EBP           \
                             __asm mov  EAX, [ECX]    \
                             __asm add  EAX, Index*4  \
                             __asm jmp  [EAX] 
//---------------------------------------------------------------------------
#endif
#ifndef JUMPEXTMEMBER                                            // for 'thiscall'
#define JUMPEXTMEMBER(Base,Offset) __asm pop  ECX                \
                                   __asm pop  EBP                \
                                   __asm mov  EAX, Base          \
                                   __asm lea  EAX, [EAX+Offset]  \
                                   __asm jmp  EAX 
//---------------------------------------------------------------------------
#endif
#ifndef JUMPEXTMEMBERD                                           // for 'COM'
#define JUMPEXTMEMBERD(Address) __asm pop  EBP                \
                                __asm mov  EAX, Address       \
                                __asm jmp  EAX 
//---------------------------------------------------------------------------
#endif


CODEHOOK HookDirectDrawCreate;
CODEHOOK HookDirect3DCreate9;
CODEHOOK HookCloseHandle;
CODEHOOK HookCreateFileW;
CODEHOOK HookReadFile;

//====================================================================================
typedef struct{
  int x, y;
  UINT w, h;
} SDL_Rect;

struct STxtSurf
{
 //SDL_Surface* surf;
 DWORD* Pixels;
 SDL_Rect dstr; 
 UINT Width;
 UINT Height;
 IDirect3DSurface9* pSurfD3D;
};
//====================================================================================

HANDLE WINAPI ProcCreateFileW(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
BOOL   WINAPI ProcReadFile(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped);
BOOL   WINAPI ProcCloseHandle(HANDLE hObject);

IDirect3D9*  WINAPI ProcDirect3DCreate9(UINT SDKVersion);
//====================================================================================
HRESULT WINAPI ProcDirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter);

//====================================================================================


void   _cdecl LogProc(LPSTR ProcName, LPSTR Message, ...);
void _stdcall LoadConfiguration(void);
void _stdcall ReleaseTestSurface(STxtSurf* surf);
bool _stdcall InitHooks_D3D(LPSTR ModName);
bool _stdcall InitHooks_DDRAW(LPSTR ModName);
//=============================== SDL PROCS ==========================================
class HookIDirect3D9: public IDirect3D9
{
public:
 static PVOID PtrCreateDevice;

 __declspec(noinline) HRESULT  __stdcall OrigCreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface){JUMPEXTMEMBERD(PtrCreateDevice)}  

 HRESULT WINAPI HookCreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface); // 16
};
PVOID HookIDirect3D9::PtrCreateDevice = NULL;
//----------------------------------------------

class HookIDirect3DDevice9: public IDirect3DDevice9
{
public:
 static PVOID PtrEndScene;
 static PVOID PtrCreateTexture;

 __declspec(noinline) HRESULT  __stdcall OrigEndScene(void){JUMPEXTMEMBERD(PtrEndScene)}   
 __declspec(noinline) HRESULT  __stdcall OrigCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle){JUMPEXTMEMBERD(PtrCreateTexture)}  

 HRESULT WINAPI HookCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle); // 23
 HRESULT WINAPI HookEndScene(void);       // 42
};
PVOID HookIDirect3DDevice9::PtrEndScene      = NULL;
PVOID HookIDirect3DDevice9::PtrCreateTexture = NULL;
//----------------------------------------------

/*class HookIDirect3DTexture9: public IDirect3DTexture9
{
public:
 static PVOID PtrLockRect;
 static PVOID PtrUnlockRect;

 __declspec(noinline) HRESULT  __stdcall OrigLockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags){JUMPEXTMEMBERD(PtrLockRect)}  
 __declspec(noinline) HRESULT  __stdcall OrigUnlockRect(UINT Level){JUMPEXTMEMBERD(PtrUnlockRect)}  

 HRESULT WINAPI HookLockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags);     // 19
 HRESULT WINAPI HookUnlockRect(UINT Level);                                                                // 20
};
PVOID HookIDirect3DTexture9::PtrLockRect   = NULL;
PVOID HookIDirect3DTexture9::PtrUnlockRect = NULL; */
//====================================================================================
class HookIDirectDraw: public IDirectDraw
{
public:
 static PVOID PtrCreateSurface;
                                    
 __declspec(noinline) HRESULT  __stdcall OrigCreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE FAR *lplpDDSurface,IUnknown FAR *pUnkOuter){JUMPEXTMEMBERD(PtrCreateSurface)}   

 HRESULT  __stdcall HookCreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE FAR *lplpDDSurface,IUnknown FAR *pUnkOuter);      // 6
};
PVOID HookIDirectDraw::PtrCreateSurface = NULL;
//----------------------------------------------
class HookIDirectDrawSurface: public IDirectDrawSurface
{
public:
static PVOID PtrBlt;

 __declspec(noinline) HRESULT  __stdcall OrigBlt(LPRECT,LPDIRECTDRAWSURFACE, LPRECT,DWORD, LPDDBLTFX){JUMPEXTMEMBERD(PtrBlt)}   

 HRESULT  __stdcall HookBlt(LPRECT rcDst, LPDIRECTDRAWSURFACE pSrcSurf, LPRECT rcSrc, DWORD Flags, LPDDBLTFX tfx);    // 5
};
PVOID HookIDirectDrawSurface::PtrBlt = NULL;
//====================================================================================
