

#pragma once
//#define _WIN32
//#undef  _NO_COM
#define WIN32_LEAN_AND_MEAN		 // Exclude rarely-used stuff from Windows headers
#define _WIN32_WINNT  0x0500     // For CoInitializeEx support

// Windows Header Files:
#include <windows.h>
#include "CommonFuncs.h"
//====================================================================================
/* Subtitle format:
 "Curier New",S:16,C:00FFFFFF,B:00000000,B,I    // "Curier New",SIZE,BOLD,ITALIC

 [MinOffset >>> MaxOffset]  // (BytPos >>> BytePOs) or (PosPerc.x >>> PosPerc.x)
 Some text
  Sometext 2
   Some text 3
 -- Empty Line


  Entry detected by 0D0A0D0A'[' and checked for '>>>' and ']'0D0A

*/
//====================================================================================
struct SSubEntry
{
 COLORREF TxtColor;
 COLORREF BkColor;
 HFONT    Font;
 PWSTR    Text;       // Points to string in a loaded file
 float PercBegin;     // union {DWORD ByteBegin; float PercBegin;};
 float PercEnd;       // union {DWORD ByteEnd;   float PercEnd;};
};

class CSubList
{
 SSubEntry* Lines;
 DWORD LineCount;
 DWORD VFileLen;
 DWORD FileSize;
 PWSTR FileData;
 HFONT TxtFont;


