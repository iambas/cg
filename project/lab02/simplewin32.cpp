// simplewin32.cpp : Defines the entry point for the application.
//
#include <stdio.h>
#include "simplewin32.h"
#define MAX_LOADSTRING  100

#include <gl\gl.h>
#include <gl\glu.h>

#include <math.h>

// object
GLuint earth, stars, moon;
// rotate
float ae = 0, am = 0;

// orbit
double timeMultiply = 0.001;
double degree = 50;
double perihelion = 50;
double scaleOwnAxisRotation = 30;
double Aphelion = 0;
double OrbitalPeriod = 0.07;
double CurrentLocationOwnAxis;
double CurrentLocationInSystem;
//Vector3 Translate;
double PositionX;
double PositionY;
double PositionZ;


// Global Variables:
HINSTANCE   hInst;                          // current instance
char        szTitle [MAX_LOADSTRING];       // The title bar text
char        szWindowClass [MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM                    RegisterMainClass (HINSTANCE hInstance);
BOOL                    InitInstance (HINSTANCE, int);
LRESULT CALLBACK        WndProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK        About (HWND, UINT, WPARAM, LPARAM);

HWND			hWndText;
char			szText [241];
unsigned char	*image	= NULL;			// image array
long			 bpp, cx = 0, cy = 0;	// image dimension
BITMAPINFO		 bi;

// OpenGL related variables
#define clamp(x) x = x > 360.0f ? x-360.0f : x < -360.0f ? x+=360.0f : x

HWND		m_hWnd	= NULL;
HDC			m_hDC	= NULL;
HGLRC		m_hRC	= NULL;
float		m_rot [2] = {0.0f, 0.0f}, m_zdist = -10.0f, m_px = 0.0, m_py = 0.0;
int			m_nDrag = 0;
int			m_elapse = 0;	


int APIENTRY _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    MSG     msg;
    HACCEL  hAccelTable;

    // Initialize global strings
    strcpy (szTitle, "Computer Graphic 523451");
    strcpy (szWindowClass, "simplewin32");
    RegisterMainClass (hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) 
    {
        return FALSE;
    }

    hAccelTable = LoadAccelerators (hInstance, MAKEINTRESOURCE (IDC_SIMPLEWIN32));

    // Main message loop:
    while (GetMessage (&msg, NULL, 0, 0)) 
    {
        if (!TranslateAccelerator (msg.hwnd, hAccelTable, &msg)) 
        {
            TranslateMessage (&msg);
            DispatchMessage (&msg);
        }
    }

    return (int) msg.wParam;
}

void resize (int cx, int cy)
{
    ::glMatrixMode (GL_PROJECTION);
    ::glLoadIdentity ();

    ::gluPerspective (30.0, (float) cx/cy, 0.001, 40.0);
    ::glViewport (0, 0, cx, cy);

    ::glMatrixMode (GL_MODELVIEW);
    ::glLoadIdentity ();

    ::glEnable (GL_DEPTH_TEST);
    ::glDepthFunc (GL_LESS);
}

void initlighting (void)
{
    //  initalize light
    GLfloat ambient [4]     = {  0.20f, 0.20f, 0.20f, 0.5f };
    GLfloat diffuse [4]     = {  0.80f, 0.80f, 0.80f, 0.9f };
    GLfloat position0 []    = { -0.2f, 0.5f, +5.0f, 0.0f };
    GLfloat position1 []    = { -0.2f, 0.5f, -5.0f, 0.0f };

    GLfloat materialShininess [1]   = { 8.0f };

    // enable all the lighting & depth effects
    ::glLightfv (GL_LIGHT0, GL_AMBIENT, ambient);
    ::glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuse);
    ::glLightfv (GL_LIGHT0, GL_POSITION, position0);

    ::glLightfv (GL_LIGHT1, GL_AMBIENT, ambient);
    ::glLightfv (GL_LIGHT1, GL_DIFFUSE, diffuse);
    ::glLightfv (GL_LIGHT1, GL_POSITION, position1);

	::glShadeModel (GL_FLAT);
    ::glMaterialfv (GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);

    ::glEnable (GL_LIGHTING);
    ::glEnable (GL_LIGHT0);
	::glEnable (GL_LIGHT1);

    ::glEnable (GL_BLEND);
    ::glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ::glEnable (GL_LINE_SMOOTH);
}

