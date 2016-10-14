	TITLE	D:\mod\Single-Player Source\dedicated\engine.cpp
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
;	COMDAT ??_C@_0O@CPOP@Sys_EngineAPI?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_08CPBM@swds?4dll?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_06MIDG@?9force?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_02DILL@?$CFs?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_00A@?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0M@OCGL@killserver?6?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
CRT$XCA	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCA	ENDS
CRT$XCU	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCU	ENDS
CRT$XCL	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCL	ENDS
CRT$XCC	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCC	ENDS
CRT$XCZ	SEGMENT DWORD USE32 PUBLIC 'DATA'
CRT$XCZ	ENDS
;	COMDAT ??8@YAHABU_GUID@@0@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _$E1
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT _$E2
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_LoadFunctions@@YAHJ@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_LoadStubs@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_Unload@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_KillEngine@@YAXPAJ@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_Frame@@YAHHN@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_SetSubState@@YAXH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?Eng_SetState@@YAXH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
FLAT	GROUP _DATA, CONST, _BSS, CRT$XCA, CRT$XCU, CRT$XCL, CRT$XCC, CRT$XCZ
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
PUBLIC	?gDLLState@@3HA					; gDLLState
PUBLIC	?gDLLStateInfo@@3HA				; gDLLStateInfo
PUBLIC	?engineapi@@3Uengine_api_s@@A			; engineapi
PUBLIC	?iWait@@3HA					; iWait
PUBLIC	?fDeferedPause@@3HA				; fDeferedPause
PUBLIC	?ghMod@@3JA					; ghMod
PUBLIC	?g_rgchBlob@@3PADA				; g_rgchBlob
PUBLIC	??_C@_00A@?$AA@					; `string'
_BSS	SEGMENT
?gDLLState@@3HA DD 01H DUP (?)				; gDLLState
?gDLLStateInfo@@3HA DD 01H DUP (?)			; gDLLStateInfo
?engineapi@@3Uengine_api_s@@A DB 0f4H DUP (?)		; engineapi
?iWait@@3HA DD	01H DUP (?)				; iWait
?fDeferedPause@@3HA DD 01H DUP (?)			; fDeferedPause
?ghMod@@3JA DD	01H DUP (?)				; ghMod
?g_rgchBlob@@3PADA DB 03800000H DUP (?)			; g_rgchBlob
_?bQuitting@?BO@??Eng_Frame@@YAHHN@Z@4HA DD 01H DUP (?)
_BSS	ENDS
CRT$XCU	SEGMENT
_$S3	DD	FLAT:_$E2
CRT$XCU	ENDS
_DATA	SEGMENT
_?szEmpty@?BL@??Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z@4PADA DD FLAT:??_C@_00A@?$AA@
_DATA	ENDS
;	COMDAT ??_C@_00A@?$AA@
CONST	SEGMENT
??_C@_00A@?$AA@ DB 00H					; `string'
CONST	ENDS
;	COMDAT _$E2
_TEXT	SEGMENT
_$E2	PROC NEAR					; COMDAT
	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi
	call	_$E1
	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
_$E2	ENDP
_TEXT	ENDS
_BSS	SEGMENT
_nullapi DB	0f4H DUP (?)
_BSS	ENDS
;	COMDAT _$E1
_TEXT	SEGMENT
_$E1	PROC NEAR					; COMDAT

; 30   : engine_api_t engineapi = nullapi;

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi
	mov	ecx, 61					; 0000003dH
	mov	esi, OFFSET FLAT:_nullapi
	mov	edi, OFFSET FLAT:?engineapi@@3Uengine_api_s@@A ; engineapi
	rep movsd
	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
