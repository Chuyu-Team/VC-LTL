        page    ,132
        title   strrchr - find last occurrence of character in string
;***
;strrchr.asm - find last occurrence of character in string
;
;       Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       defines strrchr() - find the last occurrence of a given character
;       in a string.
;
;*******************************************************************************

        .xlist
        include vcruntime.inc
        .list

page
;***
;char *strrchr(string, ch) - find last occurrence of ch in string
;
;Purpose:
;       Finds the last occurrence of ch in string.  The terminating
;       null character is used as part of the search.
;
;       Algorithm:
;       char *
;       strrchr (string, ch)
;             char *string, ch;
;             {
;             char *start = string;
;
;             while (*string++)
;                     ;
;             while (--string != start && *string != ch)
;                     ;
;             if (*string == ch)
;                     return(string);
;             return(NULL);
;             }
;
;Entry:
;       char *string - string to search in
;       char ch - character to search for
;
;Exit:
;       returns a pointer to the last occurrence of ch in the given
;       string
;       returns NULL if ch does not occur in the string
;
;Uses:
;
;Exceptions:
;
;*******************************************************************************

        CODESEG

        public  strrchr
strrchr proc \
        uses edi, \
        string:ptr byte, \
        chr:byte

;   .FPO (cdwLocals, cdwParams, cbProlog, cbRegs, fUseBP, cbFrame)
    .FPO      ( 0, 2, 0, 0, 0, 0 )

; Include SSE2/SSE4.2 code paths for platforms that support them.
        include strrchr_sse.inc

        mov     edi,[string]    ; di = string
        xor     eax,eax         ; al=null byte
        or      ecx,-1          ; cx = -1
repne   scasb                   ; find the null & count bytes
        add     ecx,1           ; cx=-byte count (with null)
        neg     ecx             ; cx=+byte count (with null)
        sub     edi,1           ; di points to terminal null
        mov     al,chr          ; al=search byte
        std                     ; count 'down' on string this time
repne   scasb                   ; find that byte
        add     edi,1           ; di points to byte which stopped scan

        cmp     [edi],al        ; see if we have a hit
        je      short returndi  ; yes, point to byte

        xor     eax,eax         ; no, return NULL
        jmp     short toend     ; do return sequence

returndi:
        mov     eax,edi         ; ax=pointer to byte

toend:
        cld

        ret                     ; _cdecl return

strrchr endp
        end
