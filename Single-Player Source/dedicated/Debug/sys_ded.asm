	TITLE	D:\mod\Single-Player Source\dedicated\sys_ded.cpp
	.386P
include listing.inc
if @Version gt 510
.model FLAT
else
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
_DATA	SEGMENT DWORD USE32 PUBLIC 'DATA'
_DATA	ENDS
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
_BSS	SEGMENT PARA USE32 PUBLIC 'BSS'
_BSS	ENDS
$$SYMBOLS	SEGMENT BYTE USE32 'DEBSYM'
$$SYMBOLS	ENDS
$$TYPES	SEGMENT BYTE USE32 'DEBTYP'
$$TYPES	ENDS
_TLS	SEGMENT DWORD USE32 PUBLIC 'TLS'
_TLS	ENDS
;	COMDAT ??_C@_08CPBM@swds?4dll?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_09OKKL@Half?9Life?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0CD@KNIH@?$CF?41f?5fps?5?$CF2i?$CI?$CF2i?5spec?$CJ?1?$CF2i?5on?5?$CF1@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_06FBCH@?9usegh?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0N@IIIH@ghostinj?4dll?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_02PIMC@?$AN?6?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_03BBFP@?$AI?5?$AI?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_02DILL@?$CFs?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_03HHKO@?$CFs?6?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_09MBEO@?9heapsize?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??8@YAHABU_GUID@@0@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_Sleep_Old@@YAXH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_Sleep_Timer@@YAXH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_Sleep_Select@@YAXH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_GetProcAddress@@YAPAXJPBD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_LoadLibrary@@YAJPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_FreeLibrary@@YAXJ@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_GetExecutableName@@YAHPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_ErrorMessage@@YAXHPBD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?UpdateStatus@@YAXH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_ConsoleOutput@@YAXPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_Printf@@YAXPADZZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Load3rdParty@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?EF_VID_ForceUnlockedAndReturnState@@YAHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?EF_VID_ForceLockState@@YAXH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?CheckParm@@YAPADPBDPAPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?InitInstance@@YAHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Sys_ConsoleInput@@YAPADXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?WriteStatusText@@YAXPAD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?CreateConsoleWindow@@YAHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?DestroyConsoleWindow@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?ProcessConsoleInput@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GameInit@@YAHXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GameShutdown@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?CtrlHandler@@YAHK@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _WinMain@16
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
FLAT	GROUP _DATA, CONST, _BSS
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
PUBLIC	??_C@_08CPBM@swds?4dll?$AA@			; `string'
PUBLIC	?ef@@3Uexefuncs_s@@A				; ef
PUBLIC	?gpszCmdLine@@3PADA				; gpszCmdLine
PUBLIC	?Sys_Sleep@@3P6AXH@ZA				; Sys_Sleep
PUBLIC	?gbAppHasBeenTerminated@@3HA			; gbAppHasBeenTerminated
_BSS	SEGMENT
_gpMemBase DD	01H DUP (?)
?ef@@3Uexefuncs_s@@A DB 084H DUP (?)			; ef
_hDLLThirdParty DD 01H DUP (?)
?gpszCmdLine@@3PADA DD 01H DUP (?)			; gpszCmdLine
?Sys_Sleep@@3P6AXH@ZA DD 01H DUP (?)			; Sys_Sleep
	ALIGN	8

_?tLast@?1??UpdateStatus@@YAXH@Z@4NA DQ 01H DUP (?)
?gbAppHasBeenTerminated@@3HA DD 01H DUP (?)		; gbAppHasBeenTerminated
	ALIGN	8

_?oldtime@?BA@??WinMain@@9@16@4NA DQ 01H DUP (?)
_BSS	ENDS
_DATA	SEGMENT
_g_pszengine DD	FLAT:??_C@_08CPBM@swds?4dll?$AA@
_giMemSize DD	02000000H
_DATA	ENDS
;	COMDAT ??_C@_08CPBM@swds?4dll?$AA@
CONST	SEGMENT
??_C@_08CPBM@swds?4dll?$AA@ DB 'swds.dll', 00H		; `string'
CONST	ENDS
PUBLIC	?Sys_Sleep_Old@@YAXH@Z				; Sys_Sleep_Old
EXTRN	__imp__Sleep@4:NEAR
;	COMDAT ?Sys_Sleep_Old@@YAXH@Z
_TEXT	SEGMENT
_msec$ = 8
?Sys_Sleep_Old@@YAXH@Z PROC NEAR			; Sys_Sleep_Old, COMDAT

; 68   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 69   : #ifdef _WIN32
; 70   : 	Sleep( msec );

	mov	eax, DWORD PTR _msec$[ebp]
	push	eax
	call	DWORD PTR __imp__Sleep@4

; 71   : #else
; 72   :     usleep(msec * 1000);
; 73   : #endif
; 74   : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_Sleep_Old@@YAXH@Z ENDP				; Sys_Sleep_Old
_TEXT	ENDS
PUBLIC	?Sys_Sleep_Timer@@YAXH@Z			; Sys_Sleep_Timer
;	COMDAT ?Sys_Sleep_Timer@@YAXH@Z
_TEXT	SEGMENT
_msec$ = 8
?Sys_Sleep_Timer@@YAXH@Z PROC NEAR			; Sys_Sleep_Timer, COMDAT

; 110  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 111  : #ifdef _WIN32
; 112  : 	Sleep( msec );

	mov	eax, DWORD PTR _msec$[ebp]
	push	eax
	call	DWORD PTR __imp__Sleep@4

; 113  : #else
; 114  : // linux runs on a 100Hz scheduling clock, so the minimum latency from
; 115  : // usleep is 10msec. However, people want lower latency than this.. 
; 116  : //
; 117  : // There are a few solutions, one is to use the realtime scheduler in the 
; 118  : // kernel BUT this needs root privelleges to start. It also can play
; 119  : // unfriendly with other programs.
; 120  : 
; 121  : // Another solution is to use software timers, they use the RTC of the 
; 122  : // system and are accurate to microseconds (or so).
; 123  : 
; 124  : // timers, via setitimer() are used here
; 125  : 
; 126  : 	struct itimerval tm;
; 127  : 
; 128  : 	tm.it_value.tv_sec=msec/1000; // convert msec to seconds
; 129  : 	tm.it_value.tv_usec=(msec%1000)*1E3; // get the number of msecs and change to micros
; 130  : 	tm.it_interval.tv_sec  = 0;
; 131  :         tm.it_interval.tv_usec = 0;
; 132  : 
; 133  : 	paused=0;
; 134  : 	if( setitimer(ITIMER_REAL,&tm,NULL)==0) 
; 135  : 	{ // set the timer to trigger
; 136  : 		pause();	 // wait for the signal
; 137  : 	}
; 138  : 	paused=1;
; 139  : 
; 140  : #endif
; 141  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_Sleep_Timer@@YAXH@Z ENDP				; Sys_Sleep_Timer
_TEXT	ENDS
PUBLIC	?Sys_Sleep_Select@@YAXH@Z			; Sys_Sleep_Select
;	COMDAT ?Sys_Sleep_Select@@YAXH@Z
_TEXT	SEGMENT
_msec$ = 8
?Sys_Sleep_Select@@YAXH@Z PROC NEAR			; Sys_Sleep_Select, COMDAT

; 145  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 146  : #ifdef _WIN32
; 147  : 	Sleep( msec );

	mov	eax, DWORD PTR _msec$[ebp]
	push	eax
	call	DWORD PTR __imp__Sleep@4

; 148  : #else	// _WIN32
; 149  : 	struct timeval tv;
; 150  : 
; 151  : 	// Assumes msec < 1000
; 152  : 	tv.tv_sec	= 0;
; 153  : 	tv.tv_usec	= 1000 * msec;
; 154  : 
; 155  : 	select( 1, NULL, NULL, NULL, &tv );
; 156  : #endif	// _WIN32
; 157  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_Sleep_Select@@YAXH@Z ENDP				; Sys_Sleep_Select
_TEXT	ENDS
PUBLIC	?Sys_GetProcAddress@@YAPAXJPBD@Z		; Sys_GetProcAddress
EXTRN	__imp__GetProcAddress@8:NEAR
;	COMDAT ?Sys_GetProcAddress@@YAPAXJPBD@Z
_TEXT	SEGMENT
_library$ = 8
_name$ = 12
?Sys_GetProcAddress@@YAPAXJPBD@Z PROC NEAR		; Sys_GetProcAddress, COMDAT

; 161  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 162  : #ifdef _WIN32
; 163  : 	return ( void * )GetProcAddress( (HMODULE)library, name );

	mov	eax, DWORD PTR _name$[ebp]
	push	eax
	mov	ecx, DWORD PTR _library$[ebp]
	push	ecx
	call	DWORD PTR __imp__GetProcAddress@8

; 164  : #else // LINUX
; 165  : 	return dlsym( (void *)library, name );
; 166  : #endif
; 167  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_GetProcAddress@@YAPAXJPBD@Z ENDP			; Sys_GetProcAddress
_TEXT	ENDS
PUBLIC	?Sys_LoadLibrary@@YAJPAD@Z			; Sys_LoadLibrary
EXTRN	__imp__LoadLibraryA@4:NEAR
;	COMDAT ?Sys_LoadLibrary@@YAJPAD@Z
_TEXT	SEGMENT
_lib$ = 8
_hDll$ = -4
?Sys_LoadLibrary@@YAJPAD@Z PROC NEAR			; Sys_LoadLibrary, COMDAT

