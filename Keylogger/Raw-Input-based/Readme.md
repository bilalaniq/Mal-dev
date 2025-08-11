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