// dx8timer.cpp
//

#include "../GameRenderer.h"

// Global (or static) variables
static IDirect3DQuery9* g_pFrequencyQuery = nullptr;
static IDirect3DQuery9* g_pTimeStartQuery = nullptr;
static IDirect3DQuery9* g_pTimeEndQuery = nullptr;
static bool             g_QueriesInitialized = false;

// Timings (in milliseconds):
static double g_ClientCpuTimeMs = 0.0;
static double g_ServerCpuTimeMs = 0.0;
static double g_PresentCpuTimeMs = 0.0;
static double g_GpuTimeMs = 0.0;
static UINT64 g_GpuFrequency = 0;

// CPU timing data:
static LARGE_INTEGER g_ClientFrameStart = { 0 };
static LARGE_INTEGER g_ServerFrameStart = { 0 };
static LARGE_INTEGER g_GpuFrameStart = { 0 };
static LARGE_INTEGER g_PresentFrameStart = { 0 };
static double        g_CpuTickFrequency = 0.0;

// Thresholds for coloring
static const float GOOD_THRESHOLD_MS = 8.0f;  // Green if <= 8 ms
static const float OK_THRESHOLD_MS = 16.0f; // Yellow if <=16 ms (otherwise red)

// Call once, after device creation, to set up GPU queries and CPU frequency.
void DX8Wrapper::InitializeTimingQueries(IDirect3DDevice9* device)
{
	if (!g_QueriesInitialized && device)
	{
		// Create queries for GPU timing
		device->CreateQuery(D3DQUERYTYPE_TIMESTAMPFREQ, &g_pFrequencyQuery);
		device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &g_pTimeStartQuery);
		device->CreateQuery(D3DQUERYTYPE_TIMESTAMP, &g_pTimeEndQuery);

		g_pFrequencyQuery->Issue(D3DISSUE_END);

		// Block until the GPU frequency is available
		while (S_OK != g_pFrequencyQuery->GetData(&g_GpuFrequency, sizeof(g_GpuFrequency), D3DGETDATA_FLUSH))
		{
			// Busy-wait loop; in production code you might sleep or yield here.
		}

		// Setup CPU frequency
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		g_CpuTickFrequency = static_cast<double>(freq.QuadPart);

		g_QueriesInitialized = true;
	}
}

//------------------------------------------------------------------------------
// Client CPU start/end
//------------------------------------------------------------------------------
void StartClientCpuFrameTimer()
{
	if (!g_QueriesInitialized)
		return;

	QueryPerformanceCounter(&g_ClientFrameStart);
}

void EndClientCpuFrameTimer()
{
	if (!g_QueriesInitialized)
		return;

	LARGE_INTEGER cpuEnd;
	QueryPerformanceCounter(&cpuEnd);

	double cpuDelta = static_cast<double>(cpuEnd.QuadPart - g_ClientFrameStart.QuadPart);
	g_ClientCpuTimeMs = (cpuDelta * 1000.0) / g_CpuTickFrequency;
}

//------------------------------------------------------------------------------
// Server CPU start/end
//------------------------------------------------------------------------------
void StartServerCpuFrameTimer()
{
	if (!g_QueriesInitialized)
		return;

	QueryPerformanceCounter(&g_ServerFrameStart);
}

void EndServerCpuFrameTimer()
{
	if (!g_QueriesInitialized)
		return;

	LARGE_INTEGER cpuEnd;
	QueryPerformanceCounter(&cpuEnd);

	double cpuDelta = static_cast<double>(cpuEnd.QuadPart - g_ServerFrameStart.QuadPart);
	g_ServerCpuTimeMs = (cpuDelta * 1000.0) / g_CpuTickFrequency;
}

void StartPresentCpuFrameTimer()
{
	if (!g_QueriesInitialized) return;

	QueryPerformanceCounter(&g_PresentFrameStart);
}

void EndPresentCpuFrameTimer()
{
	if (!g_QueriesInitialized) return;

	LARGE_INTEGER cpuEnd;
	QueryPerformanceCounter(&cpuEnd);

	double cpuDelta = static_cast<double>(cpuEnd.QuadPart - g_PresentFrameStart.QuadPart);
	g_PresentCpuTimeMs = (cpuDelta * 1000.0) / g_CpuTickFrequency;
}


//------------------------------------------------------------------------------
// GPU start/end
//   Uses D3D queries to measure GPU execution time.
//------------------------------------------------------------------------------
void StartGpuFrameTimer()
{
	if (!g_QueriesInitialized)
		return;

	// Just record the CPU time
	QueryPerformanceCounter(&g_GpuFrameStart);
	g_pTimeStartQuery->Issue(D3DISSUE_END);
}