; 170  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 68					; 00000044H
	push	ebx
	push	esi
	push	edi

; 171  : 	void *hDll = NULL;

	mov	DWORD PTR _hDll$[ebp], 0

; 172  : 
; 173  : #ifdef _WIN32
; 174  : 	hDll = ::LoadLibrary( lib );

	mov	eax, DWORD PTR _lib$[ebp]
	push	eax
	call	DWORD PTR __imp__LoadLibraryA@4
	mov	DWORD PTR _hDll$[ebp], eax

; 175  : #else
; 176  :     char    cwd[1024];
; 177  :     char    absolute_lib[1024];
; 178  :     
; 179  :     if (!getcwd(cwd, sizeof(cwd)))
; 180  :         Sys_ErrorMessage(1, "Sys_LoadLibrary: Couldn't determine current directory.");
; 181  :         
; 182  :     if (cwd[strlen(cwd)-1] == '/')
; 183  :         cwd[strlen(cwd)-1] = 0;
; 184  :         
; 185  :     snprintf(absolute_lib, sizeof(absolute_lib), "%s/%s", cwd, lib);
; 186  :     
; 187  :     hDll = dlopen( absolute_lib, RTLD_NOW );
; 188  :     if ( !hDll )
; 189  :     {
; 190  :         Sys_ErrorMessage( 1, dlerror() );
; 191  :     }   
; 192  : #endif
; 193  : 	return (long)hDll;

	mov	eax, DWORD PTR _hDll$[ebp]

; 194  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_LoadLibrary@@YAJPAD@Z ENDP				; Sys_LoadLibrary
_TEXT	ENDS
PUBLIC	?Sys_FreeLibrary@@YAXJ@Z			; Sys_FreeLibrary
EXTRN	__imp__FreeLibrary@4:NEAR
;	COMDAT ?Sys_FreeLibrary@@YAXJ@Z
_TEXT	SEGMENT
_library$ = 8
?Sys_FreeLibrary@@YAXJ@Z PROC NEAR			; Sys_FreeLibrary, COMDAT

; 197  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 198  : 	if ( !library )

	cmp	DWORD PTR _library$[ebp], 0
	jne	SHORT $L42578

; 199  : 		return;

	jmp	SHORT $L42577
$L42578:

; 200  : 
; 201  : #ifdef _WIN32
; 202  : 	::FreeLibrary( (HMODULE)library );

	mov	eax, DWORD PTR _library$[ebp]
	push	eax
	call	DWORD PTR __imp__FreeLibrary@4
$L42577:

; 203  : #else
; 204  : 	dlclose( (void *)library );
; 205  : #endif
; 206  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_FreeLibrary@@YAXJ@Z ENDP				; Sys_FreeLibrary
_TEXT	ENDS
PUBLIC	?Sys_GetExecutableName@@YAHPAD@Z		; Sys_GetExecutableName
EXTRN	__imp__GetModuleFileNameA@12:NEAR
EXTRN	__imp__GetModuleHandleA@4:NEAR
;	COMDAT ?Sys_GetExecutableName@@YAHPAD@Z
_TEXT	SEGMENT
_out$ = 8
?Sys_GetExecutableName@@YAHPAD@Z PROC NEAR		; Sys_GetExecutableName, COMDAT

; 209  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 210  : #ifdef _WIN32
; 211  : 	if ( !::GetModuleFileName( ( HINSTANCE )GetModuleHandle( NULL ), out, 256 ) )

	push	256					; 00000100H
	mov	eax, DWORD PTR _out$[ebp]
	push	eax
	push	0
	call	DWORD PTR __imp__GetModuleHandleA@4
	push	eax
	call	DWORD PTR __imp__GetModuleFileNameA@12
	test	eax, eax
	jne	SHORT $L42584

; 213  : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42582
$L42584:

; 215  : #else
; 216  : 	strcpy( out, g_szEXEName );
; 217  : #endif
; 218  : 	return 1;

	mov	eax, 1
$L42582:

; 219  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_GetExecutableName@@YAHPAD@Z ENDP			; Sys_GetExecutableName
_TEXT	ENDS
PUBLIC	?Sys_ErrorMessage@@YAXHPBD@Z			; Sys_ErrorMessage
PUBLIC	??_C@_09OKKL@Half?9Life?$AA@			; `string'
EXTRN	__imp__PostQuitMessage@4:NEAR
EXTRN	__imp__MessageBoxA@16:NEAR
;	COMDAT ??_C@_09OKKL@Half?9Life?$AA@
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
??_C@_09OKKL@Half?9Life?$AA@ DB 'Half-Life', 00H	; `string'
CONST	ENDS
;	COMDAT ?Sys_ErrorMessage@@YAXHPBD@Z
_TEXT	SEGMENT
_msg$ = 12
?Sys_ErrorMessage@@YAXHPBD@Z PROC NEAR			; Sys_ErrorMessage, COMDAT

; 229  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 230  : #ifdef _WIN32
; 231  : 	MessageBox( NULL, msg, "Half-Life", MB_OK );

	push	0
	push	OFFSET FLAT:??_C@_09OKKL@Half?9Life?$AA@ ; `string'
	mov	eax, DWORD PTR _msg$[ebp]
	push	eax
	push	0
	call	DWORD PTR __imp__MessageBoxA@16

; 232  : 	PostQuitMessage(0);	

	push	0
	call	DWORD PTR __imp__PostQuitMessage@4

; 233  : #else
; 234  : 	printf( "%s\n", msg );
; 235  : 	exit( -1 );
; 236  : #endif
; 237  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_ErrorMessage@@YAXHPBD@Z ENDP			; Sys_ErrorMessage
_TEXT	ENDS
PUBLIC	?WriteStatusText@@YAXPAD@Z			; WriteStatusText
PUBLIC	?UpdateStatus@@YAXH@Z				; UpdateStatus
PUBLIC	??_C@_0CD@KNIH@?$CF?41f?5fps?5?$CF2i?$CI?$CF2i?5spec?$CJ?1?$CF2i?5on?5?$CF1@ ; `string'
PUBLIC	__real@4@4008fa00000000000000
PUBLIC	__real@8@3ffe8000000000000000
EXTRN	__snprintf:NEAR
EXTRN	__imp__timeGetTime@0:NEAR
EXTRN	?engineapi@@3Uengine_api_s@@A:BYTE		; engineapi
EXTRN	__fltused:NEAR
;	COMDAT ??_C@_0CD@KNIH@?$CF?41f?5fps?5?$CF2i?$CI?$CF2i?5spec?$CJ?1?$CF2i?5on?5?$CF1@
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
??_C@_0CD@KNIH@?$CF?41f?5fps?5?$CF2i?$CI?$CF2i?5spec?$CJ?1?$CF2i?5on?5?$CF1@ DB '%'
	DB	'.1f fps %2i(%2i spec)/%2i on %16s', 00H	; `string'
CONST	ENDS
;	COMDAT __real@4@4008fa00000000000000
CONST	SEGMENT
__real@4@4008fa00000000000000 DD 0447a0000r	; 1000
CONST	ENDS
;	COMDAT __real@8@3ffe8000000000000000
CONST	SEGMENT
__real@8@3ffe8000000000000000 DQ 03fe0000000000000r ; 0.5
CONST	ENDS
;	COMDAT ?UpdateStatus@@YAXH@Z
_TEXT	SEGMENT
_force$ = 8
_tCurrent$ = -8
_szPrompt$ = -264
_n$ = -268
_spec$ = -272
_nMax$ = -276
_szMap$ = -308
_fps$ = -312
?UpdateStatus@@YAXH@Z PROC NEAR				; UpdateStatus, COMDAT

; 248  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 384				; 00000180H
	push	ebx
	push	esi
	push	edi

; 249  : 	static double tLast = 0.0;
; 250  : 	double	tCurrent;
; 251  : 	char	szPrompt[256];
; 252  : 	int		n, spec, nMax;
; 253  : 	char	szMap[32];
; 254  : 	float	fps;
; 255  : 
; 256  : 	if ( !engineapi.Host_GetHostInfo )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+96, 0
	jne	SHORT $L42602

; 257  : 		return;

	jmp	$L42592
$L42602:

; 258  : 
; 259  : 	tCurrent = (float)( timeGetTime() / 1000.0f );

	call	DWORD PTR __imp__timeGetTime@0
	mov	DWORD PTR -320+[ebp], eax
	mov	DWORD PTR -320+[ebp+4], 0
	fild	QWORD PTR -320+[ebp]
	fdiv	DWORD PTR __real@4@4008fa00000000000000
	fstp	QWORD PTR _tCurrent$[ebp]

; 260  : 
; 261  : 	engineapi.Host_GetHostInfo( &fps, &n, &spec, &nMax, szMap );

	lea	eax, DWORD PTR _szMap$[ebp]
	push	eax
	lea	ecx, DWORD PTR _nMax$[ebp]
	push	ecx
	lea	edx, DWORD PTR _spec$[ebp]
	push	edx
	lea	eax, DWORD PTR _n$[ebp]
	push	eax
	lea	ecx, DWORD PTR _fps$[ebp]
	push	ecx
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+96
	add	esp, 20					; 00000014H

; 262  : 
; 263  : 	if ( !force )

	cmp	DWORD PTR _force$[ebp], 0
	jne	SHORT $L42605

; 265  : 		if ( ( tCurrent - tLast ) < 0.5f )

	fld	QWORD PTR _tCurrent$[ebp]
	fsub	QWORD PTR _?tLast@?1??UpdateStatus@@YAXH@Z@4NA
	fcomp	QWORD PTR __real@8@3ffe8000000000000000
	fnstsw	ax
	test	ah, 1
	je	SHORT $L42605

; 266  : 			return;

	jmp	SHORT $L42592
$L42605:

; 268  : 
; 269  : 	tLast = tCurrent;

	mov	edx, DWORD PTR _tCurrent$[ebp]
	mov	DWORD PTR _?tLast@?1??UpdateStatus@@YAXH@Z@4NA, edx
	mov	eax, DWORD PTR _tCurrent$[ebp+4]
	mov	DWORD PTR _?tLast@?1??UpdateStatus@@YAXH@Z@4NA+4, eax

; 270  : 
; 271  : 	snprintf( szPrompt, sizeof(szPrompt), "%.1f fps %2i(%2i spec)/%2i on %16s", (float)fps, n, spec, nMax, szMap);

	lea	ecx, DWORD PTR _szMap$[ebp]
	push	ecx
	mov	edx, DWORD PTR _nMax$[ebp]
	push	edx
	mov	eax, DWORD PTR _spec$[ebp]
	push	eax
	mov	ecx, DWORD PTR _n$[ebp]
	push	ecx
	fld	DWORD PTR _fps$[ebp]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	push	OFFSET FLAT:??_C@_0CD@KNIH@?$CF?41f?5fps?5?$CF2i?$CI?$CF2i?5spec?$CJ?1?$CF2i?5on?5?$CF1@ ; `string'
	push	256					; 00000100H
	lea	edx, DWORD PTR _szPrompt$[ebp]
	push	edx
	call	__snprintf
	add	esp, 36					; 00000024H

