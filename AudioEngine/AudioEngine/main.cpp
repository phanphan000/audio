#include "AudioEngine.h"

#define PIPE_NAME TEXT("\\\\.\\pipe\\AudioPipe")

int main() {
    cout << "===== AudioEngine =====\n";
    startMainController();

    if (!BASS_Init(-1, 44100, 0, 0, NULL)) {
        cout << "[AudioEngine] Loi khoi tao BASS! (Loi: " << BASS_ErrorGetCode() << ")\n";
        return 1;
    }

    loadPlaylist();
    playMusic();

    HANDLE hPipe = CreateNamedPipe(PIPE_NAME, PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 0, 0, 0, NULL);

    while (true) {
        if (ConnectNamedPipe(hPipe, NULL) || GetLastError() == ERROR_PIPE_CONNECTED) {
            char buffer[256];
            DWORD bytesRead;

            if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL)) {
                buffer[min(bytesRead, sizeof(buffer) - 1)] = '\0';
                string command(buffer);

                if (command == "exit") break;
                handleCommand(command);
            }

            DisconnectNamedPipe(hPipe);
        }
    }

    BASS_Free();
    CloseHandle(hPipe);
    return 0;
}
