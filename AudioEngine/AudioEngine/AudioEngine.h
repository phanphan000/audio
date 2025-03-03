#pragma once

#include <windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <filesystem>
#include "bass.h"

using namespace std;
namespace fs = filesystem;

extern void playMusic();
extern vector<string> playlist;
extern bool isPlaying;
extern size_t currentTrack;
extern HSTREAM stream;

double getTrackDuration(HSTREAM stream);
double getVolume();
void setVolume(float volume);
void displayTrackInfo();
void loadPlaylist();
void stopMusic();
void pauseMusic();
void nextTrack();
void prevTrack();
void handleCommand(const string& command);
void playMusic();
void startMainController();