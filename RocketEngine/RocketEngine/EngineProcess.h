#pragma once
#include "DLLExporter.h"
#include "IEngineProcess.h"
#include <memory>
#include <windows.h>

namespace SAD
{
	class SadScene;
}

namespace RocketCore
{
	class ObjectSystem;
	class RenderSystem;
	class SceneSystem;
	class TimeSystem;
	class InputSystem;
	class TweenSystem;
	class PhysicsSystem;
	class SoundSystem;
	class DebugSystem;
	class UISystem;
	class DataSystem;
}

namespace RocketEngine
{
	class ROCKET_API EngineProcess final : public IEngineProcess
	{
		friend class SAD::SadScene;

	private:
		long InitializeForEditor(void* hWnd);
		void RunCycleOnceForEditor();

		/// Initialize, Finalize
	public:
		// ���Ϸ�ó ������. System���� ������ unique_ptr�� �����Ѵ�.
		EngineProcess();

		// ���� �ʱ�ȭ �Լ�. Windowâ�� �ʱ�ȭ�ϰ� System���� �ʱ�ȭ�Ѵ�.
		long Initialize(void* hInstance, int nCmdShow) override;

		// ���� ������ �Լ�.
		void Finalize() override;

		/// ���� ����������Ŭ�� ���õ� �Լ���.
	public:
		// ������ ����������Ŭ�� ���� �ݺ���Ų��.
		void RunEngine() override;

	private:
		// ������ ����������Ŭ�� �°� 1ȸ ���۽�Ų��.
		void RunCycleOnce();


		/// ������ ���� (â������ ����, ���콺 �Է�)
		/// Resize �Լ�.
	public:
		void Resize(int width, int height) override;

		/// System�� ����Ʈ������
		/// ���������θ� ����Ѵ�. �Ƹ� C4251 ����� �߻��� ��.
		/// ����� ��� �������� �� �����غ��� �ҵ�.
		/// 23.06.30 ������ �����.
	private:
		RocketCore::ObjectSystem& _objectSystem;
		RocketCore::RenderSystem& _renderSystem;
		RocketCore::SceneSystem& _sceneSystem;
		RocketCore::TimeSystem& _timeSystem;
		RocketCore::InputSystem& _inputSystem;
		RocketCore::TweenSystem& _tweenSystem;
		RocketCore::PhysicsSystem& _physicsSystem;
		RocketCore::SoundSystem& _soundSystem;
		RocketCore::DebugSystem& _debugSystem;
		RocketCore::UISystem& _uiSystem;
		RocketCore::DataSystem& _dataSystem;


		/// ������ ����
	private:
		HWND _hWnd;
		MSG _msg;
		int _screenWidth;
		int _screenHeight;
		const WCHAR* CLASSNAME;
		const WCHAR* WINDOWNAME;

	private:
		ATOM MyRegisterClass(HINSTANCE hInstance);
		BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
		void ChangeDisplayResolution(int width, int height);

		// �޽��� �ڵ鷯 (���� �ݹ�)
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		// for Initialize
		void RecalcWindowSize();

	};
}