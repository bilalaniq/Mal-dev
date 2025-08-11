#include <windows.h>
#include <stdio.h>




void PrintKeyName(KBDLLHOOKSTRUCT *pKey) {
    // Get scan code
    UINT scanCode = pKey->scanCode;
    
    if (pKey->flags & LLKHF_EXTENDED) {
        scanCode |= 0xE000;
    }
     // Some keys (arrows, Insert, Delete, Home, End, Page Up/Down, etc.) have an "extended" flag set. 
     // If LLKHF_EXTENDED is set, you must set bit 14 of the scan code (0xE000) so that GetKeyNameTextA() knows it’s extended and gives the right name.

    // Buffer for key name
    char keyName[128];
    if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) > 0) {    //GetKeyNameTextA() works on scan codes, not virtual key codes.
        printf("Key pressed: %s\n", keyName);
    } else {
        printf("Key pressed: [Unknown: 0x%X]\n", pKey->vkCode);
    }
}



HHOOK g_hHook = NULL;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
            PrintKeyName(pKey);
        }
    }
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void SetHook() {
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (g_hHook == NULL) {
        printf("Failed to install hook!\n");
    }
}

void RemoveHook() {
    if (g_hHook != NULL) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }
}

int main() {
    SetHook();
    if (g_hHook == NULL) {
        return 1; // failed to set hook
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    RemoveHook();
    return 0;
}