_$E1	ENDP
_TEXT	ENDS
PUBLIC	?Eng_LoadFunctions@@YAHJ@Z			; Eng_LoadFunctions
PUBLIC	??_C@_0O@CPOP@Sys_EngineAPI?$AA@		; `string'
EXTRN	?Sys_GetProcAddress@@YAPAXJPBD@Z:NEAR		; Sys_GetProcAddress
;	COMDAT ??_C@_0O@CPOP@Sys_EngineAPI?$AA@
; File d:\mod\single-player source\dedicated\engine.cpp
CONST	SEGMENT
??_C@_0O@CPOP@Sys_EngineAPI?$AA@ DB 'Sys_EngineAPI', 00H ; `string'
CONST	ENDS
;	COMDAT ?Eng_LoadFunctions@@YAHJ@Z
_TEXT	SEGMENT
_hMod$ = 8
_pfnEngineAPI$ = -4
?Eng_LoadFunctions@@YAHJ@Z PROC NEAR			; Eng_LoadFunctions, COMDAT

; 49   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 68					; 00000044H
	push	ebx
	push	esi
	push	edi

; 50   : 	engine_api_func pfnEngineAPI;
; 51   : 	
; 52   : 	pfnEngineAPI = ( engine_api_func )Sys_GetProcAddress( hMod, "Sys_EngineAPI"  );

	push	OFFSET FLAT:??_C@_0O@CPOP@Sys_EngineAPI?$AA@ ; `string'
	mov	eax, DWORD PTR _hMod$[ebp]
	push	eax
	call	?Sys_GetProcAddress@@YAPAXJPBD@Z	; Sys_GetProcAddress
	add	esp, 8
	mov	DWORD PTR _pfnEngineAPI$[ebp], eax

; 53   : 	if ( !pfnEngineAPI )

	cmp	DWORD PTR _pfnEngineAPI$[ebp], 0
	jne	SHORT $L42483

; 54   : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42479
$L42483:

; 55   : 
; 56   : 	if ( !(*pfnEngineAPI)( ENGINE_LAUNCHER_API_VERSION, sizeof( engine_api_t ), &engineapi ) )

	push	OFFSET FLAT:?engineapi@@3Uengine_api_s@@A ; engineapi
	push	244					; 000000f4H
	push	1
	call	DWORD PTR _pfnEngineAPI$[ebp]
	add	esp, 12					; 0000000cH
	test	eax, eax
	jne	SHORT $L42485

; 57   : 		return 0;

	xor	eax, eax
	jmp	SHORT $L42479
$L42485:

; 58   : 
; 59   : 	// All is okay
; 60   : 	return 1;

	mov	eax, 1
$L42479:

; 61   : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_LoadFunctions@@YAHJ@Z ENDP				; Eng_LoadFunctions
_TEXT	ENDS
PUBLIC	?Eng_LoadStubs@@YAXXZ				; Eng_LoadStubs
EXTRN	_memset:NEAR
;	COMDAT ?Eng_LoadStubs@@YAXXZ
_TEXT	SEGMENT
?Eng_LoadStubs@@YAXXZ PROC NEAR				; Eng_LoadStubs, COMDAT

; 71   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 72   : 	// No callbacks in dedicated server since engine should always be loaded.
; 73   : 	memset( &engineapi, 0, sizeof( engineapi ) );

	push	244					; 000000f4H
	push	0
	push	OFFSET FLAT:?engineapi@@3Uengine_api_s@@A ; engineapi
	call	_memset
	add	esp, 12					; 0000000cH

; 74   : 
; 75   : 	engineapi.version		= ENGINE_LAUNCHER_API_VERSION;

	mov	DWORD PTR ?engineapi@@3Uengine_api_s@@A, 1

; 76   : 	engineapi.rendertype	= RENDERTYPE_UNDEFINED;

	mov	DWORD PTR ?engineapi@@3Uengine_api_s@@A+4, 0

; 77   : 	engineapi.size			= sizeof( engine_api_t );

	mov	DWORD PTR ?engineapi@@3Uengine_api_s@@A+8, 244 ; 000000f4H

; 78   : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_LoadStubs@@YAXXZ ENDP				; Eng_LoadStubs
_TEXT	ENDS
PUBLIC	?Eng_Unload@@YAXXZ				; Eng_Unload
EXTRN	?Sys_FreeLibrary@@YAXJ@Z:NEAR			; Sys_FreeLibrary
;	COMDAT ?Eng_Unload@@YAXXZ
_TEXT	SEGMENT
?Eng_Unload@@YAXXZ PROC NEAR				; Eng_Unload, COMDAT

; 88   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 89   : 	if ( ghMod )

	cmp	DWORD PTR ?ghMod@@3JA, 0		; ghMod
	je	SHORT $L42493

; 91   : 		Sys_FreeLibrary(ghMod);

	mov	eax, DWORD PTR ?ghMod@@3JA		; ghMod
	push	eax
	call	?Sys_FreeLibrary@@YAXJ@Z		; Sys_FreeLibrary
	add	esp, 4

; 92   : 		ghMod = 0;

	mov	DWORD PTR ?ghMod@@3JA, 0		; ghMod
$L42493:

; 94   : 
; 95   : 	Eng_LoadStubs();

	call	?Eng_LoadStubs@@YAXXZ			; Eng_LoadStubs

; 96   : 
; 97   : 	gDLLState		= 0;

	mov	DWORD PTR ?gDLLState@@3HA, 0		; gDLLState

; 98   : 	gDLLStateInfo	= 0;

	mov	DWORD PTR ?gDLLStateInfo@@3HA, 0	; gDLLStateInfo

; 99   : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_Unload@@YAXXZ ENDP					; Eng_Unload
_TEXT	ENDS
PUBLIC	?Eng_KillEngine@@YAXPAJ@Z			; Eng_KillEngine
;	COMDAT ?Eng_KillEngine@@YAXPAJ@Z
_TEXT	SEGMENT
_phMod$ = 8
?Eng_KillEngine@@YAXPAJ@Z PROC NEAR			; Eng_KillEngine, COMDAT

; 109  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 110  : 	Sys_FreeLibrary( ghMod );

	mov	eax, DWORD PTR ?ghMod@@3JA		; ghMod
	push	eax
	call	?Sys_FreeLibrary@@YAXJ@Z		; Sys_FreeLibrary
	add	esp, 4

; 111  : 	ghMod = *phMod = 0;

	mov	ecx, DWORD PTR _phMod$[ebp]
	mov	DWORD PTR [ecx], 0
	mov	DWORD PTR ?ghMod@@3JA, 0		; ghMod

; 112  : 
; 113  : 	Eng_LoadStubs();

	call	?Eng_LoadStubs@@YAXXZ			; Eng_LoadStubs

; 114  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_KillEngine@@YAXPAJ@Z ENDP				; Eng_KillEngine
_TEXT	ENDS
PUBLIC	?Eng_Frame@@YAHHN@Z				; Eng_Frame
PUBLIC	?Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z	; Eng_Load
PUBLIC	?Eng_SetState@@YAXH@Z				; Eng_SetState
PUBLIC	?Eng_SetSubState@@YAXH@Z			; Eng_SetSubState
PUBLIC	??_C@_08CPBM@swds?4dll?$AA@			; `string'
PUBLIC	??_C@_06MIDG@?9force?$AA@			; `string'
PUBLIC	??_C@_02DILL@?$CFs?$AA@				; `string'
EXTRN	_stricmp:NEAR
EXTRN	?CheckParm@@YAPADPBDPAPAD@Z:NEAR		; CheckParm
EXTRN	__snprintf:NEAR
EXTRN	?Sys_LoadLibrary@@YAJPAD@Z:NEAR			; Sys_LoadLibrary
EXTRN	__fltused:NEAR
EXTRN	_strcmp:NEAR
;	COMDAT ??_C@_08CPBM@swds?4dll?$AA@
; File d:\mod\single-player source\dedicated\engine.cpp
CONST	SEGMENT
??_C@_08CPBM@swds?4dll?$AA@ DB 'swds.dll', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_06MIDG@?9force?$AA@
CONST	SEGMENT
??_C@_06MIDG@?9force?$AA@ DB '-force', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_02DILL@?$CFs?$AA@
CONST	SEGMENT
??_C@_02DILL@?$CFs?$AA@ DB '%s', 00H			; `string'
CONST	ENDS
;	COMDAT ?Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z
_TEXT	SEGMENT
_cmdline$ = 8
_pef$ = 12
_memory$ = 16
_pmembase$ = 20
_psz$ = 24
_iSubMode$ = 28
_szLastDLL$ = -100
_hMod$ = -104
_p$ = -108
_p$42525 = -112
?Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z PROC NEAR	; Eng_Load, COMDAT

; 124  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 176				; 000000b0H
	push	ebx
	push	esi
	push	edi

; 125  : 	char	szLastDLL[ 100 ];
; 126  : 	long hMod = (long)NULL;

	mov	DWORD PTR _hMod$[ebp], 0

; 127  : 
; 128  : #if defined( _DEBUG )
; 129  : 	char *p;
; 130  : 
; 131  : 	if ( psz && !stricmp( psz, "swds.dll" ) && CheckParm( "-force", &p ) && p )

	cmp	DWORD PTR _psz$[ebp], 0
	je	SHORT $L42509
	push	OFFSET FLAT:??_C@_08CPBM@swds?4dll?$AA@	; `string'
	mov	eax, DWORD PTR _psz$[ebp]
	push	eax
	call	_stricmp
	add	esp, 8
	test	eax, eax
	jne	SHORT $L42509
	lea	ecx, DWORD PTR _p$[ebp]
	push	ecx
	push	OFFSET FLAT:??_C@_06MIDG@?9force?$AA@	; `string'
	call	?CheckParm@@YAPADPBDPAPAD@Z		; CheckParm
	add	esp, 8
	test	eax, eax
	je	SHORT $L42509
	cmp	DWORD PTR _p$[ebp], 0
	je	SHORT $L42509

; 133  : 		psz = p;

	mov	edx, DWORD PTR _p$[ebp]
	mov	DWORD PTR _psz$[ebp], edx
$L42509:

; 135  : #endif
; 136  : 
; 137  : 	// Are we loading a different engine?
; 138  : 	if ( psz && ghMod && !strcmp( psz, szLastDLL ) )

	cmp	DWORD PTR _psz$[ebp], 0
	je	SHORT $L42512
	cmp	DWORD PTR ?ghMod@@3JA, 0		; ghMod
	je	SHORT $L42512
	lea	eax, DWORD PTR _szLastDLL$[ebp]
	push	eax
	mov	ecx, DWORD PTR _psz$[ebp]
	push	ecx
	call	_strcmp
	add	esp, 8
	test	eax, eax
	jne	SHORT $L42512

; 140  : 		return 1;

	mov	eax, 1
	jmp	$L42504
$L42512:

; 142  : 
; 143  : 	if ( ghMod )

	cmp	DWORD PTR ?ghMod@@3JA, 0		; ghMod
	je	SHORT $L42513

; 145  : 		Eng_KillEngine( &hMod );

	lea	edx, DWORD PTR _hMod$[ebp]
	push	edx
	call	?Eng_KillEngine@@YAXPAJ@Z		; Eng_KillEngine
	add	esp, 4
$L42513:

; 147  : 	
; 148  : 	if ( !psz )

	cmp	DWORD PTR _psz$[ebp], 0
	jne	SHORT $L42514

; 150  : 		hMod = 0;

	mov	DWORD PTR _hMod$[ebp], 0

; 151  : 		Eng_LoadStubs();

	call	?Eng_LoadStubs@@YAXXZ			; Eng_LoadStubs

; 153  : 	else if ( !ghMod )

	jmp	SHORT $L42516
$L42514:
	cmp	DWORD PTR ?ghMod@@3JA, 0		; ghMod
	jne	SHORT $L42516

; 155  : 		hMod = Sys_LoadLibrary( (char *)psz );

	mov	eax, DWORD PTR _psz$[ebp]
	push	eax
	call	?Sys_LoadLibrary@@YAJPAD@Z		; Sys_LoadLibrary
	add	esp, 4
	mov	DWORD PTR _hMod$[ebp], eax

; 156  : 		if ( !hMod )

	cmp	DWORD PTR _hMod$[ebp], 0
	jne	SHORT $L42518

; 158  : 			return 0;

	xor	eax, eax
	jmp	$L42504
$L42518:

; 160  : 
; 161  : 		// Load function table from engine
; 162  : 		if ( !Eng_LoadFunctions( hMod ) )

	mov	ecx, DWORD PTR _hMod$[ebp]
	push	ecx
	call	?Eng_LoadFunctions@@YAHJ@Z		; Eng_LoadFunctions
	add	esp, 4
	test	eax, eax
	jne	SHORT $L42519

; 164  : 			Sys_FreeLibrary( hMod );

	mov	edx, DWORD PTR _hMod$[ebp]
	push	edx
	call	?Sys_FreeLibrary@@YAXJ@Z		; Sys_FreeLibrary
	add	esp, 4

; 165  : 			Eng_LoadStubs();

	call	?Eng_LoadStubs@@YAXXZ			; Eng_LoadStubs

; 166  : 			return 0;

	xor	eax, eax
	jmp	$L42504
$L42519:

; 168  : 
; 169  : 		// Activate engine
; 170  : 		Eng_SetState( DLL_ACTIVE );

	push	1
	call	?Eng_SetState@@YAXH@Z			; Eng_SetState
	add	esp, 4
$L42516:

; 172  : 
; 173  : 	Eng_SetSubState( iSubMode );

	mov	eax, DWORD PTR _iSubMode$[ebp]
	push	eax
	call	?Eng_SetSubState@@YAXH@Z		; Eng_SetSubState
	add	esp, 4

; 174  : 
; 175  : 	snprintf( szLastDLL, sizeof( szLastDLL ), "%s", psz );

	mov	ecx, DWORD PTR _psz$[ebp]
	push	ecx
	push	OFFSET FLAT:??_C@_02DILL@?$CFs?$AA@	; `string'
	push	100					; 00000064H
	lea	edx, DWORD PTR _szLastDLL$[ebp]
	push	edx
	call	__snprintf
	add	esp, 16					; 00000010H

