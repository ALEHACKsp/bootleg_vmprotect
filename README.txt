vmprotect knockoff
----------------------

this project doesnt even deserve a name because it is currently unfinished
includes:
an assembler that assembles my own weird asm-like eso lang into bytecode with randomized "registers" and opcodes
    - generates a header file with opcodes
    - obfuscates strings and data with xor
a vm generator that generates a new vm binary for every opcode set

esolang:
only a few instructions are currently implemented but instead of registers it operates on arbitrary size memory locations thats the whole gimmick
in the future i hope to implement some sort of LLVM based compiler or translation from x86 assembly to my weird esolang
