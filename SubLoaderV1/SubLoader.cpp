
#include "SubLoader.h"
//====================================================================================
//					 
// This unit contains functions, related on global data or other functions addresses
//
//
#pragma comment(linker,"/ENTRY:DLLMain")
#pragma comment(linker,"/NODEFAULTLIB")

#define CALLOFFSIZE        5
#define CALLOFFCODE        0xE8
#define ISKEYDOWN(karray, key) (karray[key] & 0x80) 

// ---------- SETTINGS ------------------
bool DoWavRecord = false;
bool WriteLog    = true;
//---------------------------------------

IDirectDraw*        pDDraw  = NULL;
IDirectDrawSurface* pDDSurf = NULL;
IDirect3DSurface9*  MmSuD3D = NULL;
IDirect3DDevice9*   pDevD3D = NULL;
IDirect3D9*  pDir3D9  = NULL;
SSubEntry*   LastLine = NULL;
STxtSurf*    pCurText = NULL;
HANDLE    hCurWavFile = INVALID_HANDLE_VALUE;
HANDLE    hCurVidFile = INVALID_HANDLE_VALUE;
HMODULE   hMainModule = NULL;
LPSTR     LibPathName = NULL;
HWND      AppWindow   = NULL;
bool      WavRecord   = false;
bool      TxtRender   = false;
UINT      SWidth      = 0;
UINT      SHeight     = 0;

CSubList Subs;
D3DSURFACE_DESC RndrDesc;
BYTE  SysDirPath[512];
BYTE  StartUpDir[MAX_PATH];
BYTE  CfgFilePath[MAX_PATH];
BYTE  LogFilePath[MAX_PATH];
WCHAR VidFileName[MAX_PATH];