; 176  : 
; 177  : 	ghMod		= hMod;

	mov	eax, DWORD PTR _hMod$[ebp]
	mov	DWORD PTR ?ghMod@@3JA, eax		; ghMod

; 178  : 
; 179  : 	if ( ghMod )

	cmp	DWORD PTR ?ghMod@@3JA, 0		; ghMod
	je	$L42532

; 181  : 		static char *szEmpty = "";
; 182  : 
; 183  : 		char *p = (char *)cmdline;

	mov	ecx, DWORD PTR _cmdline$[ebp]
	mov	DWORD PTR _p$42525[ebp], ecx

; 184  : 		if ( !p )

	cmp	DWORD PTR _p$42525[ebp], 0
	jne	SHORT $L42527

; 186  : 			p = szEmpty;

	mov	edx, DWORD PTR _?szEmpty@?BL@??Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z@4PADA
	mov	DWORD PTR _p$42525[ebp], edx
$L42527:

; 188  : 
; 189  : 		if ( !engineapi.Game_Init( p, (unsigned char *)pmembase, memory, pef, NULL, 1) )

	push	1
	push	0
	mov	eax, DWORD PTR _pef$[ebp]
	push	eax
	mov	ecx, DWORD PTR _memory$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pmembase$[ebp]
	push	edx
	mov	eax, DWORD PTR _p$42525[ebp]
	push	eax
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+104
	add	esp, 24					; 00000018H
	test	eax, eax
	jne	SHORT $L42529

