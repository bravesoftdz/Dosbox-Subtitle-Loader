//---------------------------------------------------------------------------

#ifndef ContainersH
#define ContainersH

#include "CommonFuncs.h"
//---------------------------------------------------------------------------
// Do not calls constructors for objects
//
template<typename T, int PreAll=1> class CDynArray  // Stupid compiler cannot parse CDynBuffer<C, (PreAll*sizeof(T))> - the problem with sizeof(T) // template<typename T, int PreAll=1, int dummy=sizeof(T)> class CDynArray : public CDynBuffer<T, (PreAll*dummy)>
{
 T*   Data;
 UINT mCount;

public:
 CDynArray(const CDynArray &Arr){this->Assign(str);}
 CDynArray(void){Data=NULL;mCount=0;}   // Constructor of CDynBuffer will be called
 ~CDynArray(){}      // Destructor of CDynBuffer will be called

//----------------------
 T* ResizeFor(UINT Cnt)
 {
  if(Cnt && this->Data)this->Data = (T*)realloc(this->Data,(Cnt*sizeof(T))+8);
	else if(!this->Data)this->Data = (T*)malloc((Cnt*sizeof(T))+8);
	  else if(!Cnt && this->Data){free(this->Data);this->Data=NULL;}
  this->mCount = Cnt;
  return this->Data;
 }
 //----------------------
 T* First(void) {return (this->mCount)?(&this->Data[0]):(NULL);}                   // Uneffective for an Arrays
 T* Last(void)  {return (this->mCount)?(&this->Data[this->mCount-1]):(NULL);}      // Uneffective for an Arrays
 T* Next(T* ent){return (this->IsInArray(++ent))?(ent):(NULL);}                    // Uneffective for an Arrays		// Returns NULL at the End	of List
 T* Prev(T* ent){return (this->IsInArray(--ent))?(ent):(NULL);}                    // Uneffective for an Arrays		// Returns NULL at the End	of List
//----------------------
 T* Assign(const CDynArray &Arr, UINT Cnt=0)  // Can assign to Cnt elements fron array Arr of all content of Arr
 {
  if(!Cnt || (Cnt > Arr.mCount))Cnt = Arr.mCount;
  if(!this->ResizeFor(Cnt))return NULL;
  memcpy(this->Data, Arr.Data(), (sizeof(T)*Cnt));
  return this->Data;
 }
//----------------------
 T* Assign(const T* Arr, UINT Cnt=1)    // Pointer to N element of Array, 'arr' can be NULL
 {
  if(!this->ResizeFor(Cnt))return NULL;
  if(Arr)memcpy(this->Data, (PVOID)Arr, (sizeof(T)*Cnt));
  return this->Data;
 }
//----------------------
 T* Append(const CDynArray &Arr)  // TODO: redirect to 'Append(const T* Arr, UINT Cnt=1)'
 {
  UINT Count = this->mCount;
  if(!this->ResizeFor(Arr.mCount+Count))return NULL;
  T* start = &this->Data[Count];
  memcpy(&this->Data[Count], Arr.Data(), Arr.Size());  // Was (sizeof(T)*Arr.mCount)
  return start;
 }
//----------------------
 T* Append(const T* Arr, UINT Cnt=1)      // 'arr' can be NULL
 {
  UINT Count = this->mCount;
  if(!this->ResizeFor(Cnt+Count))return NULL;
  T* start = &this->Data[Count];
  if(Arr)memcpy(&this->Data[Count], (PVOID)Arr, (sizeof(T)*Cnt));
  return start;
 }
//----------------------
 T* Insert(const CDynArray &Arr, UINT Index)    // TODO: redirect to 'Insert(const T* Arr, UINT Index, UINT Cnt=1)'   // Inserts BEFORE the 'Index'
 {
  UINT Count = this->mCount;
  if(Index >= Count)return this->Append(Arr.GetData(),Arr.mCount);
  if(!this->ResizeFor(Arr.Count+this->mCount))return NULL;
  T* start = &this->Data[Index];
  memmove(&this->Data[Index+Arr->mCount], &this->Data[Index], (sizeof(T)*(Count-Index)));  // Get a space for new elements
  memcpy(&this->Data[Index], Arr.Data(), Arr.Size());    // Was (sizeof(T)*Arr.mCount)
  return start;
 }
//----------------------
 T* Insert(const T* Arr, UINT Index, UINT Cnt=1)      // 'arr' can be NULL     // Inserts BEFORE the 'Index'
 {
  UINT Count = this->mCount;
  if(Index >= Count)return this->Append(Arr,Cnt);
  if(!this->ResizeFor(Cnt+Count))return NULL;
  T* start = &this->Data[Index];
  memmove(&this->Data[Index+Cnt], &this->Data[Index], (sizeof(T)*(Count-Index)));  // Get a space for new elements
  if(Arr)memcpy(&this->Data[Index], (void*)Arr, (sizeof(T)*Cnt));
  return start;
 }
//----------------------
 void Delete(UINT Pos, UINT Cnt=1)
 {
  UINT Count = this->mCount;
  if(Pos >= Count)return;
  if((Pos+Cnt) >= Count)Cnt = (Count-Pos);   // Trim the Array`s tail
	else memmove(&this->Data[Pos],&this->Data[Pos+Cnt],(Count-(Pos+Cnt))*sizeof(T));   // Fix a hole in the array
  this->ResizeFor(Count-Cnt);   // Shink the used bytes/allocated bytes
 }
//---------------------
 UINT IndexOf(T* Itm)
  {
   if(!this->IsInArray(Itm))return -1;
   return (Itm - &this->Data[0]);  // Result is a Number of Objects, not a bytes!
  } 
//----------------------
 bool SwapItems(UINT IIdxA, UINT IIdxB)
  {
   if((IIdxA > this->mCount)||(IIdxB > this->mCount))return false;
   this->SwapMem(this->Data[IIdxA], this->Data[IIdxB], sizeof(T)); 
   return true;
  }
//----------------------
 template<typename S> bool SortByKeyProc(UINT IIdxA, UINT IIdxB, S IKeyA, S IKeyB)    // Sort by address
  {
   if((IKeyB < IKeyA) && this->SwapItems(IIdxA,IIdxB))return true;
   return false;
  }
//----------------------
 bool IsInArray(T* Itm){return ((Itm >= &this->Data[0])&&(Itm < &this->Data[this->mCount]));}
//----------------------
 void WriteElements(T* Dst, T* Src, UINT Cnt){memmove(Dst,Src,(sizeof(T)*Cnt));}
//----------------------
 void Clear(void){this->Free();}    // Do some more job here?
//----------------------
 UINT Count(void){return this->mCount;}   
//----------------------
 bool IsEmpty(void)  const {return !this->mCount;}
//----------------------
// bool IsEqual(const CDynArray &Arr)  const {return !this->mCount;} // in parent class!!!
//----------------------
 operator const T*() const {return this->Data;}
//----------------------
// void       operator  = (const CDynArray &Arr){this->Assign(&Arr);}    // !!!!!!!!!!!!!!    What is assigned (Array or an element?)
//----------------------
 CDynArray& operator += (const CDynArray &Arr){this->Append(&Arr);return *this;}
//----------------------
 T&         operator [] (int index){return this->Data[index];}   // No bounds check, work as static array !!!??? // What to do if OutOfBounds (Do not throw exceptions!)  // How to acces to Array of Arrays?
//----------------------
 bool       operator == (const CDynArray &Arr) {return this->IsEqual(&Arr);}
//----------------------

};
//---------------------------------------------------------------------------
template <class T> class CLinkedList
{
 struct SEntry
  {
   T  Element;
   SEntry *Prev;
   SEntry *Next;   

   void* operator new(size_t size, void *ptr){return HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,size);}
   void  operator delete(void* mem){HeapFree(GetProcessHeap(),0,mem);}    // No normal placement delete are possible
//----------------------
  SEntry(void){this->Prev = this->Next = NULL;}
  ~SEntry(){this->Prev = this->Next = NULL;}
  }*List;

 UINT   ECount;
 HANDLE hHeap;
 static __inline SEntry* EntryFromElement(T* Element){return (SEntry*)Element;}     //{return (SEntry*)(((PBYTE)Element)-(PBYTE)&((SEntry*)(0))->Element);}

