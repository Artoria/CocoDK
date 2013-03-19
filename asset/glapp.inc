#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glext.h>
#define GLvoid void
#include <windows.h>
#include "glHelper.h"


#define MAXTEX 256
//globals
bool keys[300];						// 保存键盘按键的数组
bool fullscreen=true;                  //是否全屏，默认为false
int scrWidth, scrHeight;
bool active=TRUE;					// 窗口的活动标志，缺省为TRUE
bool bQuit = FALSE;
GLHelper glhelper;
GLuint tex[MAXTEX];
GLuint nModel;
GLuint nTex;
    //宽和高必须是2的n次方；宽度和高度最小必须是64象素；并且出于兼容性的原因，图像的宽度和高度不应超过256象素。
    //如果您的原始素材的宽度和高度不是64,128,256象素的话，使用图像处理软件重新改变图像的大小。
//game db
int cam;
int Fps = 120;
#include "glapp_windowproc.inc"
int status; //0..准备游戏，1..游戏中，2..结束游戏。 3..暂停游戏
GLHelper::GLHelper()
{
    cx=10.0; cy=12.0; cz=6.5;
    theta=135;
    rotz=0.0;
    gamma=120;
}

void GLHelper::ResetView(int width, int height)
{
    if (!height)height=1;
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)width/(float)height, 0.1f, 200.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    /*gluLookAt(cx,cy,cz,
              cx+cos(theta*PI180)*sin(gamma*PI180),
              cy+sin(theta*PI180)*sin(gamma*PI180),
              cz+cos(gamma*PI180),
              0.0,0.0,cos(rotz));
*/
    gluLookAt(25.0,10.0,16.0, 10.0, 10.0, 0, 0, 0, 1);
    //debug
}

int GLHelper::InitGL(void)
{
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0,0.0,0.0,0.0);
    glClearDepth(1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// 告诉系统对透视进行修正

    glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_FRONT, GL_LINE);
    glPolygonMode(GL_BACK, GL_LINE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);

    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    return true;
}