; 191  : 			Sys_FreeLibrary(ghMod);

	mov	ecx, DWORD PTR ?ghMod@@3JA		; ghMod
	push	ecx
	call	?Sys_FreeLibrary@@YAXJ@Z		; Sys_FreeLibrary
	add	esp, 4

; 192  : 			ghMod = hMod = 0;

	mov	DWORD PTR _hMod$[ebp], 0
	mov	edx, DWORD PTR _hMod$[ebp]
	mov	DWORD PTR ?ghMod@@3JA, edx		; ghMod

; 193  : 			return 0;

	xor	eax, eax
	jmp	SHORT $L42504
$L42529:

; 195  : 		
; 196  : 		if ( engineapi.SetStartupMode )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+176, 0
	je	SHORT $L42530

; 198  : 			engineapi.SetStartupMode( 1 );

	push	1
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+176
	add	esp, 4
$L42530:

; 200  : 		
; 201  : 		if ( engineapi.Host_Frame )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+92, 0
	je	SHORT $L42531

; 203  : 			Eng_Frame( 1, 0.05 );

	push	1068079513				; 3fa99999H
	push	-1717986918				; 9999999aH
	push	1
	call	?Eng_Frame@@YAHHN@Z			; Eng_Frame
	add	esp, 12					; 0000000cH