void initopengl (void)
{
    PIXELFORMATDESCRIPTOR   pfd;
    int                     format;
    RECT                    rcclient;

    m_hDC   =   GetDC (m_hWnd);

    // set the pixel format for the DC
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize       = sizeof (pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 16;
    pfd.cDepthBits  = 32;
    pfd.iLayerType  = PFD_MAIN_PLANE;

    format = ChoosePixelFormat (m_hDC, &pfd);
    SetPixelFormat (m_hDC, format, &pfd);

    // create the render context (RC)
    m_hRC = wglCreateContext (m_hDC);

    // make it the current render context
    wglMakeCurrent (m_hDC, m_hRC);

    initlighting ();

	GetClientRect (m_hWnd, &rcclient);
	resize (rcclient.right, rcclient.bottom);
}

void purge (void)
{
    if (m_hRC) {
        wglMakeCurrent (NULL, NULL);
        wglDeleteContext (m_hRC);
    }

	if (m_hWnd && m_hDC) {
        ReleaseDC (m_hWnd, m_hDC);
    }

    m_hDC           = NULL;
    m_hRC           = NULL;
}

void recoverRigidDisplay (void)
{
    ::glTranslatef (-0.50f, -0.50f, m_zdist);

	// rotate top-bottom
    ::glRotatef (m_rot [0], 1.0f, 0.0f, 0.0f);
	// rotate left-right
    ::glRotatef (m_rot [1], 0.0f, 1.0f, 0.0f);
}

void mouse_update (float cx, float cy)
{
    if (m_nDrag == 1)
	{
		m_rot [0] += ((cy - m_py) * 0.1f);
		m_rot [1] -= ((cx - m_px) * 0.1f);

		clamp (m_rot[0]);
		clamp (m_rot[1]);
	}
	else
	{
		m_zdist += (cy - m_py)*0.01f;
		if (m_zdist > 0) m_zdist = 0;
		if (m_zdist < -17) m_zdist = -17;
	}

    m_px = cx;
    m_py = cy;
}

unsigned char *LoadBmp(char *fn, int *wi, int *hi)
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	WORD bits;
	FILE *t24;
	unsigned char *lpBitmapBits;
	long imagesize, nc;

	// read the bitmap
	t24 = fopen((char *)fn, "rb");
	if (t24 == NULL) { printf("Could not open input file\n"); exit(0); }
	fread((char *)&bmfh, sizeof(BITMAPFILEHEADER), 1, t24);
	fread((char *)&bmih, sizeof(BITMAPINFOHEADER), 1, t24);
	if (bmih.biClrUsed != 0)nc = bmih.biClrUsed;
	else {
		bits = bmih.biBitCount;
		switch (bits) {
		case 1:    nc = 2;   break;
		case 4:    nc = 16;  break;
		case 8:    nc = 256; break;
		default:   nc = 0;   break;
		}
	}
	if (nc > 0) { printf("Cannot handle paletted image\n"); exit(0); }
	imagesize = bmfh.bfSize - bmfh.bfOffBits;
	if ((lpBitmapBits = (unsigned char *)malloc(imagesize)) == NULL) { fclose(t24); exit(0); }
	fread((char *)lpBitmapBits, imagesize, 1, t24);
	fclose(t24);
	*wi = bmih.biWidth; *hi = bmih.biHeight;
	return lpBitmapBits;
}

GLuint AddTexture(char * filename)
{
	GLuint texture;
	unsigned char *pix;
	int w, h;
	glEnable(GL_TEXTURE_2D);
	pix = LoadBmp(filename, &w, &h);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pix);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	free(pix); //deletes the data held by pix
	glDisable(GL_TEXTURE_2D);

	return texture;
}

