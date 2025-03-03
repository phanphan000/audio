#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <conio.h>
#include <windowsx.h>
#include <tchar.h>

using namespace std;
#define PIPE_NAME TEXT("\\\\.\\pipe\\AudioPipe")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void sendCommand(const string& command);

HWND hwndPlay, hwndPause, hwndNext, hwndBack, hwndVolUp, hwndVolDown, hwndStop;

//void sendCommand(const string& command) {
//    HANDLE hPipe;
//    DWORD bytesWritten;
//
//    // Kết nối đến Named Pipe của AudioEngine
//    hPipe = CreateFile(PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
//
//    if (hPipe == INVALID_HANDLE_VALUE) {
//        cout << "[MainController] Khong the ket noi den AudioEngine! (Loi: " << GetLastError() << ")\n";
//        return;
//    }
//
//    // Gửi lệnh qua Named Pipe
//    if (WriteFile(hPipe, command.c_str(), command.length() + 1, &bytesWritten, NULL)) {
//        string message = "Da gui lenh: " + command;
//        MessageBoxA(NULL, message.c_str(), "Thong bao", MB_OK);
//    }
//    else {
//        MessageBoxA(NULL, "Loi khi gui lenh!", "Loi", MB_OK | MB_ICONERROR);
//    }
//    
//
//    CloseHandle(hPipe);
//}
void sendCommand(const string& command) {
    cout << "[DEBUG] Dang gui lenh: " << command << endl;
    HANDLE hPipe = NULL;
    
    int retry = 5; // Thử lại 5 lần
    while (retry--) {
        hPipe = CreateFile(PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE) break; // Nếu mở thành công thì thoát

        cout << "[ERROR] Khong the ket noi, thu lai... (" << GetLastError() << ")\n";
        Sleep(500); // Đợi 0.5s trước khi thử lại
    }

    if (hPipe == INVALID_HANDLE_VALUE) {
        cout << "[ERROR] Van khong ket noi duoc sau 5 lan! (Loi: " << GetLastError() << ")\n";
        return;
    }

    DWORD bytesWritten;
    if (WriteFile(hPipe, command.c_str(), command.length() + 1, &bytesWritten, NULL)) {
        cout << "[DEBUG] Lenh da duoc gui: " << command << endl;
    }
    else {
        cout << "[ERROR] Loi khi gui lenh! (Loi: " << GetLastError() << ")\n";
    }

    CloseHandle(hPipe);
}



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("MainControllerWindow");
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, TEXT("Main Controller"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 250, NULL, NULL, hInstance, NULL);

    hwndPlay = CreateWindow(TEXT("BUTTON"), TEXT("Play"), WS_VISIBLE | WS_CHILD,
        20, 20, 100, 30, hwnd, (HMENU)1, hInstance, NULL);
    hwndPause = CreateWindow(TEXT("BUTTON"), TEXT("Pause"), WS_VISIBLE | WS_CHILD,
        150, 20, 100, 30, hwnd, (HMENU)2, hInstance, NULL);
    hwndNext = CreateWindow(TEXT("BUTTON"), TEXT("Next"), WS_VISIBLE | WS_CHILD,
        150, 60, 100, 30, hwnd, (HMENU)3, hInstance, NULL);
    hwndBack = CreateWindow(TEXT("BUTTON"), TEXT("Back"), WS_VISIBLE | WS_CHILD,
        20, 100, 100, 30, hwnd, (HMENU)4, hInstance, NULL);
    hwndVolUp = CreateWindow(TEXT("BUTTON"), TEXT("Vol +"), WS_VISIBLE | WS_CHILD,
        150, 100, 100, 30, hwnd, (HMENU)5, hInstance, NULL);
    hwndVolDown = CreateWindow(TEXT("BUTTON"), TEXT("Vol -"), WS_VISIBLE | WS_CHILD,
        20, 140, 100, 30, hwnd, (HMENU)6, hInstance, NULL);
    hwndStop = CreateWindow(TEXT("BUTTON"), TEXT("Exit"), WS_VISIBLE | WS_CHILD,
        20, 60, 100, 30, hwnd, (HMENU)7, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 1: sendCommand("play"); break;
        case 2: sendCommand("pause"); break;
        case 3: sendCommand("next"); break;
        case 4: sendCommand("back"); break;
        case 5: sendCommand("+"); break;
        case 6: sendCommand("-"); break;
        case 7: sendCommand("exit"); PostQuitMessage(0); break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