void EndGpuFrameTimer()
{
	if (!g_QueriesInitialized)
		return;

	g_pTimeEndQuery->Issue(D3DISSUE_END);
	
	// Retrieve the data from both queries
	// We must wait for both start/end timestamps to be ready
	UINT64 startTime = 0;
	UINT64 endTime = 0;

	if (g_pTimeStartQuery && g_pTimeEndQuery && g_GpuFrequency > 0)
	{
		// Wait for startTime
		while (S_OK != g_pTimeStartQuery->GetData(&startTime, sizeof(startTime), D3DGETDATA_FLUSH))
		{
			// Busy-wait loop; in production code you might sleep or yield here.
		}

		// Wait for endTime
		while (S_OK != g_pTimeEndQuery->GetData(&endTime, sizeof(endTime), D3DGETDATA_FLUSH))
		{
			// Busy-wait loop; in production code you might sleep or yield here.
		}

		// Now calculate the GPU time in milliseconds
		double deltaTicks = static_cast<double>(endTime - startTime);
		double deltaSeconds = deltaTicks / static_cast<double>(g_GpuFrequency);
		g_GpuTimeMs = deltaSeconds * 1000.0;
	}
	else
	{
		// If something went wrong or queries are not valid, fallback to 0
		g_GpuTimeMs = 0.0;
	}
}

//------------------------------------------------------------------------------
// Color-coding for the overlay text based on thresholds
//------------------------------------------------------------------------------
static ImVec4 GetMsColor(float ms)
{
	if (ms > OK_THRESHOLD_MS)
		return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
	else if (ms > GOOD_THRESHOLD_MS)
		return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
	else
		return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
}

static ImVec4 GetDrawCallColor(int drawCalls)
{
	if (drawCalls > 3000)
		return ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
	else if (drawCalls > 2000)
		return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
	else
		return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // Green
}

//------------------------------------------------------------------------------
// DrawStatUnitOverlay()
//   Displays the timing for:
//     - Client CPU
//     - Server CPU
//     - GPU
//     - Combined Frame time
//------------------------------------------------------------------------------
void DrawStatUnitOverlay()
{
	const ImGuiIO& io = ImGui::GetIO();

	// Position top-right with some offset
	ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 400.0f, 10.0f), ImGuiCond_Always);

	// Flags for a minimal, auto-resizing overlay
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoBackground;

	ImGui::Begin("StatUnit", nullptr, flags);
	ImGui::PushFont(g_BigConsoleFont);

	// Client CPU
	ImVec4 clientColor = GetMsColor((float)g_ClientCpuTimeMs-g_PresentCpuTimeMs);
	ImGui::TextColored(clientColor, "Client CPU: %.2f ms", g_ClientCpuTimeMs-g_PresentCpuTimeMs);

	// Server CPU
	ImVec4 serverColor = GetMsColor((float)g_ServerCpuTimeMs);
	ImGui::TextColored(serverColor, "Server CPU: %.2f ms", g_ServerCpuTimeMs);

	// Present CPU
	ImVec4 presentColor = GetMsColor((float)g_PresentCpuTimeMs);
	ImGui::TextColored(presentColor, "D3D9on12 CPU: %.2f ms", g_PresentCpuTimeMs);

	// GPU
	ImVec4 gpuColor = GetMsColor((float)g_GpuTimeMs);
	ImGui::TextColored(gpuColor, "GPU: %.2f ms", g_GpuTimeMs);

	// Frame total = Client + Server + GPU
	float totalFrameMs = (float)(g_ClientCpuTimeMs + g_ServerCpuTimeMs + g_GpuTimeMs);
	ImVec4 frameColor = GetMsColor(totalFrameMs);
	extern INT TheW3DFrameLengthInMsec;
	if (totalFrameMs < TheW3DFrameLengthInMsec)
		totalFrameMs = TheW3DFrameLengthInMsec;

	ImGui::TextColored(frameColor, "Frame: %.2f ms", totalFrameMs);

	// Draw calls.
	int numDrawCalls = DX8Wrapper::GetCurrentDrawCallCount();
	ImVec4 drawCallColor = GetDrawCallColor(numDrawCalls);
	ImGui::TextColored(drawCallColor, "DrawCalls: %d", numDrawCalls);

	if (DX8Wrapper::GetNumTexturesCreated() > 0)
	{
		ImVec4 numTexturesCreatedColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		ImGui::TextColored(numTexturesCreatedColor, "Textures Created: %d", DX8Wrapper::GetNumTexturesCreated());
	}
	else
	{
		ImVec4 numTexturesCreatedColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		ImGui::TextColored(numTexturesCreatedColor, "Textures Created: None");
	}

	ImGui::PopFont();
	ImGui::End();
}
