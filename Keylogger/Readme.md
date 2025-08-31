# Keylogger Collection

This folder contains multiple implementations of keyloggers for educational and research purposes. **Do not use for illegal purposes**. The examples demonstrate different methods for capturing keystrokes on Windows systems.

---

## Directory Structure

| Folder Name                           | Description                                                                                     |
| ------------------------------------- | ----------------------------------------------------------------------------------------------- |
| [DLL-injection](./DLL-injection/)     | Keyloggers that use DLL injection into target processes to capture keystrokes.                  |
| [Hooking-based](./Hooking-based/)     | Keyloggers that use Windows API hooks (e.g., `SetWindowsHookEx`) to intercept keyboard events.  |
| [Polling-based](./Polling-based/)     | Keyloggers that periodically poll the keyboard state to record pressed keys.                    |
| [Raw-Input-based](./Raw-Input-based/) | Keyloggers that use the Windows Raw Input API for low-level keyboard event monitoring.          |
| [Service](./Service/)                 | Keyloggers implemented as Windows Services, running in the background with elevated privileges. |

---

## Usage

- Each folder contains source code examples and documentation for the corresponding keylogging technique.  
- Compile with a suitable C/C++ compiler (e.g., Visual Studio `cl.exe` or MinGW).  
- Run only in a **controlled lab environment**.  

---

## Purpose

This collection is intended for:

- Learning Windows internals and keyboard event handling.  
- Security research and defensive testing.  
- Understanding keylogger detection techniques.  

**Warning:** Never deploy on machines without explicit consent. Unauthorized keylogging is illegal.

---

