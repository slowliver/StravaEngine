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

	m_finalOutputRenderTexture.reset(new RenderTexture());
	m_finalOutputRenderTexture->Create2D(spec.m_width, spec.m_height, Format::R8G8B8A8_UNorm, 1);

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
		const UInt32 index = D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->Allocate();
		*cpuHandle = D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->GetCPUDescriptorHandleAt(index);
		*gpuHandle = D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->GetGPUDescriptorHandleAt(index);
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

	m_graphicsCommandBuffer->Reset();
	OnPrepareResource();
	OnPreRender();
	OnRender();
	ImGui::Render();
	OnPostRender();
	OnExecuteCommandBuffer();
}

void Renderer::OnPrepareResource()
{
	for (auto& func : m_resourceQueueCreate)
	{
		func();
	}
	// ‚±‚±‚ÅƒLƒ…[‚ğíœ‚·‚é‚×‚«‚È‚Ì‚©‚Í“ä.
	m_resourceQueueCreate.Clear();
}

void Renderer::OnPreRender()
{
}

void Renderer_OnRenderIMGUIWrapper(void* arg0, void*)
{
	auto* d3d12GraphicsCommandList = static_cast<ID3D12GraphicsCommandList*>(arg0);
	ID3D12DescriptorHeap* d3d12DescriptorHeaps[] =
	{
		D3D12::D3D12Core::s_instance->GetDescriptorHeapCBVSRVUAV()->GetD3D12DescriptorHeap(),
	};
	d3d12GraphicsCommandList->SetDescriptorHeaps(1, d3d12DescriptorHeaps);
//	d3d12GraphicsCommandList->SetGraphicsRootDescriptorTable()
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12GraphicsCommandList);
}

void Renderer::OnRender()
{
	//m_drawTriangleSamplePass->OnRender(*m_graphicsCommandBuffer);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	static bool show_demo_window = true;
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

//	m_graphicsCommandBuffer->SetNativeCommand(Renderer_OnRenderIMGUIWrapper, nullptr);
}

void Renderer::OnPostRender()
{

}

void Renderer::OnExecuteCommandBuffer()
{
	D3D12::D3D12Core::s_instance->OnPrepareCommandBuffer();
	D3D12::D3D12Core::s_instance->OnSubmitCommandBuffer(*m_graphicsCommandBuffer, *m_finalOutputRenderTexture);
}
}