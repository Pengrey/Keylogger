import os
import sys
import re
from ctypes import c_int32, c_int64

class Logger:
    @staticmethod
    def info(message):
        print(f"\033[34m[*]\033[0m {message}")

    @staticmethod
    def success(message):
        print(f"\033[32m[+]\033[0m {message}")

    @staticmethod
    def debug(message):
        print(f"\033[33m[^]\033[0m {message}")

    @staticmethod
    def error(message):
        print(f"\033[31m[-]\033[0m {message}")

def fill_asm(SSN):
    # Read 32 bit assembly file
    with open('src_t/syscall.32.asm', 'r') as file:
        source = file.read()

    matches = re.finditer(r"\[\[(.*?)\]\]", source)

    # Replace SSNs
    for match in matches:
        source = source.replace(match.group(0), SSN)

    # Write source file
    with  open( 'src/syscall.32.asm', 'w' ) as file:
        file.write( source )

    # Read 64 bit assembly file
    with open('src_t/syscall.64.asm', 'r') as file:
        source = file.read()

    matches = re.finditer(r"\[\[(.*?)\]\]", source)

    # Replace SSNs
    for match in matches:
        source = source.replace(match.group(0), SSN)

    # Write source file
    with  open( 'src/syscall.64.asm', 'w' ) as file:
        file.write( source )

# System Service Number (https://j00ru.vexillium.org/syscalls/win32k/64/)
SSN = "103Fh"

def main():
    # Init logger
    Log = Logger()

    Log.debug( f"Using SSN: {SSN}" )

    Log.info( "Preparing assembly files:" )
    fill_asm(SSN)
    Log.success( f"Wrote asm files")

if __name__ == '__main__':
    main()
