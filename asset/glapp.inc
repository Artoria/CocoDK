#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glext.h>
#define GLvoid void
#include <windows.h>
#include "glHelper.h"


#define MAXTEX 256
//globals
bool keys[300];						// ������̰���������
bool fullscreen=true;                  //�Ƿ�ȫ����Ĭ��Ϊfalse
int scrWidth, scrHeight;
bool active=TRUE;					// ���ڵĻ��־��ȱʡΪTRUE
bool bQuit = FALSE;
GLHelper glhelper;
GLuint tex[MAXTEX];
GLuint nModel;
GLuint nTex;
    //���͸߱�����2��n�η������Ⱥ͸߶���С������64���أ����ҳ��ڼ����Ե�ԭ��ͼ��Ŀ��Ⱥ͸߶Ȳ�Ӧ����256���ء�
    //�������ԭʼ�زĵĿ��Ⱥ͸߶Ȳ���64,128,256���صĻ���ʹ��ͼ�����������¸ı�ͼ��Ĵ�С��
//game db
int cam;
int Fps = 120;
#include "glapp_windowproc.inc"
int status; //0..׼����Ϸ��1..��Ϸ�У�2..������Ϸ�� 3..��ͣ��Ϸ
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
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// ����ϵͳ��͸�ӽ�������

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
        ChangeDisplaySettings(NULL,0);					// �ǵĻ����л�������
        ShowCursor(TRUE);						// ��ʾ���ָ��
    }

    if (hRC)								// ����ӵ��OpenGL��Ⱦ��������?
    {
        if (!wglMakeCurrent(NULL,NULL))					// �����ܷ��ͷ�DC��RC������?
        {

            MessageBox(NULL,"�ͷ�DC��RCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(hRC))					// �����ܷ�ɾ��RC?
        {

            MessageBox(NULL,"�ͷ�RCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
        }
        hRC=NULL;							// ��RC��Ϊ NULL
    }

    if (hDC && !ReleaseDC(hWnd,hDC))					// �����ܷ��ͷ� DC?
    {
        MessageBox(NULL,"�ͷ�DCʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
        hDC=NULL;							// �� DC ��Ϊ NULL
    }

    if (hWnd && !DestroyWindow(hWnd))					// �ܷ����ٴ���?
    {
        MessageBox(NULL,"�ͷŴ��ھ��ʧ�ܡ�","�رմ���",MB_OK | MB_ICONINFORMATION);
        hWnd=NULL;							// �� hWnd ��Ϊ NULL
    }

    if (!UnregisterClass("OpenG",hInstance))				// �ܷ�ע����?
    {
        MessageBox(NULL,"����ע�������ࡣ","�رմ���",MB_OK | MB_ICONINFORMATION);
        hInstance=NULL;							// �� hInstance ��Ϊ NULL
    }
}

BOOL GLHelper::CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{

    GLuint		PixelFormat;						// �������ƥ��Ľ��
    WNDCLASSEX	wcex;							// ������ṹ
    DWORD		dwExStyle;						// ��չ���ڷ��
    DWORD		dwStyle;						// ���ڷ��

    RECT WindowRect;							// ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
    WindowRect.left=(long)0;						// ��Left   ��Ϊ 0
    WindowRect.right=(long)width;						// ��Right  ��ΪҪ��Ŀ���
    WindowRect.top=(long)0;							// ��Top    ��Ϊ 0
    WindowRect.bottom=(long)height;						// ��Bottom ��ΪҪ��ĸ߶�

    fullscreen=fullscreenflag;						// ����ȫ��ȫ����־

    hInstance		    = GetModuleHandle(NULL);			// ȡ�����Ǵ��ڵ�ʵ��
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style		    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// �ƶ�ʱ�ػ�����Ϊ����ȡ��DC
    wcex.lpfnWndProc	= (WNDPROC) WindowProc;				// WndProc������Ϣ
    wcex.cbClsExtra		= 0;						// �޶��ⴰ������
    wcex.cbWndExtra		= 0;						// �޶��ⴰ������
    wcex.hInstance		= hInstance;					// ����ʵ��
    wcex.hIcon		    = LoadIcon(NULL, IDI_APPLICATION);			// װ��ȱʡͼ��
    wcex.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);			// װ�����ָ��
    wcex.hbrBackground	= NULL;						// GL����Ҫ����
    wcex.lpszMenuName	= NULL;						// ����Ҫ�˵�
    wcex.lpszClassName	= "OpenG";					// �趨������

    if (!RegisterClassEx(&wcex))						// ����ע�ᴰ����
    {
        MessageBox(NULL,"ע�ᴰ��ʧ��","����",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// �˳�������FALSE
    }

    if (fullscreenflag)								// Ҫ����ȫ��ģʽ��?
    {
        DEVMODE dmScreenSettings;						// �豸ģʽ
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));			// ȷ���ڴ����Ϊ��
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// Devmode �ṹ�Ĵ�С
        dmScreenSettings.dmPelsWidth	= width;				// ��ѡ��Ļ����
        dmScreenSettings.dmPelsHeight	= height;				// ��ѡ��Ļ�߶�
        dmScreenSettings.dmBitsPerPel	= bits;					// ÿ������ѡ��ɫ�����
        dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

        // ����������ʾģʽ�����ؽ����ע: CDS_FULLSCREEN ��ȥ��״̬����
        if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
        {
            // ��ģʽʧ�ܣ��ṩ����ѡ��˳����ڴ��������С�
            if (MessageBox(NULL,"ȫ��ģʽ�ڵ�ǰ�Կ�������ʧ�ܣ�\nʹ�ô���ģʽ��","NeHe&DancingWind G",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
            {
                fullscreen=FALSE;				// ѡ�񴰿�ģʽ(Fullscreen=FALSE)
            }
            else
            {
                // ����һ���Ի��򣬸����û��������
                MessageBox(NULL,"���򽫱��ر�","����",MB_OK|MB_ICONSTOP);
                return FALSE;					//  �˳������� FALSE
            }
        }
    }
    if (fullscreen)								// �Դ���ȫ��ģʽ��?
    {

        dwExStyle=WS_EX_APPWINDOW;					// ǰ����ʾ
        dwStyle=WS_POPUP;						// ������:�ޱ߿�
        //ShowCursor(FALSE);						// �������ָ��
    }
    else
    {
        dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// ǰ����ʾ
        dwStyle=WS_OVERLAPPEDWINDOW;					//  ������
    }

    AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// �������ڴﵽ����Ҫ��Ĵ�С
    /* create main window */
    if (!(hWnd=CreateWindowEx(	dwExStyle,				// ��չ������
                               "OpenG",				// ������
                               title,					// ���ڱ���
                               WS_CLIPSIBLINGS |			// ����Ĵ���������
                               WS_CLIPCHILDREN |			// ����Ĵ���������
                               dwStyle,				// ѡ��Ĵ�������
                               0, 0,					// ����λ��
                               WindowRect.right-WindowRect.left,	// ��������õĴ��ڿ���
                               WindowRect.bottom-WindowRect.top,	// ��������õĴ��ڸ߶�
                               NULL,					// �޸�����
                               NULL,					// �޲˵�
                               hInstance,				// ʵ��
                               NULL)))					// ����WM_CREATE�����κζ���

    {
        KillGLWindow();							// ������ʾ��
        MessageBox(NULL,"���ܴ���һ�������豸������","����",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// ���� FALSE
    }

//�����൱��CodeBlocksԤ�ú���EnableOpenGL�Ĺ���
    static	PIXELFORMATDESCRIPTOR pfd=					// /pfd ���ߴ���������ϣ���Ķ�����������ʹ�õ����ظ�ʽ
    {
        sizeof(PIXELFORMATDESCRIPTOR),					// nSize: ������ʽ�������Ĵ�С
        1,								// �汾��:nVersion
        PFD_DRAW_TO_WINDOW |						// dwFlags:��ʽ֧�ִ���
        PFD_SUPPORT_OPENGL |						// ��ʽ����֧��OpenGL
        PFD_DOUBLEBUFFER,						// ����֧��˫����
        PFD_TYPE_RGBA,							// iPixelType:���� RGBA ��ʽ
        bits,								// cColorbits: ѡ��ɫ�����
        0, 0, 0, 0, 0, 0,						// ���Ե�ɫ��λ
        0,								// ��Alpha����
        0,								// ����Shift Bit
        0,								// ���ۼӻ���
        0, 0, 0, 0,							// ���Ծۼ�λ
        16,								// cDepthBits:16λ Z-���� (��Ȼ���)
        0,								// ���ɰ建��
        0,								// �޸�������
        PFD_MAIN_PLANE,							// iLayerType:����ͼ��
        0,								// Reserved
        0, 0, 0								// ���Բ�����
    };

    if (!(hDC=GetDC(hWnd)))							// ȡ���豸��������ô?
    {
        KillGLWindow();							// ������ʾ��
        MessageBox(NULL,"���ܴ���һ����ƥ������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// ���� FALSE
    }

    if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))				// Windows �ҵ���Ӧ�����ظ�ʽ����?
    {
        KillGLWindow();							// ������ʾ��
        MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// ���� FALSE
    }


    if (!SetPixelFormat(hDC,PixelFormat,&pfd))				// �ܹ��������ظ�ʽô?
    {
        KillGLWindow();							// ������ʾ��
        MessageBox(NULL,"�����������ظ�ʽ","����",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// ���� FALSE
    }

    if (!(hRC=wglCreateContext(hDC)))					// �ܷ�ȡ����ɫ������?
    {
        KillGLWindow();							// ������ʾ��
        MessageBox(NULL,"���ܴ���OpenGL��Ⱦ������","����",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// ���� FALSE
    }

    if (!wglMakeCurrent(hDC,hRC))						// ���Լ�����ɫ������
    {
        KillGLWindow();							// ������ʾ��
        MessageBox(NULL,"���ܼ��ǰ��OpenGL��Ȼ������","����",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// ���� FALSE
    }

    ShowWindow(hWnd,SW_SHOW);						// ��ʾ����
    SetForegroundWindow(hWnd);						// ����������ȼ�
    SetFocus(hWnd);								// ���ü��̵Ľ������˴���
    ResetView(width, height);						// ����͸�� GL ��Ļ

    if (!InitGL())								// ��ʼ���½���GL����
    {
        KillGLWindow();							// ������ʾ��
        MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
        return FALSE;							// ���� FALSE
    }

    return TRUE;								// �ɹ�
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
// GL_BGR_EXT�Ƕ�����glext.h��ģ���ΪBMP��ʽ�ǰ�Blue,Green,Red˳�򴢴�ͼ�����ݵģ�����OpenGL�������෴��GL_BGR_EXT�����������֮���ת���ġ�

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

    //������ɫλͼ
    hBitmap = CreateBitmap(size.cx, size.cy, 1, 1, NULL);
    hOldBmp = (HBITMAP)SelectObject(bm_hDC, hBitmap);
    SetBkColor(bm_hDC, RGB(0, 0, 0));
    SetTextColor(bm_hDC, RGB(255, 255, 255));
    SetBkMode(bm_hDC, OPAQUE);
    TextOut(bm_hDC, 0, 0, text, strlen(text));
    //ת��
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
    //λͼ��Ϣ�����ַ���
    GetDIBits(bm_hDC, hBitmap, 0, bm.bmHeight, pBmpBits, binfo, DIB_RGB_COLORS);
    //�ù������ı���ɫ
    GLfloat LightDiffuse[4]={1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat LightPosition[4]={0.0f, 0.0f, 5.0f, 1.0f};
    glLightfv(GL_LIGHT2, GL_AMBIENT, color);				// ���û�����
    glLightfv(GL_LIGHT2, GL_DIFFUSE, LightDiffuse);				// ���������
    glLightfv(GL_LIGHT2, GL_POSITION, LightPosition);			// ���ù�Դλ��
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);							// ����2�Ź�Դ

    //��ʾ
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glRasterPos3fv(position);
    glBitmap(size.cx, size.cy, 0.0, 0.0, 0.0, 0.0, pBmpBits);

    //����
    delete pBmpBits;
    SelectObject(bm_hDC, hOldBmp);
    DeleteObject(hBitmap);
    DeleteDC(bm_hDC);

    glPopMatrix();
    glDisable(GL_LIGHT2);
    glPopAttrib();

}