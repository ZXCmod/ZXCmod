	TITLE	D:\mod\Single-Player Source\dedicated\conproc.cpp
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
_BSS	SEGMENT DWORD USE32 PUBLIC 'BSS'
_BSS	ENDS
$$SYMBOLS	SEGMENT BYTE USE32 'DEBSYM'
$$SYMBOLS	ENDS
$$TYPES	SEGMENT BYTE USE32 'DEBTYP'
$$TYPES	ENDS
_TLS	SEGMENT DWORD USE32 PUBLIC 'TLS'
_TLS	ENDS
;	COMDAT ??_C@_0CG@KKL@Request?5Proc?3?5?5Invalid?5?9HFILE?5ha@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_06POGL@?9HFILE?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_08HMIJ@?9HPARENT?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_07GNDC@?9HCHILD?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0CO@LMDN@?6?6InitConProc?3?5?5Setting?5up?5exter@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0CK@FDIC@InitConProc?3?5?5Couldn?8t?5create?5he@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0DC@NMHI@InitConProc?3?5?5Couldn?8t?5create?5th@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??_C@_0L@BCHG@?9conheight?$AA@
CONST	SEGMENT DWORD USE32 PUBLIC 'CONST'
CONST	ENDS
;	COMDAT ??8@YAHABU_GUID@@0@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SetConsoleCXCY@@YAHPAXHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetMappedBuffer@@YAPAXPAX@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?ReleaseMappedBuffer@@YAXPAX@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?GetScreenBufferLines@@YAHPAH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?SetScreenBufferLines@@YAHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?ReadText@@YAHPADHH@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?CharToCode@@YAHD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?WriteText@@YAHPBD@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?RequestProc@@YGIPAX@Z
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?DeinitConProc@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
;	COMDAT ?InitConProc@@YAXXZ
_TEXT	SEGMENT PARA USE32 PUBLIC 'CODE'
_TEXT	ENDS
FLAT	GROUP _DATA, CONST, _BSS
	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT
endif
PUBLIC	?SetConsoleCXCY@@YAHPAXHH@Z			; SetConsoleCXCY
EXTRN	__imp__GetConsoleScreenBufferInfo@8:NEAR
EXTRN	__imp__GetLargestConsoleWindowSize@4:NEAR
EXTRN	__imp__SetConsoleScreenBufferSize@8:NEAR
EXTRN	__imp__SetConsoleWindowInfo@12:NEAR
;	COMDAT ?SetConsoleCXCY@@YAHPAXHH@Z
_TEXT	SEGMENT
_hStdout$ = 8
_cx$ = 12
_cy$ = 16
_info$ = -24
_coordMax$ = -28
$T42698 = -32
?SetConsoleCXCY@@YAHPAXHH@Z PROC NEAR			; SetConsoleCXCY, COMDAT

; 30   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 96					; 00000060H
	push	ebx
	push	esi
	push	edi

; 31   : 	CONSOLE_SCREEN_BUFFER_INFO	info;
; 32   : 	COORD						coordMax;
; 33   :  
; 34   : 	coordMax = GetLargestConsoleWindowSize(hStdout);

	mov	eax, DWORD PTR _hStdout$[ebp]
	push	eax
	call	DWORD PTR __imp__GetLargestConsoleWindowSize@4
	mov	DWORD PTR $T42698[ebp], eax
	mov	ecx, DWORD PTR $T42698[ebp]
	mov	DWORD PTR _coordMax$[ebp], ecx

; 35   : 
; 36   : 	if (cy > coordMax.Y)

	movsx	edx, WORD PTR _coordMax$[ebp+2]
	cmp	DWORD PTR _cy$[ebp], edx
	jle	SHORT $L42533

; 37   : 		cy = coordMax.Y;

	movsx	eax, WORD PTR _coordMax$[ebp+2]
	mov	DWORD PTR _cy$[ebp], eax
$L42533:

; 38   : 
; 39   : 	if (cx > coordMax.X)

	movsx	ecx, WORD PTR _coordMax$[ebp]
	cmp	DWORD PTR _cx$[ebp], ecx
	jle	SHORT $L42534

; 40   : 		cx = coordMax.X;

	movsx	edx, WORD PTR _coordMax$[ebp]
	mov	DWORD PTR _cx$[ebp], edx
$L42534:

; 41   :  
; 42   : 	if (!GetConsoleScreenBufferInfo(hStdout, &info))

	lea	eax, DWORD PTR _info$[ebp]
	push	eax
	mov	ecx, DWORD PTR _hStdout$[ebp]
	push	ecx
	call	DWORD PTR __imp__GetConsoleScreenBufferInfo@8
	test	eax, eax
	jne	SHORT $L42535

; 43   : 		return FALSE;

	xor	eax, eax
	jmp	$L42529
$L42535:

; 44   : 
; 45   : // height
; 46   :     info.srWindow.Left = 0;         

	mov	WORD PTR _info$[ebp+10], 0

; 47   :     info.srWindow.Right = info.dwSize.X - 1;                

	movsx	edx, WORD PTR _info$[ebp]
	sub	edx, 1
	mov	WORD PTR _info$[ebp+14], dx

; 48   :     info.srWindow.Top = 0;

	mov	WORD PTR _info$[ebp+12], 0

; 49   :     info.srWindow.Bottom = cy - 1;          

	mov	eax, DWORD PTR _cy$[ebp]
	sub	eax, 1
	mov	WORD PTR _info$[ebp+16], ax

; 50   :  
; 51   : 	if (cy < info.dwSize.Y)

	movsx	ecx, WORD PTR _info$[ebp+2]
	cmp	DWORD PTR _cy$[ebp], ecx
	jge	SHORT $L42536

; 53   : 		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))

	lea	edx, DWORD PTR _info$[ebp+10]
	push	edx
	push	1
	mov	eax, DWORD PTR _hStdout$[ebp]
	push	eax
	call	DWORD PTR __imp__SetConsoleWindowInfo@12
	test	eax, eax
	jne	SHORT $L42537

; 54   : 			return FALSE;

	xor	eax, eax
	jmp	$L42529
$L42537:

