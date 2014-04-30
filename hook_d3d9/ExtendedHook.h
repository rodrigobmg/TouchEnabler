#ifndef _EHOOK
#define _EHOOK

typedef struct _EHOOKSTRUCT{

DWORD * adr_init;

DWORD * adr_redirect;

DWORD * adr_new_api;

DWORD bytes_size;

}EHOOKSTRUCT, *PEHOOKSTRUCT;

unsigned int Reckon_bytes_jmp(void * adr);

void * UninstallEHook(PEHOOKSTRUCT EHookA);
bool InstallEHookEx(void * TargetAddress, PEHOOKSTRUCT EHookA, void * redit);
bool InstallEHook(LPCSTR API, LPCTSTR lib, PEHOOKSTRUCT  EHookA, void * redit);

bool CustomEHook(void * TargetAddress, PEHOOKSTRUCT EHookA, void * redit, unsigned int bytes_jmp);

#endif