$L42531:

; 205  : 
; 206  : 		if ( engineapi.SetStartupMode )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+176, 0
	je	SHORT $L42532

; 208  : 			engineapi.SetStartupMode( 0 );

	push	0
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+176
	add	esp, 4
$L42532:

; 211  : 	return 1;

	mov	eax, 1
$L42504:

; 212  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_Load@@YAHPBDPAUexefuncs_s@@HPAX0H@Z ENDP		; Eng_Load
_TEXT	ENDS
PUBLIC	??_C@_0M@OCGL@killserver?6?$AA@			; `string'
EXTRN	?Sys_Sleep@@3P6AXH@ZA:DWORD			; Sys_Sleep
EXTRN	__imp__PostQuitMessage@4:NEAR
;	COMDAT ??_C@_0M@OCGL@killserver?6?$AA@
; File d:\mod\single-player source\dedicated\engine.cpp
CONST	SEGMENT
??_C@_0M@OCGL@killserver?6?$AA@ DB 'killserver', 0aH, 00H ; `string'
CONST	ENDS
;	COMDAT ?Eng_Frame@@YAHHN@Z
_TEXT	SEGMENT
_fForce$ = 8
_time$ = 12
_iState$42539 = -4
?Eng_Frame@@YAHHN@Z PROC NEAR				; Eng_Frame, COMDAT

