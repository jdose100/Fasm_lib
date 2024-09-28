; платформо зависимые системные вызовы
macro stdout len, [str] ; вызов системного write в консоль
{
    push rcx
    mov rax, 1 ; номер системного вызова write 
    mov rdi, 1 ; дескриптор стандартного вывода (консоль)
    if len eqtype ""
        local string
        local string_len
        local end_str
        jmp @f
        string db len, str
        string_len = $ - string
        @@:
            mov rsi, string
            mov rdx, string_len
    else
        mov rsi, str ; аддрес строки вывода
        mov rdx, len ; длина строки
    end if
    syscall
    pop rcx
}

macro exit exit_code ; выход из программы
{
    mov rax, 60
    mov rdi, exit_code
    syscall
}