; 272  : 
; 273  : 	WriteStatusText( szPrompt );

	lea	eax, DWORD PTR _szPrompt$[ebp]
	push	eax
	call	?WriteStatusText@@YAXPAD@Z		; WriteStatusText
	add	esp, 4
$L42592:

; 274  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?UpdateStatus@@YAXH@Z ENDP				; UpdateStatus
_TEXT	ENDS
PUBLIC	?Sys_ConsoleOutput@@YAXPAD@Z			; Sys_ConsoleOutput
EXTRN	__imp__WriteFile@20:NEAR
EXTRN	_memset:NEAR
EXTRN	_strlen:NEAR
_BSS	SEGMENT
_houtput DD	01H DUP (?)
_console_text DB 0100H DUP (?)
_console_textlen DD 01H DUP (?)
_BSS	ENDS
;	COMDAT ?Sys_ConsoleOutput@@YAXPAD@Z
_TEXT	SEGMENT
_string$ = 8
_dummy$ = -4
_text$ = -260
?Sys_ConsoleOutput@@YAXPAD@Z PROC NEAR			; Sys_ConsoleOutput, COMDAT

; 285  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 324				; 00000144H
	push	ebx
	push	esi
	push	edi

; 286  : #ifdef _WIN32
; 287  : 	unsigned long dummy;
; 288  : 	char	text[256];
; 289  : 
; 290  : 	if (console_textlen)

	cmp	DWORD PTR _console_textlen, 0
	je	SHORT $L42613

; 292  : 		text[0] = '\r';

	mov	BYTE PTR _text$[ebp], 13		; 0000000dH

; 293  : 		memset(&text[1], ' ', console_textlen);

	mov	eax, DWORD PTR _console_textlen
	push	eax
	push	32					; 00000020H
	lea	ecx, DWORD PTR _text$[ebp+1]
	push	ecx
	call	_memset
	add	esp, 12					; 0000000cH

; 294  : 		text[console_textlen+1] = '\r';

	mov	edx, DWORD PTR _console_textlen
	mov	BYTE PTR _text$[ebp+edx+1], 13		; 0000000dH

; 295  : 		text[console_textlen+2] = 0;

	mov	eax, DWORD PTR _console_textlen
	mov	BYTE PTR _text$[ebp+eax+2], 0

; 296  : 		WriteFile(houtput, text, console_textlen+2, &dummy, NULL);

	push	0
	lea	ecx, DWORD PTR _dummy$[ebp]
	push	ecx
	mov	edx, DWORD PTR _console_textlen
	add	edx, 2
	push	edx
	lea	eax, DWORD PTR _text$[ebp]
	push	eax
	mov	ecx, DWORD PTR _houtput
	push	ecx
	call	DWORD PTR __imp__WriteFile@20
$L42613:

; 298  : 
; 299  : 	WriteFile(houtput, string, strlen(string), &dummy, NULL);

	push	0
	lea	edx, DWORD PTR _dummy$[ebp]
	push	edx
	mov	eax, DWORD PTR _string$[ebp]
	push	eax
	call	_strlen
	add	esp, 4
	push	eax
	mov	ecx, DWORD PTR _string$[ebp]
	push	ecx
	mov	edx, DWORD PTR _houtput
	push	edx
	call	DWORD PTR __imp__WriteFile@20

; 300  : 
; 301  : 	if (console_textlen)

	cmp	DWORD PTR _console_textlen, 0
	je	SHORT $L42614

; 303  : 		WriteFile(houtput, console_text, console_textlen, &dummy, NULL);

	push	0
	lea	eax, DWORD PTR _dummy$[ebp]
	push	eax
	mov	ecx, DWORD PTR _console_textlen
	push	ecx
	push	OFFSET FLAT:_console_text
	mov	edx, DWORD PTR _houtput
	push	edx
	call	DWORD PTR __imp__WriteFile@20
$L42614:

; 305  : 	UpdateStatus( 1 /* force */ );

	push	1
	call	?UpdateStatus@@YAXH@Z			; UpdateStatus
	add	esp, 4

; 306  : #else
; 307  : 	printf( "%s", string );
; 308  : 	fflush( stdout );
; 309  : #endif
; 310  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_ConsoleOutput@@YAXPAD@Z ENDP			; Sys_ConsoleOutput
_TEXT	ENDS
PUBLIC	?Sys_Printf@@YAXPADZZ				; Sys_Printf
EXTRN	__vsnprintf:NEAR
;	COMDAT ?Sys_Printf@@YAXPADZZ
_TEXT	SEGMENT
_fmt$ = 8
_argptr$ = -4
_szText$ = -1028
?Sys_Printf@@YAXPADZZ PROC NEAR				; Sys_Printf, COMDAT

; 320  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 1092				; 00000444H
	push	ebx
	push	esi
	push	edi

; 321  : 	// Dump text to debugging console.
; 322  : 	va_list argptr;
; 323  : 	char szText[1024];
; 324  : 
; 325  : 	va_start (argptr, fmt);

	lea	eax, DWORD PTR _fmt$[ebp+4]
	mov	DWORD PTR _argptr$[ebp], eax

; 326  : 	vsnprintf (szText, sizeof(szText), fmt, argptr);

	mov	ecx, DWORD PTR _argptr$[ebp]
	push	ecx
	mov	edx, DWORD PTR _fmt$[ebp]
	push	edx
	push	1024					; 00000400H
	lea	eax, DWORD PTR _szText$[ebp]
	push	eax
	call	__vsnprintf
	add	esp, 16					; 00000010H

; 327  : 	va_end (argptr);

	mov	DWORD PTR _argptr$[ebp], 0

; 328  : 
; 329  : 	// Get Current text and append it.
; 330  : 	Sys_ConsoleOutput( szText );

	lea	ecx, DWORD PTR _szText$[ebp]
	push	ecx
	call	?Sys_ConsoleOutput@@YAXPAD@Z		; Sys_ConsoleOutput
	add	esp, 4

; 331  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_Printf@@YAXPADZZ ENDP				; Sys_Printf
_TEXT	ENDS
PUBLIC	?CheckParm@@YAPADPBDPAPAD@Z			; CheckParm
PUBLIC	?Load3rdParty@@YAXXZ				; Load3rdParty
PUBLIC	??_C@_06FBCH@?9usegh?$AA@			; `string'
PUBLIC	??_C@_0N@IIIH@ghostinj?4dll?$AA@		; `string'
;	COMDAT ??_C@_06FBCH@?9usegh?$AA@
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
??_C@_06FBCH@?9usegh?$AA@ DB '-usegh', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_0N@IIIH@ghostinj?4dll?$AA@
CONST	SEGMENT
??_C@_0N@IIIH@ghostinj?4dll?$AA@ DB 'ghostinj.dll', 00H	; `string'
CONST	ENDS
;	COMDAT ?Load3rdParty@@YAXXZ
_TEXT	SEGMENT
?Load3rdParty@@YAXXZ PROC NEAR				; Load3rdParty, COMDAT

; 341  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 342  : 	// Only do this if the server operator wants the support.
; 343  : 	// ( In case of malicious code, too )
; 344  : 	if ( CheckParm( "-usegh" ) )   

	push	0
	push	OFFSET FLAT:??_C@_06FBCH@?9usegh?$AA@	; `string'
	call	?CheckParm@@YAPADPBDPAPAD@Z		; CheckParm
	add	esp, 8
	test	eax, eax
	je	SHORT $L42627

