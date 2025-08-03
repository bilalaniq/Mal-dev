; nasm HOST.asm -f bin -o HOST.com


org 100h

    mov ah, 9
    mov dx, message
    int 21h

    mov ax, 0x4C00
    int 21h

message db 'You have just released a virus! Have a nice day!$'