; 55   :  
; 56   : 		info.dwSize.Y = cy;

	mov	cx, WORD PTR _cy$[ebp]
	mov	WORD PTR _info$[ebp+2], cx

; 57   :  
; 58   : 		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))

	mov	edx, DWORD PTR _info$[ebp]
	push	edx
	mov	eax, DWORD PTR _hStdout$[ebp]
	push	eax
	call	DWORD PTR __imp__SetConsoleScreenBufferSize@8
	test	eax, eax
	jne	SHORT $L42538

; 59   : 			return FALSE;

	xor	eax, eax
	jmp	$L42529
$L42538:

; 61   :     else if (cy > info.dwSize.Y)

	jmp	SHORT $L42542
$L42536:
	movsx	ecx, WORD PTR _info$[ebp+2]
	cmp	DWORD PTR _cy$[ebp], ecx
	jle	SHORT $L42542

; 63   : 		info.dwSize.Y = cy;

	mov	dx, WORD PTR _cy$[ebp]
	mov	WORD PTR _info$[ebp+2], dx

; 64   :  
; 65   : 		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))

	mov	eax, DWORD PTR _info$[ebp]
	push	eax
	mov	ecx, DWORD PTR _hStdout$[ebp]
	push	ecx
	call	DWORD PTR __imp__SetConsoleScreenBufferSize@8
	test	eax, eax
	jne	SHORT $L42541

; 66   : 			return FALSE;

	xor	eax, eax
	jmp	$L42529
$L42541:

; 67   :  
; 68   : 		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))

	lea	edx, DWORD PTR _info$[ebp+10]
	push	edx
	push	1
	mov	eax, DWORD PTR _hStdout$[ebp]
	push	eax
	call	DWORD PTR __imp__SetConsoleWindowInfo@12
	test	eax, eax
	jne	SHORT $L42542

; 69   : 			return FALSE;

	xor	eax, eax
	jmp	$L42529
$L42542:

; 71   :  
; 72   : 	if (!GetConsoleScreenBufferInfo(hStdout, &info))

	lea	ecx, DWORD PTR _info$[ebp]
	push	ecx
	mov	edx, DWORD PTR _hStdout$[ebp]
	push	edx
	call	DWORD PTR __imp__GetConsoleScreenBufferInfo@8
	test	eax, eax
	jne	SHORT $L42543

; 73   : 		return FALSE;

	xor	eax, eax
	jmp	$L42529
$L42543:

; 74   :  
; 75   : // width
; 76   : 	info.srWindow.Left = 0;         

	mov	WORD PTR _info$[ebp+10], 0

; 77   : 	info.srWindow.Right = cx - 1;

	mov	eax, DWORD PTR _cx$[ebp]
	sub	eax, 1
	mov	WORD PTR _info$[ebp+14], ax

; 78   : 	info.srWindow.Top = 0;

	mov	WORD PTR _info$[ebp+12], 0

; 79   : 	info.srWindow.Bottom = info.dwSize.Y - 1;               

	movsx	ecx, WORD PTR _info$[ebp+2]
	sub	ecx, 1
	mov	WORD PTR _info$[ebp+16], cx

; 80   :  
; 81   : 	if (cx < info.dwSize.X)

	movsx	edx, WORD PTR _info$[ebp]
	cmp	DWORD PTR _cx$[ebp], edx
	jge	SHORT $L42544

; 83   : 		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))

	lea	eax, DWORD PTR _info$[ebp+10]
	push	eax
	push	1
	mov	ecx, DWORD PTR _hStdout$[ebp]
	push	ecx
	call	DWORD PTR __imp__SetConsoleWindowInfo@12
	test	eax, eax
	jne	SHORT $L42545

; 84   : 			return FALSE;

	xor	eax, eax
	jmp	SHORT $L42529
$L42545:

; 85   :  
; 86   : 		info.dwSize.X = cx;

	mov	dx, WORD PTR _cx$[ebp]
	mov	WORD PTR _info$[ebp], dx

; 87   :     
; 88   : 		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))

	mov	eax, DWORD PTR _info$[ebp]
	push	eax
	mov	ecx, DWORD PTR _hStdout$[ebp]
	push	ecx
	call	DWORD PTR __imp__SetConsoleScreenBufferSize@8
	test	eax, eax
	jne	SHORT $L42546

; 89   : 			return FALSE;

	xor	eax, eax
	jmp	SHORT $L42529
$L42546:

; 91   : 	else if (cx > info.dwSize.X)

	jmp	SHORT $L42550
$L42544:
	movsx	edx, WORD PTR _info$[ebp]
	cmp	DWORD PTR _cx$[ebp], edx
	jle	SHORT $L42550

; 93   : 		info.dwSize.X = cx;

	mov	ax, WORD PTR _cx$[ebp]
	mov	WORD PTR _info$[ebp], ax

; 94   :  
; 95   : 		if (!SetConsoleScreenBufferSize(hStdout, info.dwSize))

	mov	ecx, DWORD PTR _info$[ebp]
	push	ecx
	mov	edx, DWORD PTR _hStdout$[ebp]
	push	edx
	call	DWORD PTR __imp__SetConsoleScreenBufferSize@8
	test	eax, eax
	jne	SHORT $L42549

; 96   : 			return FALSE;

	xor	eax, eax
	jmp	SHORT $L42529
$L42549:

; 97   :  
; 98   : 		if (!SetConsoleWindowInfo(hStdout, TRUE, &info.srWindow))

	lea	eax, DWORD PTR _info$[ebp+10]
	push	eax
	push	1
	mov	ecx, DWORD PTR _hStdout$[ebp]
	push	ecx
	call	DWORD PTR __imp__SetConsoleWindowInfo@12
	test	eax, eax
	jne	SHORT $L42550

; 99   : 			return FALSE;

	xor	eax, eax
	jmp	SHORT $L42529
$L42550:

; 101  :  
; 102  : 	return TRUE;

	mov	eax, 1
$L42529:

