#include <Windows.h>
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx12.h>

#include <Engine/Graphics/Renderer.h>
#include <Engine/Graphics/CommandBuffer.h>

#include "D3D12/D3D12Core.h"
#include "D3D12/D3D12DescriptorHeap.h"
#include "DrawTriangleSamplePass.h"
#include "RenderTexture.h"

namespace StravaEngine::Graphics
{
std::unique_ptr<Renderer> Renderer::s_instance = nullptr;

Renderer* Renderer::CreateInstance()
{
	s_instance.reset(new Renderer());
	return GetInstance();
}

Renderer* Renderer::GetInstance()
{
	return s_instance.get();
}

void Renderer::DestroyInstance()
{
	s_instance.reset(nullptr);
}

Renderer::Renderer()
{}

bool Renderer::Initialize(const RendererSpec& spec)
{
	bool result = false;

	D3D12::D3D12Core::s_instance.reset(new D3D12::D3D12Core());
	result = D3D12::D3D12Core::s_instance->Initialize(spec);
	if (!result)
	{
		return false;
	}

	m_graphicsCommandBuffer.reset(new GraphicsCommandBuffer(1024 * 512));
	m_graphicsCommandBuffer->Initialize();

	m_drawTriangleSamplePass.reset(new DrawTriangleSamplePass());
	m_drawTriangleSamplePass->Initialize();

#if 1
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init((HWND)spec.m_nativeWindowHandle);

	ImGui_ImplDX12_InitInfo imGuiInitInfo = {};
	imGuiInitInfo.Device = D3D12::D3D12Core::s_instance->GetD3D12Device();
	imGuiInitInfo.CommandQueue = D3D12::D3D12Core::s_instance->GetD3D12CommandQueue();
	imGuiInitInfo.NumFramesInFlight = 3;
	imGuiInitInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	imGuiInitInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
	// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
	// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
	imGuiInitInfo.SrvDescriptorHeap = D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->GetD3D12DescriptorHeap();
	imGuiInitInfo.SrvDescriptorAllocFn =
	[](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandle)
	{
		*cpuHandle = D3D12::D3D12Core::s_instance->GetCPUDescriptorHeapCBVSRVUAV()->Allocate();
	};
	imGuiInitInfo.SrvDescriptorFreeFn =
	[](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{
		D3D12::D3D12Core::s_instance->GetCPUDescriptorHeapCBVSRVUAV()->Free(cpuHandle);
	};
	ImGui_ImplDX12_Init(&imGuiInitInfo);
#endif

	return true;
}

void Renderer::Terminate()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	D3D12::D3D12Core::s_instance->Terminate();
	D3D12::D3D12Core::s_instance.reset(nullptr);
}

void Renderer::OnUpdate()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Render();

	m_graphicsCommandBuffer->Reset();
	OnPrepareResource();
	OnPreRender();
	OnRender();
	OnPostRender();
	OnExecuteCommandBuffer();
}

void Renderer::SetFinalOutput(RenderTexture* renderTexture)
{
	m_finalOutputRenderTexture = renderTexture;
}

void Renderer::OnPrepareResource()
{
	for (auto& func : m_resourceQueueCreate)
	{
		func();
	}
	// ‚±‚±‚ÅƒLƒ…[‚ðíœ‚·‚é‚×‚«‚È‚Ì‚©‚Í“ä.
	m_resourceQueueCreate.Clear();
}

void Renderer::OnPreRender()
{
}

void Renderer::OnRender()
{
	m_drawTriangleSamplePass->OnRender(*m_graphicsCommandBuffer);
}

void Renderer::OnPostRender()
{

}

void Renderer::OnExecuteCommandBuffer()
{
	D3D12::D3D12Core::s_instance->OnPrepareCommandBuffer();
	D3D12::D3D12Core::s_instance->OnSubmitCommandBuffer(*m_graphicsCommandBuffer);
}
}