void orbit() {
	long newStopWatch = 1000;
	degree = timeMultiply * ((newStopWatch) / 20);
	CurrentLocationOwnAxis += degree * scaleOwnAxisRotation;
	CurrentLocationInSystem += degree / (OrbitalPeriod == 0 ? 1 : OrbitalPeriod);

	CurrentLocationInSystem = CurrentLocationInSystem > 360 ? CurrentLocationInSystem - 360 : CurrentLocationInSystem;
	CurrentLocationOwnAxis = CurrentLocationOwnAxis > 360 ? CurrentLocationOwnAxis - 360 : CurrentLocationOwnAxis;

	const double pi = 3.1415;
	const double degreeToRadian = 57.29577951;
	double translationInSystemX = 0.05 * (Aphelion + perihelion);
	double translationInSystemZ = 0.05 * (Aphelion + perihelion) / 2;
	PositionX = -cos((double)(CurrentLocationInSystem / degreeToRadian + pi)) *translationInSystemX;
	PositionY = 0;
	PositionZ = sin((double)(CurrentLocationInSystem / degreeToRadian + pi)) * translationInSystemZ;
}

void draw(GLuint texture, float radius)
{
	GLUquadric *qobj = gluNewQuadric();
	gluQuadricTexture(qobj, GL_TRUE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glPushMatrix();
	if (texture == earth) {
		glRotatef(ae++, 1.0f, 1.0f, 1.0f);
	}
	else if (texture == moon) {
		glRotatef(am+=1, 1.0f, 1.0f, 1.0f);
	}
	gluSphere(qobj, radius, 100, 100);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	gluDeleteQuadric(qobj);
}

void initTexture()
{
	earth = AddTexture("Earth.bmp");
	stars = AddTexture("Stars.bmp");
	moon = AddTexture("Moon.bmp");
}

void display (void)
{
    ::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ::glPushMatrix ();

	orbit();

	// rotate and zoom
	recoverRigidDisplay ();
	// draw object
	draw(earth, 1);
	draw(stars, 20);

	glTranslated(PositionX, PositionY, PositionZ);
	draw(moon, 0.33);

    ::glFlush();
    ::glPopMatrix ();
    ::SwapBuffers (m_hDC);          /* nop if singlebuffered */
}

ATOM RegisterMainClass (HINSTANCE hInstance)
{
    WNDCLASSEX  wcex;

    wcex.cbSize = sizeof (WNDCLASSEX); 

    wcex.style              = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc        = (WNDPROC)WndProc;
    wcex.cbClsExtra         = 0;
    wcex.cbWndExtra         = 0;
    wcex.hInstance          = hInstance;
    wcex.hIcon              = LoadIcon (hInstance, (LPCTSTR) IDI_SIMPLEWIN32);
    wcex.hCursor            = LoadCursor (NULL, IDC_ARROW);
    wcex.hbrBackground      = (HBRUSH) (COLOR_WINDOW+1);
    wcex.lpszMenuName       = (LPCTSTR) IDC_SIMPLEWIN32;
    wcex.lpszClassName      = szWindowClass;
    wcex.hIconSm            = LoadIcon (wcex.hInstance, (LPCTSTR) IDI_SMALL);

    return RegisterClassEx (&wcex);
}

BOOL InitInstance (HINSTANCE hInstance, int nCmdShow)
{
    hInst   = hInstance; // Store instance handle in our global variable
	// width 1400, height 800
    m_hWnd	= CreateWindow (szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                            0, 0, 1400, 750, NULL, NULL, hInstance, NULL);

	if (!m_hWnd)
    {
        return FALSE;
    }

    ShowWindow (m_hWnd, nCmdShow);
	UpdateWindow (m_hWnd);

	
	initopengl();
	initTexture();

	SetTimer (m_hWnd, 100, 10, NULL);

    return TRUE;
}

void mydraw (HDC hdc)
{
	if (image == NULL) return;

	SetDIBitsToDevice (hdc, 0, 0, cx, cy, 0, 0, 0, cy, image, &bi, DIB_RGB_COLORS);
}
 
void process (unsigned char *ig, long w, long h)
{
	int	r = 0, g = 255, b = 0;
	int x, y;

//	ig [3*(10 + 100*w)+0] = 255;

	for (y = 0; y < h; y ++)
		for (x = 0; x < w; x ++) {
			ig [3*(x + y*w) + 0] = y;
			ig [3*(x + y*w) + 1] = g;
			ig [3*(x + y*w) + 2] = x;
		}
}

void initfbuffer (int w, int h)
{
	bi.bmiHeader.biSize			= sizeof (BITMAPINFOHEADER);
	bi.bmiHeader.biWidth		= w;
	bi.bmiHeader.biHeight		= -h;
	bi.bmiHeader.biPlanes		= 1;
	bi.bmiHeader.biBitCount		= 24;
	bi.bmiHeader.biCompression	= BI_RGB;
	bi.bmiHeader.biSizeImage	= w*h*3;

	bi.bmiHeader.biXPelsPerMeter	= 0;
	bi.bmiHeader.biYPelsPerMeter	= 0;
	bi.bmiHeader.biClrUsed			= 0;
	bi.bmiHeader.biClrImportant		= 0;

	cx		= w;
	cy		= h;
	image	= (unsigned char *) malloc (cx*cy*3);
	memset (image, 0, w*h*3);
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int         wmId, wmEvent;
	int			tm;
/*    PAINTSTRUCT ps;
    HDC         hdc;
*/
    switch (message) 
    {
		case WM_TIMER:		if (wParam == 100) {
								display ();
								m_elapse ++;
								m_elapse = m_elapse % 21;
							}
							break;

		case WM_COMMAND :   wmId    = LOWORD (wParam);
                            wmEvent = HIWORD (wParam);

                            // Parse the menu selections:
                            switch (wmId)
                            {
                                case IDM_ABOUT  :   DialogBox (hInst, (LPCTSTR) IDD_ABOUTBOX, hWnd, (DLGPROC) About);
                                                    break;

								case IDM_EXIT   :   initopengl ();
													/*initfbuffer (320, 240);
													process (image, 320, 240);
													hdc = GetDC (hWnd);
													mydraw (hdc);
													ReleaseDC (hWnd, hdc);
													*/
													break;
                                                       
                                default         :   
                                                    return DefWindowProc(hWnd, message, wParam, lParam);
                            }
                            break;
/*
        case WM_PAINT   :   display ();
							break;
*/
		case WM_LBUTTONDOWN :   SetCapture (m_hWnd);
								m_px    =   (float) LOWORD (lParam);
                                m_py    =   (float) HIWORD (lParam);
                                m_nDrag = 1;
                                break;

        case WM_LBUTTONUP   :   ReleaseCapture ();
                                m_px    = 0.0f;
                                m_py    = 0.0f;
                                m_nDrag = 0;
								display ();
                                break;

        case WM_RBUTTONDOWN :   SetCapture (m_hWnd);
                                m_px    =   (float) LOWORD (lParam);
                                m_py    =   (float) HIWORD (lParam);
                                m_nDrag = 2;
                                break;

        case WM_RBUTTONUP   :   ReleaseCapture ();
                                m_px    = 0.0f;
                                m_py    = 0.0f;
                                m_nDrag = 0;
								display ();
                                break;

        case WM_MOUSEMOVE   :   if (m_nDrag)
                                {
                                    int mx, my;

                                    mx = LOWORD (lParam);
                                    my = HIWORD (lParam);

                                    if (mx & (1 << 15)) mx -= (1 << 16);
                                    if (my & (1 << 15)) my -= (1 << 16);

                                    mouse_update ((float) mx, (float) my);
                                    display ();
                                }
                                break;

        case WM_DESTROY :	purge ();//if (image != NULL) free (image);
							PostQuitMessage (0);
							KillTimer (m_hWnd, 100);
                            break;

        default         :   return DefWindowProc (hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
LRESULT CALLBACK About (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG  :   return TRUE;

        case WM_COMMAND     :   if (LOWORD (wParam) == IDOK || LOWORD (wParam) == IDCANCEL)
                                {
                                    EndDialog (hDlg, LOWORD (wParam));
                                    return TRUE;
                                }
                                break;
    }
    return FALSE;
}

LRESULT CALLBACK Event (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_INITDIALOG  :   return TRUE;

        case WM_COMMAND     :   if (LOWORD (wParam) == IDOK || LOWORD (wParam) == IDCANCEL)
                                {
                                    EndDialog (hDlg, LOWORD (wParam));
                                    return TRUE;
                                }
                                break;
    }
    return FALSE;
}