; 103  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?SetConsoleCXCY@@YAHPAXHH@Z ENDP			; SetConsoleCXCY
_TEXT	ENDS
PUBLIC	?GetMappedBuffer@@YAPAXPAX@Z			; GetMappedBuffer
EXTRN	__imp__MapViewOfFile@20:NEAR
;	COMDAT ?GetMappedBuffer@@YAPAXPAX@Z
_TEXT	SEGMENT
_hfileBuffer$ = 8
_pBuffer$ = -4
?GetMappedBuffer@@YAPAXPAX@Z PROC NEAR			; GetMappedBuffer, COMDAT

; 112  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 68					; 00000044H
	push	ebx
	push	esi
	push	edi

; 113  : 	LPVOID pBuffer;
; 114  : 
; 115  : 	pBuffer = MapViewOfFile (hfileBuffer,
; 116  : 							FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);

	push	0
	push	0
	push	0
	push	6
	mov	eax, DWORD PTR _hfileBuffer$[ebp]
	push	eax
	call	DWORD PTR __imp__MapViewOfFile@20
	mov	DWORD PTR _pBuffer$[ebp], eax

; 117  : 
; 118  : 	return pBuffer;

	mov	eax, DWORD PTR _pBuffer$[ebp]

; 119  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?GetMappedBuffer@@YAPAXPAX@Z ENDP			; GetMappedBuffer
_TEXT	ENDS
PUBLIC	?ReleaseMappedBuffer@@YAXPAX@Z			; ReleaseMappedBuffer
EXTRN	__imp__UnmapViewOfFile@4:NEAR
;	COMDAT ?ReleaseMappedBuffer@@YAXPAX@Z
_TEXT	SEGMENT
_pBuffer$ = 8
?ReleaseMappedBuffer@@YAXPAX@Z PROC NEAR		; ReleaseMappedBuffer, COMDAT

; 128  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 129  : 	UnmapViewOfFile (pBuffer);

	mov	eax, DWORD PTR _pBuffer$[ebp]
	push	eax
	call	DWORD PTR __imp__UnmapViewOfFile@4

; 130  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?ReleaseMappedBuffer@@YAXPAX@Z ENDP			; ReleaseMappedBuffer
_TEXT	ENDS
PUBLIC	?GetScreenBufferLines@@YAHPAH@Z			; GetScreenBufferLines
_BSS	SEGMENT
_hStdout DD	01H DUP (?)
_BSS	ENDS
;	COMDAT ?GetScreenBufferLines@@YAHPAH@Z
_TEXT	SEGMENT
_piLines$ = 8
_info$ = -24
_bRet$ = -28
?GetScreenBufferLines@@YAHPAH@Z PROC NEAR		; GetScreenBufferLines, COMDAT

; 139  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 92					; 0000005cH
	push	ebx
	push	esi
	push	edi

; 140  : 	CONSOLE_SCREEN_BUFFER_INFO	info;							  
; 141  : 	BOOL						bRet;
; 142  : 
; 143  : 	bRet = GetConsoleScreenBufferInfo (hStdout, &info);

	lea	eax, DWORD PTR _info$[ebp]
	push	eax
	mov	ecx, DWORD PTR _hStdout
	push	ecx
	call	DWORD PTR __imp__GetConsoleScreenBufferInfo@8
	mov	DWORD PTR _bRet$[ebp], eax

; 144  : 		
; 145  : 	if (bRet)

	cmp	DWORD PTR _bRet$[ebp], 0
	je	SHORT $L42563

; 146  : 		*piLines = info.dwSize.Y;

	movsx	edx, WORD PTR _info$[ebp+2]
	mov	eax, DWORD PTR _piLines$[ebp]
	mov	DWORD PTR [eax], edx
$L42563:

; 147  : 
; 148  : 	return bRet;

	mov	eax, DWORD PTR _bRet$[ebp]

; 149  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?GetScreenBufferLines@@YAHPAH@Z ENDP			; GetScreenBufferLines
_TEXT	ENDS
PUBLIC	?SetScreenBufferLines@@YAHH@Z			; SetScreenBufferLines
;	COMDAT ?SetScreenBufferLines@@YAHH@Z
_TEXT	SEGMENT
_iLines$ = 8
?SetScreenBufferLines@@YAHH@Z PROC NEAR			; SetScreenBufferLines, COMDAT

; 158  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 159  : 	return SetConsoleCXCY (hStdout, 80, iLines);

	mov	eax, DWORD PTR _iLines$[ebp]
	push	eax
	push	80					; 00000050H
	mov	ecx, DWORD PTR _hStdout
	push	ecx
	call	?SetConsoleCXCY@@YAHPAXHH@Z		; SetConsoleCXCY
	add	esp, 12					; 0000000cH

; 160  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?SetScreenBufferLines@@YAHH@Z ENDP			; SetScreenBufferLines
_TEXT	ENDS
PUBLIC	?ReadText@@YAHPADHH@Z				; ReadText
EXTRN	__imp__ReadConsoleOutputCharacterA@20:NEAR
;	COMDAT ?ReadText@@YAHPADHH@Z
_TEXT	SEGMENT
_pszText$ = 8
_iBeginLine$ = 12
_iEndLine$ = 16
_coord$ = -4
_dwRead$ = -8
_bRet$ = -12
?ReadText@@YAHPADHH@Z PROC NEAR				; ReadText, COMDAT

; 169  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 76					; 0000004cH
	push	ebx
	push	esi
	push	edi

; 170  : 	COORD	coord;
; 171  : 	DWORD	dwRead;
; 172  : 	BOOL	bRet;
; 173  : 
; 174  : 	coord.X = 0;

	mov	WORD PTR _coord$[ebp], 0

; 175  : 	coord.Y = iBeginLine;

	mov	ax, WORD PTR _iBeginLine$[ebp]
	mov	WORD PTR _coord$[ebp+2], ax