public:
 CLinkedList(void){this->ECount=0;this->List=NULL;this->hHeap=GetProcessHeap();}
 ~CLinkedList(){this->Clear();}
//----------------------
 SEntry* AllocEntry(void){return new ((void*)this->hHeap) SEntry();}   //(SEntry*)HeapAlloc(this->hHeap,HEAP_ZERO_MEMORY,sizeof(SEntry));}
//----------------------
 void FreeEntry(SEntry* Entry){delete(Entry);}
//----------------------
 UINT GetCount(void){return this->ECount;}
//----------------------
 void Clear(void){while(this->List && this->Remove(&this->List->Element));}
//----------------------
 T*   GetFirst(void){return (this->List)?(&this->List->Element):(NULL);}           // Don`t do checks?
 T*   GetLast(void){return (this->List)?(&this->List->Prev->Element):(NULL);}     // Don`t do checks?
 static T*   GetNext(T* Element){return &this->EntryFromElement(Element)->Next->Element;}   // Works for ANY CLinkedList instance, but no check of end of the list
 static T*   GetPrev(T* Element){return &this->EntryFromElement(Element)->Prev->Element;}   // Works for ANY CLinkedList instance, but no check of end of the list
//----------------------
 T*   GetSNext(T* Element)            // Check List to NULL or GetHead is enough?
  {
   SEntry* entry = this->EntryFromElement(Element)->Next;  // Last is Tail
   if(entry == this->List)return NULL;    // Head again
   return &entry->Element;
  }