; 346  : 		hDLLThirdParty = Sys_LoadLibrary( "ghostinj.dll" );

	push	OFFSET FLAT:??_C@_0N@IIIH@ghostinj?4dll?$AA@ ; `string'
	call	?Sys_LoadLibrary@@YAJPAD@Z		; Sys_LoadLibrary
	add	esp, 4
	mov	DWORD PTR _hDLLThirdParty, eax
$L42627:

; 348  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Load3rdParty@@YAXXZ ENDP				; Load3rdParty
_TEXT	ENDS
PUBLIC	?EF_VID_ForceUnlockedAndReturnState@@YAHXZ	; EF_VID_ForceUnlockedAndReturnState
;	COMDAT ?EF_VID_ForceUnlockedAndReturnState@@YAHXZ
_TEXT	SEGMENT
?EF_VID_ForceUnlockedAndReturnState@@YAHXZ PROC NEAR	; EF_VID_ForceUnlockedAndReturnState, COMDAT

; 358  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 359  : 	return 0;

	xor	eax, eax

; 360  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?EF_VID_ForceUnlockedAndReturnState@@YAHXZ ENDP		; EF_VID_ForceUnlockedAndReturnState
_TEXT	ENDS
PUBLIC	?EF_VID_ForceLockState@@YAXH@Z			; EF_VID_ForceLockState
;	COMDAT ?EF_VID_ForceLockState@@YAXH@Z
_TEXT	SEGMENT
?EF_VID_ForceLockState@@YAXH@Z PROC NEAR		; EF_VID_ForceLockState, COMDAT

; 370  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 371  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?EF_VID_ForceLockState@@YAXH@Z ENDP			; EF_VID_ForceLockState
_TEXT	ENDS
EXTRN	_strstr:NEAR
_BSS	SEGMENT
_?sz@?1??CheckParm@@YAPADPBDPAPAD@Z@4PADA DB 080H DUP (?)
_BSS	ENDS
;	COMDAT ?CheckParm@@YAPADPBDPAPAD@Z
_TEXT	SEGMENT
_psz$ = 8
_ppszValue$ = 12
_i$ = -4
_pret$ = -8
_p1$42646 = -12
_p2$42651 = -16
?CheckParm@@YAPADPBDPAPAD@Z PROC NEAR			; CheckParm, COMDAT

; 382  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 80					; 00000050H
	push	ebx
	push	esi
	push	edi

; 383  : 	int i;
; 384  : 	static char sz[128];
; 385  : 	char *pret;
; 386  : 
; 387  : 	if (!gpszCmdLine)

	cmp	DWORD PTR ?gpszCmdLine@@3PADA, 0	; gpszCmdLine
	jne	SHORT $L42644

; 388  : 		return NULL;

	xor	eax, eax
	jmp	$L42639
$L42644:

; 389  : 
; 390  : 	pret = strstr( gpszCmdLine, psz );

	mov	eax, DWORD PTR _psz$[ebp]
	push	eax
	mov	ecx, DWORD PTR ?gpszCmdLine@@3PADA	; gpszCmdLine
	push	ecx
	call	_strstr
	add	esp, 8
	mov	DWORD PTR _pret$[ebp], eax

; 391  : 
; 392  : 	// should we return a pointer to the value?
; 393  : 	if (pret && ppszValue)

	cmp	DWORD PTR _pret$[ebp], 0
	je	$L42650
	cmp	DWORD PTR _ppszValue$[ebp], 0
	je	$L42650

; 395  : 		char *p1 = pret;

	mov	edx, DWORD PTR _pret$[ebp]
	mov	DWORD PTR _p1$42646[ebp], edx

; 396  : 		*ppszValue = NULL;

	mov	eax, DWORD PTR _ppszValue$[ebp]
	mov	DWORD PTR [eax], 0
$L42648:

; 397  : 
; 398  : 		while ( *p1 && (*p1 != 32))

	mov	ecx, DWORD PTR _p1$42646[ebp]
	movsx	edx, BYTE PTR [ecx]
	test	edx, edx
	je	SHORT $L42649
	mov	eax, DWORD PTR _p1$42646[ebp]
	movsx	ecx, BYTE PTR [eax]
	cmp	ecx, 32					; 00000020H
	je	SHORT $L42649

; 399  : 			p1++;

	mov	edx, DWORD PTR _p1$42646[ebp]
	add	edx, 1
	mov	DWORD PTR _p1$42646[ebp], edx
	jmp	SHORT $L42648
$L42649:

; 400  : 
; 401  : 		if (p1 != 0)

	cmp	DWORD PTR _p1$42646[ebp], 0
	je	SHORT $L42650

; 403  : 			char *p2 = ++p1;

	mov	eax, DWORD PTR _p1$42646[ebp]
	add	eax, 1
	mov	DWORD PTR _p1$42646[ebp], eax
	mov	ecx, DWORD PTR _p1$42646[ebp]
	mov	DWORD PTR _p2$42651[ebp], ecx

; 404  : 
; 405  : 			for ( i = 0; i < 128; i++ )

	mov	DWORD PTR _i$[ebp], 0
	jmp	SHORT $L42652
$L42653:
	mov	edx, DWORD PTR _i$[ebp]
	add	edx, 1
	mov	DWORD PTR _i$[ebp], edx
$L42652:
	cmp	DWORD PTR _i$[ebp], 128			; 00000080H
	jge	SHORT $L42654

; 407  : 				if ( !*p2 || (*p2 == 32))

	mov	eax, DWORD PTR _p2$42651[ebp]
	movsx	ecx, BYTE PTR [eax]
	test	ecx, ecx
	je	SHORT $L42656
	mov	edx, DWORD PTR _p2$42651[ebp]
	movsx	eax, BYTE PTR [edx]
	cmp	eax, 32					; 00000020H
	jne	SHORT $L42655
$L42656:

; 408  : 					break;

	jmp	SHORT $L42654
$L42655:

; 409  : 				sz[i] = *p2++;

	mov	ecx, DWORD PTR _i$[ebp]
	mov	edx, DWORD PTR _p2$42651[ebp]
	mov	al, BYTE PTR [edx]
	mov	BYTE PTR _?sz@?1??CheckParm@@YAPADPBDPAPAD@Z@4PADA[ecx], al
	mov	ecx, DWORD PTR _p2$42651[ebp]
	add	ecx, 1
	mov	DWORD PTR _p2$42651[ebp], ecx

; 410  : 			}

	jmp	SHORT $L42653
$L42654:

; 411  : 
; 412  : 			sz[i] = 0;

	mov	edx, DWORD PTR _i$[ebp]
	mov	BYTE PTR _?sz@?1??CheckParm@@YAPADPBDPAPAD@Z@4PADA[edx], 0

; 413  : 			*ppszValue = &sz[0];		

	mov	eax, DWORD PTR _ppszValue$[ebp]
	mov	DWORD PTR [eax], OFFSET FLAT:_?sz@?1??CheckParm@@YAPADPBDPAPAD@Z@4PADA
$L42650:

; 416  : 
; 417  : 	return pret;

	mov	eax, DWORD PTR _pret$[ebp]
$L42639:

; 418  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?CheckParm@@YAPADPBDPAPAD@Z ENDP			; CheckParm
_TEXT	ENDS
PUBLIC	?InitInstance@@YAHXZ				; InitInstance
EXTRN	?Eng_SetState@@YAXH@Z:NEAR			; Eng_SetState
EXTRN	?PROC_GetSpeed@@YAHXZ:NEAR			; PROC_GetSpeed
EXTRN	?PROC_IsMMX@@YAHXZ:NEAR				; PROC_IsMMX
;	COMDAT ?InitInstance@@YAHXZ
_TEXT	SEGMENT
?InitInstance@@YAHXZ PROC NEAR				; InitInstance, COMDAT

; 427  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 428  : 	Load3rdParty();

	call	?Load3rdParty@@YAXXZ			; Load3rdParty

; 429  : 
; 430  : 	Eng_SetState( DLL_INACTIVE );

	push	0
	call	?Eng_SetState@@YAXH@Z			; Eng_SetState
	add	esp, 4

; 431  : 
; 432  : 	memset( &ef, 0, sizeof( ef ) );

	push	132					; 00000084H
	push	0
	push	OFFSET FLAT:?ef@@3Uexefuncs_s@@A	; ef
	call	_memset
	add	esp, 12					; 0000000cH

; 433  : 	
; 434  : 	// Function pointers used by dedicated server
; 435  : 	ef.Sys_Printf							= Sys_Printf;

	mov	DWORD PTR ?ef@@3Uexefuncs_s@@A+84, OFFSET FLAT:?Sys_Printf@@YAXPADZZ ; Sys_Printf

; 436  : 	ef.ErrorMessage							= Sys_ErrorMessage;

	mov	DWORD PTR ?ef@@3Uexefuncs_s@@A+76, OFFSET FLAT:?Sys_ErrorMessage@@YAXHPBD@Z ; Sys_ErrorMessage

; 437  : 
; 438  : 	ef.VID_ForceLockState					= EF_VID_ForceLockState;

	mov	DWORD PTR ?ef@@3Uexefuncs_s@@A+24, OFFSET FLAT:?EF_VID_ForceLockState@@YAXH@Z ; EF_VID_ForceLockState

; 439  : 	ef.VID_ForceUnlockedAndReturnState		= EF_VID_ForceUnlockedAndReturnState;

	mov	DWORD PTR ?ef@@3Uexefuncs_s@@A+28, OFFSET FLAT:?EF_VID_ForceUnlockedAndReturnState@@YAHXZ ; EF_VID_ForceUnlockedAndReturnState

; 440  : 
; 441  : #ifdef _WIN32
; 442  : 	// Data
; 443  : 	ef.fMMX									= PROC_IsMMX();

	call	?PROC_IsMMX@@YAHXZ			; PROC_IsMMX
	mov	DWORD PTR ?ef@@3Uexefuncs_s@@A, eax

; 444  : 	ef.iCPUMhz								= PROC_GetSpeed();	// in MHz

	call	?PROC_GetSpeed@@YAHXZ			; PROC_GetSpeed
	mov	DWORD PTR ?ef@@3Uexefuncs_s@@A+4, eax

; 445  : #endif
; 446  : 
; 447  : 	return 1;

	mov	eax, 1

; 448  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?InitInstance@@YAHXZ ENDP				; InitInstance
_TEXT	ENDS
PUBLIC	?Sys_ConsoleInput@@YAPADXZ			; Sys_ConsoleInput
PUBLIC	??_C@_02PIMC@?$AN?6?$AA@			; `string'
PUBLIC	??_C@_03BBFP@?$AI?5?$AI?$AA@			; `string'
EXTRN	_exit:NEAR
EXTRN	__imp__ReadConsoleInputA@16:NEAR
EXTRN	__imp__GetNumberOfConsoleInputEvents@8:NEAR
EXTRN	__chkstk:NEAR
_BSS	SEGMENT
_hinput	DD	01H DUP (?)
_BSS	ENDS
;	COMDAT ??_C@_02PIMC@?$AN?6?$AA@
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
??_C@_02PIMC@?$AN?6?$AA@ DB 0dH, 0aH, 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_03BBFP@?$AI?5?$AI?$AA@
CONST	SEGMENT
??_C@_03BBFP@?$AI?5?$AI?$AA@ DB 08H, ' ', 08H, 00H	; `string'
CONST	ENDS
;	COMDAT ?Sys_ConsoleInput@@YAPADXZ
_TEXT	SEGMENT
_recs$ = -20480
_dummy$ = -20484
_ch$ = -20488
_numread$ = -20492
_numevents$ = -20496
?Sys_ConsoleInput@@YAPADXZ PROC NEAR			; Sys_ConsoleInput, COMDAT