; 176  : 
; 177  : 	bRet = ReadConsoleOutputCharacter(
; 178  : 		hStdout,
; 179  : 		pszText,
; 180  : 		80 * (iEndLine - iBeginLine + 1),
; 181  : 		coord,
; 182  : 		&dwRead);

	lea	ecx, DWORD PTR _dwRead$[ebp]
	push	ecx
	mov	edx, DWORD PTR _coord$[ebp]
	push	edx
	mov	eax, DWORD PTR _iEndLine$[ebp]
	sub	eax, DWORD PTR _iBeginLine$[ebp]
	add	eax, 1
	imul	eax, 80					; 00000050H
	push	eax
	mov	ecx, DWORD PTR _pszText$[ebp]
	push	ecx
	mov	edx, DWORD PTR _hStdout
	push	edx
	call	DWORD PTR __imp__ReadConsoleOutputCharacterA@20
	mov	DWORD PTR _bRet$[ebp], eax

; 183  : 
; 184  : 	// Make sure it's null terminated.
; 185  : 	if (bRet)

	cmp	DWORD PTR _bRet$[ebp], 0
	je	SHORT $L42575

; 186  : 		pszText[dwRead] = '\0';

	mov	eax, DWORD PTR _pszText$[ebp]
	add	eax, DWORD PTR _dwRead$[ebp]
	mov	BYTE PTR [eax], 0
$L42575:

; 187  : 
; 188  : 	return bRet;

	mov	eax, DWORD PTR _bRet$[ebp]

; 189  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?ReadText@@YAHPADHH@Z ENDP				; ReadText
_TEXT	ENDS
PUBLIC	?CharToCode@@YAHD@Z				; CharToCode
EXTRN	_isalpha:NEAR
EXTRN	_isdigit:NEAR
EXTRN	_toupper:NEAR
;	COMDAT ?CharToCode@@YAHD@Z
_TEXT	SEGMENT
_c$ = 8
_upper$ = -4
?CharToCode@@YAHD@Z PROC NEAR				; CharToCode, COMDAT

; 198  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 72					; 00000048H
	push	ebx
	push	esi
	push	edi

; 199  : 	char upper;
; 200  : 		
; 201  : 	upper = toupper(c);

	movsx	eax, BYTE PTR _c$[ebp]
	push	eax
	call	_toupper
	add	esp, 4
	mov	BYTE PTR _upper$[ebp], al

; 204  : 	{

	mov	cl, BYTE PTR _c$[ebp]
	mov	BYTE PTR -8+[ebp], cl
	cmp	BYTE PTR -8+[ebp], 13			; 0000000dH
	je	SHORT $L42584
	jmp	SHORT $L42585
$L42584:

; 205  : 		case 13:
; 206  : 			return 28;

	mov	eax, 28					; 0000001cH
	jmp	SHORT $L42578
$L42585:

; 211  : 
; 212  : 	if (isalpha(c))

	movsx	edx, BYTE PTR _c$[ebp]
	push	edx
	call	_isalpha
	add	esp, 4
	test	eax, eax
	je	SHORT $L42586

; 213  : 		return (30 + upper - 65); 

	movsx	eax, BYTE PTR _upper$[ebp]
	sub	eax, 35					; 00000023H
	jmp	SHORT $L42578
$L42586:

; 214  : 
; 215  : 	if (isdigit(c))

	movsx	eax, BYTE PTR _c$[ebp]
	push	eax
	call	_isdigit
	add	esp, 4
	test	eax, eax
	je	SHORT $L42587

; 216  : 		return (1 + upper - 47);

	movsx	eax, BYTE PTR _upper$[ebp]
	sub	eax, 46					; 0000002eH
	jmp	SHORT $L42578
$L42587:

; 217  : 
; 218  : 	return c;

	movsx	eax, BYTE PTR _c$[ebp]
$L42578:

; 219  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?CharToCode@@YAHD@Z ENDP				; CharToCode
_TEXT	ENDS
PUBLIC	?WriteText@@YAHPBD@Z				; WriteText
EXTRN	__imp__WriteConsoleInputA@16:NEAR
EXTRN	_isupper:NEAR
_BSS	SEGMENT
_hStdin	DD	01H DUP (?)
_BSS	ENDS
;	COMDAT ?WriteText@@YAHPBD@Z
_TEXT	SEGMENT
_szText$ = 8
_dwWritten$ = -4
_rec$ = -24
_upper$ = -28
_sz$ = -32
?WriteText@@YAHPBD@Z PROC NEAR				; WriteText, COMDAT

; 228  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 96					; 00000060H
	push	ebx
	push	esi
	push	edi

; 229  : 	DWORD			dwWritten;
; 230  : 	INPUT_RECORD	rec;
; 231  : 	char			upper, *sz;
; 232  : 
; 233  : 	sz = (LPTSTR) szText;

	mov	eax, DWORD PTR _szText$[ebp]
	mov	DWORD PTR _sz$[ebp], eax
$L42597:

; 234  : 
; 235  : 	while (*sz)

	mov	ecx, DWORD PTR _sz$[ebp]
	movsx	edx, BYTE PTR [ecx]
	test	edx, edx
	je	$L42598

; 237  : 	// 13 is the code for a carriage return (\n) instead of 10.
; 238  : 		if (*sz == 10)

	mov	eax, DWORD PTR _sz$[ebp]
	movsx	ecx, BYTE PTR [eax]
	cmp	ecx, 10					; 0000000aH
	jne	SHORT $L42599

; 239  : 			*sz = 13;

	mov	edx, DWORD PTR _sz$[ebp]
	mov	BYTE PTR [edx], 13			; 0000000dH
$L42599:

; 240  : 
; 241  : 		upper = toupper(*sz);

	mov	eax, DWORD PTR _sz$[ebp]
	movsx	ecx, BYTE PTR [eax]
	push	ecx
	call	_toupper
	add	esp, 4
	mov	BYTE PTR _upper$[ebp], al

; 242  : 
; 243  : 		rec.EventType = KEY_EVENT;

	mov	WORD PTR _rec$[ebp], 1

; 244  : 		rec.Event.KeyEvent.bKeyDown = TRUE;

	mov	DWORD PTR _rec$[ebp+4], 1

; 245  : 		rec.Event.KeyEvent.wRepeatCount = 1;

	mov	WORD PTR _rec$[ebp+8], 1

; 246  : 		rec.Event.KeyEvent.wVirtualKeyCode = upper;

	movsx	dx, BYTE PTR _upper$[ebp]
	mov	WORD PTR _rec$[ebp+10], dx

; 247  : 		rec.Event.KeyEvent.wVirtualScanCode = CharToCode (*sz);

	mov	eax, DWORD PTR _sz$[ebp]
	mov	cl, BYTE PTR [eax]
	push	ecx
	call	?CharToCode@@YAHD@Z			; CharToCode
	add	esp, 4
	mov	WORD PTR _rec$[ebp+12], ax

; 248  : 		rec.Event.KeyEvent.uChar.AsciiChar = *sz;

	mov	edx, DWORD PTR _sz$[ebp]
	mov	al, BYTE PTR [edx]
	mov	BYTE PTR _rec$[ebp+14], al

; 249  : 		rec.Event.KeyEvent.uChar.UnicodeChar = *sz;

	mov	ecx, DWORD PTR _sz$[ebp]
	movsx	dx, BYTE PTR [ecx]
	mov	WORD PTR _rec$[ebp+14], dx

; 250  : 		rec.Event.KeyEvent.dwControlKeyState = isupper(*sz) ? 0x80 : 0x0; 

	mov	eax, DWORD PTR _sz$[ebp]
	movsx	ecx, BYTE PTR [eax]
	push	ecx
	call	_isupper
	add	esp, 4
	neg	eax
	sbb	eax, eax
	and	eax, 128				; 00000080H
	mov	DWORD PTR _rec$[ebp+16], eax

; 251  : 
; 252  : 		WriteConsoleInput(
; 253  : 			hStdin,
; 254  : 			&rec,
; 255  : 			1,
; 256  : 			&dwWritten);

	lea	edx, DWORD PTR _dwWritten$[ebp]
	push	edx
	push	1
	lea	eax, DWORD PTR _rec$[ebp]
	push	eax
	mov	ecx, DWORD PTR _hStdin
	push	ecx
	call	DWORD PTR __imp__WriteConsoleInputA@16

; 257  : 
; 258  : 		rec.Event.KeyEvent.bKeyDown = FALSE;

	mov	DWORD PTR _rec$[ebp+4], 0

; 259  : 
; 260  : 		WriteConsoleInput(
; 261  : 			hStdin,
; 262  : 			&rec,
; 263  : 			1,
; 264  : 			&dwWritten);

	lea	edx, DWORD PTR _dwWritten$[ebp]
	push	edx
	push	1
	lea	eax, DWORD PTR _rec$[ebp]
	push	eax
	mov	ecx, DWORD PTR _hStdin
	push	ecx
	call	DWORD PTR __imp__WriteConsoleInputA@16

; 265  : 
; 266  : 		sz++;

	mov	edx, DWORD PTR _sz$[ebp]
	add	edx, 1
	mov	DWORD PTR _sz$[ebp], edx

; 267  : 	}

	jmp	$L42597