//----------------------
 T*   GetSPrev(T* Element)            // Check List to NULL or GetTail is enough?
  {
   SEntry* entry = this->EntryFromElement(Element)->Prev; // Last is Head
   if(entry == this->List->Prev)return NULL;    // Tail again
   return &entry->Element;
  }
//----------------------
 T* Add(T* Element, PUINT Index=NULL){return this->Insert(Element,NULL,Index);}
//----------------------
 bool MoveFirst(UINT Index)
  {
   if(!this->MoveAfter(Index, 0))return false;
   this->List = this->List->Next; // Set new entry as list head	 // The list is cycled
   return true;
  }
//----------------------
 bool MoveAfter(UINT Index, UINT IndexAfter)
  {
   SEntry* After   = NULL;
   SEntry* Current = NULL;

   T* NE1 = this->GetEntry(IndexAfter, &After);
   if(!NE1)return false;
   T* NE2 = this->GetEntry(Index, &Current);
   if(!NE2)return false;

   Current->Prev->Next = Current->Next;
   Current->Next->Prev = Current->Prev;

   Current->Prev = After;
   Current->Next = After->Next;
   After->Next->Prev = Current;
   After->Next   = Current;
   return true;
  }
//----------------------
 T* InsertFirst(PUINT Index)
  {
   T* NEl = this->Insert(NULL,Index);
   if(NEl)this->List = this->List->Prev; // Set new entry as list head	 // The list is cycled
   return NEl;
  }
//----------------------
 T* InsertFirst(T* Element, PUINT Index)
  {
   T* NEl = this->Insert(NULL,Index);
   if(NEl)
	{
	 this->List = this->List->Prev; // Set new entry as list head	 // The list is cycled
	 memcpy(NEl,Element,sizeof(T));
	}
   return NEl;
  }
//----------------------
 T* Insert(T* Element, PVOID After, PUINT Index=NULL)
  {
   T* NEl = this->Insert(After,Index);
   if(NEl)memcpy(NEl,Element,sizeof(T));
   return NEl;
  }
//----------------------
 T* InsertBefore(PVOID Before, PUINT Index=NULL)
  {
  // SEntry * ent1 = this->List;

   T* NEl = this->Insert(((SEntry*)Before)->Prev,Index);
  // SEntry * ent2 = this->List;

   if(this->List == Before)this->List = ((SEntry*)Before)->Prev;
   return NEl;
  }