; 222  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 76					; 0000004cH
	push	ebx
	push	esi
	push	edi

; 223  : 	if ( ( gDLLState != DLL_ACTIVE ) && !fForce )

	cmp	DWORD PTR ?gDLLState@@3HA, 1		; gDLLState
	je	SHORT $L42537
	cmp	DWORD PTR _fForce$[ebp], 0
	jne	SHORT $L42537

; 224  : 		return 0;

	xor	eax, eax
	jmp	$L42536
$L42537:

; 225  : 
; 226  : 	if ( gDLLState )

	cmp	DWORD PTR ?gDLLState@@3HA, 0		; gDLLState
	je	$L42553

; 228  : 		gDLLStateInfo = DLL_NORMAL;

	mov	DWORD PTR ?gDLLStateInfo@@3HA, 0	; gDLLStateInfo

; 229  : 
; 230  : 		int iState = engineapi.Host_Frame ( (float)time, gDLLState, &gDLLStateInfo ); 

	push	OFFSET FLAT:?gDLLStateInfo@@3HA		; gDLLStateInfo
	mov	eax, DWORD PTR ?gDLLState@@3HA		; gDLLState
	push	eax
	fld	QWORD PTR _time$[ebp]
	fst	DWORD PTR -8+[ebp]
	push	ecx
	fstp	DWORD PTR [esp]
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+92
	add	esp, 12					; 0000000cH
	mov	DWORD PTR _iState$42539[ebp], eax

; 231  : 
; 232  : 		// Special Signal
; 233  : 		if ( gDLLStateInfo != DLL_NORMAL )

	cmp	DWORD PTR ?gDLLStateInfo@@3HA, 0	; gDLLStateInfo
	je	SHORT $L42543