 typedef bool (_fastcall *COMPARATOR)(WCHAR ChrA, WCHAR ChrB);
 static bool _fastcall ComparatorE (WCHAR ChrA, WCHAR ChrB){return (ChrA == ChrB);}
 static bool _fastcall ComparatorL (WCHAR ChrA, WCHAR ChrB){return (ChrA  < ChrB);}
 static bool _fastcall ComparatorG (WCHAR ChrA, WCHAR ChrB){return (ChrA  > ChrB);}
 static bool _fastcall ComparatorEL(WCHAR ChrA, WCHAR ChrB){return (ChrA <= ChrB);}
 static bool _fastcall ComparatorEG(WCHAR ChrA, WCHAR ChrB){return (ChrA >= ChrB);}
 static bool _fastcall ComparatorNE(WCHAR ChrA, WCHAR ChrB){return (ChrA != ChrB);}

//---------------------
int Pos(WCHAR chr, PWSTR str, UINT slen, COMPARATOR cmpr=ComparatorE, int from=0)
{
 if(from < 0)   // Search backwards
  {
   from = -from;
   if(from >= slen)return -1;   // Out of the string
   for(int idx=from;idx >= 0;idx--){if(cmpr(str[idx],chr))return idx;}
  }
   else for(int end=slen,idx=from;idx < end;idx++){if(cmpr(str[idx],chr))return idx;}
 return -1;
}
//---------------------
bool MakeFont(PWSTR FontStr, int TotalChars, COLORREF* TxtColor, COLORREF* BkColor)
{
 bool  FntBold = false;
 bool  FntItal = false;
 int   FntWi   = 0;
 int   FntHi   = 20;
 int   Offset  = 0;
 int   nbeg    = Pos('"',  this->FileData, TotalChars, ComparatorE,  Offset);
 int   nend    = Pos('"',  this->FileData, TotalChars, ComparatorE,  nbeg+1);
 PWSTR Name    = &this->FileData[nbeg+1];
 if((nbeg < 0)||(nend < 0))return false;
 this->FileData[nend] = 0;
 Offset = nend + 1; 
 for(;;)
  {
   int pdel = Pos(':',  this->FileData, TotalChars, ComparatorE,  Offset);
   if(pdel <= 0)break;
   int pend = Pos(',',  this->FileData, TotalChars, ComparatorE,  pdel+1);
   if(pend < 0)pend = TotalChars;
   Offset = pend + 1;
   pend   = Pos(0x20, this->FileData, TotalChars, ComparatorG, -(pend-1));
   this->FileData[pend+1] = 0;
   int pbeg = Pos(0x20, this->FileData, TotalChars, ComparatorG,  pdel+1);
   if(pbeg <= 0)break;
   PWSTR Value = &this->FileData[pbeg]; 
   switch(this->FileData[pdel-1])
    {
     case 'O':
      {
       this->VOffset = tiny_strtol<wchar_t>(Value, NULL, 10);
      }
       break;
     case 'W':
      {
       FntWi     = tiny_strtol<wchar_t>(Value, NULL, 10);
      }
       break;
     case 'H':
      {
       FntHi     = tiny_strtol<wchar_t>(Value, NULL, 10);
      }
       break;
     case 'C':
      {
       *TxtColor = tiny_strtol<wchar_t>(Value, NULL, 16);
      }
       break;
     case 'B':
      {
       *BkColor  = tiny_strtol<wchar_t>(Value, NULL, 16);
      }
       break;
     case 'F':
      {
       for(;*Value;Value++)
        {
         FntBold = (!FntBold)?(*Value == 'B'):(FntBold);
         FntItal = (!FntItal)?(*Value == 'I'):(FntItal);
         this->Transpar = (!this->Transpar)?(*Value == 'T'):(this->Transpar);
        }
      }
       break;
    }
  }

 this->TxtFont = CreateFontW(
					  FntHi,
					  FntWi,
                          0,
                          0,
  ((FntBold)?(FW_BOLD):(0)),
                    FntItal,
                      false,
				      false,
            RUSSIAN_CHARSET,      // DEFAULT_CHARSET,
         OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
              PROOF_QUALITY,
              DEFAULT_PITCH,      // FIXED_PITCH | FF_MODERN,
                      Name);      // "Times New Roman"
                      
 return (bool)this->TxtFont;
}
//---------------------

public:
int  VOffset;
bool Transpar;


void Initialize(void)
{
 this->Lines     = NULL;
 this->TxtFont   = NULL;
 this->FileData  = NULL;
 this->FileSize  = 0;
 this->LineCount = 0;
 this->VOffset   = -40;
 this->Transpar  = false;
}
//------------------------------------------
void Release(void)
{
 if(this->TxtFont)DeleteObject(this->TxtFont);
 if(this->FileData)HeapFree(GetProcessHeap(),0, this->FileData);
 if(this->Lines)HeapFree(GetProcessHeap(),0, this->FileData);
 this->Initialize();
}
//------------------------------------------
bool LoadSubsFile(PWSTR FilePath, DWORD TgtFileSize)
{
 DWORD  Result;
 HANDLE hFile = CreateFileW(FilePath,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
 if(hFile == INVALID_HANDLE_VALUE)return false;
 this->Release();
 this->VFileLen = TgtFileSize;
 this->FileSize = GetFileSize(hFile,NULL);
 this->FileData = (PWSTR)HeapAlloc(GetProcessHeap(),0,this->FileSize+16); 
 ReadFile(hFile,this->FileData,this->FileSize,&Result,NULL);   
 CloseHandle(hFile);

 COLORREF BkColor  = 0;
 COLORREF TxtColor = 0x00FFFFFF;

 int   TotalChars = this->FileSize / sizeof(WCHAR);
 int   Offset     = 0;
 int   cpos       = Pos(0x20, this->FileData, TotalChars, ComparatorG, Offset);
 PWSTR FontStr    = &this->FileData[cpos];
 Offset = cpos;
 cpos   = Pos(0x20, this->FileData, TotalChars, ComparatorL, Offset);
 if(cpos >= 0)FontStr[cpos] = 0;
 this->FileData[TotalChars] = 0;
 if(!MakeFont(FontStr,cpos,&TxtColor,&BkColor)){}     // Do something?
 Offset = cpos;
 for(;Offset < TotalChars;)
  {
   int tbeg = Pos('[',  this->FileData, TotalChars, ComparatorE,  Offset);
   int tend = Pos(']',  this->FileData, TotalChars, ComparatorE,  tbeg);
   int ndel = Pos('>',  this->FileData, TotalChars, ComparatorE,  tbeg);
   int lend = Pos(0x20, this->FileData, TotalChars, ComparatorL,  tbeg);
   if(!this->FileData[lend] || (tbeg < 0)|| (tend < 0)|| (ndel < 0))break;  // No more lines
   Offset   = Pos(0x20, this->FileData, TotalChars, ComparatorEG, lend);
   if((tbeg < tend)&&(lend > tbeg)&&(lend > tend)&&(ndel > tbeg)&&(ndel < tend))   // New entry
    {
     int llst = Pos(0x20,  this->FileData, TotalChars, ComparatorEG, -(tbeg-1));
     if(llst >= 0)this->FileData[llst+1] = 0;
     int fbeg  = Pos(0x20,  this->FileData, TotalChars, ComparatorG, tbeg+1);
     int fend  = Pos(0x20,  this->FileData, TotalChars, ComparatorE, fbeg);
     if((fbeg <= 0)||(fend <= 0))continue;
     int nend  = Pos(0x20,  this->FileData, TotalChars, ComparatorG, -(tend-1));
     int nbeg  = Pos(0x20,  this->FileData, TotalChars, ComparatorE, -nend);
     if((nend <= 0)||(nend > tend))nend = tend;
     if((nbeg <= 0)||(nend <= 0))continue;
     PWSTR LNum = &this->FileData[fbeg];
     PWSTR RNum = &this->FileData[++nbeg];
     this->FileData[fend] = this->FileData[++nend] = 0;
     int ptl = Pos('.', LNum, fend-fbeg, ComparatorE, 0);
     int ptr = Pos('.', RNum, nend-nbeg, ComparatorE, 0);
     if(!((ptl < 0)&&(ptr < 0))&&!((ptl > 0)&&(ptr > 0)))continue;  // Must be of same format     
     this->LineCount++;
     if(!this->Lines)this->Lines = (SSubEntry*)HeapAlloc(GetProcessHeap(),0,(sizeof(SSubEntry)*this->LineCount)); 
       else this->Lines = (SSubEntry*)HeapReAlloc(GetProcessHeap(),0,this->Lines,(sizeof(SSubEntry)*this->LineCount)); 
     SSubEntry* Ent = &this->Lines[this->LineCount-1]; 
     Ent->Text      = &this->FileData[Offset];
     Ent->Font      = this->TxtFont;
     Ent->BkColor   = BkColor;
     Ent->TxtColor  = TxtColor;
     if(ptl > 0)
      {
       Ent->PercBegin = tiny_strtod<wchar_t>(LNum,NULL);
       Ent->PercEnd   = tiny_strtod<wchar_t>(RNum,NULL);
      }
       else
        {         
         Ent->PercBegin = NumToPerc((double)tiny_strtol<wchar_t>(LNum, NULL, 10), (double)this->VFileLen);
         Ent->PercEnd   = NumToPerc((double)tiny_strtol<wchar_t>(RNum, NULL, 10), (double)this->VFileLen);
        }
      // NOTE: Test if  (Ent->PercEnd < Ent->PercBegin) ?
    }
  }  
 return true;
} 
//------------------------------------------
SSubEntry* FindSubtitle(DWORD VidOffset)
{
 float OffPerc = NumToPerc((double)VidOffset, (double)this->VFileLen);
 for(UINT ctr=0;ctr < this->LineCount;ctr++)
  {
   if(OffPerc < this->Lines[ctr].PercBegin)continue;
   if(OffPerc > this->Lines[ctr].PercEnd)continue;
   return &this->Lines[ctr];
  }
 return NULL;
}
//------------------------------------------


};
//====================================================================================