//----------------------
 T* Insert(PVOID After, PUINT Index=NULL)
  {
   SEntry *NewEntry = this->AllocEntry();
   if(Index)*Index  = this->ECount;
   if(!this->List)
	{
	 this->List        = NewEntry;
	 this->List->Prev  = NewEntry;   // Set end of list
	 this->List->Next  = NewEntry;
	 this->ECount      = 1;
	}
	 else
	  {
	   SEntry *AfterEn = (After)?((SEntry*)After):(this->List->Prev); // After=NULL - Add to end of list
	   AfterEn->Next->Prev = NewEntry;
	   NewEntry->Next  = AfterEn->Next;
	   NewEntry->Prev  = AfterEn;
	   AfterEn->Next   = NewEntry;
	   this->ECount++;
	  }
   return &NewEntry->Element;
  }
//----------------------
 bool Remove(UINT Index){return this->Remove(this->GetEntry(Index));}
//----------------------
 bool Remove(T* Element)
  {
   if(!this->List || !Element)return false;
   SEntry* Removing     = (SEntry*)(((PBYTE)Element)-(PBYTE)&((SEntry*)(0))->Element);
   Removing->Prev->Next = Removing->Next;
   Removing->Next->Prev = Removing->Prev;
   if(Removing == this->List)
	{
	 if(Removing->Next == this->List)this->List = NULL;  // Removing one last entry (head)
	   else this->List = Removing->Next; // Set new head
	}
   this->FreeEntry(Removing);
   this->ECount--;
   return true;
  }
//----------------------
 T* FindEntry(T* Element, PUINT Index=NULL)
  {
   SEntry* CurEn = this->List;
   for(UINT ctr=0;CurEn;ctr++)
	{
	 if(RtlCompareMemory(&CurEn->Element,Element,sizeof(T))==sizeof(T)){if(Index)*Index=ctr;return CurEn->Element;}
	 CurEn = CurEn->Next;
	 if(CurEn == this->List)break;
	}
   return NULL;
  }