//===========================================================================
//  Only one thread will execute within DLLMAIN
BOOL APIENTRY DLLMain(HMODULE hModule, DWORD ReasonCall, LPVOID lpReserved) 
{	
 switch (ReasonCall)	    
  {
   case DLL_PROCESS_ATTACH:
    {
	 hMainModule  = GetModuleHandle(NULL);
     LibPathName  = (LPSTR)&SysDirPath;
     GetModuleFileName(hMainModule,(LPSTR)&StartUpDir,sizeof(StartUpDir));        
     GetSystemDirectory(LibPathName,sizeof(SysDirPath));
     lstrcat(LibPathName,"\\hid.dll");
     lstrcpy((LPSTR)&CfgFilePath,(LPSTR)&StartUpDir);
     lstrcpy((LPSTR)&LogFilePath,(LPSTR)&StartUpDir);
     TrimFilePath((LPSTR)&CfgFilePath);
     TrimFilePath((LPSTR)&LogFilePath);
     lstrcat((LPSTR)&CfgFilePath,CFGFILE);
     lstrcat((LPSTR)&LogFilePath,LOGFILE);
     LoadConfiguration();
     Subs.Initialize();
                        
     InitHooks_D3D("d3d9.dll");
     InitHooks_DDRAW("ddraw.dll");   
                             
     HookCloseHandle.SetHook(&ProcCloseHandle, "Kernel32.dll", "CloseHandle", TRUE);
     HookCreateFileW.SetHook(&ProcCreateFileW, "Kernel32.dll", "CreateFileW", TRUE);
     HookReadFile.SetHook(&ProcReadFile, "Kernel32.dll", "ReadFile", TRUE);                                       
    }
     break;									

   case DLL_THREAD_ATTACH:
     break; // Ignore creation own thread if this may happen

   case DLL_THREAD_DETACH:
     break;
   case DLL_PROCESS_DETACH:    
     HookReadFile.RemoveHook();
     HookCreateFileW.RemoveHook();
     HookCloseHandle.RemoveHook();
     //if(pCurText)ReleaseTestSurface(pCurText);
     //Subs.Release();
	 break;
   default : return false;  // WRONG REASON CODE !!!!!!
  }
 return true;
}
//====================================================================================
void  _cdecl LogProc(LPSTR ProcName, LPSTR Message, ...)
{
 static HANDLE hLogFile = INVALID_HANDLE_VALUE;
 if(!Message || !*Message)return;

 va_list args;
 BYTE Buffer[1025+256];     //  > 4k creates __chkstk   // 1024 is MAX for wsprintf
 BYTE OutBuffer[1025+256];

 va_start(args,Message);
 Buffer[0] = 0;
 if(ProcName && Message && Message[0])
  {
   lstrcat((LPSTR)&Buffer,ProcName);
   lstrcat((LPSTR)&Buffer," -> ");
  }
 lstrcat((LPSTR)&Buffer,Message);
 int msglen  = wvsprintf((LPSTR)&OutBuffer,(LPSTR)&Buffer,args);
 if(hLogFile == INVALID_HANDLE_VALUE)hLogFile = CreateFile((LPSTR)&LogFilePath,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN,NULL);
 if((hLogFile != INVALID_HANDLE_VALUE) && (msglen > 0))
  {
   DWORD Result;
   WriteFile(hLogFile,&OutBuffer,msglen,&Result,NULL);
   WriteFile(hLogFile,"\r\n",2,&Result,NULL);
  }
 va_end(args);
}
//---------------------------------------------------------------------------
void _stdcall LoadConfiguration(void)
{ 
 WriteLog    = RefreshINIValueInt(CFGSECNAME,"WriteLog",    WriteLog,    (LPSTR)&CfgFilePath);
 DoWavRecord = RefreshINIValueInt(CFGSECNAME,"DoWavRecord", DoWavRecord, (LPSTR)&CfgFilePath);
}
//====================================================================================
bool _stdcall InitHooks_D3D(LPSTR ModName)
{
 HookDirect3DCreate9.SetHook(&ProcDirect3DCreate9, ModName, "Direct3DCreate9", TRUE);
 return HookDirect3DCreate9.IsHookValid();
}
//====================================================================================
bool _stdcall InitHooks_DDRAW(LPSTR ModName)
{
 HookDirectDrawCreate.SetHook(&ProcDirectDrawCreate, ModName, "DirectDrawCreate", TRUE);
 return HookDirectDrawCreate.IsHookValid();
}
//====================================================================================
HWND _stdcall FindWindowOfThread(DWORD ThID, LPSTR WndName, LPSTR ClassName)
{
 struct CBK
  {
   HWND  wnd;
   LPSTR WndName;
   LPSTR WndClass;

   static BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
    {
     CBK* This = (CBK*)lParam; 
     BYTE Buffer[64];  
     if((!This->WndClass || (GetClassName(hwnd,(LPSTR)&Buffer,sizeof(Buffer)) && (lstrcmpi((LPSTR)&Buffer,This->WndClass)==0))) && (!This->WndName || (GetClassName(hwnd,(LPSTR)&Buffer,sizeof(Buffer)) && (lstrcmpi((LPSTR)&Buffer,This->WndName)==0))) )
      {
       This->wnd = hwnd;
       return FALSE;
      }
     EnumChildWindows(hwnd,((CBK*)(0))->EnumThreadWndProc,(LPARAM)This);
     return TRUE;
    }
  }Cbk;
 Cbk.wnd      = NULL;
 Cbk.WndName  = WndName;
 Cbk.WndClass = ClassName;
 EnumThreadWindows(ThID,Cbk.EnumThreadWndProc,(LPARAM)&Cbk);   
 return Cbk.wnd;
}
//------------------------------------------------------------------------------------
HWND _stdcall FindAppWindowSDL(void)
{
 HWND wnd = NULL;
 THREADENTRY32 tent32;
 HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0); 
 tent32.dwSize = sizeof(THREADENTRY32);
 if(!Thread32First(hThreadSnap, &tent32))return NULL;
 do
  { 
   wnd = FindWindowOfThread(tent32.th32ThreadID, NULL, "SDL_app");
   if(wnd)break;
  }
   while(Thread32Next(hThreadSnap, &tent32));
 LOGMSG(": %08X",wnd);
 return wnd;
}
//===========================================================================
void _stdcall ToggleWavRecording(void)
{
 if(!AppWindow)return;
 PostMessage(AppWindow,WM_KEYDOWN,VK_LCONTROL ,NULL);
 PostMessage(AppWindow,WM_KEYDOWN,VK_F6  ,NULL);
 Sleep(500);
 PostMessage(AppWindow,WM_KEYUP,VK_F6,NULL);
 PostMessage(AppWindow,WM_KEYUP,VK_LCONTROL,NULL);
 LOGMSG("Window=%08x, CurState: %u",AppWindow,(UINT)WavRecord);
}
//===========================================================================
// Multiline must be supported
//
STxtSurf* _stdcall CreateTextSurfaceFromString(PWSTR str, HFONT Font, int MaxWidth, COLORREF TxtColor, COLORREF BgrColor, bool Transp, IDirect3DDevice9* d3ddev) 
{
 RECT trect;
 STxtSurf tsurf;         
 BITMAPINFO binf;
 HDC     hScrDC  = GetDC(NULL);
 HDC     hMemDC  = CreateCompatibleDC(hScrDC);
 int slen        = lstrlenW(str);
 trect.top       = trect.bottom = trect.left = 0;
 trect.right     = MaxWidth;  
 if(Font)SelectObject(hMemDC,Font);     // Set font BEFORE calculating the text rectangle
 int resA        = DrawTextW(hMemDC,str,slen,&trect,DT_CALCRECT|DT_NOCLIP|DT_WORDBREAK|DT_CENTER);
 int TxtWidth    = trect.right  - trect.left;
 int TxtHeight   = trect.bottom - trect.top;
 HBITMAP hMemBmp = CreateCompatibleBitmap(hScrDC,TxtWidth,TxtHeight);  
 SelectObject(hMemDC,hMemBmp);             
 
 SetBkMode(hMemDC,OPAQUE);
 SetTextColor(hMemDC,(COLORREF)TxtColor);
 SetBkColor(hMemDC,(COLORREF)BgrColor);    //0x00bbggrr 0x00F0CAA6 - SkyBlue
 int resB = DrawTextW(hMemDC,str,slen,&trect,DT_NOCLIP|DT_WORDBREAK|DT_CENTER);

 memset(&binf,0,sizeof(binf));
 binf.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
 binf.bmiHeader.biWidth       = TxtWidth;
 binf.bmiHeader.biHeight      = -TxtHeight;
 binf.bmiHeader.biPlanes      = 1;
 binf.bmiHeader.biBitCount    = 32;
 binf.bmiHeader.biCompression = BI_RGB;
 UINT PixBufSize = TxtWidth * TxtHeight * 4;
 tsurf.Pixels = (DWORD*)HeapAlloc(GetProcessHeap(),0,PixBufSize);  
 int resC = GetDIBits(hMemDC,hMemBmp,0,TxtHeight,tsurf.Pixels,&binf,DIB_RGB_COLORS);
 DeleteObject(hMemBmp);
 DeleteDC(hMemDC);
 ReleaseDC(NULL,hScrDC);

 if(d3ddev)
  {                         
   HRESULT res = d3ddev->CreateOffscreenPlainSurface(TxtWidth,TxtHeight,D3DFMT_X8R8G8B8,D3DPOOL_SYSTEMMEM,&tsurf.pSurfD3D,NULL);
   if((res == 0) && !Transp)  // Prepare text now, no need to merge it with a background later
    {
     D3DLOCKED_RECT srect;
     if(!tsurf.pSurfD3D->LockRect(&srect,NULL,D3DLOCK_NOSYSLOCK))
      {
       for(UINT pmax = (TxtWidth * TxtHeight), ctr=0;ctr < pmax;ctr++)((DWORD*)srect.pBits)[ctr] = tsurf.Pixels[ctr];
       tsurf.pSurfD3D->UnlockRect();
      }
    }
  }

 tsurf.Width  = TxtWidth;
 tsurf.Height = TxtHeight;
 STxtSurf* reslt = (STxtSurf*)HeapAlloc(GetProcessHeap(),0,sizeof(STxtSurf)); 
 memcpy(reslt,&tsurf,sizeof(tsurf));
 return reslt;
}
//------------------------------------------------------------------------------------
void _stdcall ReleaseTestSurface(STxtSurf* surf)
{               
 if(surf->pSurfD3D){surf->pSurfD3D->Release();surf->pSurfD3D=NULL;}             
 HeapFree(GetProcessHeap(),0,surf->Pixels);
 HeapFree(GetProcessHeap(),0,surf);
}
//------------------------------------------------------------------------------------
// OffsX: Positive - Offset from left; Negative - Offset from right
// OffsY: Positive - Offset from top;  Negative - Offset from bottom
//
bool _stdcall DrawTextSurface_D3D(IDirect3DSurface9* tgt, D3DSURFACE_DESC* sdescD, STxtSurf* txt, int OffsX, int OffsY, bool Transp)
{                          
 if(!pDevD3D || !tgt || !txt || !LastLine)return false;   //  || tgt->GetDesc(&sdescD)
 txt->dstr.x  = txt->dstr.y = 0;            
 if(OffsX < 0)txt->dstr.x = sdescD->Width  - txt->Width; 
 if(OffsY < 0)txt->dstr.y = sdescD->Height - txt->Height;
 txt->dstr.x += OffsX;
 txt->dstr.y += OffsY;             
 txt->dstr.w  = txt->Width;
 txt->dstr.h  = txt->Height;            

 HRESULT rlt;
 if(Transp)
  {
   D3DLOCKED_RECT  drect;
   D3DLOCKED_RECT  srect;
   D3DSURFACE_DESC sdescM;
   sdescM.Width = sdescM.Height = 0; 
   if(MmSuD3D)MmSuD3D->GetDesc(&sdescM);                    
   if((sdescM.Width != sdescD->Width)||(sdescM.Height != sdescD->Height))
    {
     if(MmSuD3D){MmSuD3D->Release();MmSuD3D = NULL;}
     rlt = pDevD3D->CreateOffscreenPlainSurface(sdescD->Width,sdescD->Height,sdescD->Format,D3DPOOL_SYSTEMMEM,&MmSuD3D,NULL);
    }
   if(MmSuD3D)     
    {              
     if(!pDevD3D->GetRenderTargetData(tgt, MmSuD3D) && !txt->pSurfD3D->LockRect(&srect,NULL,D3DLOCK_NOSYSLOCK) && !MmSuD3D->LockRect(&drect,NULL,D3DLOCK_NOSYSLOCK))
      {
       for(int trow=0;trow < txt->Height;trow++)
        {
         int     Trowidx = trow * txt->Width;
         PDWORD  TxtRow  = &txt->Pixels[Trowidx];
         PDWORD  DstRow  = &((DWORD*)srect.pBits)[Trowidx];
         PDWORD  BgrRow  = &((DWORD*)drect.pBits)[((trow + txt->dstr.y) * sdescD->Width) + txt->dstr.x];
         for(int tcol=0;tcol < txt->Width;tcol++)
          {
           if(TxtRow[tcol] != LastLine->BkColor)DstRow[tcol] = TxtRow[tcol];
             else DstRow[tcol] = BgrRow[tcol];
          }
        }
      }
     txt->pSurfD3D->UnlockRect();
     MmSuD3D->UnlockRect();
    }
  }

 POINT dstpt = {txt->dstr.x,txt->dstr.y};
 rlt = pDevD3D->UpdateSurface(pCurText->pSurfD3D,NULL,tgt,&dstpt);   // The text is already prepared    
 return (rlt == 0);
}
//====================================================================================
//
// All this code are supposedly to be moved into main executable 
//
//------------------------------------------------------------------------------------
HANDLE WINAPI ProcCreateFileW(LPCWSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile)
{
 WCHAR FPath[MAX_PATH];
 bool CreateMode = ((CREATE_ALWAYS == dwCreationDisposition)||(CREATE_NEW == dwCreationDisposition));
 if(CreateMode && DoWavRecord)
  {
   int nlen = lstrlenW(lpFileName);
   if((lstrcmpiW(&lpFileName[nlen-4],L".wav")==0)&&(lpFileName[nlen-8] =='_'))
    {
     lstrcpyW((PWSTR)&FPath,lpFileName);
     FPath[nlen-7] = 0;
     lstrcatW((PWSTR)&FPath,(PWSTR)&VidFileName);
     lstrcatW((PWSTR)&FPath,L".wav");
     lpFileName = (LPCWSTR)&FPath;
     LOGMSG("Recording: %ls",(PWSTR)&FPath);
    }
  }

 HANDLE hRes = CreateFileW(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
 if((hRes != INVALID_HANDLE_VALUE) && !CreateMode)
  {                         
   lstrcpyW((PWSTR)&FPath,lpFileName);
   lstrcatW((PWSTR)&FPath,SUBSEXT);
   if(Subs.LoadSubsFile((PWSTR)&FPath, GetFileSize(hRes,NULL)))
    {
     hCurVidFile = hRes;
     lstrcpyW((PWSTR)&VidFileName,GetFileName((PWSTR)lpFileName));
     if(DoWavRecord)
      {
       if(WavRecord)ToggleWavRecording();        // Close recording of a prev file
       ToggleWavRecording(); 
      } 
     LOGMSG("Subtitles: %ls",(PWSTR)&FPath);
    }
     else VidFileName[0] = 0;
   LastLine = NULL;
  }
 return hRes;
}
//------------------------------------------------------------------------------------
BOOL WINAPI ProcReadFile(HANDLE hFile,LPVOID lpBuffer,DWORD nNumberOfBytesToRead,LPDWORD lpNumberOfBytesRead,LPOVERLAPPED lpOverlapped)
{
 DWORD Position;
 if(hFile == hCurVidFile)Position = SetFilePointer(hFile,0,NULL,FILE_CURRENT);
 BOOL Result = ReadFile(hFile,lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead,lpOverlapped);
 if(Result && (hFile == hCurVidFile))
  {    
   SSubEntry* Ent  = Subs.FindSubtitle(Position);           
   if(Ent && (Ent != LastLine))
    {         
     LastLine = Ent;  
     LOGMSG("Text line for %u: \r\n----\r\n%ls\r\n----",Position,Ent->Text);    
     TxtRender = false;                                
     if(pCurText)ReleaseTestSurface(pCurText);                            
     if(pDevD3D)pCurText = CreateTextSurfaceFromString(Ent->Text,Ent->Font,RndrDesc.Width,Ent->TxtColor,Ent->BkColor,Subs.Transpar,pDevD3D);
    }
   TxtRender = (Ent && pCurText); 
  } 
 return Result;
}
//------------------------------------------------------------------------------------
BOOL WINAPI ProcCloseHandle(HANDLE hObject)
{
 bool res = CloseHandle(hObject);
 if(hObject == hCurWavFile){WavRecord = false; hCurWavFile = INVALID_HANDLE_VALUE; LOGMSG("The Recording file is closed",0);}           // The recording file has been closed
 if(hObject == hCurVidFile){TxtRender = false; if(DoWavRecord && WavRecord)ToggleWavRecording(); hCurVidFile = INVALID_HANDLE_VALUE; LOGMSG("The Subtitled file is closed",0);}   // Stop recording when the video file is closed
 return res;
}
//====================================================================================
//
//====================================================================================
IDirect3D9* WINAPI ProcDirect3DCreate9(UINT SDKVersion)
{                                                    
 pDir3D9 = ((IDirect3D9* (WINAPI*)(UINT))HookDirect3DCreate9.ProcAddr)(SDKVersion);    
 if(pDir3D9)
  {
   HookEntryVFT(16, pDir3D9, &HookIDirect3D9::PtrCreateDevice, &HookIDirect3D9::HookCreateDevice);
   LOGMSG("Hooked 'IDirect3D9::CreateDevice'",0);
  }
 return pDir3D9;
}
//------------------------------------------------------------------------------------
HRESULT WINAPI HookIDirect3D9::HookCreateDevice(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS *pPresentationParameters,IDirect3DDevice9 **ppReturnedDeviceInterface)
{
 HRESULT res = this->OrigCreateDevice(Adapter,DeviceType,hFocusWindow,BehaviorFlags,pPresentationParameters,ppReturnedDeviceInterface);
 if(res == 0)
  {
   pDevD3D         = *ppReturnedDeviceInterface;
   AppWindow       = pPresentationParameters->hDeviceWindow;
   RndrDesc.Width  = pPresentationParameters->BackBufferWidth;
   RndrDesc.Height = pPresentationParameters->BackBufferHeight;
   if(!AppWindow)AppWindow = FindAppWindowSDL();
   if(DoWavRecord)DoWavRecord = (bool)AppWindow;
  
   //HookEntryVFT(23, *ppReturnedDeviceInterface, &HookIDirect3DDevice9::PtrCreateTexture, &HookIDirect3DDevice9::HookCreateTexture);
   HookEntryVFT(42, *ppReturnedDeviceInterface, &HookIDirect3DDevice9::PtrEndScene, &HookIDirect3DDevice9::HookEndScene);
   LOGMSG("Hooked 'IDirect3DDevice9::EndScene'",0);
  }
 return res;
}
//------------------------------------------------------------------------------------
HRESULT WINAPI HookIDirect3DDevice9::HookEndScene(void)
{
 if(pCurText && TxtRender && pCurText->pSurfD3D && (hCurVidFile != INVALID_HANDLE_VALUE))
  {
   IDirect3DSurface9* ppRenderTarget = NULL;
   if(!this->GetRenderTarget(0,&ppRenderTarget))
    {                                                  
     ppRenderTarget->GetDesc(&RndrDesc);     // Update info about the render target
     DrawTextSurface_D3D(ppRenderTarget, &RndrDesc, pCurText, (RndrDesc.Width - pCurText->Width) / 2, Subs.VOffset, Subs.Transpar); 
     ppRenderTarget->Release();
    }  
  }
 HRESULT res = this->OrigEndScene();
 return  res;
} 
//------------------------------------------------------------------------------------
/*HRESULT WINAPI HookIDirect3DDevice9::HookCreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9 **ppTexture, HANDLE *pSharedHandle)
{
 HRESULT res = this->OrigCreateTexture(Width,Height,Levels,Usage,Format,Pool,ppTexture,pSharedHandle);     
 if(res == 0)
  {
   HookEntryVFT(19, *ppTexture, &HookIDirect3DTexture9::PtrLockRect, &HookIDirect3DTexture9::HookLockRect);
   HookEntryVFT(20, *ppTexture, &HookIDirect3DTexture9::PtrUnlockRect, &HookIDirect3DTexture9::HookUnlockRect);
  }
 return res;
}
//------------------------------------------------------------------------------------
HRESULT WINAPI HookIDirect3DTexture9::HookLockRect(UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags)
{                
 HRESULT res = this->OrigLockRect(Level,pLockedRect,pRect,Flags); 
 return res;  
}
//------------------------------------------------------------------------------------
HRESULT WINAPI HookIDirect3DTexture9::HookUnlockRect(UINT Level)
{
 HRESULT res = this->OrigUnlockRect(Level); 
 return res;
}*/
//====================================================================================
//
//
//
//====================================================================================
HRESULT WINAPI ProcDirectDrawCreate(GUID FAR *lpGUID, LPDIRECTDRAW FAR *lplpDD, IUnknown FAR *pUnkOuter)
{
 HRESULT res = ((HRESULT (WINAPI*)(GUID FAR*, LPDIRECTDRAW FAR*, IUnknown FAR*))HookDirectDrawCreate.ProcAddr)(lpGUID,lplpDD,pUnkOuter); 
 if(!res && lplpDD && *lplpDD)
  {
   pDDraw = *lplpDD;
   HookEntryVFT(6, pDDraw, &HookIDirectDraw::PtrCreateSurface, &HookIDirectDraw::HookCreateSurface);
   LOGMSG("Hooked 'IDirectDraw::CreateSurface'",0);
  }
 return res;
}
//------------------------------------------------------------------------------------
HRESULT  __stdcall HookIDirectDraw::HookCreateSurface(LPDDSURFACEDESC lpDDSurfaceDesc,LPDIRECTDRAWSURFACE FAR *lplpDDSurface,IUnknown FAR *pUnkOuter)
{
 HRESULT res = this->OrigCreateSurface(lpDDSurfaceDesc,lplpDDSurface,pUnkOuter);
 if(!res && lplpDDSurface && *lplpDDSurface)
  {
   pDDSurf   = *lplpDDSurface;
   AppWindow = FindAppWindowSDL();  // DosBox only
   HookEntryVFT(5, pDDSurf, &HookIDirectDrawSurface::PtrBlt, &HookIDirectDrawSurface::HookBlt);
      // pDDSurf->Blt(0,0,0,0,0);
   LOGMSG("Hooked 'IDirectDrawSurface::Blt'",0);
  }
 return res;
}
//------------------------------------------------------------------------------------
HRESULT  __stdcall HookIDirectDrawSurface::HookBlt(LPRECT rcDst, LPDIRECTDRAWSURFACE pSrcSurf, LPRECT rcSrc, DWORD Flags, LPDDBLTFX tfx)
{



 return this->OrigBlt(rcDst, pSrcSurf, rcSrc, Flags, tfx);
}
//------------------------------------------------------------------------------------



#pragma code_seg()

//====================================================================================
//								 WRAPPER FUNCTIONS
//------------------------------------------------------------------------------------
APIWRAPPER(LibPathName,HidD_FlushQueue)
APIWRAPPER(LibPathName,HidD_FreePreparsedData)
APIWRAPPER(LibPathName,HidD_GetAttributes)
APIWRAPPER(LibPathName,HidD_GetConfiguration)
APIWRAPPER(LibPathName,HidD_GetFeature)
APIWRAPPER(LibPathName,HidD_GetHidGuid)
APIWRAPPER(LibPathName,HidD_GetIndexedString)
APIWRAPPER(LibPathName,HidD_GetInputReport)
APIWRAPPER(LibPathName,HidD_GetManufacturerString)
APIWRAPPER(LibPathName,HidD_GetMsGenreDescriptor)
APIWRAPPER(LibPathName,HidD_GetNumInputBuffers)
APIWRAPPER(LibPathName,HidD_GetPhysicalDescriptor)
APIWRAPPER(LibPathName,HidD_GetPreparsedData)
APIWRAPPER(LibPathName,HidD_GetProductString)
APIWRAPPER(LibPathName,HidD_GetSerialNumberString)
APIWRAPPER(LibPathName,HidD_Hello)
APIWRAPPER(LibPathName,HidD_SetConfiguration)
APIWRAPPER(LibPathName,HidD_SetFeature)
APIWRAPPER(LibPathName,HidD_SetNumInputBuffers)
APIWRAPPER(LibPathName,HidD_SetOutputReport)
APIWRAPPER(LibPathName,HidP_GetButtonCaps)
APIWRAPPER(LibPathName,HidP_GetCaps)
APIWRAPPER(LibPathName,HidP_GetData)
APIWRAPPER(LibPathName,HidP_GetExtendedAttributes)
APIWRAPPER(LibPathName,HidP_GetLinkCollectionNodes)
APIWRAPPER(LibPathName,HidP_GetScaledUsageValue)
APIWRAPPER(LibPathName,HidP_GetSpecificButtonCaps)
APIWRAPPER(LibPathName,HidP_GetSpecificValueCaps)
APIWRAPPER(LibPathName,HidP_GetUsageValue)
APIWRAPPER(LibPathName,HidP_GetUsageValueArray)
APIWRAPPER(LibPathName,HidP_GetUsages)
APIWRAPPER(LibPathName,HidP_GetUsagesEx)
APIWRAPPER(LibPathName,HidP_GetValueCaps)
APIWRAPPER(LibPathName,HidP_InitializeReportForID)
APIWRAPPER(LibPathName,HidP_MaxDataListLength)
APIWRAPPER(LibPathName,HidP_MaxUsageListLength)
APIWRAPPER(LibPathName,HidP_SetData)
APIWRAPPER(LibPathName,HidP_SetScaledUsageValue)
APIWRAPPER(LibPathName,HidP_SetUsageValue)
APIWRAPPER(LibPathName,HidP_SetUsageValueArray)
APIWRAPPER(LibPathName,HidP_SetUsages)
APIWRAPPER(LibPathName,HidP_TranslateUsagesToI8042ScanCodes)
APIWRAPPER(LibPathName,HidP_UnsetUsages)
APIWRAPPER(LibPathName,HidP_UsageListDifference)
//====================================================================================


