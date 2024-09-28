.code equ readable executable ; секция кода
.data equ readable writeable ; секция данных

; типы данных (является лишь именем, которое помогает понять что лежит в
; переменной, никакой проверки типа нет)
define true 1
define false 0
define bool byte
define char byte
define uint8_t byte
define int8_t byte
define uint16_t word
define int16_t word
define uint32_t dword
define int32_t dword
define uint64_t qword
define int64_t qword
define prt qword
define float_t qword

; для данных в памяти озу
define prt_d dq
define uint64_d dq
define int64_d dq
define uint32_d dd
define int32_d dd
define uint16_d dw
define int16_d dw
define uint8_d db
define int8_d db
define bool_d db
define char_d db

struc str [string] ; создаёт строку и указывает длину len
{
    common
      . db string
      .len = $ - .
}

; улучшение мнемоник
macro push [arg] { push arg }
macro pop [arg] { pop arg }
macro clr reg { xor reg, reg }

; добавление новых команд
; даёт имя значению, аналог equ
macro use [vars] { forward match name:varible, vars \{ name equ varible \} }

macro scall proc, [args]
{
    rcall qword [proc], args
}

; передаёт параметры через стек, если передаётся аддрес
; памяти то использует регистры r8-r15
macro rcall proc, [arg]
{    
    common
    if defined WINDOWS & ~ defined LINUX
          push rcx
          fastcall proc, arg
          pop rcx
    end if
    if defined LINUX & ~ defined WINDOWS 
          local arg_index, arg_count, stk_count, float_arg_index
          arg_count = 0
          float_arg_index = 1
          define is_float_arg 0
          push rcx
        forward
          arg_count = arg_count + 1
        common
          arg_index = arg_count
          if arg_count > 6
              stk_count = arg_count - 6
          else
              stk_count = 0
          end if

          sub rsp, 64 + 8 * (stk_count and 1)
        reverse
          match =float value, arg
          \{
              if float_arg_index = 1
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      movq xmm0, rcx
                  else 
                      movq xmm0, value
                  end if
              else if float_arg_index = 2
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      movq xmm1, rcx
                  else
                      movq xmm1, value
              else if float_arg_index = 3
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      movq xmm2, rcx
                  else
                      movq xmm2, value
                  end if
              else if float_arg_index = 4
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      mov xmm3, rcx
                  else
                      movq xmm3, value
                  end if
              else if float_arg_index = 5
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      movq xmm4, rcx
                  else
                      movq xmm4, value
                  end if
              else if float_arg_index = 6
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      movq xmm5, rcx
                  else
                      movq xmm5, value
                  end if
              else if float_arg_index = 7
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      movq xmm6, rcx
                  else
                      movq xmm6, value
                  end if
              else if float_arg_index = 8
                  if value eqtype 0.0 | value eqtype 0
                      mov rcx, value
                      movq xmm7, rcx
                  else
                      movq xmm7, value
                  end if
              end if
              end if

              float_arg_index = float_arg_index + 1
              define is_float_arg 1
          \}
          if arg_index > 6 & is_float_arg eq 0
              push arg
          else if arg_index = 1 & is_float_arg eq 0
              if ~ arg eq
                  mov rdi, arg
              end if
          else if arg_index = 2 & is_float_arg eq 0
              mov rsi, arg
          else if arg_index = 3 & is_float_arg eq 0
              mov rdx, arg
          else if arg_index = 4 & is_float_arg eq 0
              mov rcx, arg
          else if arg_index = 5 & is_float_arg eq 0
              mov r8, arg
          else if arg_index = 6 & is_float_arg eq 0
              mov r9, arg
          end if
          arg_index = arg_index - 1
          define is_float_arg 0

        common
          xor rax, rax
          call proc
          add rsp, 64 + (8 * stk_count) + (8 * (stk_count and 1))
          pop rcx
    end if
}

macro get_args [args]
{
    common
      local num_arg, is_size_arg
      num_arg = 0
      is_size_arg = 0

    forward
      if defined LINUX
          match arg:type, args
          \{
              if num_arg = 0
                  mov [arg], rdi
              else if num_arg = 1
                  mov [arg], rsi
              else if num_arg = 2
                  mov [arg], rdx
              else if num_arg = 3
                  mov [arg], rcx
              else if num_arg = 4
                  mov [arg], r8
              else if num_arg = 5
                  mov [arg], r9
              end if
              is_size_arg = 1
          \}
          if num_arg = 0 & is_size_arg = 0
              mov [args], rdi
          else if num_arg = 1 & is_size_arg = 0
              mov [args], rsi
          else if num_arg = 2 & is_size_arg = 0
              mov [args], rdx
          else if num_arg = 3 & is_size_arg = 0
              mov [args], rcx
          else if num_arg = 4 & is_size_arg = 0
              mov [args], r8
          else if num_arg = 5 & is_size_arg = 0
              mov [args], r9
          end if
      else if defined WINDOWS
          match arg:type, args
          \{
              if num_arg = 0
                  mov [arg], rcx
              else if num_arg = 1
                  mov [arg], rdx
              else if num_arg = 2
                  mov [arg], r8
              else if num_arg = 3
                  mov [arg], 4
              end if
            \}
        if num_arg = 0 & is_size_arg = 0
            mov [args], rcx
        else if num_arg = 1 & is_size_arg = 0
            mov [args], rdx
        else if num_arg = 2 & is_size_arg = 0
            mov [args], r8
        else if num_arg = 3 & is_size_arg = 0
            mov [args], 4
        end if
      end if

      num_arg = num_arg + 1
      is_size_arg = 0
}

; аналог return из python, c++ и т.д
; value - что возвращать
macro return value
{
    mov rax, value
    ret
}

; реализация процедур
; let позволяет использовать некоторые конструкции
; высокоуровневых языков, например reg++ или reg = some_value
; только для простых чисел, вещественные вызовут ошибку
macro let param*
{
    local status
    define status 0

    match =0 dest+==src, status param ; dest += src
    \{
        add dest, src
        define status 1
    \}
    match =0 dest-==src, status param ; dest -= src
    \{
        sub dest, src
        define status 1
    \}
    match =0 dest==src, status param ; dest = src
    \{
        mov dest, src
        define status 1
    \}
    match =0 dest--, status param ; dest--
    \{
        dec dest
        define status 1
    \}
    match =0 dest++, status param ; dest++
    \{
        inc dest
        define status 1
    \}
}

