#include <windows.h>
#include <stdio.h>

void PrintRawKeyName(RAWKEYBOARD *kb)
{
    UINT scanCode = kb->MakeCode;

    // Extended keys (arrows, numpad Enter, etc.)
    if (kb->Flags & RI_KEY_E0)
    {
        scanCode |= 0xE000; // same idea as LLKHF_EXTENDED
    }

    char keyName[128];
    if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) > 0)
    {
        printf("Key: %s\n",
               keyName);
    }
    else
    {
        printf("Key: [Unknown vKey=0x%X]\n", kb->VKey);
    }
}

// Register for raw keyboard input
void RegisterRawInput(HWND hWnd)
{
    RAWINPUTDEVICE rid[1]; // Array of input devices (here, only 1 for keyboard)

    rid[0].usUsagePage = 0x01;        // specifies the type of device 0x01 = Generic Desktop Controls, which is a standard HID (Human Interface Device) page for devices like keyboards, mice, and joysticks.
    rid[0].usUsage = 0x06;            // usUsage specifies the specific device type within the usage page. 0x06 = Keyboard.
    rid[0].dwFlags = RIDEV_INPUTSINK; // dwFlags controls how Windows sends input to this app. RIDEV_INPUTSINK = Receive input even when the application window is not in the foreground.
    rid[0].hwndTarget = hWnd;         // Target window (NULL for no window)

    if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0])))
    {
        printf("Failed to register raw input! Error: %d\n", GetLastError());
    }
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) // WindowProc (also called a window procedure) is just a callback function that Windows calls whenever your window receives a message.
{
    //  Window Procedure (often called WndProc) is a callback function that processes messages sent to a window.
    switch (uMsg)
    {
    case WM_INPUT: // WM_INPUT is sent by Windows when a device using Raw Input API (keyboard/mouse) generates input.
    {

        // Here you handle the raw keystrokes.

        UINT dwSize = 0;

        // Get size of raw input data
        GetRawInputData(
            (HRAWINPUT)lParam,       // handle to the raw input  NPUT message gives you a handle to the input data
            RID_INPUT,               // what kind of data you want you’re asking for the actual raw input data
            NULL,                    // buffer to receive data (or NULL) special trick: tells Windows “I don’t want the data yet, just tell me how many bytes I need to allocate.”
            &dwSize,                 // size of the buffer (in/out)
            sizeof(RAWINPUTHEADER)); // tells Windows the size of the header struct so it knows how to fill things.

        // Because raw input packets vary in size (mouse, keyboard, HID devices all have different data lengths).

        // So the correct pattern is always :
        // Call once with NULL → get required size.
        // malloc(dwSize) → allocate buffer.
        // Call again with real buffer to copy the data.

        // Allocate buffer
        LPBYTE lpb = malloc(dwSize);

        // LPBYTE = Long Pointer to BYTE → basically unsigned char*.

        if (!lpb)
            break;

        // For keyboards, you can skip malloc and just declare RAWINPUT raw; on the stack — malloc is only needed if you want to handle all raw input devices safely.

        // Retrieve raw input data
        if (GetRawInputData(
                (HRAWINPUT)lParam,
                RID_INPUT,
                lpb,
                &dwSize,
                sizeof(RAWINPUTHEADER)) != dwSize)
        {
            free(lpb);
            break;
        }

        RAWINPUT *raw = (RAWINPUT *)lpb; // Cast buffer to RAWINPUT *.

        if (raw->header.dwType == RIM_TYPEKEYBOARD) // Check if it’s from a keyboard (RIM_TYPEKEYBOARD).
        {
            // Extract key press info
            USHORT vKey = raw->data.keyboard.VKey;                    // VKey → Virtual-Key code (Windows key code, e.g. 0x41 = ‘A’).
            USHORT scanCode = raw->data.keyboard.MakeCode;            // MakeCode → Scan code (hardware-specific code for the key).
            BOOL isKeyUp = (raw->data.keyboard.Flags & RI_KEY_BREAK); // RI_KEY_BREAK indicates if the key is released (up) or pressed (down).

            if (!isKeyUp)
            {
                // Only log when the key is pressed (not released)
                PrintRawKeyName(&raw->data.keyboard);
            }
        }

        free(lpb);
        break;
    }
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

HWND CreateHiddenWindow()
{
    WNDCLASS wc = {0};                    // Initializes a window class structure. It defines a window class, which is basically a blueprint for creating windows.
    wc.lpfnWndProc = WindowProc;          // lpfnWndProc = pointer to window procedure.
    wc.hInstance = GetModuleHandle(NULL); // hInstance is a handle to the executable module that is creating this window class. GetModuleHandle(NULL) returns the current program’s handle.
    wc.lpszClassName = "RawInputLogger";  // It’s a string pointer that gives a name to your window class.

    RegisterClass(&wc); // “Here is a window class definition (WNDCLASS). Please register it with the system so I can later create windows of this type.”

    return CreateWindowEx(
        0,                //  [in] dwExStyle  0 = no special styles.
        "RawInputLogger", //  lpClassName Name of the window class. Must match the wc.lpszClassName you registered earlier with RegisterClass.
        NULL,             // lpWindowName  Window title (NULL means no title).
        0,                // dwStyle 0 = none → no border, no caption, no visibility.
        0,
        0,
        0,
        0,
        // (x, y, width, height)
        // Position and size of the window.
        // All zero = irrelevant, since the window won’t be shown.

        HWND_MESSAGE, // HWND_MESSAGE tells Windows: “Make this a message-only window.”
        NULL,
        NULL,
        NULL

        // NULL, NULL, NULL (hMenu, hInstance, lpParam)
        // hMenu = NULL → no menu.
        // hInstance = NULL → normally the app instance, but message-only windows don’t need it.
        // lpParam = NULL → no extra parameters passed to WM_CREATE.
    );

    // That call creates a hidden message-only window named "RawInputLogger" that exists only to receive and process Windows messages (like raw keyboard input), never visible on screen.
}

int main()
{
    HWND hWnd = CreateHiddenWindow();
    RegisterRawInput(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Raw Input in Windows is part of the GUI message system.
// Windows delivers WM_INPUT messages only to windows that:

// Registered a window class (RegisterClass).

// Created a window (CreateWindowEx).

// Registered devices with RegisterRawInputDevices.

// A console by itself is not a GUI window — it’s managed by the OS, not your app. So your program can’t “receive” WM_INPUT directly in the console.

// 👉 That’s why you create a hidden GUI window (like with HWND_MESSAGE).
// It acts as a mailbox for input events, while you still log to your console.