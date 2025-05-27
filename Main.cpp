//
// Main.cpp
//

#include "pch.h"
#include "Game.h"
#include <Mouse.h>
#include <Keyboard.h>

using namespace DirectX;

#include "imgui/imgui_impl_win32.h"

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

namespace
{
    // �Q�[���N���X��錾
    std::unique_ptr<Game> g_game;
}
// �A�v���P�[�V���������`����
LPCWSTR g_szAppName = L"Balloon";
// �v���g�^�C�v��錾����
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void ExitGame() noexcept;

// �n�C�u���b�h�O���t�B�b�N�X�V�X�e���Ƀf�t�H���g�Ńf�B�X�N���[�g�p�[�c��D�悳����悤�w������
extern "C"
{
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

// �E�B���h�E�G���g���|�C���g
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    if (!XMVerifyCPUSupport())
        return 1;

    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    if (FAILED(hr))
        return 1;

    // �t���X�N���[���Ή�
    static bool s_fullscreen = false;
    // ���b�Z�[�W�{�b�N�X�̕\��
    int result = MessageBox(NULL, L"�t���X�N���[���ɂ��܂����H", L"��ʃ��[�h�ݒ�", MB_YESNO | MB_ICONQUESTION);

    // �u�͂��v�܂��́u�������v�̑I���ɉ����ăt���X�N���[���ݒ�
    if (result == IDYES) {
        s_fullscreen = true;
    }
    else {
        s_fullscreen = false;
    }

    // �Q�[���N���X�̃C���X�^���X�𐶐�����
    g_game = std::make_unique<Game>();

    // �N���X��o�^���E�B���h�E�𐶐�����
    {
        // �E�B���h�E�N���X��o�^����
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIconW(hInstance, L"IDI_ICON");
        wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszClassName = L"BalloonWindowClass";
        wcex.hIconSm = LoadIconW(wcex.hInstance, L"IDI_ICON");
        if (!RegisterClassExW(&wcex))
            return 1;

        // �E�B���h�E�̊���T�C�Y���擾����
        int w, h;
        g_game->GetDefaultSize(w, h);

        RECT rc = { 0, 0, static_cast<LONG>(w), static_cast<LONG>(h) };
        // �E�B���h�E�̗̈�𒲐�����
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        // �E�B���h�E�𐶐�����
        HWND hwnd = CreateWindowExW(
            0,
            L"BalloonWindowClass", // �E�B���h�E�N���X��
            g_szAppName,                     // �E�B���h�E�^�C�g��
            WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, // �ő剻�ƃT�C�Y�ύX�𖳌���
            CW_USEDEFAULT, CW_USEDEFAULT,    // �����ʒu
            rc.right - rc.left, rc.bottom - rc.top, // �����T�C�Y
            nullptr, nullptr, hInstance, nullptr
        );


        // TODO: Change to CreateWindowExW(WS_EX_TOPMOST, L"DirectX11LessonV2WindowClass", g_szAppName, WS_POPUP,
        // to default to fullscreen.
        if (!hwnd)
            return 1;
        // �E�B���h�E��\������
        ShowWindow(hwnd, nCmdShow);
        // �f�t�H���g�Ńt���X�N���[���ɂ���ɂ�nCmdShow��SW_SHOWMAXIMIZED�ɕύX����
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));
        // �N���C�A���g�̈���擾����
        GetClientRect(hwnd, &rc);
        // �Q�[���N���X������������
        g_game->Initialize(hwnd, rc.right - rc.left, rc.bottom - rc.top);
        if (s_fullscreen) g_game->SetFullscreenState(TRUE);
    }

    // ���b�Z�[�W���[�v
    MSG msg = {};
    while (WM_QUIT != msg.message)
    {
        // ���b�Z�[�W���s�[�N����
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // ���b�Z�[�W���g�����X���[�g����
            TranslateMessage(&msg);
            // ���b�Z�[�W�𑗐M����
            DispatchMessage(&msg);
        }
        else
        {
            // Tick�֐����Ăяo��
            g_game->Tick();
        }
    }

    // �Q�[�������Z�b�g����
    if (s_fullscreen) g_game->SetFullscreenState(FALSE);
    g_game.reset();

    CoUninitialize();
    // ���b�Z�[�W�̃p�����[�^��Ԃ�
    return static_cast<int>(msg.wParam);
}

// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    //  ImGui�̃��b�Z�[�W����
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    static bool s_in_sizemove = false;
    static bool s_in_suspend = false;
    static bool s_minimized = false;
    static bool s_fullscreen = false;
    // �f�t�H���g�Ńt���X�N���[���ɐݒ肳��Ă���ꍇ�As_fullscreen �� true �ɐݒ肷��.

    auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
        case WM_PAINT:
            if (s_in_sizemove && game)
            {
                game->Tick();
            }
            else
            {
                PAINTSTRUCT ps;
                std::ignore = BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
            }
            break;

        case WM_DISPLAYCHANGE:
            if (game)
            {
                game->OnDisplayChange();
            }
            break;

        case WM_MOVE:
            if (game)
            {
                game->OnWindowMoved();
            }
            break;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
            {
                if (!s_minimized)
                {
                    s_minimized = true;
                    if (!s_in_suspend && game)
                        game->OnSuspending();
                    s_in_suspend = true;
                }
            }
            else if (s_minimized)
            {
                s_minimized = false;
                if (s_in_suspend && game)
                    game->OnResuming();
                s_in_suspend = false;
            }
            //else if (!s_in_sizemove && game)
            //{
            //    // �E�B���h�E�̃T�C�Y���ύX���ꂽ�ꍇ�Ɏ��̊֐����Ăяo��
            //    game->OnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));
            //}
            break;

        case WM_ENTERSIZEMOVE:
            s_in_sizemove = true;
            break;

        case WM_EXITSIZEMOVE:
            s_in_sizemove = false;
            if (game)
            {
                RECT rc;
                // �N���C�A���g�̈���擾����
                GetClientRect(hWnd, &rc);
                // �E�B���h�E�T�C�Y���ύX���ꂽ�ꍇ�Ɏ��̊֐����Ăяo��
                game->OnWindowSizeChanged(rc.right - rc.left, rc.bottom - rc.top);
            }
            break;

        case WM_GETMINMAXINFO:
            if (lParam)
            {
                auto info = reinterpret_cast<MINMAXINFO*>(lParam);
                // �E�B���h�E�̍ŏ������� 1280 �s�N�Z���ɐݒ�
                info->ptMinTrackSize.x = 1280;
                // �E�B���h�E�̍ŏ������� 720 �s�N�Z���ɐݒ�
                info->ptMinTrackSize.y = 720;
            }
            break;

        case WM_ACTIVATEAPP:
            // �A�v���P�[�V�������A�N�e�B�u�܂��͔�A�N�e�B�u�ɂȂ����ꍇ
            DirectX::Mouse::ProcessMessage(message, wParam, lParam);
            DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
            if (game)
            {
                if (wParam)
                {
                    game->OnActivated();
                }
                else
                {
                    game->OnDeactivated();
                }
            }
            break;

        case WM_POWERBROADCAST:
            switch (wParam)
            {
                case PBT_APMQUERYSUSPEND:
                    if (!s_in_suspend && game)
                        game->OnSuspending();
                    s_in_suspend = true;
                    return TRUE;

                case PBT_APMRESUMESUSPEND:
                    if (!s_minimized)
                    {
                        if (s_in_suspend && game)
                            game->OnResuming();
                        s_in_suspend = false;
                    }
                    return TRUE;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SYSKEYDOWN:
            if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
            {
                // ALT+ENTER  �t���X�N���[���؂�ւ�����������
                if (s_fullscreen)
                {
                    SetWindowLongPtr(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
                    SetWindowLongPtr(hWnd, GWL_EXSTYLE, 0);

                    int width = 1280;
                    int height = 720;
                    if (game)
                        game->GetDefaultSize(width, height);

                    ShowWindow(hWnd, SW_SHOWNORMAL);

                    SetWindowPos(hWnd, HWND_TOP, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
                else
                {
                    SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP);
                    SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOPMOST);

                    SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

                    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
                }

                s_fullscreen = !s_fullscreen;
            }
            break;

        case WM_MENUCHAR:
            // ���j���[���A�N�e�B�u�Ń��[�U�[���C�ӂ̃j�[���j�b�N�܂��̓A�N�Z�����[�^�L�[�ɑΉ����Ȃ��L�[���������ꍇ��
                 // �G���[�r�[�v�𔭐������Ȃ��悤�ɖ�������
            return MAKELRESULT(0, MNC_CLOSE);

        case WM_INPUT:
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONUP:
        case WM_MOUSEHOVER:
            DirectX::Mouse::ProcessMessage(message, wParam, lParam);
            break;

        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
            break;

        case WM_MOUSEACTIVATE:
            // �N���b�N���ăE�B���h�E���A�N�e�B�u�ɂ���ƁA�}�E�X�͂��̃C�x���g�𖳎�����
            return MA_ACTIVATEANDEAT;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

// Exit helper
void ExitGame() noexcept
{
    PostQuitMessage(0);
}