; Macroinstructions for HLL-style conditional operations

macro .if [arg]
{
  common
  __IF equ
  local ..endif
  __ENDIF equ ..endif
  local ..else
  __ELSE equ ..else
  JNCOND __ELSE,arg
}

macro .else
{
  jmp __ENDIF
  __ELSE:
  restore __IF
  __IF equ ,
}

macro .elseif [arg]
{
  common
  jmp __ENDIF
  __ELSE:
  restore __ELSE
  local ..else
  __ELSE equ ..else
  JNCOND __ELSE,arg
}

macro .endif
{
  if __IF eq
   __ELSE:
  end if
  __ENDIF:
  restore __ELSE
  restore __ENDIF
  restore __IF
}

macro .while [arg]
{
  common
  local ..while
  __WHILE equ ..while
  local ..endw
  __ENDW equ ..endw
  __WHILE:
  JNCOND __ENDW,arg
}

macro .endw
{
  jmp __WHILE
  __ENDW:
  restore __ENDW
  restore __WHILE
}

macro .repeat
{
  local ..repeat
  __REPEAT equ ..repeat
  __REPEAT:
}

macro .until [arg]
{
  common
  JNCOND __REPEAT,arg
  restore __REPEAT
}

jnne equ je
jnna equ ja
jnnb equ jb
jnng equ jg
jnnl equ jl
jnnae equ jae
jnnbe equ jbe
jnnge equ jge
jnnle equ jle

macro JNCOND label,v1,c,v2
{
 match any,c
 \{
   cmp v1,v2
   jn\#c label
 \}
 match ,c
 \{
   PARSECOND parsed@cond,v1
   match cond,parsed@cond \\{ JNCONDEXPR label,cond \\}
 \}
}

gt equ >
lt equ <