//----------------------
 T* GetEntry(UINT Index)
  {
   SEntry* CurEn = this->List;
   for(UINT ctr=0;CurEn;ctr++)
	{
	 if(ctr == Index)return &CurEn->Element;
	 CurEn = CurEn->Next;
	 if(CurEn == this->List)break;
	}
   return NULL;
  }

};
//-----------------------------------------------------------------------------
class CMiniStr
{
 HANDLE hHeap;
 LPSTR  Data;
 UINT   SLength;     // Not including terminating 0
 UINT   Allocated;

//----------------------
 void Initialize(void)
 {
  this->Data      = NULL;
  this->SLength   = 0;
  this->Allocated = 0;
  this->hHeap     = GetProcessHeap();
 }
//----------------------
public:
 UINT  BufSize(void) const {return this->Allocated;}
//----------------------
 LPSTR ResizeFor(UINT Len)
 {
  if(Len >= this->Allocated)
   {
	this->Allocated = Len + ((this->Allocated)?(this->Allocated):(256));  // Reserve some space, initial prealloc 256
	if(!this->Data)this->Data = (LPSTR)HeapAlloc(this->hHeap,HEAP_ZERO_MEMORY,this->Allocated);
	  else this->Data = (LPSTR)HeapReAlloc(this->hHeap,HEAP_ZERO_MEMORY,this->Data,this->Allocated);
	if(!this->Data)this->Allocated = 0;
   }
  return this->Data;
 }
//----------------------
 LPSTR Assign(const CMiniStr *str)
 {
  if(!str)return NULL;
  if(!this->ResizeFor(str->Length()+1))return NULL;
  memcpy(this->Data,str->c_str(),str->Length()+1);         // Copy string including terminating Null
  this->SLength = str->Length();         // Not include terminating Null
  return this->Data;
 }
//----------------------
 LPSTR Assign(LPSTR str)    // Assign a Null terminated string
 {
  if(!str)return NULL;
  UINT Len = lstrlen(str);        // Not includes terminating Null   // Beware a not Null terminated strings                        
  if(!this->ResizeFor(Len+1))return NULL;     // +1 for terminating Null
  memcpy(this->Data,str,Len+1);             // Copy string including terminating Null
  this->SLength = Len;    
  return this->Data;
 }
//----------------------
 LPSTR Assign(LPSTR str, UINT Len)
 { 
  if(!str)return NULL;                         
  if(!this->ResizeFor(Len+1))return NULL;    // +1 for terminating Null
  memcpy(this->Data,str,Len);          // Copy string, no terminating Null
  this->Data[Len] = 0;          // Add terminating Null
  this->SLength = Len;        // Not include terminating Null
  return this->Data;
 }
//----------------------
 LPSTR Append(const CMiniStr *str)
 {
  if(!str)return NULL;
  if(!this->ResizeFor(str->Length()+this->SLength+1))return NULL;     // +1 for terminating Null
  memcpy(&this->Data[this->SLength],str->c_str(),str->Length()+1);    // Copy string including terminating Null
  this->SLength += str->Length();
  return this->Data;
 }
//----------------------
 LPSTR Append(LPSTR str)
 {
  if(!str)return NULL;
  UINT Len = lstrlen(str);     // Not includes terminating Null   // Beware a not Null terminated strings
  if(!this->ResizeFor(1+Len+this->SLength))return NULL;  // 1 for terminating Null
  memcpy(&this->Data[this->SLength],str,Len+1);           // Copy string including terminating Null
  this->SLength += Len;
  return this->Data;
 }
//----------------------
 LPSTR Append(LPSTR str, UINT Len)
 {
  if(!str)return NULL;
  if(!this->ResizeFor(1+Len+this->SLength))return NULL;   // +1 for terminating Null
  memcpy(&this->Data[this->SLength],str,Len);            // Copy string, no terminating Null
  this->SLength += Len;
  this->Data[this->SLength] = 0;          // Add terminating Null
  return this->Data;
 }
//----------------------
 bool ChangeEncoding(UINT FromCP, UINT ToCP)
 {
  UINT  TmpLen = (this->Length()*sizeof(WCHAR));   // twice as size of current string
  PBYTE TmpBuf = (PBYTE)HeapAlloc(this->hHeap,HEAP_ZERO_MEMORY,(TmpLen*2)+64);
  if(!TmpBuf)return false;
  int wlen = MultiByteToWideChar(FromCP,0,this->Data,this->SLength,(PWSTR)TmpBuf,(TmpLen/2));
  if(wlen <= 0)return false;
  int ulen = WideCharToMultiByte(ToCP,0,(PWSTR)TmpBuf,wlen,(LPSTR)&TmpBuf[TmpLen],TmpLen,NULL,NULL); 
  if(ulen <= 0)return false;
  this->ResizeFor(ulen);
  this->SLength = ulen-1; // Skip Null terminator
  lstrcpy(this->Data,(LPSTR)&TmpBuf[TmpLen]);    
  HeapFree(this->hHeap,0,TmpBuf);
  return true;
 }
//----------------------

public:
 CMiniStr(void){this->Initialize();}
 CMiniStr(LPSTR str){this->Initialize();this->Append(str);}
 CMiniStr(const CMiniStr &str){this->Initialize();this->Append(&str);}
//----------------------
 ~CMiniStr()
 {
  if(this->Data)HeapFree(this->hHeap,0,Data);
 }
//----------------------
 CMiniStr& Utf8ToAnsi(void)
 {
  this->ChangeEncoding(CP_UTF8, CP_ACP);
  return *this;
 }
//----------------------
 CMiniStr& AnsiToUtf8(void)
 {
  this->ChangeEncoding(CP_ACP, CP_UTF8);
  return *this;
 }
//----------------------
 UINT  Length(void) const {return this->SLength;}
//----------------------
 LPSTR c_str(void) const {if(this->Data)return this->Data;return "";}
//----------------------
 PBYTE c_data(void) const {if(this->SLength)return (PBYTE)this->Data;return NULL;}  // String is empty if SLength=0, but the buffer may contain something
//----------------------
 void  operator = (const CMiniStr &str){this->Assign(&str);}
//----------------------
 void  operator = (LPSTR str){this->Assign(str);}
//----------------------
 CMiniStr& operator += (const CMiniStr &str){this->Append(&str);return *this;}
//----------------------
 CMiniStr& operator += (LPSTR str){this->Append(str);return *this;}

};
//-----------------------------------------------------------------------------


//---------------------------------------------------------------------------
#endif
