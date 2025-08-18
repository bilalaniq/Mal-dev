# Raw Input Keyboard Logger (Windows API)

This project demonstrates how to capture **raw keyboard input** on Windows using the Win32 API.  
It uses a **message-only window** (`HWND_MESSAGE`) to receive `WM_INPUT` messages from the system,  
which allows it to log key presses even if the application does not have focus.

## Features
- Uses `RegisterRawInputDevices()` to subscribe to keyboard events.
- Creates a hidden **message-only window** with `CreateWindowEx()`.
- Handles input inside a `WindowProc` callback function.
- Extracts `RAWINPUT` data via `GetRawInputData()`.
- Converts scan codes to human-readable key names with `GetKeyNameTextA()`.
- Logs **key down** events (ignores key releases).

## How It Works
1. A hidden message-only window is created.
2. Windows sends all raw keyboard events (`WM_INPUT`) to this window.
3. The `WindowProc` function extracts the raw data.
4. Key names are printed to the console and saved to a file.

## Build Instructions
Compile with MinGW or MSVC:

```sh
gcc main.c -o keylogger.exe -lgdi32 -luser32
````

or with MSVC:

```sh
cl main.c user32.lib gdi32.lib
```

## Notes

* Console output will show each key press with its name.
* A log file is also written for persistence.
* Only works on **Windows** (Win32 API).
* Runs invisibly (no GUI), but keeps a console for output.

### Additional Note

Raw Input in Windows is part of the **GUI message system**.
Windows delivers `WM_INPUT` messages only to windows that:

* Registered a window class (`RegisterClass`).
* Created a window (`CreateWindowEx`).
* Registered devices with `RegisterRawInputDevices`.

A **console by itself is not a GUI window** — it’s managed by the OS, not your app.
So your program can’t receive `WM_INPUT` directly in the console.

👉 That’s why you create a **hidden GUI window** (e.g., with `HWND_MESSAGE`).
It acts as a **mailbox for input events**, while you still log to your console.

⚠️ **Disclaimer:** This code is for **educational purposes only**.
Do not use it for malicious activity (such as keylogging without consent).

