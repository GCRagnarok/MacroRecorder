#pragma once

#include <thread>
#include <atomic>
#include <vector>
#include <utility>
#include <Windows.h>
#include <mutex>

class ImGuiApp;

using KeyEvent = std::pair<int, DWORD>;

class MacroManager
{
public:
	MacroManager(ImGuiApp& p_ImGuiApp);
	~MacroManager();

	void StartRecordMacro();
	void StopRecordMacro();


	void StartPlayMacro();
	void StopPlayMacro();

	std::atomic<bool> m_IsRecording;
	std::atomic<bool> m_IsPlaying;

	std::atomic<bool> m_macroHasBeenRecorded;

private:
	ImGuiApp& m_ImGuiApp;
	std::thread m_RecordingThread;
	std::thread m_PlaybackThread;
	std::vector<KeyEvent> m_recordedKeys;
	std::mutex m_RecordedKeysMutex;

	bool m_keysPressed;
};