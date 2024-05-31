ProjectName		=  test
CONFIG          =  src/openssl.cnf
PASSWORD        =  verysecuresecretpassword

CCX64			=  x86_64-w64-mingw32-gcc
CCX86			=  i686-w64-mingw32-gcc
RC64            =  x86_64-w64-mingw32-windres
RC86            =  i686-w64-mingw32-windres

OSSL            = openssl
ESC             = osslsigncode

CFLAGS			=  -Os -fno-asynchronous-unwind-tables
CFLAGS 			+= -fno-ident -fpack-struct=8 -falign-functions=1
CFLAGS  		+= -s -ffunction-sections -falign-jumps=1 -w
CFLAGS			+= -Wl,-s,--no-seh,--enable-stdcall-fixup

CHEADERS		=  -I src/include

SOURCES			=  src/*.c
SOURCES			+= src/hash/*.c
SOURCES			+= src/env/*.c

all: prep x64 x86

prep: clean
	@ echo "[*] Preparing files ..."
	@ python3 scripts/apihash.py

	@ echo "[*] Generating pem files ..."
	@ $(OSSL) req -x509 -newkey rsa:4096 -keyout src/key.pem -out src/cert.pem -sha256 -days 365 -nodes -config $(CONFIG) 2> /dev/null
	@ echo "[*] DONE"
	
	@ echo "[*] Generating pfx file ..."
	@ $(OSSL) pkcs12 -export -in src/cert.pem -inkey src/key.pem -passin pass:$(PASSWORD) -out src/sign.pfx -passout pass:$(PASSWORD)
	@ echo "[*] DONE"

x64: prep
	@ echo "[*] Compiling x64 metadata..."
	@ $(RC64) -i src/metadata.rc -O coff -o src/metadata.64.o
	@ echo "[*] DONE"

	@ echo "[*] Compiling x64 executable..."
	@ $(CCX64) $(SOURCES) src/metadata.64.o $(CHEADERS) $(CFLAGS) -o bin/$(ProjectName).x64.exe
	@ echo "[*] DONE"

	@ echo "[*] Signing x64 executable..."
	@ $(ESC) sign -pkcs12 src/sign.pfx -in bin/$(ProjectName).x64.exe -out bin/$(ProjectName).s.x64.exe -pass $(PASSWORD) > /dev/null
	@ echo "[*] DONE"

x86: prep
	@ echo "[*] Compiling x86 metadata..."
	@ $(RC86) -i src/metadata.rc -O coff -o src/metadata.86.o
	@ echo "[*] DONE"

	@ echo "[*] Compiling x86 executable..."
	@ $(CCX86) $(SOURCES) src/metadata.86.o $(CHEADERS) $(CFLAGS) -o bin/$(ProjectName).x86.exe
	@ echo "[*] DONE"

	@ echo "[*] Signing x86 executable..."
	@ $(ESC) sign -pkcs12 src/sign.pfx -in bin/$(ProjectName).x86.exe -out bin/$(ProjectName).s.x86.exe -pass $(PASSWORD) > /dev/null
	@ echo "[*] DONE"

clean:
	@ rm -rf src/*.pem 
	@ rm -rf src/*.pfx
	@ rm -rf src/main.c
	@ rm -rf src/metadata.*.o
	@ rm -rf src/include/apihash.h
	@ rm -rf bin/*.exe