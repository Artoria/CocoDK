

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
    // ��ʾ�û�ѡ������ģʽ
    if (MessageBox(NULL,"������ȫ��ģʽ������ô��", "����ȫ��ģʽ",MB_YESNO|MB_ICONQUESTION)==IDNO)
    {
        fullscreen=FALSE;           // FALSEΪ����ģʽ
        scrWidth=WindowWidth;
        scrHeight=WindowHeight;
    }

    // ����OpenGL����
    if (!glhelper.CreateGLWindow((char *)"NeHe's OpenGL������ (C::B REMIX) by viktor",scrWidth,scrHeight,16,fullscreen))
    {
        return 0;							// ʧ���˳�
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
            // ��Ϸ������
            GameMain();
            SwapBuffers(glhelper.hDC);			// �������� (˫����)
        }
    }

    // �رճ���
    glhelper.KillGLWindow();								// ���ٴ���
    return (msg.wParam);							// �˳�����
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_ACTIVATE:						// ���Ӵ��ڼ�����Ϣ
    {
        if (!HIWORD(wParam))					// �����С��״̬
        {
            active=TRUE;					// �����ڼ���״̬
        }
        else
        {
            active=FALSE;					// �����ټ���
        }

        return DefWindowProc(hwnd, uMsg, wParam, lParam);			// ������Ϣѭ��
    }
    case WM_SYSCOMMAND:						// ϵͳ�ж�����
    {
        switch (wParam)						// ���ϵͳ����
        {
            case SC_SCREENSAVE:				// ����Ҫ����?
            case SC_MONITORPOWER:			// ��ʾ��Ҫ����ڵ�ģʽ?
            return 0;					// ��ֹ����
        }
        break;							// �˳�
    }


    case WM_DESTROY:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);

    case WM_KEYDOWN:						// �м�����ô?
    {
        keys[wParam] = TRUE;					// ����ǣ���ΪTRUE
        return 0;						// ����
    }
    case WM_KEYUP:							// �м��ſ�ô?
    {
        keys[wParam] = FALSE;					// ����ǣ���ΪFALSE
        return 0;						// ����
    }
    case WM_SIZE:							// ����OpenGL���ڴ�С
    {
        glhelper.ResetView(LOWORD(lParam),HIWORD(lParam));		// LoWord=Width,HiWord=Height
        return DefWindowProc(hwnd, uMsg, wParam, lParam);						// ����
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