$L42598:

; 268  : 
; 269  : 	return TRUE;

	mov	eax, 1

; 270  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?WriteText@@YAHPBD@Z ENDP				; WriteText
_TEXT	ENDS
PUBLIC	?RequestProc@@YGIPAX@Z				; RequestProc
PUBLIC	??_C@_0CG@KKL@Request?5Proc?3?5?5Invalid?5?9HFILE?5ha@ ; `string'
EXTRN	?Sys_Printf@@YAXPADZZ:NEAR			; Sys_Printf
EXTRN	__endthreadex:NEAR
EXTRN	__imp__SetEvent@4:NEAR
EXTRN	__imp__WaitForMultipleObjects@16:NEAR
_BSS	SEGMENT
_heventDone DD	01H DUP (?)
_hfileBuffer DD	01H DUP (?)
_heventChildSend DD 01H DUP (?)
_heventParentSend DD 01H DUP (?)
_BSS	ENDS
;	COMDAT ??_C@_0CG@KKL@Request?5Proc?3?5?5Invalid?5?9HFILE?5ha@
; File d:\mod\single-player source\dedicated\conproc.cpp
CONST	SEGMENT
??_C@_0CG@KKL@Request?5Proc?3?5?5Invalid?5?9HFILE?5ha@ DB 'Request Proc: '
	DB	' Invalid -HFILE handle', 0aH, 00H		; `string'
CONST	ENDS
;	COMDAT ?RequestProc@@YGIPAX@Z
_TEXT	SEGMENT
_pBuffer$ = -4
_dwRet$ = -8
_heventWait$ = -16
_iBeginLine$ = -20
_iEndLine$ = -24
?RequestProc@@YGIPAX@Z PROC NEAR			; RequestProc, COMDAT

; 279  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 92					; 0000005cH
	push	ebx
	push	esi
	push	edi

; 280  : 	int		*pBuffer;
; 281  : 	DWORD	dwRet;
; 282  : 	HANDLE	heventWait[2];
; 283  : 	int		iBeginLine, iEndLine;
; 284  : 	
; 285  : 	heventWait[0] = heventParentSend;

	mov	eax, DWORD PTR _heventParentSend
	mov	DWORD PTR _heventWait$[ebp], eax

; 286  : 	heventWait[1] = heventDone;

	mov	ecx, DWORD PTR _heventDone
	mov	DWORD PTR _heventWait$[ebp+4], ecx
$L42609:

; 287  : 
; 288  : 	while (1)

	mov	edx, 1
	test	edx, edx
	je	$L42610

; 290  : 		dwRet = WaitForMultipleObjects (2, heventWait, FALSE, INFINITE);

	push	-1
	push	0
	lea	eax, DWORD PTR _heventWait$[ebp]
	push	eax
	push	2
	call	DWORD PTR __imp__WaitForMultipleObjects@16
	mov	DWORD PTR _dwRet$[ebp], eax

; 291  : 
; 292  : 	// heventDone fired, so we're exiting.
; 293  : 		if (dwRet == WAIT_OBJECT_0 + 1)	

	cmp	DWORD PTR _dwRet$[ebp], 1
	jne	SHORT $L42612

; 294  : 			break;

	jmp	$L42610
$L42612:

; 295  : 
; 296  : 		pBuffer = (int *) GetMappedBuffer (hfileBuffer);

	mov	ecx, DWORD PTR _hfileBuffer
	push	ecx
	call	?GetMappedBuffer@@YAPAXPAX@Z		; GetMappedBuffer
	add	esp, 4
	mov	DWORD PTR _pBuffer$[ebp], eax

; 297  : 		
; 298  : 	// hfileBuffer is invalid.  Just leave.
; 299  : 		if (!pBuffer)

	cmp	DWORD PTR _pBuffer$[ebp], 0
	jne	SHORT $L42614

; 301  : 			Sys_Printf ("Request Proc:  Invalid -HFILE handle\n");

	push	OFFSET FLAT:??_C@_0CG@KKL@Request?5Proc?3?5?5Invalid?5?9HFILE?5ha@ ; `string'
	call	?Sys_Printf@@YAXPADZZ			; Sys_Printf
	add	esp, 4

; 302  : 			break;

	jmp	$L42610
$L42614:

; 306  : 		{

	mov	edx, DWORD PTR _pBuffer$[ebp]
	mov	eax, DWORD PTR [edx]
	mov	DWORD PTR -28+[ebp], eax
	mov	ecx, DWORD PTR -28+[ebp]
	sub	ecx, 2
	mov	DWORD PTR -28+[ebp], ecx
	cmp	DWORD PTR -28+[ebp], 3
	ja	SHORT $L42617
	mov	edx, DWORD PTR -28+[ebp]
	jmp	DWORD PTR $L42715[edx*4]
$L42620:

; 307  : 			case CCOM_WRITE_TEXT:
; 308  : 			// Param1 : Text
; 309  : 				pBuffer[0] = WriteText ((LPCTSTR) (pBuffer + 1));

	mov	eax, DWORD PTR _pBuffer$[ebp]
	add	eax, 4
	push	eax
	call	?WriteText@@YAHPBD@Z			; WriteText
	add	esp, 4
	mov	ecx, DWORD PTR _pBuffer$[ebp]
	mov	DWORD PTR [ecx], eax

; 310  : 				break;

	jmp	SHORT $L42617
$L42622:

; 311  : 
; 312  : 			case CCOM_GET_TEXT:
; 313  : 			// Param1 : Begin line
; 314  : 			// Param2 : End line
; 315  : 				iBeginLine = pBuffer[1];

	mov	edx, DWORD PTR _pBuffer$[ebp]
	mov	eax, DWORD PTR [edx+4]
	mov	DWORD PTR _iBeginLine$[ebp], eax

; 316  : 				iEndLine = pBuffer[2];

	mov	ecx, DWORD PTR _pBuffer$[ebp]
	mov	edx, DWORD PTR [ecx+8]
	mov	DWORD PTR _iEndLine$[ebp], edx

; 317  : 				pBuffer[0] = ReadText ((LPTSTR) (pBuffer + 1), iBeginLine, 
; 318  : 									   iEndLine);

	mov	eax, DWORD PTR _iEndLine$[ebp]
	push	eax
	mov	ecx, DWORD PTR _iBeginLine$[ebp]
	push	ecx
	mov	edx, DWORD PTR _pBuffer$[ebp]
	add	edx, 4
	push	edx
	call	?ReadText@@YAHPADHH@Z			; ReadText
	add	esp, 12					; 0000000cH
	mov	ecx, DWORD PTR _pBuffer$[ebp]
	mov	DWORD PTR [ecx], eax

; 319  : 				break;

	jmp	SHORT $L42617
$L42624:

; 320  : 
; 321  : 			case CCOM_GET_SCR_LINES:
; 322  : 			// No params
; 323  : 				pBuffer[0] = GetScreenBufferLines (&pBuffer[1]);

	mov	edx, DWORD PTR _pBuffer$[ebp]
	add	edx, 4
	push	edx
	call	?GetScreenBufferLines@@YAHPAH@Z		; GetScreenBufferLines
	add	esp, 4
	mov	ecx, DWORD PTR _pBuffer$[ebp]
	mov	DWORD PTR [ecx], eax

; 324  : 				break;

	jmp	SHORT $L42617
$L42625:

; 325  : 
; 326  : 			case CCOM_SET_SCR_LINES:
; 327  : 			// Param1 : Number of lines
; 328  : 				pBuffer[0] = SetScreenBufferLines (pBuffer[1]);

	mov	edx, DWORD PTR _pBuffer$[ebp]
	mov	eax, DWORD PTR [edx+4]
	push	eax
	call	?SetScreenBufferLines@@YAHH@Z		; SetScreenBufferLines
	add	esp, 4
	mov	ecx, DWORD PTR _pBuffer$[ebp]
	mov	DWORD PTR [ecx], eax
$L42617:

; 331  : 
; 332  : 		ReleaseMappedBuffer (pBuffer);

	mov	edx, DWORD PTR _pBuffer$[ebp]
	push	edx
	call	?ReleaseMappedBuffer@@YAXPAX@Z		; ReleaseMappedBuffer
	add	esp, 4

; 333  : 		SetEvent (heventChildSend);

	mov	eax, DWORD PTR _heventChildSend
	push	eax
	call	DWORD PTR __imp__SetEvent@4

; 334  : 	}

	jmp	$L42609
$L42610:

; 335  : 
; 336  : 	_endthreadex (0);

	push	0
	call	__endthreadex
	add	esp, 4

; 337  : 	return 0;

	xor	eax, eax

; 338  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	4
$L42715:
	DD	$L42620
	DD	$L42622
	DD	$L42624
	DD	$L42625
?RequestProc@@YGIPAX@Z ENDP				; RequestProc
_TEXT	ENDS
PUBLIC	?DeinitConProc@@YAXXZ				; DeinitConProc
;	COMDAT ?DeinitConProc@@YAXXZ
_TEXT	SEGMENT
?DeinitConProc@@YAXXZ PROC NEAR				; DeinitConProc, COMDAT

; 347  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 64					; 00000040H
	push	ebx
	push	esi
	push	edi

; 348  : 	if ( heventDone )

	cmp	DWORD PTR _heventDone, 0
	je	SHORT $L42629

; 350  : 		SetEvent ( heventDone );

	mov	eax, DWORD PTR _heventDone
	push	eax
	call	DWORD PTR __imp__SetEvent@4
$L42629:

; 352  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?DeinitConProc@@YAXXZ ENDP				; DeinitConProc
_TEXT	ENDS
PUBLIC	?InitConProc@@YAXXZ				; InitConProc
PUBLIC	??_C@_06POGL@?9HFILE?$AA@			; `string'
PUBLIC	??_C@_08HMIJ@?9HPARENT?$AA@			; `string'
PUBLIC	??_C@_07GNDC@?9HCHILD?$AA@			; `string'
PUBLIC	??_C@_0CO@LMDN@?6?6InitConProc?3?5?5Setting?5up?5exter@ ; `string'
PUBLIC	??_C@_0CK@FDIC@InitConProc?3?5?5Couldn?8t?5create?5he@ ; `string'
PUBLIC	??_C@_0DC@NMHI@InitConProc?3?5?5Couldn?8t?5create?5th@ ; `string'
PUBLIC	??_C@_0L@BCHG@?9conheight?$AA@			; `string'
EXTRN	__imp__GetStdHandle@4:NEAR
EXTRN	__imp__CloseHandle@4:NEAR
EXTRN	?CheckParm@@YAPADPBDPAPAD@Z:NEAR		; CheckParm
EXTRN	__beginthreadex:NEAR
EXTRN	__imp__CreateEventA@16:NEAR
EXTRN	_atoi:NEAR
;	COMDAT ??_C@_06POGL@?9HFILE?$AA@
; File d:\mod\single-player source\dedicated\conproc.cpp
CONST	SEGMENT
??_C@_06POGL@?9HFILE?$AA@ DB '-HFILE', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_08HMIJ@?9HPARENT?$AA@
CONST	SEGMENT
??_C@_08HMIJ@?9HPARENT?$AA@ DB '-HPARENT', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_07GNDC@?9HCHILD?$AA@
CONST	SEGMENT
??_C@_07GNDC@?9HCHILD?$AA@ DB '-HCHILD', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_0CO@LMDN@?6?6InitConProc?3?5?5Setting?5up?5exter@
CONST	SEGMENT
??_C@_0CO@LMDN@?6?6InitConProc?3?5?5Setting?5up?5exter@ DB 0aH, 0aH, 'Ini'
	DB	'tConProc:  Setting up external control.', 0aH, 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_0CK@FDIC@InitConProc?3?5?5Couldn?8t?5create?5he@