; 236  : 			{

	mov	ecx, DWORD PTR ?gDLLStateInfo@@3HA	; gDLLStateInfo
	mov	DWORD PTR -12+[ebp], ecx
	cmp	DWORD PTR -12+[ebp], 4
	je	SHORT $L42546
	cmp	DWORD PTR -12+[ebp], 6
	je	SHORT $L42547
	jmp	SHORT $L42548
$L42546:

; 237  : 			case DLL_QUIT:
; 238  : 				Eng_Unload();

	call	?Eng_Unload@@YAXXZ			; Eng_Unload

; 239  : #ifdef _WIN32
; 240  : 				PostQuitMessage(0);

	push	0
	call	DWORD PTR __imp__PostQuitMessage@4

; 241  : #else			
; 242  : 				exit( 0 );
; 243  : #endif
; 244  : 				break;

	jmp	SHORT $L42543
$L42547:

; 245  : 			case DLL_RESTART:
; 246  : 				Eng_Unload();

	call	?Eng_Unload@@YAXXZ			; Eng_Unload

; 247  : #ifdef _WIN32
; 248  : 				PostQuitMessage(1);

	push	1
	call	DWORD PTR __imp__PostQuitMessage@4
$L42548:
$L42543:

; 257  : 
; 258  : 		// Are we in our transistion counter?
; 259  : 		if (iWait)

	cmp	DWORD PTR ?iWait@@3HA, 0		; iWait
	je	SHORT $L42551

; 261  : 			iWait--;

	mov	edx, DWORD PTR ?iWait@@3HA		; iWait
	sub	edx, 1
	mov	DWORD PTR ?iWait@@3HA, edx		; iWait

; 262  : 
; 263  : 			// Defer all pauses until we're ready to bring up the launcher
; 264  : 			if (iState == DLL_PAUSED)

	cmp	DWORD PTR _iState$42539[ebp], 2
	jne	SHORT $L42550

; 266  : 				fDeferedPause = 1;

	mov	DWORD PTR ?fDeferedPause@@3HA, 1	; fDeferedPause

; 267  : 				Eng_SetState(DLL_ACTIVE);

	push	1
	call	?Eng_SetState@@YAXH@Z			; Eng_SetState
	add	esp, 4

; 268  : 				iState = DLL_ACTIVE;

	mov	DWORD PTR _iState$42539[ebp], 1
$L42550:

; 270  : 
; 271  : 			// Are we done waiting, if so, did someone request a pause?
; 272  : 			if (!iWait && fDeferedPause)

	cmp	DWORD PTR ?iWait@@3HA, 0		; iWait
	jne	SHORT $L42551
	cmp	DWORD PTR ?fDeferedPause@@3HA, 0	; fDeferedPause
	je	SHORT $L42551

; 274  : 				//force a pause
; 275  : 				iState = DLL_PAUSED;

	mov	DWORD PTR _iState$42539[ebp], 2

; 276  : 				gDLLState = DLL_ACTIVE; 

	mov	DWORD PTR ?gDLLState@@3HA, 1		; gDLLState

; 277  : 				fDeferedPause = 0;

	mov	DWORD PTR ?fDeferedPause@@3HA, 0	; fDeferedPause
$L42551:

; 280  : 
; 281  : 		// Are we now in a transistion?
; 282  : 		if (iState == DLL_TRANS)

	cmp	DWORD PTR _iState$42539[ebp], 4
	jne	SHORT $L42552

; 284  : 			iState = DLL_ACTIVE;

	mov	DWORD PTR _iState$42539[ebp], 1

; 285  : 			iWait = 5; // Let's wait N frames before we'll allow a pause

	mov	DWORD PTR ?iWait@@3HA, 5		; iWait

; 286  : 			Eng_SetState(DLL_ACTIVE);

	push	1
	call	?Eng_SetState@@YAXH@Z			; Eng_SetState
	add	esp, 4
$L42552:

; 288  : 
; 289  : 		// Has the state changed?
; 290  : 		if (iState != gDLLState)

	mov	eax, DWORD PTR _iState$42539[ebp]
	cmp	eax, DWORD PTR ?gDLLState@@3HA		; gDLLState
	je	SHORT $L42553

; 292  : 			Eng_SetState(iState);

	mov	ecx, DWORD PTR _iState$42539[ebp]
	push	ecx
	call	?Eng_SetState@@YAXH@Z			; Eng_SetState
	add	esp, 4
$L42553:

; 295  : 
; 296  : 	if ( gDLLState == DLL_CLOSE || gDLLState == DLL_RESTART)

	cmp	DWORD PTR ?gDLLState@@3HA, 3		; gDLLState
	je	SHORT $L42555
	cmp	DWORD PTR ?gDLLState@@3HA, 6		; gDLLState
	jne	SHORT $L42554
$L42555:

; 298  : 		static int bQuitting = 0;
; 299  : 		
; 300  : 		if ( !bQuitting )

	cmp	DWORD PTR _?bQuitting@?BO@??Eng_Frame@@YAHHN@Z@4HA, 0
	jne	SHORT $L42558

; 302  : 			bQuitting = 1;

	mov	DWORD PTR _?bQuitting@?BO@??Eng_Frame@@YAHHN@Z@4HA, 1

; 303  : 			engineapi.Cbuf_AddText( "killserver\n" );

	push	OFFSET FLAT:??_C@_0M@OCGL@killserver?6?$AA@ ; `string'
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+16
	add	esp, 4

; 304  : 			Eng_Frame( 1, 0.05 );

	push	1068079513				; 3fa99999H
	push	-1717986918				; 9999999aH
	push	1
	call	?Eng_Frame@@YAHHN@Z			; Eng_Frame
	add	esp, 12					; 0000000cH

; 305  : 			Sys_Sleep( 100 );

	push	100					; 00000064H
	call	DWORD PTR ?Sys_Sleep@@3P6AXH@ZA		; Sys_Sleep
	add	esp, 4

; 306  : 			Eng_Frame( 1, 0.05 );

	push	1068079513				; 3fa99999H
	push	-1717986918				; 9999999aH
	push	1
	call	?Eng_Frame@@YAHHN@Z			; Eng_Frame
	add	esp, 12					; 0000000cH

; 307  : 			Sys_Sleep( 100 );

	push	100					; 00000064H
	call	DWORD PTR ?Sys_Sleep@@3P6AXH@ZA		; Sys_Sleep
	add	esp, 4

; 308  : 			return gDLLState;

	mov	eax, DWORD PTR ?gDLLState@@3HA		; gDLLState
	jmp	SHORT $L42536
$L42558:

; 310  : 		
; 311  : 		Eng_Unload();

	call	?Eng_Unload@@YAXXZ			; Eng_Unload

; 312  : 
; 313  : #ifdef _WIN32
; 314  : 		PostQuitMessage(0);

	push	0
	call	DWORD PTR __imp__PostQuitMessage@4
$L42554:

; 326  : 
; 327  : 	return gDLLState;

	mov	eax, DWORD PTR ?gDLLState@@3HA		; gDLLState
$L42536:

; 328  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_Frame@@YAHHN@Z ENDP				; Eng_Frame
_TEXT	ENDS
;	COMDAT ?Eng_SetSubState@@YAXH@Z
_TEXT	SEGMENT
_iSubState$ = 8
?Eng_SetSubState@@YAXH@Z PROC NEAR			; Eng_SetSubState, COMDAT

; 337  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 338  : 	if ( !engineapi.GameSetSubState )

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+84, 0
	jne	SHORT $L42563

; 339  : 		return;

	jmp	SHORT $L42562
$L42563:

; 340  : 
; 341  : 	if ( iSubState != ENG_NORMAL )

	cmp	DWORD PTR _iSubState$[ebp], 1
	je	SHORT $L42564

; 343  : 		engineapi.GameSetSubState( iSubState );

	mov	eax, DWORD PTR _iSubState$[ebp]
	push	eax
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+84
	add	esp, 4
$L42564:
$L42562:

; 345  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_SetSubState@@YAXH@Z ENDP				; Eng_SetSubState
_TEXT	ENDS
;	COMDAT ?Eng_SetState@@YAXH@Z
_TEXT	SEGMENT
_iState$ = 8
?Eng_SetState@@YAXH@Z PROC NEAR				; Eng_SetState, COMDAT

; 354  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 355  : 	gDLLState = iState;

	mov	eax, DWORD PTR _iState$[ebp]
	mov	DWORD PTR ?gDLLState@@3HA, eax		; gDLLState

; 356  : 
; 357  : 	if (engineapi.GameSetState)

	cmp	DWORD PTR ?engineapi@@3Uengine_api_s@@A+80, 0
	je	SHORT $L42568

; 359  : 		engineapi.GameSetState( iState );

	mov	ecx, DWORD PTR _iState$[ebp]
	push	ecx
	call	DWORD PTR ?engineapi@@3Uengine_api_s@@A+80
	add	esp, 4
$L42568:

; 361  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?Eng_SetState@@YAXH@Z ENDP				; Eng_SetState
_TEXT	ENDS
END
