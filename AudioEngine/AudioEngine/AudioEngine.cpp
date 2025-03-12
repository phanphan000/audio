#include "AudioEngine.h"

vector<string> playlist;
bool isPlaying = false;
bool isSpeedX2 = false;
float defaultFreq = 44100.0f;
size_t currentTrack = 0;
HSTREAM stream;

// 🔹 Hàm tăng tốc độ
void setPlaybackSpeed(bool speedX2) {
    if (!stream) return;

    float newFreq = speedX2 ? defaultFreq * 2 : defaultFreq;
    BASS_ChannelSetAttribute(stream, BASS_ATTRIB_FREQ, newFreq);

    cout << "[AudioMp3] Toc do phat nhac: " << (speedX2 ? "x2" : "x1") << endl;
}

// 🔹 Lấy thời lượng bài hát (giây)
double getTrackDuration(HSTREAM stream) {
    QWORD len = BASS_ChannelGetLength(stream, BASS_POS_BYTE);
    return BASS_ChannelBytes2Seconds(stream, len);
}

// 🔹 Lấy âm lượng hiện tại
double getVolume() {
    float volume;
    BASS_ChannelGetAttribute(stream, BASS_ATTRIB_VOL, &volume);
    return volume * 100; // Chuyển thành %
}

// 🔹 Đặt âm lượng
void setVolume(float volume) {
    volume = max(0.0f, min(1.0f, volume)); // Giới hạn từ 0 đến 1
    BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, volume);
    cout << "[AudioMp3] Am luong moi: " << (int)(volume * 100) << "%" << endl;
}

// 🔹 Hiển thị thông tin bài hát
void displayTrackInfo() {
    if (!stream) return;
    double duration = getTrackDuration(stream);
    double volume = getVolume();

    cout << "[AudioMp3] Dang phat: " << playlist[currentTrack] << endl;
    cout << "[AudioMp3] Thoi gian: " << duration << " giay" << endl;
    cout << "[AudioMp3] Am luong: " << volume << "%" << endl;
}

// 🔹 Load danh sách nhạc từ thư mục LofiChill
void loadPlaylist() {
    playlist.clear();
    string folderPath = "../../LofiChill";

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".mp3") {
            playlist.push_back(entry.path().u8string());
        }
    }

    if (playlist.empty()) {
        cout << "[AudioMp3] Khong tim thay file nhac trong " << folderPath << "!\n";
    }
    else {
        cout << "[AudioMp3] Da load " << playlist.size() << " bai nhac.\n";
    }
}

// 🔹 Dừng nhạc
//void stopMusic() {
//    BASS_ChannelStop(stream);
//    cout << "[AudioEngine] Nhac da dung.\n";
//}

// 🔹 Tạm dừng
void pauseMusic() {
    BASS_ChannelPause(stream);
    cout << "[AudioMp3] Nhac da tam dung.\n";
}

// 🔹 Next bài hát
void nextTrack() { 
    if (playlist.empty()) return;
    if (stream) {
        BASS_ChannelStop(stream);
        BASS_StreamFree(stream);
        stream = 0;
    }

    currentTrack = (currentTrack + 1) % playlist.size();
    playMusic();
}

// 🔹 Back bài hát
void prevTrack() {
    if (playlist.empty()) return;
    if (stream) {
        BASS_ChannelStop(stream);
        BASS_StreamFree(stream);
        stream = 0;
    }
    currentTrack = (currentTrack == 0) ? playlist.size() - 1 : currentTrack - 1;
    playMusic();
}

// 🔹 Nhận lệnh từ MainController
void handleCommand(const string& command) {
    if (command == "play") playMusic();
    else if (command == "pause") pauseMusic();
    else if (command == "next") nextTrack();
    else if (command == "back") prevTrack();
    else if (command == "+") setVolume(getVolume() / 100.0f + 0.1f);
    else if (command == "-") setVolume(getVolume() / 100.0f - 0.1f);
    else if (command == "speedx2") {
        isSpeedX2 = true;
        setPlaybackSpeed(isSpeedX2);
    }
    else if (command == "speedx1") {
        isSpeedX2 = false;
        setPlaybackSpeed(isSpeedX2);
    }

    else cout << "[AudioMp3] Lenh khong hop le!\n";
}

// 🔹 Phát bài hát hiện tại
void playMusic() {
    if (playlist.empty()) {
        cout << "[AudioMp3] Chua co bai nhac nao trong danh sach!\n";
        return;
    }

    if (stream) {
        BASS_ChannelPlay(stream,FALSE);
        isPlaying = true;
        cout <<"[AudioMp3] Tiep tuc phat nhac.\n";
        return;
    }

    stream = BASS_StreamCreateFile(FALSE, playlist[currentTrack].c_str(), 0, 0, 0);

    if (!stream) {
        cout << "[AudioMp3] Loi phat nhac! (Loi: " << BASS_ErrorGetCode() << ")\n";
    }
    else {
        BASS_ChannelPlay(stream, FALSE);
        isPlaying = true;
        displayTrackInfo();
        // tạo 1 thread để kiểm tra khi bài hát kết thúc => next
        thread([]() {
            while (isPlaying) {
                if (BASS_ChannelIsActive(stream) == BASS_ACTIVE_STOPPED) {
                    isPlaying = false;
                    nextTrack();
                    break;
                }
                this_thread::sleep_for(chrono::milliseconds(500));
            }
            }).detach();
    }
}

void startMainController() {
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    // Chỉnh lại đường dẫn chính xác
    wchar_t cmdLine[] = L"..\\..\\MainController\\Debug\\MainController.exe";

    if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        /*wcout << L"[AudioMp3] Da mo MainController.exe\n";*/
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        wcout << L"[AudioMp3] Khong the mo MainController.exe - Loi: " << GetLastError() << endl;
    }
}

