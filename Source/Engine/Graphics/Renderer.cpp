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

#if 0
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init((HWND)spec.m_nativeWindowHandle);

	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = D3D12::D3D12Core::s_instance->GetD3D12Device();
	init_info.CommandQueue = D3D12::D3D12Core::s_instance->GetD3D12CommandQueue();
	init_info.NumFramesInFlight = 3;
	init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;
	// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
	// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
	init_info.SrvDescriptorHeap = D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->GetD3D12DescriptorHeap();
	init_info.SrvDescriptorAllocFn =
		[](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandle)
		{
			D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->Allocate(cpuHandle, gpuHandle);
		};
	init_info.SrvDescriptorFreeFn =
		[](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
		{
			D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->
		};
	ImGui_ImplDX12_Init(&init_info);
#endif

	return true;
}

void Renderer::Terminate()
{
	D3D12::D3D12Core::s_instance->Terminate();
	D3D12::D3D12Core::s_instance.reset(nullptr);
}

void Renderer::OnUpdate()
{
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