CONST	SEGMENT
??_C@_0CK@FDIC@InitConProc?3?5?5Couldn?8t?5create?5he@ DB 'InitConProc:  '
	DB	'Couldn''t create heventDone', 0aH, 00H	; `string'
CONST	ENDS
;	COMDAT ??_C@_0DC@NMHI@InitConProc?3?5?5Couldn?8t?5create?5th@
CONST	SEGMENT
??_C@_0DC@NMHI@InitConProc?3?5?5Couldn?8t?5create?5th@ DB 'InitConProc:  '
	DB	'Couldn''t create third party thread', 0aH, 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_0L@BCHG@?9conheight?$AA@
CONST	SEGMENT
??_C@_0L@BCHG@?9conheight?$AA@ DB '-conheight', 00H	; `string'
CONST	ENDS
;	COMDAT ?InitConProc@@YAXXZ
_TEXT	SEGMENT
_threadAddr$ = -4
_hFile$ = -8
_heventParent$ = -12
_heventChild$ = -16
_WantHeight$ = -20
_p$ = -24
?InitConProc@@YAXXZ PROC NEAR				; InitConProc, COMDAT

; 361  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 88					; 00000058H
	push	ebx
	push	esi
	push	edi

; 362  : 	unsigned	threadAddr;
; 363  : 	HANDLE		hFile			= (HANDLE)0;

	mov	DWORD PTR _hFile$[ebp], 0

; 364  : 	HANDLE		heventParent	= (HANDLE)0;

	mov	DWORD PTR _heventParent$[ebp], 0

; 365  : 	HANDLE		heventChild		= (HANDLE)0;

	mov	DWORD PTR _heventChild$[ebp], 0

; 366  : 	int			WantHeight = 50;

	mov	DWORD PTR _WantHeight$[ebp], 50		; 00000032H

; 367  : 	char		*p;
; 368  : 
; 369  : 	// give external front ends a chance to hook into the console
; 370  : 	if ( CheckParm ( "-HFILE", &p ) && p )

	lea	eax, DWORD PTR _p$[ebp]
	push	eax
	push	OFFSET FLAT:??_C@_06POGL@?9HFILE?$AA@	; `string'
	call	?CheckParm@@YAPADPBDPAPAD@Z		; CheckParm
	add	esp, 8
	test	eax, eax
	je	SHORT $L42642
	cmp	DWORD PTR _p$[ebp], 0
	je	SHORT $L42642

; 372  : 		hFile = (HANDLE)atoi ( p );

	mov	ecx, DWORD PTR _p$[ebp]
	push	ecx
	call	_atoi
	add	esp, 4
	mov	DWORD PTR _hFile$[ebp], eax
$L42642:

; 374  : 
; 375  : 	if ( CheckParm ( "-HPARENT", &p ) && p )

	lea	edx, DWORD PTR _p$[ebp]
	push	edx
	push	OFFSET FLAT:??_C@_08HMIJ@?9HPARENT?$AA@	; `string'
	call	?CheckParm@@YAPADPBDPAPAD@Z		; CheckParm
	add	esp, 8
	test	eax, eax
	je	SHORT $L42645
	cmp	DWORD PTR _p$[ebp], 0
	je	SHORT $L42645

; 377  : 		heventParent = (HANDLE)atoi ( p );

	mov	eax, DWORD PTR _p$[ebp]
	push	eax
	call	_atoi
	add	esp, 4
	mov	DWORD PTR _heventParent$[ebp], eax
$L42645:

; 379  : 
; 380  : 	if ( CheckParm ( "-HCHILD", &p ) && p )

	lea	ecx, DWORD PTR _p$[ebp]
	push	ecx
	push	OFFSET FLAT:??_C@_07GNDC@?9HCHILD?$AA@	; `string'
	call	?CheckParm@@YAPADPBDPAPAD@Z		; CheckParm
	add	esp, 8
	test	eax, eax
	je	SHORT $L42648
	cmp	DWORD PTR _p$[ebp], 0
	je	SHORT $L42648

; 382  : 		heventChild = (HANDLE)atoi ( p );

	mov	edx, DWORD PTR _p$[ebp]
	push	edx
	call	_atoi
	add	esp, 4
	mov	DWORD PTR _heventChild$[ebp], eax
$L42648:

; 384  : 
; 385  : 	// ignore if we don't have all the events.
; 386  : 	if ( !hFile || !heventParent || !heventChild )

	cmp	DWORD PTR _hFile$[ebp], 0
	je	SHORT $L42652
	cmp	DWORD PTR _heventParent$[ebp], 0
	je	SHORT $L42652
	cmp	DWORD PTR _heventChild$[ebp], 0
	jne	SHORT $L42651
$L42652:

; 388  : 		//Sys_Printf ("\n\nNo external front end present.\n" );
; 389  : 		return;

	jmp	$L42632
$L42651:

; 391  : 
; 392  : 	Sys_Printf( "\n\nInitConProc:  Setting up external control.\n" );

	push	OFFSET FLAT:??_C@_0CO@LMDN@?6?6InitConProc?3?5?5Setting?5up?5exter@ ; `string'
	call	?Sys_Printf@@YAXPADZZ			; Sys_Printf
	add	esp, 4

; 393  : 
; 394  : 	hfileBuffer			= hFile;

	mov	eax, DWORD PTR _hFile$[ebp]
	mov	DWORD PTR _hfileBuffer, eax

; 395  : 	heventParentSend	= heventParent;

	mov	ecx, DWORD PTR _heventParent$[ebp]
	mov	DWORD PTR _heventParentSend, ecx

; 396  : 	heventChildSend		= heventChild;

	mov	edx, DWORD PTR _heventChild$[ebp]
	mov	DWORD PTR _heventChildSend, edx

; 397  : 
; 398  : 	// So we'll know when to go away.
; 399  : 	heventDone = CreateEvent (NULL, FALSE, FALSE, NULL);

	push	0
	push	0
	push	0
	push	0
	call	DWORD PTR __imp__CreateEventA@16
	mov	DWORD PTR _heventDone, eax

; 400  : 	if (!heventDone)

	cmp	DWORD PTR _heventDone, 0
	jne	SHORT $L42654

; 402  : 		Sys_Printf ("InitConProc:  Couldn't create heventDone\n");

	push	OFFSET FLAT:??_C@_0CK@FDIC@InitConProc?3?5?5Couldn?8t?5create?5he@ ; `string'
	call	?Sys_Printf@@YAXPADZZ			; Sys_Printf
	add	esp, 4

; 403  : 		return;

	jmp	$L42632
$L42654:

; 405  : 
; 406  : 	if (!_beginthreadex (NULL, 0, RequestProc, NULL, 0, &threadAddr))

	lea	eax, DWORD PTR _threadAddr$[ebp]
	push	eax
	push	0
	push	0
	push	OFFSET FLAT:?RequestProc@@YGIPAX@Z	; RequestProc
	push	0
	push	0
	call	__beginthreadex
	add	esp, 24					; 00000018H
	test	eax, eax
	jne	SHORT $L42656

; 408  : 		CloseHandle (heventDone);

	mov	ecx, DWORD PTR _heventDone
	push	ecx
	call	DWORD PTR __imp__CloseHandle@4

; 409  : 		Sys_Printf ("InitConProc:  Couldn't create third party thread\n");

	push	OFFSET FLAT:??_C@_0DC@NMHI@InitConProc?3?5?5Couldn?8t?5create?5th@ ; `string'
	call	?Sys_Printf@@YAXPADZZ			; Sys_Printf
	add	esp, 4

; 410  : 		return;

	jmp	SHORT $L42632
$L42656:

; 412  : 
; 413  : 	// save off the input/output handles.
; 414  : 	hStdout	= GetStdHandle (STD_OUTPUT_HANDLE);

	push	-11					; fffffff5H
	call	DWORD PTR __imp__GetStdHandle@4
	mov	DWORD PTR _hStdout, eax

; 415  : 	hStdin	= GetStdHandle (STD_INPUT_HANDLE);

	push	-10					; fffffff6H
	call	DWORD PTR __imp__GetStdHandle@4
	mov	DWORD PTR _hStdin, eax

; 416  : 
; 417  : 	if ( CheckParm( "-conheight", &p ) && p )

	lea	edx, DWORD PTR _p$[ebp]
	push	edx
	push	OFFSET FLAT:??_C@_0L@BCHG@?9conheight?$AA@ ; `string'
	call	?CheckParm@@YAPADPBDPAPAD@Z		; CheckParm
	add	esp, 8
	test	eax, eax
	je	SHORT $L42660
	cmp	DWORD PTR _p$[ebp], 0
	je	SHORT $L42660

; 419  : 		WantHeight = atoi( p );

	mov	eax, DWORD PTR _p$[ebp]
	push	eax
	call	_atoi
	add	esp, 4
	mov	DWORD PTR _WantHeight$[ebp], eax
$L42660:

; 421  : 
; 422  : 	// Force 80 character width, at least 25 character height
; 423  : 	SetConsoleCXCY( hStdout, 80, WantHeight );

	mov	ecx, DWORD PTR _WantHeight$[ebp]
	push	ecx
	push	80					; 00000050H
	mov	edx, DWORD PTR _hStdout
	push	edx
	call	?SetConsoleCXCY@@YAHPAXHH@Z		; SetConsoleCXCY
	add	esp, 12					; 0000000cH
$L42632:

; 424  : }

	pop	edi
	pop	esi
	pop	ebx
	mov	esp, ebp
	pop	ebp
	ret	0
?InitConProc@@YAXXZ ENDP				; InitConProc
_TEXT	ENDS
END