; 458  : {

	push	ebp
	mov	ebp, esp
	mov	eax, 20564				; 00005054H
	call	__chkstk
	push	ebx
	push	esi
	push	edi
$L42669:

; 459  : 	INPUT_RECORD	recs[1024];
; 460  : 	unsigned long	dummy;
; 461  : 	int				ch;
; 462  : 	unsigned long	numread, numevents;
; 463  : 
; 464  : 	while ( 1 )

	mov	eax, 1
	test	eax, eax
	je	$L42670

; 466  : 		if (!GetNumberOfConsoleInputEvents (hinput, &numevents))

	lea	ecx, DWORD PTR _numevents$[ebp]
	push	ecx
	mov	edx, DWORD PTR _hinput
	push	edx
	call	DWORD PTR __imp__GetNumberOfConsoleInputEvents@8
	test	eax, eax
	jne	SHORT $L42671

; 468  : 			exit( -1 );

	push	-1
	call	_exit
$L42671:

; 470  : 
; 471  : 		if (numevents <= 0)

	cmp	DWORD PTR _numevents$[ebp], 0
	ja	SHORT $L42672

; 472  : 			break;

	jmp	$L42670
$L42672:

; 473  : 
; 474  : 		if ( !ReadConsoleInput(hinput, recs, 1, &numread) )

	lea	eax, DWORD PTR _numread$[ebp]
	push	eax
	push	1
	lea	ecx, DWORD PTR _recs$[ebp]
	push	ecx
	mov	edx, DWORD PTR _hinput
	push	edx
	call	DWORD PTR __imp__ReadConsoleInputA@16
	test	eax, eax
	jne	SHORT $L42673

; 476  : 			exit( -1 );

	push	-1
	call	_exit
$L42673:

; 478  : 
; 479  : 		if (numread != 1)

	cmp	DWORD PTR _numread$[ebp], 1
	je	SHORT $L42674

; 481  : 			exit( -1 );

	push	-1
	call	_exit
$L42674:

; 483  : 
; 484  : 		if ( recs[0].EventType == KEY_EVENT )

	mov	eax, DWORD PTR _recs$[ebp]
	and	eax, 65535				; 0000ffffH
	cmp	eax, 1
	jne	$L42678

; 486  : 			if ( !recs[0].Event.KeyEvent.bKeyDown )

	cmp	DWORD PTR _recs$[ebp+4], 0
	jne	$L42678

; 488  : 				ch = recs[0].Event.KeyEvent.uChar.AsciiChar;

	movsx	ecx, BYTE PTR _recs$[ebp+14]
	mov	DWORD PTR _ch$[ebp], ecx

; 490  : 				{

	mov	edx, DWORD PTR _ch$[ebp]
	mov	DWORD PTR -20500+[ebp], edx
	cmp	DWORD PTR -20500+[ebp], 8
	je	SHORT $L42684
	cmp	DWORD PTR -20500+[ebp], 13		; 0000000dH
	je	SHORT $L42681
	jmp	$L42687
$L42681:

; 491  : 					case '\r':
; 492  : 						WriteFile(houtput, "\r\n", 2, &dummy, NULL);	

	push	0
	lea	eax, DWORD PTR _dummy$[ebp]
	push	eax
	push	2
	push	OFFSET FLAT:??_C@_02PIMC@?$AN?6?$AA@	; `string'
	mov	ecx, DWORD PTR _houtput
	push	ecx
	call	DWORD PTR __imp__WriteFile@20

; 493  : 						if (console_textlen)

	cmp	DWORD PTR _console_textlen, 0
	je	SHORT $L42683

; 495  : 							console_text[console_textlen] = 0;

	mov	edx, DWORD PTR _console_textlen
	mov	BYTE PTR _console_text[edx], 0

; 496  : 							console_textlen = 0;

	mov	DWORD PTR _console_textlen, 0

; 497  : 							return console_text;

	mov	eax, OFFSET FLAT:_console_text
	jmp	$L42662
$L42683:

; 499  : 						break;

	jmp	$L42678
$L42684:

; 500  : 
; 501  : 					case '\b':
; 502  : 						if (console_textlen)

	cmp	DWORD PTR _console_textlen, 0
	je	SHORT $L42685

; 504  : 							console_textlen--;

	mov	eax, DWORD PTR _console_textlen
	sub	eax, 1
	mov	DWORD PTR _console_textlen, eax

; 505  : 							WriteFile(houtput, "\b \b", 3, &dummy, NULL);	

	push	0
	lea	ecx, DWORD PTR _dummy$[ebp]
	push	ecx
	push	3
	push	OFFSET FLAT:??_C@_03BBFP@?$AI?5?$AI?$AA@ ; `string'
	mov	edx, DWORD PTR _houtput
	push	edx
	call	DWORD PTR __imp__WriteFile@20
$L42685:

; 507  : 						break;

	jmp	SHORT $L42678
$L42687:

; 508  : 
; 509  : 					default:
; 510  : 						if (ch >= ' ')

	cmp	DWORD PTR _ch$[ebp], 32			; 00000020H
	jl	SHORT $L42689

; 512  : 							if (console_textlen < sizeof(console_text)-2)

	cmp	DWORD PTR _console_textlen, 254		; 000000feH
	jae	SHORT $L42689

; 514  : 								WriteFile(houtput, &ch, 1, &dummy, NULL);	

	push	0
	lea	eax, DWORD PTR _dummy$[ebp]
	push	eax
	push	1
	lea	ecx, DWORD PTR _ch$[ebp]
	push	ecx
	mov	edx, DWORD PTR _houtput
	push	edx
	call	DWORD PTR __imp__WriteFile@20

; 515  : 								console_text[console_textlen] = ch;

	mov	eax, DWORD PTR _console_textlen
	mov	cl, BYTE PTR _ch$[ebp]
	mov	BYTE PTR _console_text[eax], cl

; 516  : 								console_textlen++;

	mov	edx, DWORD PTR _console_textlen
	add	edx, 1
	mov	DWORD PTR _console_textlen, edx
$L42689:
$L42678:

; 525  : 	}

	jmp	$L42669
$L42670:

; 526  : 
; 527  : 	return NULL;

	xor	eax, eax
$L42662:

; 528  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Sys_ConsoleInput@@YAPADXZ ENDP				; Sys_ConsoleInput
_TEXT	ENDS
PUBLIC	??_C@_02DILL@?$CFs?$AA@				; `string'
EXTRN	__imp__WriteConsoleOutputCharacterA@20:NEAR
EXTRN	__imp__WriteConsoleOutputAttribute@20:NEAR
;	COMDAT ??_C@_02DILL@?$CFs?$AA@
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
??_C@_02DILL@?$CFs?$AA@ DB '%s', 00H			; `string'
CONST	ENDS
;	COMDAT ?WriteStatusText@@YAXPAD@Z
_TEXT	SEGMENT
_szText$ = 8
_szFullLine$ = -84
_coord$ = -88
_dwWritten$ = -92
_wAttrib$ = -252
_i$ = -256
?WriteStatusText@@YAXPAD@Z PROC NEAR			; WriteStatusText, COMDAT

; 585  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 320				; 00000140H
	push	ebx
	push	esi
	push	edi

; 586  : 	char szFullLine[81];
; 587  : 	COORD coord;
; 588  : 	DWORD dwWritten = 0;

	mov	DWORD PTR _dwWritten$[ebp], 0

; 589  : 	WORD wAttrib[80];
; 590  : 	
; 591  : 	int i;
; 592  : 	
; 593  : 	for ( i = 0; i < 80; i++ )

	mov	DWORD PTR _i$[ebp], 0
	jmp	SHORT $L42698
$L42699:
	mov	eax, DWORD PTR _i$[ebp]
	add	eax, 1
	mov	DWORD PTR _i$[ebp], eax
$L42698:
	cmp	DWORD PTR _i$[ebp], 80			; 00000050H
	jge	SHORT $L42700

; 595  : 		wAttrib[i] = FOREGROUND_RED | FOREGROUND_INTENSITY;

	mov	ecx, DWORD PTR _i$[ebp]
	mov	WORD PTR _wAttrib$[ebp+ecx*2], 12	; 0000000cH

; 596  : 	}

	jmp	SHORT $L42699