macro PARSECOND parsed,cond
{
 define parsed
 define neg@cond
 define status@cond
 define nest@cond
 irps symb,cond
 \{
   define symb@cond symb
   match >,symb
   \\{
      define symb@cond gt
   \\}
   match <,symb
   \\{
      define symb@cond lt
   \\}
   current@cond equ status@cond
   match ,current@cond
   \\{
      match ~,symb
      \\\{
	  neg@cond equ neg@cond ~
	  match ~~,neg@cond
	  \\\\{
	       define neg@cond
	  \\\\}
	  define symb@cond
      \\\}
      match (,symb
      \\\{
	  parsed equ parsed neg@cond,<
	  define nest@cond +
	  define symb@cond
	  define neg@cond
      \\\}
      match any,symb@cond
      \\\{
	  parsed equ parsed neg@cond,symb@cond
	  define status@cond +
      \\\}
   \\}
   match status,current@cond
   \\{
      match &,symb
      \\\{
	  parsed equ parsed,&,
	  define status@cond
	  define symb@cond
	  define neg@cond
      \\\}
      match |,symb
      \\\{
	  parsed equ parsed,|,
	  define status@cond
	  define symb@cond
	  define neg@cond
      \\\}
      match (,symb
      \\\{
	  define nest@cond (
      \\\}
      match ),symb
      \\\{
	  match +,nest@cond
	  \\\\{
	       parsed equ parsed>
	       define symb@cond
	  \\\\}
	  restore nest@cond
      \\\}
      match any,symb@cond
      \\\{
	  parsed equ parsed symb@cond
      \\\}
   \\}
 \}
}

macro define_JNCONDEXPR
{
 macro JNCONDEXPR elabel,[mod,cond,op]
 \{
  \common
   \local ..t,..f
   define t@cond ..t
   define f@cond ..f
  \forward
   match ,op
   \\{
      match ,mod \\\{ JNCONDEL elabel,<cond> \\\}
      match ~,mod \\\{ JCONDEL elabel,<cond> \\\}
   \\}
   match &:flabel:tlabel, op:f@cond:t@cond
   \\{
      match ,mod \\\{ JNCONDEL flabel,<cond> \\\}
      match ~,mod \\\{ JCONDEL flabel,<cond> \\\}
      tlabel:
      \\local ..tnew
      restore t@cond
      define t@cond ..tnew
   \\}
   match |:flabel:tlabel, op:f@cond:t@cond
   \\{
      match ,mod \\\{ JCONDEL tlabel,<cond> \\\}
      match ~,mod \\\{ JNCONDEL tlabel,<cond> \\\}
      flabel:
      \\local ..fnew
      restore f@cond
      define f@cond ..fnew
   \\}
  \common
   label f@cond at elabel
   t@cond:
   restore t@cond
   restore f@cond
 \}
}

macro define_JCONDEXPR
{
 macro JCONDEXPR elabel,[mod,cond,op]
 \{
  \common
   \local ..t,..f
   define t@cond ..t
   define f@cond ..f
  \forward
   match ,op
   \\{
      match ,mod \\\{ JCONDEL elabel,<cond> \\\}
      match ~,mod \\\{ JNCONDEL elabel,<cond> \\\}
   \\}
   match |:flabel:tlabel, op:f@cond:t@cond
   \\{
      match ,mod \\\{ JCONDEL flabel,<cond> \\\}
      match ~,mod \\\{ JNCONDEL flabel,<cond> \\\}
      tlabel:
      \\local ..tnew
      restore t@cond
      define t@cond ..tnew
   \\}
   match &:flabel:tlabel, op:f@cond:t@cond
   \\{
      match ,mod \\\{ JNCONDEL tlabel,<cond> \\\}
      match ~,mod \\\{ JCONDEL tlabel,<cond> \\\}
      flabel:
      \\local ..fnew
      restore f@cond
      define f@cond ..fnew
   \\}
  \common
   label f@cond at elabel
   t@cond:
   restore t@cond
   restore f@cond
 \}
}

macro define_JNCONDEL
{
 macro JNCONDEL label,cond
 \{
   \local COND
   match car=,cdr,:cond
   \\{
      define_JNCONDEXPR
      define_JCONDEXPR
      define_JCONDEL
      define_JNCONDEL
      JNCONDEXPR label,cond
      purge JNCONDEXPR,JCONDEXPR,JCONDEL,JNCONDEL
      define COND
   \\}
   match c,cond ; replace gt and lt
   \\{
      match =COND =signed v1>==v2, COND c
      \\\{
	  cmp v1,v2
	  jl label
	  define COND
      \\\}
      match =COND =signed v1<==v2, COND c
      \\\{
	  cmp v1,v2
	  jg label
	  define COND
      \\\}
      match =COND v1>==v2, COND c
      \\\{
	  cmp v1,v2
	  jb label
	  define COND
      \\\}
      match =COND v1<==v2, COND c
      \\\{
	  cmp v1,v2
	  ja label
	  define COND
      \\\}
      match =COND v1==v2, COND c
      \\\{
	  cmp v1,v2
	  jne label
	  define COND
      \\\}
      match =COND v1<>v2, COND c
      \\\{
	  cmp v1,v2
	  je label
	  define COND
      \\\}
      match =COND =signed v1>v2, COND c
      \\\{
	  cmp v1,v2
	  jle label
	  define COND
      \\\}
      match =COND =signed v1<v2, COND c
      \\\{
	  cmp v1,v2
	  jge label
	  define COND
      \\\}
      match =COND v1>v2, COND c
      \\\{
	  cmp v1,v2
	  jbe label
	  define COND
      \\\}
      match =COND v1<v2, COND c
      \\\{
	  cmp v1,v2
	  jae label
	  define COND
      \\\}
      match =COND =ZERO?, COND c
      \\\{
	  jnz label
	  define COND
      \\\}
      match =COND =CARRY?, COND c
      \\\{
	  jnc label
	  define COND
      \\\}
      match =COND =OVERFLOW?, COND c
      \\\{
	  jno label
	  define COND
      \\\}
      match =COND =SIGN?, COND c
      \\\{
	  jns label
	  define COND
      \\\}
      match =COND =PARITY?, COND c
      \\\{
	  jnp label
	  define COND
      \\\}
      match =COND v, COND c
      \\\{
	  if v eqtype 0
	   if ~ v
	    jmp label
	   end if
	  else if v eqtype eax
	   test v,v
	   jz label
	  else
	   cmp v,0
	   je label
	  end if
      \\\}
   \\}
 \}
}

macro define_JCONDEL
{
 macro JCONDEL label,cond
 \{
   \local COND
   match car=,cdr,:cond
   \\{
      define_JNCONDEXPR
      define_JCONDEXPR
      define_JCONDEL
      define_JNCONDEL
      JCONDEXPR label,cond
      purge JNCONDEXPR,JCONDEXPR,JCONDEL,JNCONDEL
      define COND
   \\}
   match c,cond ; replace gt and lt
   \\{
      match =COND =signed v1>==v2, COND c
      \\\{
	  cmp v1,v2
	  jge label
	  define COND
      \\\}
      match =COND =signed v1<==v2, COND c
      \\\{
	  cmp v1,v2
	  jle label
	  define COND
      \\\}
      match =COND v1>==v2, COND c
      \\\{
	  cmp v1,v2
	  jae label
	  define COND
      \\\}
      match =COND v1<==v2, COND c
      \\\{
	  cmp v1,v2
	  jbe label
	  define COND
      \\\}
      match =COND v1==v2, COND c
      \\\{
	  cmp v1,v2
	  je label
	  define COND
      \\\}
      match =COND v1<>v2, COND c
      \\\{
	  cmp v1,v2
	  jne label
	  define COND
      \\\}
      match =COND =signed v1>v2, COND c
      \\\{
	  cmp v1,v2
	  jg label
	  define COND
      \\\}
      match =COND =signed v1<v2, COND c
      \\\{
	  cmp v1,v2
	  jl label
	  define COND
      \\\}
      match =COND v1>v2, COND c
      \\\{
	  cmp v1,v2
	  ja label
	  define COND
      \\\}
      match =COND v1<v2, COND c
      \\\{
	  cmp v1,v2
	  jb label
	  define COND
      \\\}
      match =COND =ZERO?, COND c
      \\\{
	  jz label
	  define COND
      \\\}
      match =COND =CARRY?, COND c
      \\\{
	  jc label
	  define COND
      \\\}
      match =COND =OVERFLOW?, COND c
      \\\{
	  jo label
	  define COND
      \\\}
      match =COND =SIGN?, COND c
      \\\{
	  js label
	  define COND
      \\\}
      match =COND =PARITY?, COND c
      \\\{
	  jp label
	  define COND
      \\\}
      match =COND v, COND c
      \\\{
	  if v eqtype 0
	   if v
	    jmp label
	   end if
	  else if v eqtype eax
	   test v,v
	   jnz label
	  else
	   cmp v,0
	   jne label
	  end if
      \\\}
   \\}
 \}
}

define_JNCONDEXPR
define_JCONDEXPR
define_JNCONDEL
define_JCONDEL

; file elf.inc
macro Elf64_Sym name,value,size,bind,type,other,shndx
{
 dd name+0
 db (bind+0) shl 4 + (type+0)
 db other+0
 dw shndx+0
 dq value+0
 dq size+0
}

virtual at 0
 Elf64_Sym
 sizeof.Elf64_Sym = $
end virtual

macro Elf64_Rel offset,symbol,type
{
  dq offset+0
  dq (symbol+0) shl 32 + (type+0)
}

virtual at 0
 Elf64_Rel
 sizeof.Elf64_Rel = $
end virtual

macro Elf64_Rela offset,symbol,type,addend
{
  dq offset+0
  dq (symbol+0) shl 32 + (type+0)
  dq addend+0
}

virtual at 0
 Elf64_Rela
 sizeof.Elf64_Rela = $
end virtual

DT_NULL    = 0
DT_NEEDED  = 1
DT_HASH    = 4
DT_STRTAB  = 5
DT_SYMTAB  = 6
DT_RELA    = 7
DT_RELASZ  = 8
DT_RELAENT = 9
DT_STRSZ   = 10
DT_SYMENT  = 11
DT_REL	   = 17
DT_RELSZ   = 18
DT_RELENT  = 19

STB_LOCAL  = 0
STB_GLOBAL = 1
STB_WEAK   = 2

STT_NOTYPE  = 0
STT_OBJECT  = 1
STT_FUNC    = 2
STT_SECTION = 3
STT_FILE    = 4

R_386_NONE     = 0
R_386_32       = 1
R_386_PC32     = 2
R_386_GOT32    = 3
R_386_PLT32    = 4
R_386_COPY     = 5
R_386_GLOB_DAT = 6
R_386_JMP_SLOT = 7
R_386_RELATIVE = 8
R_386_GOTOFF   = 9
R_386_GOTPC    = 10

R_X86_64_NONE	   = 0
R_X86_64_64	   = 1
R_X86_64_PC32	   = 2
R_X86_64_GOT32	   = 3
R_X86_64_PLT32	   = 4
R_X86_64_COPY	   = 5
R_X86_64_GLOB_DAT  = 6
R_X86_64_JUMP_SLOT = 7
R_X86_64_RELATIVE  = 8
R_X86_64_GOTPCREL  = 9
R_X86_64_32	   = 10
R_X86_64_32S	   = 11
R_X86_64_16	   = 12
R_X86_64_PC16	   = 13
R_X86_64_8	   = 14
R_X86_64_PC8	   = 15
R_X86_64_DPTMOD64  = 16
R_X86_64_DTPOFF64  = 17
R_X86_64_TPOFF64   = 18
R_X86_64_TLSGD	   = 19
R_X86_64_TLSLD	   = 20
R_X86_64_DTPOFF32  = 21
R_X86_64_GOTTPOFF  = 22
R_X86_64_TPOFF32   = 23
R_X86_64_PC64	   = 24
R_X86_64_GOTOFF64  = 25
R_X86_64_GOTPC32   = 26

; Macroinstructions for making import section (64-bit)

; for linux
macro interpreter [library]
{
 segment interpreter readable
 db library,0
}

macro needed [library]
{
 local str
 match needed,needed@dynamic \{ define needed@dynamic needed,str:library \}
 match ,needed@dynamic \{ define needed@dynamic str:library \}
}
define needed@dynamic

macro import [name]
{
 common
  local strtab,strsz,symtab,rel,relsz,hash
  segment dynamic readable
  match needed,needed@dynamic
  \{ irp item,needed \\{ match str:library,item \\\{ dq DT_NEEDED,str-strtab \\\} \\} \}
  dq DT_STRTAB,strtab
  dq DT_STRSZ,strsz
  dq DT_SYMTAB,symtab
  dq DT_SYMENT,sizeof.Elf64_Sym
  dq DT_RELA,rela
  dq DT_RELASZ,relasz
  dq DT_RELAENT,sizeof.Elf64_Rela
  dq DT_HASH,hash
  dq DT_NULL,0
  segment readable writeable
  symtab: Elf64_Sym
 forward
  local fstr
  Elf64_Sym fstr-strtab,0,0,STB_GLOBAL,STT_FUNC,0,0
 common
  rela:
  local counter
  counter = 1
 forward
   Elf64_Rela name,counter,R_X86_64_64
   counter = counter+1
 common
  relasz = $-rela
  hash:
  dd 1,counter
  dd 0
  repeat counter
   if %=counter
    dd 0
   else
    dd %
   end if
  end repeat
  strtab db 0
 forward
  fstr db `name,0
 common
  match needed,needed@dynamic
  \{ irp item,needed \\{ match str:library,item \\\{ str db library,0 \\\} \\} \}
  strsz = $-strtab
 forward
  name dq 0
}

; for windows
macro library [name,string]
 { common
    import.data:
   forward
    local _label
    if defined name#.redundant
     if ~ name#.redundant
      dd RVA name#.lookup,0,0,RVA _label,RVA name#.address
     end if
    end if
    name#.referred = 1
   common
    dd 0,0,0,0,0
   forward
    if defined name#.redundant
     if ~ name#.redundant
      _label db string,0
	     rb RVA $ and 1
     end if
    end if }

macro import name,[label,string]
 { common
    rb (- rva $) and 7
    if defined name#.referred
     name#.lookup:
   forward
     if used label
      if string eqtype ''
       local _label
       dq RVA _label
      else
       dq 8000000000000000h + string
      end if
     end if
   common
     if $ > name#.lookup
      name#.redundant = 0
      dq 0
     else
      name#.redundant = 1
     end if
     name#.address:
   forward
     if used label
      if string eqtype ''
       label dq RVA _label
      else
       label dq 8000000000000000h + string
      end if
     end if
   common
     if ~ name#.redundant
      dq 0
     end if
   forward
     if used label & string eqtype ''
     _label dw 0
	    db string,0
	    rb RVA $ and 1
     end if
   common
    end if }

macro api [name] {}

; Macroinstructions for defining and calling procedures (x64 version)

macro invoke proc,[arg]
 { common fastcall [proc],arg }

macro fastcall proc,[arg]
 { common local stackspace,argscount,counter
    if argscount < 4
     stackspace = 4*8
    else if argscount and 1
     stackspace = (argscount+1)*8
    else
     stackspace = argscount*8
    end if
    counter = 0
    if stackspace
     if defined current@frame
      if current@frame<stackspace
       current@frame = stackspace
      end if
     else
      if stackspace
       sub rsp,stackspace
      end if
     end if
    end if
   forward
    counter = counter + 1
    define type@param
    define definition@param arg
    match =float value,definition@param
    \{ define definition@param value
       define type@param float \}
    match =addr value,definition@param
    \{ define definition@param value
       define type@param addr \}
    match any=,any,definition@param
    \{ \local ..string,..continue
       jmp ..continue
       align sizeof.TCHAR
       ..string TCHAR definition@param,0
       ..continue:
       define definition@param ..string
       define type@param addr \}
    match any,definition@param
    \{ match \`any,any
       \\{ \\local ..string,..continue
	   jmp ..continue
	   align sizeof.TCHAR
	   ..string TCHAR definition@param,0
	   ..continue:
	   define definition@param ..string
	   define type@param addr \\} \}
    match param,definition@param
    \{ local opcode,origin
       size@param = 0
       if param eqtype 0 | param eqtype 0f | type@param eq addr
	size@param = 8
       else if param eqtype byte 0 | param eqtype byte 0f
	match prefix value,definition@param
	 \\{ if prefix eq qword
	      size@param = 8
	     else if prefix eq dword
	      size@param = 4
	     else if prefix eq word
	      size@param = 2
	     else if prefix eq byte
	      size@param = 1
	     end if \\}
       else if ~ param in <xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7,xmm8,xmm9,xmm10,xmm11,xmm12,xmm13,xmm14,xmm15>
	virtual
	 origin = $
	 inc param
	 load opcode byte from origin
	 if opcode = 67h | ( opcode > 40h & opcode < 48h )
	  load opcode byte from origin+1
	 end if
	 if opcode and 0F8h = 48h
	  size@param = 8
	 else if opcode = 66h
	  size@param = 2
	 else if opcode = 0FFh
	  size@param = 4
	 else
	  size@param = 1
	 end if
	end virtual
       end if
       if counter = 1
	if type@param eq float
	 if ~ param eq xmm0
	  if size@param = 4
	   if param eqtype byte 0 | param eqtype byte 0f
	    mov eax,param
	    movd xmm0,eax
	   else
	    movd xmm0,param
	   end if
	  else
	   if param eqtype 0 | param eqtype 0f | param eqtype byte 0 | param eqtype byte 0f
	    mov rax,param
	    movq xmm0,rax
	   else
	    movq xmm0,param
	   end if
	  end if
	 end if
	 if vararg@fastcall & ~ param eq rcx
	   movq rcx,xmm0
	 end if
	else if type@param eq addr
	 if ~ param eq rcx
	  lea rcx,[param]
	 end if
	else if size@param = 8
	 if ~ param eq rcx
	  mov rcx,param
	 end if
	else if size@param = 4
	 if ~ param eq ecx
	  mov ecx,param
	 end if
	else if size@param = 2
	 if ~ param eq cx
	  mov cx,param
	 end if
	else if size@param = 1
	 if ~ param eq cl
	  mov cl,param
	 end if
	end if
       else if counter = 2
	if type@param eq float
	 if ~ param eq xmm1
	  if size@param = 4
	   if param eqtype byte 0 | param eqtype byte 0f
	    mov eax,param
	    movd xmm1,eax
	   else
	    movd xmm1,param
	   end if
	  else
	   if param eqtype 0 | param eqtype 0f | param eqtype byte 0 | param eqtype byte 0f
	    mov rax,param
	    movq xmm1,rax
	   else
	    movq xmm1,param
	   end if
	  end if
	 end if
	 if vararg@fastcall & ~ param eq rdx
	   movq rdx,xmm1
	 end if
	else if type@param eq addr
	 if ~ param eq rdx
	  lea rdx,[param]
	 end if
	else if size@param = 8
	 if ~ param eq rdx
	  mov rdx,param
	 end if
	else if size@param = 4
	 if ~ param eq edx
	  mov edx,param
	 end if
	else if size@param = 2
	 if ~ param eq dx
	  mov dx,param
	 end if
	else if size@param = 1
	 if ~ param eq dl
	  mov dl,param
	 end if
	end if
       else if counter = 3
	if type@param eq float
	 if ~ param eq xmm2
	  if size@param = 4
	   if param eqtype byte 0 | param eqtype byte 0f
	    mov eax,param
	    movd xmm2,eax
	   else
	    movd xmm2,param
	   end if
	  else
	   if param eqtype 0 | param eqtype 0f | param eqtype byte 0 | param eqtype byte 0f
	    mov rax,param
	    movq xmm2,rax
	   else
	    movq xmm2,param
	   end if
	  end if
	 end if
	 if vararg@fastcall & ~ param eq r8
	   movq r8,xmm2
	 end if
	else if type@param eq addr
	 if ~ param eq r8
	  lea r8,[param]
	 end if
	else if size@param = 8
	 if ~ param eq r8
	  mov r8,param
	 end if
	else if size@param = 4
	 if ~ param eq r8d
	  mov r8d,param
	 end if
	else if size@param = 2
	 if ~ param eq r8w
	  mov r8w,param
	 end if
	else if size@param = 1
	 if ~ param eq r8b
	  mov r8b,param
	 end if
	end if
       else if counter = 4
	if type@param eq float
	 if ~ param eq xmm3
	  if size@param = 4
	   if param eqtype byte 0 | param eqtype byte 0f
	    mov eax,param
	    movd xmm3,eax
	   else
	    movd xmm3,param
	   end if
	  else
	   if param eqtype 0 | param eqtype 0f | param eqtype byte 0 | param eqtype byte 0f
	    mov rax,param
	    movq xmm3,rax
	   else
	    movq xmm3,param
	   end if
	  end if
	 end if
	 if vararg@fastcall & ~ param eq r9
	   movq r9,xmm3
	 end if
	else if type@param eq addr
	 if ~ param eq r9
	  lea r9,[param]
	 end if
	else if size@param = 8
	 if ~ param eq r9
	  mov r9,param
	 end if
	else if size@param = 4
	 if ~ param eq r9d
	  mov r9d,param
	 end if
	else if size@param = 2
	 if ~ param eq r9w
	  mov r9w,param
	 end if
	else if size@param = 1
	 if ~ param eq r9b
	  mov r9b,param
	 end if
	end if
       else
	 if type@param eq addr
	  lea rax,[param]
	  mov [rsp+(counter-1)*8],rax
	 else if param eqtype [0] | param eqtype byte [0]
	  if size@param = 8
	   mov rax,param
	   mov [rsp+(counter-1)*8],rax
	  else if size@param = 4
	   mov eax,param
	   mov [rsp+(counter-1)*8],eax
	  else if size@param = 2
	   mov ax,param
	   mov [rsp+(counter-1)*8],ax
	  else
	   mov al,param
	   mov [rsp+(counter-1)*8],al
	  end if
	 else if size@param = 8
	  virtual
	   origin = $
	   mov rax,param
	   load opcode byte from origin+1
	  end virtual
	  if opcode = 0B8h
	   mov rax,param
	   mov [rsp+(counter-1)*8],rax
	  else
	   mov qword [rsp+(counter-1)*8],param
	  end if
	 else if param in <xmm0,xmm1,xmm2,xmm3,xmm4,xmm5,xmm6,xmm7,xmm8,xmm9,xmm10,xmm11,xmm12,xmm13,xmm14,xmm15>
	  movq [rsp+(counter-1)*8],param
	 else
	  mov [rsp+(counter-1)*8],param
	 end if
       end if \}
   common
    argscount = counter
    call proc
    if stackspace & ~defined current@frame
     add rsp,stackspace
    end if }

macro proc [args]
 { common
    match name params, args>
    \{ define@proc name,<params \} }

prologue@proc equ prologuedef

macro prologuedef procname,flag,parmbytes,localbytes,reglist
 { local loc,fill,counter
   loc = (localbytes+15) and (not 15)
   parmbase@proc equ rbp+16
   localbase@proc equ rbp-loc
   push rbp
   mov rbp,rsp
   if loc+fill
    sub rsp,loc+fill
   end if
   counter = 0
   irps reg, reglist \{ push reg
			counter = counter+1 \}
   fill = 8*(counter and 1) }

epilogue@proc equ epiloguedef

macro epiloguedef procname,flag,parmbytes,localbytes,reglist
 { irps reg, reglist \{ reverse pop reg \}
   leave
   retn }

close@proc equ

macro define@proc name,statement
 { local params,flag,regs,parmbytes,localbytes,current
   if used name
   name:
   match =stdcall args, statement \{ params equ args
				     flag = 11b \}
   match =stdcall, statement \{ params equ
				flag = 11b \}
   match =c args, statement \{ params equ args
			       flag = 10001b \}
   match =c, statement \{ params equ
			  flag = 10001b \}
   match =params, params \{ params equ statement
			    flag = 10000b \}
   match =uses reglist=,args, params \{ regs equ reglist
					params equ args \}
   match =regs =uses reglist, regs params \{ regs equ reglist
					     params equ \}
   match =regs, regs \{ regs equ \}
   match prologue:reglist, prologue@proc:<regs> \{ prologue name,flag,parmbytes,localbytes,reglist \}
   virtual at parmbase@proc
   match =,args, params \{ defargs@proc args \}
   match =args@proc args, args@proc params \{ defargs@proc args \}
   parmbytes = $-(parmbase@proc)
   end virtual
   name # % = parmbytes/8
   all@vars equ
   current = 0
   macro locals
   \{ virtual at localbase@proc+current
      macro label def \\{ match . type,def> \\\{ deflocal@proc .,label,<type \\\} \\}
      struc db [val] \\{ \common deflocal@proc .,db,val \\}
      struc du [val] \\{ \common deflocal@proc .,du,val \\}
      struc dw [val] \\{ \common deflocal@proc .,dw,val \\}
      struc dp [val] \\{ \common deflocal@proc .,dp,val \\}
      struc dd [val] \\{ \common deflocal@proc .,dd,val \\}
      struc dt [val] \\{ \common deflocal@proc .,dt,val \\}
      struc dq [val] \\{ \common deflocal@proc .,dq,val \\}
      struc rb cnt \\{ deflocal@proc .,rb cnt, \\}
      struc rw cnt \\{ deflocal@proc .,rw cnt, \\}
      struc rp cnt \\{ deflocal@proc .,rp cnt, \\}
      struc rd cnt \\{ deflocal@proc .,rd cnt, \\}
      struc rt cnt \\{ deflocal@proc .,rt cnt, \\}
      struc rq cnt \\{ deflocal@proc .,rq cnt, \\} \}
   macro endl
   \{ purge label
      restruc db,du,dw,dp,dd,dt,dq
      restruc rb,rw,rp,rd,rt,rq
      current = $-(localbase@proc)
      end virtual \}
   macro ret operand
   \{ match any, operand \\{ retn operand \\}
      match , operand \\{ match epilogue:reglist, epilogue@proc:<regs> \\\{ epilogue name,flag,parmbytes,localbytes,reglist \\\} \\} \}
   macro finish@proc
   \{ localbytes = current
      match close:reglist, close@proc:<regs> \\{ close name,flag,parmbytes,localbytes,reglist \\}
      end if \} }

macro defargs@proc [arg]
 { common
    if ~ arg eq
   forward
     local ..arg,current@arg
     match argname:type, arg
      \{ current@arg equ argname
	 label ..arg type
	 argname equ ..arg
	 if qqword eq type
	   dq ?,?,?,?
	 else if dqword eq type
	   dq ?,?
	 else if tbyte eq type
	   dq ?,?
	 else
	   dq ?
	 end if \}
     match =current@arg,current@arg
      \{ current@arg equ arg
	 arg equ ..arg
	 ..arg dq ? \}
   common
     args@proc equ current@arg
   forward
     restore current@arg
   common
    end if }

macro deflocal@proc name,def,[val] { name def val }

macro deflocal@proc name,def,[val]
 { common
    match vars, all@vars \{ all@vars equ all@vars, \}
    all@vars equ all@vars name
   forward
    local ..var,..tmp
    ..var def val
    match =?, val \{ ..tmp equ \}
    match any =?, val \{ ..tmp equ \}
    match any (=?), val \{ ..tmp equ \}
    match =label, def \{ ..tmp equ \}
    match tmp : value, ..tmp : val
     \{ tmp: end virtual
	initlocal@proc ..var,def value
	virtual at tmp\}
   common
    match first rest, ..var, \{ name equ first \} }

struc label type { label . type }

macro initlocal@proc name,def
 { virtual at name
    def
    size@initlocal = $ - name
   end virtual
   position@initlocal = 0
   while size@initlocal > position@initlocal
    virtual at name
     def
     if size@initlocal - position@initlocal < 2
      current@initlocal = 1
      load byte@initlocal byte from name+position@initlocal
     else if size@initlocal - position@initlocal < 4
      current@initlocal = 2
      load word@initlocal word from name+position@initlocal
     else if size@initlocal - position@initlocal < 8
      current@initlocal = 4
      load dword@initlocal dword from name+position@initlocal
     else
      load qword@initlocal qword from name+position@initlocal
      if ( qword@initlocal > 0 & qword@initlocal < 80000000h ) | ( qword@initlocal < 0 & qword@initlocal >= -80000000h )
       current@initlocal = 8
      else
       current@initlocal = 4
       dword@initlocal = qword@initlocal and 0FFFFFFFFh
      end if
     end if
    end virtual
    if current@initlocal = 1
     mov byte [name+position@initlocal],byte@initlocal
    else if current@initlocal = 2
     mov word [name+position@initlocal],word@initlocal
    else if current@initlocal = 4
     mov dword [name+position@initlocal],dword@initlocal
    else
     mov qword [name+position@initlocal],qword@initlocal
    end if
    position@initlocal = position@initlocal + current@initlocal
   end while }

macro endp
 { purge ret,locals,endl
   finish@proc
   purge finish@proc
   restore regs@proc
   match all,args@proc \{ restore all \}
   restore args@proc
   match all,all@vars \{ restore all \} }

macro local [var]
 { common
    locals
   forward done@local equ
    match varname[count]:vartype, var
    \{ match =byte, vartype \\{ varname rb count
				restore done@local \\}
       match =word, vartype \\{ varname rw count
				restore done@local \\}
       match =dword, vartype \\{ varname rd count
				 restore done@local \\}
       match =pword, vartype \\{ varname rp count
				 restore done@local \\}
       match =qword, vartype \\{ varname rq count
				 restore done@local \\}
       match =tbyte, vartype \\{ varname rt count
				 restore done@local \\}
       match =dqword, vartype \\{ label varname dqword
				  rq count*2
				  restore done@local \\}
       match =qqword, vartype \\{ label varname qqword
				  rq count*4
				  restore done@local \\}
       match =xword, vartype \\{ label varname xword
				 rq count*2
				 restore done@local \\}
       match =yword, vartype \\{ label varname yword
				 rq count*4
				 restore done@local \\}
       match , done@local \\{ virtual
			       varname vartype
			      end virtual
			      rb count*sizeof.\#vartype
			      restore done@local \\} \}
    match :varname:vartype, done@local:var
    \{ match =byte, vartype \\{ varname db ?
				restore done@local \\}
       match =word, vartype \\{ varname dw ?
				restore done@local \\}
       match =dword, vartype \\{ varname dd ?
				 restore done@local \\}
       match =pword, vartype \\{ varname dp ?
				 restore done@local \\}
       match =qword, vartype \\{ varname dq ?
				 restore done@local \\}
       match =tbyte, vartype \\{ varname dt ?
				 restore done@local \\}
       match =dqword, vartype \\{ label varname dqword
				  dq ?,?
				  restore done@local \\}
       match =qqword, vartype \\{ label varname qqword
				  dq ?,?,?,?
				  restore done@local \\}
       match =xword, vartype \\{ label varname xword
				 dq ?,?
				 restore done@local \\}
       match =yword, vartype \\{ label varname yword
				 dq ?,?,?,?
				 restore done@local \\}
       match , done@local \\{ varname vartype
			      restore done@local \\} \}
    match ,done@local
    \{ var
       restore done@local \}
   common
    endl }

macro frame
 { local size,current
   if size
    sub rsp,size
   end if
   current = 0
   current@frame equ current
   size@frame equ size }

macro endf
 { size@frame = current@frame
   if size@frame
    add rsp,size@frame
   end if
   restore size@frame,current@frame }

macro static_rsp_prologue procname,flag,parmbytes,localbytes,reglist
 { local counter,loc,frame,current 
   counter = 0 
   irps reg, reglist \{ push reg 
			counter = counter+1 \} 
   loc = (localbytes+7) and (not 7) 
   if frame & (counter+loc shr 3+1) and 1 
    loc = loc + 8 
   end if 
   framebytes@proc equ frame+loc 
   if framebytes@proc 
    sub rsp,framebytes@proc 
   end if 
   localbase@proc equ rsp+frame 
   regsbase@proc equ rsp+frame+loc 
   parmbase@proc equ rsp+frame+loc+counter*8+8 
   current = 0 
   current@frame equ current 
   size@frame equ frame } 

macro static_rsp_epilogue procname,flag,parmbytes,localbytes,reglist 
 { if framebytes@proc 
    add rsp,framebytes@proc 
   end if 
   irps reg, reglist \{ reverse pop reg \} 
   retn } 

macro static_rsp_close procname,flag,parmbytes,localbytes,reglist 
 { size@frame = current@frame 
   restore size@frame,current@frame }

stdcall fix fastcall

macro cinvoke proc,[arg]
 { common ccall [proc],arg }

macro ccall proc,[arg]
 { common vararg@fastcall = 1
	  fastcall proc,arg
	  vararg@fastcall = 0 }

vararg@fastcall = 0

; Macroinstructions for defining data structures

macro struct name
 { virtual at 0
   define @struct
   field@struct equ name
   match child parent, name \{ restore field@struct
			       field@struct equ child,fields@\#parent \}
   sub@struct equ
   struc db [val] \{ \common define field@struct .,db,<val> \}
   struc dw [val] \{ \common define field@struct .,dw,<val> \}
   struc du [val] \{ \common define field@struct .,du,<val> \}
   struc dd [val] \{ \common define field@struct .,dd,<val> \}
   struc dp [val] \{ \common define field@struct .,dp,<val> \}
   struc dq [val] \{ \common define field@struct .,dq,<val> \}
   struc dt [val] \{ \common define field@struct .,dt,<val> \}
   struc rb count \{ define field@struct .,db,count dup (?) \}
   struc rw count \{ define field@struct .,dw,count dup (?) \}
   struc rd count \{ define field@struct .,dd,count dup (?) \}
   struc rp count \{ define field@struct .,dp,count dup (?) \}
   struc rq count \{ define field@struct .,dq,count dup (?) \}
   struc rt count \{ define field@struct .,dt,count dup (?) \}
   macro db [val] \{ \common \local anonymous
		     define field@struct anonymous,db,<val> \}
   macro dw [val] \{ \common \local anonymous
		     define field@struct anonymous,dw,<val> \}
   macro du [val] \{ \common \local anonymous
		     define field@struct anonymous,du,<val> \}
   macro dd [val] \{ \common \local anonymous
		     define field@struct anonymous,dd,<val> \}
   macro dp [val] \{ \common \local anonymous
		     define field@struct anonymous,dp,<val> \}
   macro dq [val] \{ \common \local anonymous
		     define field@struct anonymous,dq,<val> \}
   macro dt [val] \{ \common \local anonymous
		     define field@struct anonymous,dt,<val> \}
   macro rb count \{ \local anonymous
		     define field@struct anonymous,db,count dup (?) \}
   macro rw count \{ \local anonymous
		     define field@struct anonymous,dw,count dup (?) \}
   macro rd count \{ \local anonymous
		     define field@struct anonymous,dd,count dup (?) \}
   macro rp count \{ \local anonymous
		     define field@struct anonymous,dp,count dup (?) \}
   macro rq count \{ \local anonymous
		     define field@struct anonymous,dq,count dup (?) \}
   macro rt count \{ \local anonymous
		     define field@struct anonymous,dt,count dup (?) \}
   macro union \{ field@struct equ ,union,<
		  sub@struct equ union \}
   macro struct \{ field@struct equ ,substruct,<
		  sub@struct equ substruct \} }

macro ends
 { match , sub@struct \{ restruc db,dw,du,dd,dp,dq,dt
			 restruc rb,rw,rd,rp,rq,rt
			 purge db,dw,du,dd,dp,dq,dt
			 purge rb,rw,rd,rp,rq,rt
			 purge union,struct
			 irpv fields,field@struct \\{ restore field@struct
						      \\common define fields@struct fields \\}
			 match name tail,fields@struct, \\{ if $
							    display 'Error: definition of ',\\`name,' contains illegal instructions.',0Dh,0Ah
							    err
							    end if \\}
			 match name=,fields,fields@struct \\{ restore @struct
							      make@struct name,fields
							      define fields@\\#name fields \\}
			 end virtual \}
   match any, sub@struct \{ tmp@struct equ field@struct
			    restore field@struct
			    field@struct equ tmp@struct> \}
   restore sub@struct }

macro make@struct name,[field,type,def]
 { common
    local define
    define equ name
   forward
    local sub
    match , field \{ make@substruct type,name,sub def
		     define equ define,.,sub, \}
    match any, field \{ define equ define,.#field,type,<def> \}
   common
    match fields, define \{ define@struct fields \} }

macro define@struct name,[field,type,def]
 { common
    virtual
    db `name
    load initial@struct byte from 0
    if initial@struct = '.'
    display 'Error: name of structure should not begin with a dot.',0Dh,0Ah
    err
    end if
    end virtual
    local list
    list equ
   forward
    if ~ field eq .
     name#field type def
     sizeof.#name#field = $ - name#field
    else
     label name#.#type
     rb sizeof.#type
    end if
    local value
    match any, list \{ list equ list, \}
    list equ list <value>
   common
    sizeof.#name = $
    restruc name
    match values, list \{
    struc name value \\{ \\local \\..base
    match , @struct \\\{ define field@struct .,name,<values> \\\}
    match no, @struct \\\{ label \\..base
   forward
     match , value \\\\{ field type def \\\\}
     match any, value \\\\{ field type value
			    if ~ field eq .
			     rb sizeof.#name#field - ($-field)
			    end if \\\\}
   common label . at \\..base \\\}
   \\}
    macro name value \\{
    match , @struct \\\{ \\\local anonymous
				  define field@struct anonymous,name,<values> \\\}
    match no, @struct \\\{
   forward
     match , value \\\\{ type def \\\\}
     match any, value \\\\{ \\\\local ..field
			   ..field = $
			   type value
			   if ~ field eq .
			    rb sizeof.#name#field - ($-..field)
			   end if \\\\}
   common \\\} \\} \} }

macro enable@substruct
 { macro make@substruct substruct,parent,name,[field,type,def]
    \{ \common
	\local define
	define equ parent,name
       \forward
	\local sub
	match , field \\{ match any, type \\\{ enable@substruct
					       make@substruct type,parent,sub def
					       purge make@substruct
					       define equ define,.,sub, \\\} \\}
	match any, field \\{ define equ define,.\#field,type,<def> \\}
       \common
	match fields, define \\{ define@\#substruct fields \\} \} }

enable@substruct

macro define@union parent,name,[field,type,def]
 { common
    virtual at parent#.#name
   forward
    if ~ field eq .
     virtual at parent#.#name
      parent#field type def
      sizeof.#parent#field = $ - parent#field
     end virtual
     if sizeof.#parent#field > $ - parent#.#name
      rb sizeof.#parent#field - ($ - parent#.#name)
     end if
    else
     virtual at parent#.#name
      label parent#.#type
      type def
     end virtual
     label name#.#type at parent#.#name
     if sizeof.#type > $ - parent#.#name
      rb sizeof.#type - ($ - parent#.#name)
     end if
    end if
   common
    sizeof.#name = $ - parent#.#name
    end virtual
    struc name [value] \{ \common
    label .\#name
    last@union equ
   forward
    match any, last@union \\{ virtual at .\#name
			       field type def
			      end virtual \\}
    match , last@union \\{ match , value \\\{ field type def \\\}
			   match any, value \\\{ field type value \\\} \\}
    last@union equ field
   common rb sizeof.#name - ($ - .\#name) \}
    macro name [value] \{ \common \local ..anonymous
			  ..anonymous name value \} }

macro define@substruct parent,name,[field,type,def]
 { common
    virtual at parent#.#name
   forward
    local value
    if ~ field eq .
     parent#field type def
     sizeof.#parent#field = $ - parent#field
    else
     label parent#.#type
     rb sizeof.#type
    end if
   common
    sizeof.#name = $ - parent#.#name
    end virtual
    struc name value \{
    label .\#name
   forward
     match , value \\{ field type def \\}
     match any, value \\{ field type value
			  if ~ field eq .
			   rb sizeof.#parent#field - ($-field)
			  end if \\}
   common \}
    macro name value \{ \local ..anonymous
			..anonymous name \} }
