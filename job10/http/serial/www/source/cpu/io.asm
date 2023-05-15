%define port [esp + 4]
%define value [esp + 8]

        global in_byte
        global out_byte
        global in_word
        global out_word
        global in_long
        global out_long

        section .text
in_byte:
        mov     dx, port
        mov     al, value
        in      al, dx
        movzx   eax, al
        ret

out_byte:
        mov     dx, port
        mov     al, value
        out     dx, al
        ret

in_word:
        mov     dx, port
        mov     ax, value
        in      ax, dx
        movzx   eax, ax
        ret

out_word:
        mov     dx, port
        mov     ax, value
        out     dx, ax
        ret

in_long:
        mov     dx, port
        mov     eax, value
        in      eax, dx
        ret

out_long:
        mov     dx, port
        mov     eax, value
        out     dx, eax
        ret
