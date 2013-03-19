

#define GLOBAL(a) ({ int g_##a(); g_##a(); })
void GameMain();


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    const int WindowWidth  = GLOBAL(winwidth);
    const int WindowHeight = GLOBAL(winheight);
    const int FullWidth    = GLOBAL(fullwidth);
    const int FullHeight   = GLOBAL(fullheight);

    MSG msg;
    glhelper.hInstance=hInstance;
    scrWidth=GetSystemMetrics(SM_CXSCREEN);
    scrHeight=GetSystemMetrics(SM_CYSCREEN);
    // 提示用户选择运行模式
    if (MessageBox(NULL,"你想在全屏模式下运行么？", "设置全屏模式",MB_YESNO|MB_ICONQUESTION)==IDNO)
    {
        fullscreen=FALSE;           // FALSE为窗口模式
        scrWidth=WindowWidth;
        scrHeight=WindowHeight;
    }

    // 创建OpenGL窗口
    if (!glhelper.CreateGLWindow((char *)"NeHe's OpenGL程序框架 (C::B REMIX) by viktor",scrWidth,scrHeight,16,fullscreen))
    {
        return 0;							// 失败退出
    }
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            // 游戏主过程
            GameMain();
            SwapBuffers(glhelper.hDC);			// 交换缓存 (双缓存)
        }
    }

    // 关闭程序
    glhelper.KillGLWindow();								// 销毁窗口
    return (msg.wParam);							// 退出程序
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_ACTIVATE:						// 监视窗口激活消息
    {
        if (!HIWORD(wParam))					// 检查最小化状态
        {
            active=TRUE;					// 程序处于激活状态
        }
        else
        {
            active=FALSE;					// 程序不再激活
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);			// 返回消息循环
    }
    case WM_SYSCOMMAND:						// 系统中断命令
    {
        switch (wParam)						// 检查系统调用
        {
            case SC_SCREENSAVE:				// 屏保要运行?
            case SC_MONITORPOWER:			// 显示器要进入节电模式?
            return 0;					// 阻止发生
        }
        break;							// 退出
    }


    case WM_DESTROY:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);

    case WM_KEYDOWN:						// 有键按下么?
    {
        keys[wParam] = TRUE;					// 如果是，设为TRUE
        return 0;						// 返回
    }
    case WM_KEYUP:							// 有键放开么?
    {
        keys[wParam] = FALSE;					// 如果是，设为FALSE
        return 0;						// 返回
    }
    case WM_SIZE:							// 调整OpenGL窗口大小
    {
        glhelper.ResetView(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
        return DefWindowProc(hwnd, uMsg, wParam, lParam);						// 返回
    }
    case WM_LBUTTONDOWN:
    {
        keys[1]=true;
        return 0;
    }
    break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
