#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std; 
bool isSpeedx2 = false;
#define PIPE_NAME TEXT("\\\\.\\pipe\\AudioPipe")

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void sendCommand(const string& command);

HWND hwndPlay, hwndPause, hwndNext, hwndBack, hwndVolUp, hwndVolDown, hwndStop, hwndSpeedX2;

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
    HWND hwndSpeed;
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("MainControllerWindow");
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, TEXT("Main Controller"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, NULL, NULL, hInstance, NULL);

    hwndPlay = CreateWindow(TEXT("BUTTON"), TEXT("Play"), WS_VISIBLE | WS_CHILD,
        50, 50, 100, 30, hwnd, (HMENU)1, hInstance, NULL);
    hwndBack = CreateWindow(TEXT("BUTTON"), TEXT("Back"), WS_VISIBLE | WS_CHILD,
        50, 120, 100, 30, hwnd, (HMENU)4, hInstance, NULL);
    hwndVolDown = CreateWindow(TEXT("BUTTON"), TEXT("Vol -"), WS_VISIBLE | WS_CHILD,
        50, 190, 100, 30, hwnd, (HMENU)6, hInstance, NULL);
    hwndSpeedX2 = CreateWindow(TEXT("BUTTON"), TEXT("Speed X2"), WS_VISIBLE | WS_CHILD,
        50, 260, 100, 30, hwnd, (HMENU)8, hInstance, NULL);
    hwndPause = CreateWindow(TEXT("BUTTON"), TEXT("Pause"), WS_VISIBLE | WS_CHILD,
        230, 50, 100, 30, hwnd, (HMENU)2, hInstance, NULL);
    hwndNext = CreateWindow(TEXT("BUTTON"), TEXT("Next"), WS_VISIBLE | WS_CHILD,
        230, 120, 100, 30, hwnd, (HMENU)3, hInstance, NULL);
    hwndVolUp = CreateWindow(TEXT("BUTTON"), TEXT("Vol +"), WS_VISIBLE | WS_CHILD,
        230, 190, 100, 30, hwnd, (HMENU)5, hInstance, NULL);
    hwndStop = CreateWindow(TEXT("BUTTON"), TEXT("Exit"), WS_VISIBLE | WS_CHILD,
        230, 260, 100, 30, hwnd, (HMENU)7, hInstance, NULL);

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
        case 8: // Nút Speed X2
            isSpeedx2 = !isSpeedx2; // Đảo trạng thái
            if (isSpeedx2) {
                sendCommand("speedx2");
                SetWindowText(hwndSpeedX2, TEXT("Speed X1")); // Đổi tên nút
            }
            else {
                sendCommand("speedx1");
                SetWindowText(hwndSpeedX2, TEXT("Speed X2")); // Đổi lại tên
            }
            break;

        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
   
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
