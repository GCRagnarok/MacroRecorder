#include "../include/MacroManager.h"
#include "../Include/ImGuiApp.h"
#include <vector>
#include <utility>
#include <chrono>
#include <thread>
#include <iostream>

MacroManager::MacroManager(ImGuiApp& p_ImGuiApp)
	: m_ImGuiApp(p_ImGuiApp)
{
	// Constructor implementation
}

MacroManager::~MacroManager()
{
    // Ensure threads are properly joined before destruction
    if (m_RecordingThread.joinable())
    {
        m_IsRecording = false;
        m_RecordingThread.join();
    }
    if (m_PlaybackThread.joinable())
    {
        m_IsPlaying = false;
        m_PlaybackThread.join();
    }
}

void MacroManager::StartRecordMacro()
{
    if (m_IsRecording)
    {
        std::cout << "Already recording." << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_RecordedKeysMutex);
        m_recordedKeys.clear();
    }

    m_IsRecording = true;

    // Initialize the macro recorded flag to false
    m_macroHasBeenRecorded = false;

    m_keysPressed = false;

    std::cout << "Recording macro..." << std::endl;

    // Capture the start time of the recording
    DWORD recordingStartTime = GetTickCount64();

    m_RecordingThread = std::thread([this, recordingStartTime]()
    {
        while (m_IsRecording)
        {
            for (int key = 0x08; key <= 0xFF; ++key) // Check all possible key codes
            {
                if (GetAsyncKeyState(key) & 0x8000) // Key is pressed
                {
                    DWORD currentTime = GetTickCount64();

                    {
                        std::lock_guard<std::mutex> lock(m_RecordedKeysMutex);
                        // Record the key and the timestamp relative to the recording start time
                        m_recordedKeys.emplace_back(key, currentTime - recordingStartTime);
                    }

                    // Set the macro recorded flag to true
                    m_keysPressed = true;

                    // Wait until the key is released
                    while (GetAsyncKeyState(key) & 0x8000)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Small delay to prevent excessive CPU usage
                    }
                }
            }
        }
    });
}

void MacroManager::StopRecordMacro()
{
    if (!m_IsRecording)
    {
        std::cout << "Cannot record during playback!" << std::endl;
        return;
    }

    m_IsRecording = false;

	if (m_keysPressed)
	{
		m_macroHasBeenRecorded = true;
	}
    else
    {
        m_macroHasBeenRecorded = false;
    }

    if (m_RecordingThread.joinable())
    {
        m_RecordingThread.join();
    }

    std::cout << "Recording stopped." << std::endl;
}

void MacroManager::StartPlayMacro()
{
    if (m_IsPlaying)
    {
        std::cout << "Already playing back a macro." << std::endl;
        return;
    }

    {
        std::lock_guard<std::mutex> lock(m_RecordedKeysMutex);
        if (m_recordedKeys.empty())
        {
            std::cout << "No macro recorded to play." << std::endl;
            return;
        }
    }

    m_IsPlaying = true;

    std::cout << "Playing back macro..." << std::endl;

    m_PlaybackThread = std::thread([this]()
        {
            while (m_IsPlaying)
            {
                DWORD initialDelay;

                {
                    std::lock_guard<std::mutex> lock(m_RecordedKeysMutex);
                    initialDelay = m_recordedKeys.front().second; // Get the timestamp of the first key
                }

                // Introduce the initial delay before the first key of each iteration
                std::this_thread::sleep_for(std::chrono::milliseconds(initialDelay));

                DWORD previousTimestamp = 0;

                for (const auto& keyEvent : m_recordedKeys)
                {
                    if (!m_IsPlaying)
                    {
                        std::cout << "Playback interrupted." << std::endl;
                        return;
                    }

                    int key;
                    DWORD timestamp;

                    {
                        std::lock_guard<std::mutex> lock(m_RecordedKeysMutex);
                        key = keyEvent.first;
                        timestamp = keyEvent.second;
                    }

                    // Calculate the delay relative to the previous key press
                    DWORD delay = (previousTimestamp == 0) ? 0 : (timestamp - previousTimestamp);
                    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

                    // Simulate key press
                    keybd_event(key, 0, 0, 0); // Key down
                    keybd_event(key, 0, KEYEVENTF_KEYUP, 0); // Key up

                    previousTimestamp = timestamp; // Update the previous timestamp
                }
            }
        });
}

void MacroManager::StopPlayMacro()
{
    if (!m_IsPlaying)
    {
        std::cout << "Not currently playing back a macro." << std::endl;
        return;
    }

    m_IsPlaying = false;

    if (m_PlaybackThread.joinable())
    {
        m_PlaybackThread.join();
    }

    std::cout << "Playback stopped." << std::endl;
}