void GLHelper::KillGLWindow(GLvoid)
{
    if (fullscreen)
    {
        ChangeDisplaySettings(NULL,0);					// 是的话，切换回桌面
        ShowCursor(TRUE);						// 显示鼠标指针
    }

    if (hRC)								// 我们拥有OpenGL渲染描述表吗?
    {
        if (!wglMakeCurrent(NULL,NULL))					// 我们能否释放DC和RC描述表?
        {

            MessageBox(NULL,"释放DC或RC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(hRC))					// 我们能否删除RC?
        {

            MessageBox(NULL,"释放RC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
        }
        hRC=NULL;							// 将RC设为 NULL
    }

    if (hDC && !ReleaseDC(hWnd,hDC))					// 我们能否释放 DC?
    {
        MessageBox(NULL,"释放DC失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
        hDC=NULL;							// 将 DC 设为 NULL
    }

    if (hWnd && !DestroyWindow(hWnd))					// 能否销毁窗口?
    {
        MessageBox(NULL,"释放窗口句柄失败。","关闭错误",MB_OK | MB_ICONINFORMATION);
        hWnd=NULL;							// 将 hWnd 设为 NULL
    }

    if (!UnregisterClass("OpenG",hInstance))				// 能否注销类?
    {
        MessageBox(NULL,"不能注销窗口类。","关闭错误",MB_OK | MB_ICONINFORMATION);
        hInstance=NULL;							// 将 hInstance 设为 NULL
    }
}

BOOL GLHelper::CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{

    GLuint		PixelFormat;						// 保存查找匹配的结果
    WNDCLASSEX	wcex;							// 窗口类结构
    DWORD		dwExStyle;						// 扩展窗口风格
    DWORD		dwStyle;						// 窗口风格

    RECT WindowRect;							// 取得矩形的左上角和右下角的坐标值
    WindowRect.left=(long)0;						// 将Left   设为 0
    WindowRect.right=(long)width;						// 将Right  设为要求的宽度
    WindowRect.top=(long)0;							// 将Top    设为 0
    WindowRect.bottom=(long)height;						// 将Bottom 设为要求的高度

    fullscreen=fullscreenflag;						// 设置全局全屏标志

    hInstance		    = GetModuleHandle(NULL);			// 取得我们窗口的实例
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style		    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// 移动时重画，并为窗口取得DC
    wcex.lpfnWndProc	= (WNDPROC) WindowProc;				// WndProc处理消息
    wcex.cbClsExtra		= 0;						// 无额外窗口数据
    wcex.cbWndExtra		= 0;						// 无额外窗口数据
    wcex.hInstance		= hInstance;					// 设置实例
    wcex.hIcon		    = LoadIcon(NULL, IDI_APPLICATION);			// 装入缺省图标
    wcex.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);			// 装入鼠标指针
    wcex.hbrBackground	= NULL;						// GL不需要背景
    wcex.lpszMenuName	= NULL;						// 不需要菜单
    wcex.lpszClassName	= "OpenG";					// 设定类名字

    if (!RegisterClassEx(&wcex))						// 尝试注册窗口类
    {
        MessageBox(NULL,"注册窗口失败","错误",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 退出并返回FALSE
    }

    if (fullscreenflag)								// 要尝试全屏模式吗?
    {
        DEVMODE dmScreenSettings;						// 设备模式
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// 确保内存清空为零
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Devmode 结构的大小
        dmScreenSettings.dmPelsWidth	= width;				// 所选屏幕宽度
        dmScreenSettings.dmPelsHeight	= height;				// 所选屏幕高度
        dmScreenSettings.dmBitsPerPel	= bits;					// 每象素所选的色彩深度
        dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

        // 尝试设置显示模式并返回结果。注: CDS_FULLSCREEN 移去了状态条。
        if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
        {
            // 若模式失败，提供两个选项：退出或在窗口内运行。
            if (MessageBox(NULL,"全屏模式在当前显卡上设置失败！\n使用窗口模式？","NeHe&DancingWind G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
            {
                fullscreen=FALSE;				// 选择窗口模式(Fullscreen=FALSE)
            }
            else
            {
                // 弹出一个对话框，告诉用户程序结束
                MessageBox(NULL,"程序将被关闭","错误",MB_OK|MB_ICONSTOP);
                return FALSE;					//  退出并返回 FALSE
            }
        }
    }
    if (fullscreen)								// 仍处于全屏模式吗?
    {

        dwExStyle=WS_EX_APPWINDOW;					// 前端显示
        dwStyle=WS_POPUP;						// 窗体风格:无边框
        //ShowCursor(FALSE);						// 隐藏鼠标指针
    }
    else
    {
        dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// 前端显示
        dwStyle=WS_OVERLAPPEDWINDOW;					//  窗体风格
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// 调整窗口达到真正要求的大小
    /* create main window */
    if (!(hWnd=CreateWindowEx(	dwExStyle,				// 扩展窗体风格
                               "OpenG",				// 类名字
                               title,					// 窗口标题
                               WS_CLIPSIBLINGS |			// 必须的窗体风格属性
                               WS_CLIPCHILDREN |			// 必须的窗体风格属性
                               dwStyle,				// 选择的窗体属性
                               0, 0,					// 窗口位置
                               WindowRect.right-WindowRect.left,	// 计算调整好的窗口宽度
                               WindowRect.bottom-WindowRect.top,	// 计算调整好的窗口高度
                               NULL,					// 无父窗口
                               NULL,					// 无菜单
                               hInstance,				// 实例
                               NULL)))					// 不向WM_CREATE传递任何东东

    {
        KillGLWindow();							// 重置显示区
        MessageBox(NULL,"不能创建一个窗口设备描述表","错误",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 返回 FALSE
    }

//以下相当于CodeBlocks预置函数EnableOpenGL的功能
    static	PIXELFORMATDESCRIPTOR pfd=					// /pfd 告诉窗口我们所希望的东东，即窗口使用的像素格式
    {
        sizeof(PIXELFORMATDESCRIPTOR),					// nSize: 上述格式描述符的大小
        1,								// 版本号:nVersion
        PFD_DRAW_TO_WINDOW |						// dwFlags:格式支持窗口
        PFD_SUPPORT_OPENGL |						// 格式必须支持OpenGL
        PFD_DOUBLEBUFFER,						// 必须支持双缓冲
        PFD_TYPE_RGBA,							// iPixelType:申请 RGBA 格式
        bits,								// cColorbits: 选定色彩深度
        0, 0, 0, 0, 0, 0,						// 忽略的色彩位
        0,								// 无Alpha缓存
        0,								// 忽略Shift Bit
        0,								// 无累加缓存
        0, 0, 0, 0,							// 忽略聚集位
        16,								// cDepthBits:16位 Z-缓存 (深度缓存)
        0,								// 无蒙板缓存
        0,								// 无辅助缓存
        PFD_MAIN_PLANE,							// iLayerType:主绘图层
        0,								// Reserved
        0, 0, 0								// 忽略层遮罩
    };

    if (!(hDC=GetDC(hWnd)))							// 取得设备描述表了么?
    {
        KillGLWindow();							// 重置显示区
        MessageBox(NULL,"不能创建一种相匹配的像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 返回 FALSE
    }

    if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Windows 找到相应的象素格式了吗?
    {
        KillGLWindow();							// 重置显示区
        MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 返回 FALSE
    }


    if (!SetPixelFormat(hDC,PixelFormat,&pfd))				// 能够设置象素格式么?
    {
        KillGLWindow();							// 重置显示区
        MessageBox(NULL,"不能设置像素格式","错误",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 返回 FALSE
    }

    if (!(hRC=wglCreateContext(hDC)))					// 能否取得着色描述表?
    {
        KillGLWindow();							// 重置显示区
        MessageBox(NULL,"不能创建OpenGL渲染描述表","错误",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 返回 FALSE
    }

    if (!wglMakeCurrent(hDC,hRC))						// 尝试激活着色描述表
    {
        KillGLWindow();							// 重置显示区
        MessageBox(NULL,"不能激活当前的OpenGL渲然描述表","错误",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 返回 FALSE
    }

    ShowWindow(hWnd,SW_SHOW);						// 显示窗口
    SetForegroundWindow(hWnd);						// 略略提高优先级
    SetFocus(hWnd);								// 设置键盘的焦点至此窗口
    ResetView(width, height);						// 设置透视 GL 屏幕

    if (!InitGL())								// 初始化新建的GL窗口
    {
        KillGLWindow();							// 重置显示区
        MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// 返回 FALSE
    }

    return TRUE;								// 成功
}

bool GLHelper::LoadTexture(LPTSTR szFileName, GLuint &texid)     // Creates Texture From A Bitmap File
{
 HBITMAP hBMP;             // Handle Of The Bitmap
 BITMAP BMP;             // Bitmap Structure

 glGenTextures(1, &texid);          // Create The Texture
 hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL), szFileName, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );

 if (!hBMP)              // Does The Bitmap Exist?
  return FALSE;            // If Not Return False

 GetObject(hBMP, sizeof(BMP), &BMP);        // Get The Object
                 // hBMP:        Handle To Graphics Object
                 // sizeof(BMP): Size Of Buffer For Object Information
                 // &BMP:        Buffer For Object Information

 glPixelStorei(GL_UNPACK_ALIGNMENT, 4);       // Pixel Storage Mode (Word Alignment / 4 Bytes)

 // Typical Texture Generation Using Data From The Bitmap
 glBindTexture(GL_TEXTURE_2D, texid);        // Bind To The Texture ID
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Linear Min Filter
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Linear Mag Filter
 glTexImage2D(GL_TEXTURE_2D, 0, 3, BMP.bmWidth, BMP.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, BMP.bmBits);
// GL_BGR_EXT是定义在glext.h里的，因为BMP格式是按Blue,Green,Red顺序储存图像数据的，这与OpenGL中正好相反。GL_BGR_EXT就是完成两者之间的转换的。

 DeleteObject(hBMP);            // Delete The Object

 return TRUE;             // Loading Was Successful
}

void GLHelper::PrintText(HFONT hFont, LPTSTR text, float position[3], float color[4])
{
    HBITMAP hBitmap, hOldBmp;
    BITMAP bm;
    SIZE size;
    HDC bm_hDC = CreateCompatibleDC(0);

    glPushMatrix();
    glPushAttrib(GL_ENABLE_BIT);
    //glLoadIdentity();
    /*glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    */
    SelectObject(bm_hDC, hFont);
    GetTextExtentPoint32(bm_hDC, text, strlen(text), &size);

    //创建单色位图
    hBitmap = CreateBitmap(size.cx, size.cy, 1, 1, NULL);
    hOldBmp = (HBITMAP)SelectObject(bm_hDC, hBitmap);
    SetBkColor(bm_hDC, RGB(0, 0, 0));
    SetTextColor(bm_hDC, RGB(255, 255, 255));
    SetBkMode(bm_hDC, OPAQUE);
    TextOut(bm_hDC, 0, 0, text, strlen(text));
    //转存
    GetObject(hBitmap, sizeof(bm), &bm);
    size.cx = (bm.bmWidth+31)&(~0x1f);      //???
    size.cy = bm.bmHeight;
    int bufsize=size.cx*size.cy/8;
    struct
    {
        BITMAPINFOHEADER bih;
        RGBQUAD col[2];
    }bic;       //???
    BITMAPINFO *binfo = (BITMAPINFO *)&bic;

    binfo->bmiHeader.biSize     = sizeof(binfo->bmiHeader);
    binfo->bmiHeader.biWidth    = bm.bmWidth;
    binfo->bmiHeader.biHeight   = bm.bmHeight;
    binfo->bmiHeader.biPlanes   = 1;
    binfo->bmiHeader.biBitCount = 1;
    binfo->bmiHeader.biCompression = BI_RGB;
    binfo->bmiHeader.biSizeImage= bufsize;
    binfo->bmiHeader.biXPelsPerMeter = 1;
    binfo->bmiHeader.biYPelsPerMeter = 1;
    binfo->bmiHeader.biClrUsed  = 0;
    binfo->bmiHeader.biClrImportant = 0;

    UCHAR* pBmpBits = new UCHAR[bufsize];
    memset(pBmpBits, 0, sizeof(UCHAR)*bufsize);
    //位图信息放入字符串
    GetDIBits(bm_hDC, hBitmap, 0, bm.bmHeight, pBmpBits, binfo, DIB_RGB_COLORS);
    //用光照来改变颜色
    GLfloat LightDiffuse[4]={1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat LightPosition[4]={0.0f, 0.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT2, GL_AMBIENT, color);				// 设置环境光
    glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuse);				// 设置漫射光
    glLightfv(GL_LIGHT2, GL_POSITION, LightPosition);			// 设置光源位置
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);							// 启用2号光源

    //显示
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glRasterPos3fv(position);
    glBitmap(size.cx, size.cy, 0.0, 0.0, 0.0, 0.0, pBmpBits);

    //清理
    delete pBmpBits;
    SelectObject(bm_hDC, hOldBmp);
    DeleteObject(hBitmap);
    DeleteDC(bm_hDC);

    glPopMatrix();
    glDisable(GL_LIGHT2);
    glPopAttrib();

}
