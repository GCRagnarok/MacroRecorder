#include <glew.h>
#include <glfw3.h>
#include <iostream>
#include "../Include/ImGuiApp.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

// Constructor and Destructor ------------------------------------------------------

ImGuiApp::ImGuiApp() 
    : m_TextColorRed(1.0f, 0.0f, 0.0f, 1.0f),
    m_TextColorGreen(0.0f, 1.0f, 0.0f, 1.0f),
    m_TextColorYellow(1.0f, 1.0f, 0.0f, 1.0f)
{
	Init();
    m_MacroManager = new MacroManager(*this);
}

ImGuiApp::~ImGuiApp()
{
	Clean();
}

// Main Loop Functions -------------------------------------------------------------

void ImGuiApp::Init()
{
    std::cout << "Initializing ImGuiApp with GLFW and OpenGL..." << std::endl;

    // Initialize GLFW
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");

    // Create a GLFW window
    int windowWidth = 640;
    int windowHeight = 240;
    m_Window = glfwCreateWindow(windowWidth, windowHeight, "Macro Manager", NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    // Get the primary monitor
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (!primaryMonitor)
        throw std::runtime_error("Failed to get primary monitor");

    // Get the monitor's work area
    int monitorX, monitorY, monitorWidth, monitorHeight;
    glfwGetMonitorWorkarea(primaryMonitor, &monitorX, &monitorY, &monitorWidth, &monitorHeight);

    // Calculate the centered position
    int windowPosX = monitorX + (monitorWidth - windowWidth) / 2;
    int windowPosY = monitorY + (monitorHeight - windowHeight) / 2;

    // Set the window position
    glfwSetWindowPos(m_Window, windowPosX, windowPosY);

    // Make the OpenGL context current
    glfwMakeContextCurrent(m_Window);

    // Enable V-Sync
    glfwSwapInterval(1);

    // Set minimum window size
    glfwSetWindowSizeLimits(m_Window, windowWidth, windowHeight, GLFW_DONT_CARE, GLFW_DONT_CARE);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        throw std::runtime_error("Failed to initialize GLEW");

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Set ImGui style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();

    // Load custom fonts
    io.Fonts->AddFontFromFileTTF("assets/fonts/dogica/dogicapixel.ttf", 8.0f); // default font
    io.Fonts->AddFontFromFileTTF("assets/fonts/dogica/dogicapixel.ttf", 6.0f); // small font
    io.Fonts->AddFontFromFileTTF("assets/fonts/dogica/dogicapixel.ttf", 32.0f); // large font
    io.Fonts->AddFontFromFileTTF("assets/fonts/dogica/dogicapixel.ttf", 22.0f); // H1 font
    io.Fonts->AddFontFromFileTTF("assets/fonts/dogica/dogicapixel.ttf", 16.0f); // H2 font
    io.Fonts->AddFontFromFileTTF("assets/fonts/dogica/dogicapixel.ttf", 10.0f); // H3 font

    // Change layout properties
    style.WindowPadding = ImVec2(15, 15);
    style.WindowRounding = 0.0f;
    style.FramePadding = ImVec2(5, 5);
    style.FrameRounding = 0.0f;
    style.ItemSpacing = ImVec2(12, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 15.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 5.0f;
    style.GrabRounding = 3.0f;

    // Initialize ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void ImGuiApp::Run()
{
    std::cout << "Running ImGuiApp..." << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(m_Window))
    { 
        // Poll and handle events
        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the GUI
        Render();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(m_Window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(m_Window);
    }
}

void ImGuiApp::Render()
{
    // Get the size of the GLFW window
    int glfwWindow_w, glfwWindow_h;
    glfwGetWindowSize(m_Window, &glfwWindow_w, &glfwWindow_h);

    float titleWindow_h = 50.0f;

    // Title window
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)glfwWindow_w, titleWindow_h));
    ImGui::Begin("Title Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
    CenteredText("Macro Manager");
    ImGui::PopFont();

    ImGui::Spacing();

    ImGui::End();

    // Record window
    ImGui::SetNextWindowPos(ImVec2(0, titleWindow_h));
    ImGui::SetNextWindowSize(ImVec2((float)glfwWindow_w * 0.5f, (float)glfwWindow_h - titleWindow_h));
    ImGui::Begin("Record Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
    CenteredText("Record");
    ImGui::PopFont();

    ImGui::Spacing();

    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(30.0f, 30.0f)); 
    ImGui::Spacing();
    ImGui::PopStyleVar();
    RecordMacroButton();

    ImGui::End();

    // Playback window
    ImGui::SetNextWindowPos(ImVec2((float)glfwWindow_w * 0.5f, titleWindow_h));
    ImGui::SetNextWindowSize(ImVec2((float)glfwWindow_w * 0.5f, (float)glfwWindow_h - titleWindow_h));
    ImGui::Begin("Playback Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
    CenteredText("Playback");
    ImGui::PopFont();

    ImGui::Spacing();

    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(30.0f, 30.0f));
    ImGui::Spacing();
    ImGui::PopStyleVar();

    PlayMacroButton();

    ImGui::End();
}

void ImGuiApp::Clean()
{
    std::cout << "Cleaning up ImGuiApp..." << std::endl;

    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup GLFW
    glfwDestroyWindow(m_Window);
    glfwTerminate();

}
// Macro Manager Functions -------------------------------------------------------------

void ImGuiApp::RecordMacroButton()
{
    const char* recordButtonLabel = m_MacroManager->m_IsRecording ? "Stop Recording Macro" : "Start Recording Macro";

	CenteredButton(recordButtonLabel, [this]()
	{
       if (!m_MacroManager->m_IsRecording && !m_MacroManager->m_IsPlaying)
	    {
			// Start recording macro
            m_MacroManager->StartRecordMacro();
			
		}
		else
		{
			// Stop recording macro
            m_MacroManager->StopRecordMacro();

		}
    });

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 20.0f));
    ImGui::Spacing();
    ImGui::PopStyleVar();

    if (m_MacroManager->m_IsRecording)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColorYellow);
        CenteredText("Macro Recording in Progress...");
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    if (!m_MacroManager->m_macroHasBeenRecorded && !m_MacroManager->m_IsRecording)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColorRed);
        CenteredText("No Macro Recorded");
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    else if (m_MacroManager->m_macroHasBeenRecorded)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColorGreen);
        CenteredText("Macro Recorded!");
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
}