$L42700:

; 597  : 
; 598  : 	memset( szFullLine, 0, sizeof(szFullLine) );

	push	81					; 00000051H
	push	0
	lea	edx, DWORD PTR _szFullLine$[ebp]
	push	edx
	call	_memset
	add	esp, 12					; 0000000cH

; 599  : 	snprintf( szFullLine, sizeof( szFullLine ), "%s", szText );

	mov	eax, DWORD PTR _szText$[ebp]
	push	eax
	push	OFFSET FLAT:??_C@_02DILL@?$CFs?$AA@	; `string'
	push	81					; 00000051H
	lea	ecx, DWORD PTR _szFullLine$[ebp]
	push	ecx
	call	__snprintf
	add	esp, 16					; 00000010H

; 600  : 
; 601  : 	coord.X = 0;

	mov	WORD PTR _coord$[ebp], 0

; 602  : 	coord.Y = 0;

	mov	WORD PTR _coord$[ebp+2], 0

; 603  : 
; 604  : 	WriteConsoleOutputAttribute( houtput, wAttrib, 80, coord, &dwWritten );

	lea	edx, DWORD PTR _dwWritten$[ebp]
	push	edx
	mov	eax, DWORD PTR _coord$[ebp]
	push	eax
	push	80					; 00000050H
	lea	ecx, DWORD PTR _wAttrib$[ebp]
	push	ecx
	mov	edx, DWORD PTR _houtput
	push	edx
	call	DWORD PTR __imp__WriteConsoleOutputAttribute@20

; 605  : 	WriteConsoleOutputCharacter( houtput, szFullLine, 80, coord, &dwWritten );	

	lea	eax, DWORD PTR _dwWritten$[ebp]
	push	eax
	mov	ecx, DWORD PTR _coord$[ebp]
	push	ecx
	push	80					; 00000050H
	lea	edx, DWORD PTR _szFullLine$[ebp]
	push	edx
	mov	eax, DWORD PTR _houtput
	push	eax
	call	DWORD PTR __imp__WriteConsoleOutputCharacterA@20

; 606  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?WriteStatusText@@YAXPAD@Z ENDP				; WriteStatusText
_TEXT	ENDS
PUBLIC	?CreateConsoleWindow@@YAHXZ			; CreateConsoleWindow
EXTRN	?InitConProc@@YAXXZ:NEAR			; InitConProc
EXTRN	__imp__GetStdHandle@4:NEAR
EXTRN	__imp__AllocConsole@0:NEAR
;	COMDAT ?CreateConsoleWindow@@YAHXZ
_TEXT	SEGMENT
?CreateConsoleWindow@@YAHXZ PROC NEAR			; CreateConsoleWindow, COMDAT

; 617  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 618  : #ifdef _WIN32
; 619  : 	if ( !AllocConsole () )

	call	DWORD PTR __imp__AllocConsole@0
	test	eax, eax
	jne	SHORT $L42705

; 621  : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42704
$L42705:

; 623  : 
; 624  : 	hinput	= GetStdHandle (STD_INPUT_HANDLE);

	push	-10					; fffffff6H
	call	DWORD PTR __imp__GetStdHandle@4
	mov	DWORD PTR _hinput, eax

; 625  : 	houtput = GetStdHandle (STD_OUTPUT_HANDLE);

	push	-11					; fffffff5H
	call	DWORD PTR __imp__GetStdHandle@4
	mov	DWORD PTR _houtput, eax

; 626  : 	
; 627  : 	InitConProc();

	call	?InitConProc@@YAXXZ			; InitConProc

; 628  : #endif
; 629  : 
; 630  : 	return 1;

	mov	eax, 1
$L42704:

; 631  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?CreateConsoleWindow@@YAHXZ ENDP			; CreateConsoleWindow
_TEXT	ENDS
PUBLIC	?DestroyConsoleWindow@@YAXXZ			; DestroyConsoleWindow
EXTRN	?DeinitConProc@@YAXXZ:NEAR			; DeinitConProc
EXTRN	__imp__FreeConsole@0:NEAR
;	COMDAT ?DestroyConsoleWindow@@YAXXZ
_TEXT	SEGMENT
?DestroyConsoleWindow@@YAXXZ PROC NEAR			; DestroyConsoleWindow, COMDAT

; 640  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 641  : #ifdef _WIN32
; 642  : 	FreeConsole ();

	call	DWORD PTR __imp__FreeConsole@0

; 643  : 
; 644  : 	// shut down QHOST hooks if necessary
; 645  : 	DeinitConProc ();

	call	?DeinitConProc@@YAXXZ			; DeinitConProc

; 646  : #endif
; 647  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?DestroyConsoleWindow@@YAXXZ ENDP			; DestroyConsoleWindow
_TEXT	ENDS
PUBLIC	?ProcessConsoleInput@@YAXXZ			; ProcessConsoleInput
PUBLIC	??_C@_03HHKO@?$CFs?6?$AA@			; `string'
;	COMDAT ??_C@_03HHKO@?$CFs?6?$AA@
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
??_C@_03HHKO@?$CFs?6?$AA@ DB '%s', 0aH, 00H		; `string'
CONST	ENDS
;	COMDAT ?ProcessConsoleInput@@YAXXZ
_TEXT	SEGMENT
_s$ = -4
_szBuf$42720 = -260
?ProcessConsoleInput@@YAXXZ PROC NEAR			; ProcessConsoleInput, COMDAT

; 656  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 324				; 00000144H
	push	ebx
	push	esi
	push	edi

; 657  : 	char *s;
; 658  : 
; 659  : 	if ( !engineapi.Cbuf_AddText )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+16, 0
	jne	SHORT $L42715

; 660  : 		return;

	jmp	SHORT $L42713
$L42715:

; 664  : 		s = Sys_ConsoleInput ();

	call	?Sys_ConsoleInput@@YAPADXZ		; Sys_ConsoleInput
	mov	DWORD PTR _s$[ebp], eax

; 665  : 		if (s)

	cmp	DWORD PTR _s$[ebp], 0
	je	SHORT $L42719

; 667  : 			char szBuf[ 256 ];
; 668  : 			snprintf( szBuf, sizeof(szBuf), "%s\n", s );

	mov	eax, DWORD PTR _s$[ebp]
	push	eax
	push	OFFSET FLAT:??_C@_03HHKO@?$CFs?6?$AA@	; `string'
	push	256					; 00000100H
	lea	ecx, DWORD PTR _szBuf$42720[ebp]
	push	ecx
	call	__snprintf
	add	esp, 16					; 00000010H

; 669  : 			engineapi.Cbuf_AddText ( szBuf );

	lea	edx, DWORD PTR _szBuf$42720[ebp]
	push	edx
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+16
	add	esp, 4
$L42719:

; 671  : 	} while (s);

	cmp	DWORD PTR _s$[ebp], 0
	jne	SHORT $L42715
$L42713:

; 672  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?ProcessConsoleInput@@YAXXZ ENDP			; ProcessConsoleInput
_TEXT	ENDS
PUBLIC	?GameInit@@YAHXZ				; GameInit
PUBLIC	??_C@_09MBEO@?9heapsize?$AA@			; `string'
EXTRN	?Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z:NEAR	; Eng_Load
EXTRN	_atoi:NEAR
EXTRN	__imp__GetVersionExA@4:NEAR
EXTRN	__imp__GlobalAlloc@8:NEAR
;	COMDAT ??_C@_09MBEO@?9heapsize?$AA@
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
??_C@_09MBEO@?9heapsize?$AA@ DB '-heapsize', 00H	; `string'
CONST	ENDS
;	COMDAT ?GameInit@@YAHXZ
_TEXT	SEGMENT
_p$ = -4
_vinfo$ = -152
?GameInit@@YAHXZ PROC NEAR				; GameInit, COMDAT

; 680  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 216				; 000000d8H
	push	ebx
	push	esi
	push	edi

