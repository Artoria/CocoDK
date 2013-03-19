#ifndef _GLHELPER_H_
#define _GLHELPER_H_

class GLHelper
{
public:
    HINSTANCE hInstance;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
//    GLuint	filter;									// ¬À≤®¿‡–Õ

    float cx, cy, cz;
    float gamma, theta, rotz;

    GLHelper();
    ~GLHelper(){}

    void ResetView(int width, int height);
    int InitGL(void);
    void KillGLWindow(GLvoid);
    BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);
    bool LoadTexture(LPTSTR szFileName, GLuint &texid);
    void PrintText(HFONT hFont, LPTSTR text, float position[3], float color[4]);
};

#endif // _GLHELPER_H_