void ImGuiApp::PlayMacroButton()
{
    const char* playbackButtonLabel = m_MacroManager->m_IsPlaying ? "Stop Macro Playback" : "Start Macro Playback";

    CenteredButton(playbackButtonLabel, [this]()
    {
        if (!m_MacroManager->m_IsPlaying)
        {
            // Start playback
            m_MacroManager->StartPlayMacro();
        }
        else
        {
            // Stop playback
            m_MacroManager->StopPlayMacro();
        }
    });

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20.0f, 20.0f));
    ImGui::Spacing();
    ImGui::PopStyleVar();

    if (m_MacroManager->m_IsPlaying)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColorYellow);
        CenteredText("Macro Playback in Progress...");
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    else if (m_MacroManager->m_macroHasBeenRecorded)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColorGreen);
        CenteredText("Macro Ready for Playback!");
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
    else
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::PushStyleColor(ImGuiCol_Text, m_TextColorRed);
        CenteredText("No Macro Ready to Play");
        ImGui::PopFont();
        ImGui::PopStyleColor();
    }
}

// ImGui Helper Functions --------------------------------------------------------------

void ImGuiApp::CenteredText(const std::string& text)
{
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

    float textPosX = (windowSize.x - textSize.x) * 0.5f;

    if (textPosX < 0.0f)
    {
        textPosX = 0.0f;
    }

    ImGui::SetCursorPosX(textPosX);

    ImGui::Text("%s", text.c_str());
}

void ImGuiApp::CenteredButton(const char* label, std::function<void()> onClick)
{
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 buttonSize = ImGui::CalcTextSize(label);
    buttonSize.x += ImGui::GetStyle().FramePadding.x * 2.0f;
    buttonSize.y += ImGui::GetStyle().FramePadding.y * 2.0f;

    float buttonPosX = (windowSize.x - buttonSize.x) * 0.5f;

    if (buttonPosX < 0.0f)
    {
        buttonPosX = 0.0f;
    }

    ImGui::SetCursorPosX(buttonPosX);

    if (ImGui::Button(label))
    {
        onClick();
    }
}