; 681  : 	char *p;
; 682  : 
; 683  : 	// Command line override
; 684  : 	if ( (CheckParm ("-heapsize", &p ) ) && p )

	lea	eax, DWORD PTR _p$[ebp]
	push	eax
	push	OFFSET FLAT:??_C@_09MBEO@?9heapsize?$AA@ ; `string'
	call	?CheckParm@@YAPADPBDPAPAD@Z		; CheckParm
	add	esp, 8
	test	eax, eax
	je	SHORT $L42726
	cmp	DWORD PTR _p$[ebp], 0
	je	SHORT $L42726

; 686  : 		giMemSize = atoi( p ) * 1024;

	mov	ecx, DWORD PTR _p$[ebp]
	push	ecx
	call	_atoi
	add	esp, 4
	shl	eax, 10					; 0000000aH
	mov	DWORD PTR _giMemSize, eax
$L42726:

; 688  : 
; 689  : 	Sys_Sleep=Sys_Sleep_Old;

	mov	DWORD PTR ?Sys_Sleep@@3P6AXH@ZA, OFFSET FLAT:?Sys_Sleep_Old@@YAXH@Z ; Sys_Sleep, Sys_Sleep_Old

; 690  : 
; 691  : #if !defined ( _WIN32 )
; 692  : 	char *pPingType;
; 693  : 	int type;
; 694  : 	if ( (CheckParm ("-pingboost", &pPingType)) && pPingType )
; 695  : 	{
; 696  : 		type=atoi( pPingType );
; 697  : 		switch( type ) {
; 698  : 			case 1:
; 699  : 				//printf("Using timer method\n");
; 700  : 				signal(SIGALRM,alarmFunc);
; 701  : 				Sys_Sleep=Sys_Sleep_Timer;
; 702  : 				break;
; 703  : 			case 2:
; 704  : 				Sys_Sleep = Sys_Sleep_Select;
; 705  : 				break;
; 706  : 			case 3:
; 707  : 				Sys_Sleep = Sys_Sleep_Net;
; 708  : 				// we Sys_GetProcAddress NET_Sleep() from 
; 709  : 				//engine_i386.so later in this function
; 710  : 				break;
; 711  : 			default: // just in case
; 712  : 				Sys_Sleep=Sys_Sleep_Old;
; 713  : 				break;
; 714  : 		}
; 715  : 
; 716  : 	}
; 717  : #endif
; 718  : 	
; 719  : 
; 720  : 	// Try and allocated it
; 721  : #ifdef _WIN32
; 722  : 	gpMemBase = (unsigned char *)::GlobalAlloc( GMEM_FIXED, giMemSize );

	mov	edx, DWORD PTR _giMemSize
	push	edx
	push	0
	call	DWORD PTR __imp__GlobalAlloc@8
	mov	DWORD PTR _gpMemBase, eax

; 723  : #else
; 724  : 	gpMemBase = (unsigned char *)malloc( giMemSize );
; 725  : #endif
; 726  : 	if (!gpMemBase)

	cmp	DWORD PTR _gpMemBase, 0
	jne	SHORT $L42729

; 728  : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42724
$L42729:

; 734  : 	vinfo.dwOSVersionInfoSize = sizeof(vinfo);

	mov	DWORD PTR _vinfo$[ebp], 148		; 00000094H

; 735  : 
; 736  : 	if ( !GetVersionEx ( &vinfo ) )

	lea	eax, DWORD PTR _vinfo$[ebp]
	push	eax
	call	DWORD PTR __imp__GetVersionExA@4
	test	eax, eax
	jne	SHORT $L42731

; 738  : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42724
$L42731:

; 740  : 
; 741  : 	if ( vinfo.dwPlatformId == VER_PLATFORM_WIN32s )

	cmp	DWORD PTR _vinfo$[ebp+16], 0
	jne	SHORT $L42732

; 743  : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42724
$L42732:

; 745  : 	
; 746  : #endif
; 747  : 
; 748  : 	if ( !Eng_Load( gpszCmdLine, &ef, giMemSize, gpMemBase, g_pszengine, DLL_NORMAL ) )

	push	0
	mov	ecx, DWORD PTR _g_pszengine
	push	ecx
	mov	edx, DWORD PTR _gpMemBase
	push	edx
	mov	eax, DWORD PTR _giMemSize
	push	eax
	push	OFFSET FLAT:?ef@@3Uexefuncs_s@@A	; ef
	mov	ecx, DWORD PTR ?gpszCmdLine@@3PADA	; gpszCmdLine
	push	ecx
	call	?Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z ; Eng_Load
	add	esp, 24					; 00000018H
	test	eax, eax
	jne	SHORT $L42733

; 750  : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42724
$L42733:

; 752  : 
; 753  : #if !defined ( _WIN32 )
; 754  : 	if ( type == 3 ) 
; 755  : 	{
; 756  : 		NET_Sleep=(NET_Sleep_t)Sys_GetProcAddress(ghMod,"NET_Sleep_Timeout");
; 757  : 		//printf("Net_Sleep:%p\n",NET_Sleep);
; 758  : 	}
; 759  : #endif
; 760  : 
; 761  : 	Eng_SetState( DLL_ACTIVE );

	push	1
	call	?Eng_SetState@@YAXH@Z			; Eng_SetState
	add	esp, 4

; 762  : 
; 763  : 	return 1;

	mov	eax, 1
$L42724:

; 764  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?GameInit@@YAHXZ ENDP					; GameInit
_TEXT	ENDS
PUBLIC	?GameShutdown@@YAXXZ				; GameShutdown
EXTRN	__imp__GlobalFree@4:NEAR
EXTRN	?Eng_Unload@@YAXXZ:NEAR				; Eng_Unload
;	COMDAT ?GameShutdown@@YAXXZ
_TEXT	SEGMENT
?GameShutdown@@YAXXZ PROC NEAR				; GameShutdown, COMDAT

; 773  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 774  : 	Eng_Unload();

	call	?Eng_Unload@@YAXXZ			; Eng_Unload

; 775  : 
; 776  : 	if ( gpMemBase )

	cmp	DWORD PTR _gpMemBase, 0
	je	SHORT $L42737

; 778  : #ifdef _WIN32
; 779  : 		::GlobalFree( gpMemBase );

	mov	eax, DWORD PTR _gpMemBase
	push	eax
	call	DWORD PTR __imp__GlobalFree@4

; 780  : #else
; 781  : 		free( gpMemBase );
; 782  : #endif
; 783  : 		gpMemBase = NULL;

	mov	DWORD PTR _gpMemBase, 0
$L42737:

; 785  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?GameShutdown@@YAXXZ ENDP				; GameShutdown
_TEXT	ENDS
PUBLIC	?CtrlHandler@@YAHK@Z				; CtrlHandler
;	COMDAT ?CtrlHandler@@YAHK@Z
_TEXT	SEGMENT
_fdwCtrlType$ = 8
?CtrlHandler@@YAHK@Z PROC NEAR				; CtrlHandler, COMDAT

; 791  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 68					; 00000044H
	push	ebx
	push	esi
	push	edi

; 793  : 	{

	mov	eax, DWORD PTR _fdwCtrlType$[ebp]
	mov	DWORD PTR -4+[ebp], eax
	cmp	DWORD PTR -4+[ebp], 6
	ja	SHORT $L42747
	mov	edx, DWORD PTR -4+[ebp]
	xor	ecx, ecx
	mov	cl, BYTE PTR $L42904[edx]
	jmp	DWORD PTR $L42905[ecx*4]
$L42746:

; 794  : 		case CTRL_C_EVENT:
; 795  : 		case CTRL_CLOSE_EVENT:
; 796  : 		case CTRL_BREAK_EVENT:
; 797  : 		case CTRL_LOGOFF_EVENT:
; 798  : 		case CTRL_SHUTDOWN_EVENT:
; 799  : 			gbAppHasBeenTerminated = TRUE;

	mov	DWORD PTR ?gbAppHasBeenTerminated@@3HA, 1 ; gbAppHasBeenTerminated

; 800  : 			return TRUE;

	mov	eax, 1
	jmp	SHORT $L42743
$L42747:

; 801  : 
; 802  : 		default:
; 803  : 			return FALSE;

	xor	eax, eax
$L42743:

; 805  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
$L42905:
	DD	$L42746
	DD	$L42747
$L42904:
	DB	0
	DB	0
	DB	0
	DB	1
	DB	1
	DB	0
	DB	0
?CtrlHandler@@YAHK@Z ENDP				; CtrlHandler
_TEXT	ENDS
PUBLIC	__real@8@3ffaccccccccccccd000
PUBLIC	__real@8@3ff583126e978d4fe000
PUBLIC	_WinMain@16
EXTRN	?Eng_Frame@@YAHHN@Z:NEAR			; Eng_Frame
EXTRN	__imp__GetCommandLineA@0:NEAR
EXTRN	_free:NEAR
EXTRN	__imp__GetMessageA@16:NEAR
EXTRN	__imp__TranslateMessage@4:NEAR
EXTRN	__imp__DispatchMessageA@4:NEAR
EXTRN	__imp__PeekMessageA@20:NEAR
EXTRN	_strdup:NEAR
;	COMDAT __real@8@3ffaccccccccccccd000
; File d:\mod\single-player source\dedicated\sys_ded.cpp
CONST	SEGMENT
__real@8@3ffaccccccccccccd000 DQ 03fa999999999999ar ; 0.05
CONST	ENDS
;	COMDAT __real@8@3ff583126e978d4fe000
CONST	SEGMENT
__real@8@3ff583126e978d4fe000 DQ 03f50624dd2f1a9fcr ; 0.001
CONST	ENDS
;	COMDAT _WinMain@16
_TEXT	SEGMENT
_iret$ = -4
_bDone$42766 = -8
_msg$42769 = -36
_newtime$42770 = -44
_dtime$42771 = -52
_WinMain@16 PROC NEAR					; COMDAT

; 815  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 116				; 00000074H
	push	ebx
	push	esi
	push	edi

; 816  : 	int		iret = -1;

	mov	DWORD PTR _iret$[ebp], -1

; 817  : 
; 818  : 	// Store off command line for argument searching
; 819  : 	gpszCmdLine = strdup( GetCommandLine() );

	call	DWORD PTR __imp__GetCommandLineA@0
	push	eax
	call	_strdup
	add	esp, 4
	mov	DWORD PTR ?gpszCmdLine@@3PADA, eax	; gpszCmdLine

; 820  : 
; 821  : 	Sys_Sleep=Sys_Sleep_Old; // win32 doesn't have pingbooster options :)

	mov	DWORD PTR ?Sys_Sleep@@3P6AXH@ZA, OFFSET FLAT:?Sys_Sleep_Old@@YAXH@Z ; Sys_Sleep, Sys_Sleep_Old

; 822  : 
; 823  : 	if ( !InitInstance() )

	call	?InitInstance@@YAHXZ			; InitInstance
	test	eax, eax
	jne	SHORT $L42755

; 825  : 		goto cleanup;

	jmp	$L42757
$L42755:

; 827  : 
; 828  : 	if ( !CreateConsoleWindow() )

	call	?CreateConsoleWindow@@YAHXZ		; CreateConsoleWindow
	test	eax, eax
	jne	SHORT $L42758

; 830  : 		goto cleanup;

	jmp	$L42759
$L42758:

; 832  : 
; 833  : 	if ( !GameInit() )

	call	?GameInit@@YAHXZ			; GameInit
	test	eax, eax
	jne	SHORT $L42760

; 835  : 		goto cleanup;

	jmp	$L42761
$L42760:

; 837  : 
; 838  : 	if ( engineapi.SetStartupMode )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+176, 0
	je	SHORT $L42762

; 840  : 		engineapi.SetStartupMode( 1 );

	push	1
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+176
	add	esp, 4
$L42762:

; 842  : 
; 843  : 	while ( 1 )

	mov	eax, 1
	test	eax, eax
	je	$L42765

; 845  : 		int bDone = 0;

	mov	DWORD PTR _bDone$42766[ebp], 0

; 846  : 
; 847  : 		static double oldtime = 0.0;
; 848  : 
; 849  : 		MSG msg;
; 850  : 		double newtime;
; 851  : 		double dtime;
; 852  : 
; 853  : 		if ( gbAppHasBeenTerminated )

	cmp	DWORD PTR ?gbAppHasBeenTerminated@@3HA, 0 ; gbAppHasBeenTerminated
	je	SHORT $L42772

; 854  : 			break;

	jmp	$L42765
$L42772:

; 855  : 
; 856  : 		// Try to allow other apps to get some CPU
; 857  : 		Sys_Sleep( 1 );

	push	1
	call	DWORD PTR ?Sys_Sleep@@3P6AXH@ZA		; Sys_Sleep
	add	esp, 4

; 858  : 
; 859  : 		if ( !engineapi.Sys_FloatTime )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+196, 0
	jne	SHORT $L42773

; 860  : 			break;

	jmp	$L42765
$L42773:

; 861  : 
; 862  : 		while ( 1 )

	mov	ecx, 1
	test	ecx, ecx
	je	SHORT $L42776

; 864  : 			newtime = engineapi.Sys_FloatTime();

	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+196
	fstp	QWORD PTR _newtime$42770[ebp]

; 865  : 			if ( newtime < oldtime )

	fld	QWORD PTR _newtime$42770[ebp]
	fcomp	QWORD PTR _?oldtime@?BA@??WinMain@@9@16@4NA
	fnstsw	ax
	test	ah, 1
	je	SHORT $L42777

; 867  : 				oldtime = newtime - 0.05;

	fld	QWORD PTR _newtime$42770[ebp]
	fsub	QWORD PTR __real@8@3ffaccccccccccccd000
	fstp	QWORD PTR _?oldtime@?BA@??WinMain@@9@16@4NA
$L42777:

; 869  : 			
; 870  : 			dtime = newtime - oldtime;

	fld	QWORD PTR _newtime$42770[ebp]
	fsub	QWORD PTR _?oldtime@?BA@??WinMain@@9@16@4NA
	fstp	QWORD PTR _dtime$42771[ebp]

; 871  : 
; 872  : 			if ( gbAppHasBeenTerminated )

	cmp	DWORD PTR ?gbAppHasBeenTerminated@@3HA, 0 ; gbAppHasBeenTerminated
	je	SHORT $L42778

; 873  : 				break;

	jmp	SHORT $L42776
$L42778:

; 874  : 
; 875  : 			if ( dtime > 0.001 )

	fld	QWORD PTR _dtime$42771[ebp]
	fcomp	QWORD PTR __real@8@3ff583126e978d4fe000
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $L42779

; 876  : 				break;

	jmp	SHORT $L42776
$L42779:

; 877  : 
; 878  : 			// Running really fast, yield some time to other apps
; 879  : 			Sys_Sleep( 1 );

	push	1
	call	DWORD PTR ?Sys_Sleep@@3P6AXH@ZA		; Sys_Sleep
	add	esp, 4

; 880  : 		}

	jmp	SHORT $L42773
$L42776:

; 881  : 		
; 882  : 
; 883  : 		while ( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )

	push	0
	push	0
	push	0
	push	0
	lea	edx, DWORD PTR _msg$42769[ebp]
	push	edx
	call	DWORD PTR __imp__PeekMessageA@20
	test	eax, eax
	je	SHORT $L42782

; 885  : 			if ( gbAppHasBeenTerminated )

	cmp	DWORD PTR ?gbAppHasBeenTerminated@@3HA, 0 ; gbAppHasBeenTerminated
	je	SHORT $L42783

; 886  : 				break;

	jmp	SHORT $L42782
$L42783:

; 887  : 
; 888  : 			if (!::GetMessage( &msg, NULL, 0, 0))

	push	0
	push	0
	push	0
	lea	eax, DWORD PTR _msg$42769[ebp]
	push	eax
	call	DWORD PTR __imp__GetMessageA@16
	test	eax, eax
	jne	SHORT $L42784

; 890  : 				bDone = 1;

	mov	DWORD PTR _bDone$42766[ebp], 1

; 891  : 				break;

	jmp	SHORT $L42782
$L42784:

; 893  : 
; 894  : 			::TranslateMessage( &msg );

	lea	ecx, DWORD PTR _msg$42769[ebp]
	push	ecx
	call	DWORD PTR __imp__TranslateMessage@4

; 895  : 			::DispatchMessage( &msg );

	lea	edx, DWORD PTR _msg$42769[ebp]
	push	edx
	call	DWORD PTR __imp__DispatchMessageA@4

; 896  : 		}

	jmp	SHORT $L42776
$L42782:

; 897  : 
; 898  : 		if ( bDone )

	cmp	DWORD PTR _bDone$42766[ebp], 0
	je	SHORT $L42785

; 899  : 			break;

	jmp	SHORT $L42765
$L42785:

; 900  : 
; 901  : 		ProcessConsoleInput();

	call	?ProcessConsoleInput@@YAXXZ		; ProcessConsoleInput

; 902  : 
; 903  : 		if ( engineapi.Host_Frame )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+92, 0
	je	SHORT $L42786

; 905  : 			Eng_Frame( 0, dtime );

	mov	eax, DWORD PTR _dtime$42771[ebp+4]
	push	eax
	mov	ecx, DWORD PTR _dtime$42771[ebp]
	push	ecx
	push	0
	call	?Eng_Frame@@YAHHN@Z			; Eng_Frame
	add	esp, 12					; 0000000cH
$L42786:

; 907  : 
; 908  : 		UpdateStatus( 0  /* don't force */ );

	push	0
	call	?UpdateStatus@@YAXH@Z			; UpdateStatus
	add	esp, 4

; 909  : 
; 910  : 		oldtime = newtime;

	mov	edx, DWORD PTR _newtime$42770[ebp]
	mov	DWORD PTR _?oldtime@?BA@??WinMain@@9@16@4NA, edx
	mov	eax, DWORD PTR _newtime$42770[ebp+4]
	mov	DWORD PTR _?oldtime@?BA@??WinMain@@9@16@4NA+4, eax

; 911  : 	}

	jmp	$L42762
$L42765:

; 912  : 
; 913  : 	GameShutdown();

	call	?GameShutdown@@YAXXZ			; GameShutdown

; 914  : 
; 915  : 	DestroyConsoleWindow();

	call	?DestroyConsoleWindow@@YAXXZ		; DestroyConsoleWindow

; 916  : 
; 917  : 	iret = 1;

	mov	DWORD PTR _iret$[ebp], 1
$cleanup$42756:

; 918  : 
; 919  : cleanup:
; 920  : 
; 921  : 	if ( gpszCmdLine )

	cmp	DWORD PTR ?gpszCmdLine@@3PADA, 0	; gpszCmdLine
	je	SHORT $L42787

; 923  : 		free( gpszCmdLine );

	mov	ecx, DWORD PTR ?gpszCmdLine@@3PADA	; gpszCmdLine
	push	ecx
	call	_free
	add	esp, 4
$L42787:

; 925  : 
; 926  : 	return iret;

	mov	eax, DWORD PTR _iret$[ebp]
	jmp	SHORT $L42753
$L42761:

; 927  : }

	jmp	SHORT $cleanup$42756
$L42759:
	jmp	SHORT $cleanup$42756
$L42757:
	jmp	SHORT $cleanup$42756
$L42753:
	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	16					; 00000010H
_WinMain@16 ENDP
_TEXT	ENDS
END
