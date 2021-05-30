// Serpiente.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Serpiente.h"

#define MAX_LOADSTRING 100
#define TAMSERP 20

#define CUERPO  1
#define CABEZA  2
#define COLA    3

#define IZQ 1
#define DER 2
#define ARRIBA  3
#define ABAJO   4

#define IDT_TIMER1 1

struct pos {
    int x;
    int y;
};
typedef struct pos POS;

struct PedacitoS {
    POS pos;
    int tipo;
    int dir;
};
typedef struct PedacitoS PEDACITOS;

PEDACITOS* NuevaSerpiente(int);
void DibujarSerpiente(HDC, const PEDACITOS *);
int MoverSerpiente(PEDACITOS *, int, RECT, int);
int Colisionar(PEDACITOS *, int);

// Variables globales:
HINSTANCE hInst;                                // instancia actual
WCHAR szTitle[MAX_LOADSTRING];                  // Texto de la barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // nombre de clase de la ventana principal

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Colocar código aquí.

    // Inicializar cadenas globales
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SERPIENTE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Realizar la inicialización de la aplicación:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SERPIENTE));

    MSG msg;

    // Bucle principal de mensajes:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: Registra la clase de ventana.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SERPIENTE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SERPIENTE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: Guarda el identificador de instancia y crea la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Almacenar identificador de instancia en una variable global

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCIÓN: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PROPÓSITO: Procesa mensajes de la ventana principal.
//
//  WM_COMMAND  - procesar el menú de aplicaciones
//  WM_PAINT    - Pintar la ventana principal
//  WM_DESTROY  - publicar un mensaje de salida y volver
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect = {NULL};
    static PEDACITOS* serpiente = NULL;
    static int tams = 5;

    switch (message)
    {
    case WM_CREATE:
        {
            serpiente = NuevaSerpiente(tams);
            SetTimer(hWnd, IDT_TIMER1, 500, NULL);
        }    
        break;
    case WM_TIMER:
        {
            switch (wParam) {
            case IDT_TIMER1:
            {
                GetClientRect(hWnd, &rect);
                if (!MoverSerpiente(serpiente, serpiente[tams - 1].dir, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                InvalidateRect(hWnd, NULL, TRUE);
            }
                break;
            }
        }
        break;
    case WM_KEYDOWN:
        {
            GetClientRect(hWnd, &rect);
            switch (wParam) {
            case VK_UP:
                {
                if (!MoverSerpiente(serpiente, ARRIBA, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            case VK_DOWN:
                {
                if (!MoverSerpiente(serpiente, ABAJO, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            case VK_LEFT:
                {
                if (!MoverSerpiente(serpiente, IZQ, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            case VK_RIGHT:
                {
                if (!MoverSerpiente(serpiente, DER, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            }
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizar las selecciones de menú:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            hdc = BeginPaint(hWnd, &ps);

            DibujarSerpiente(hdc, serpiente);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Controlador de mensajes del cuadro Acerca de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

PEDACITOS * NuevaSerpiente(int tams)
{
    PEDACITOS* serpiente = NULL;
    int i;

    if (tams < 2)
        tams = 2;
    serpiente = (PEDACITOS*)malloc(sizeof(PEDACITOS)*tams);
    if (serpiente == NULL)
    {
        MessageBox(NULL, L"Sin memoria", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }
    serpiente[0].tipo = COLA;
    serpiente[0].pos.x = 1;
    serpiente[0].pos.y = 1;
    serpiente[0].dir = DER;
    for (i = 1; i < tams-1; i++)
    {
        serpiente[i].tipo = CUERPO;
        serpiente[i].pos.x = i+1;
        serpiente[i].pos.y = 1;
        serpiente[i].dir = DER;
    }
    serpiente[i].tipo = CABEZA;
    serpiente[i].pos.x = tams;
    serpiente[i].pos.y = 1;
    serpiente[i].dir = DER;

    return serpiente;
}
void DibujarSerpiente(HDC hdc, const PEDACITOS *serpiente)
{
    int i = 1;
    //se traza la cola según la dirección
    switch (serpiente[0].dir) {
    case DER:
        MoveToEx(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
            serpiente[0].pos.y * TAMSERP, NULL);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP + TAMSERP/2);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
            serpiente[0].pos.y * TAMSERP + TAMSERP);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
            serpiente[0].pos.y * TAMSERP);
        break;
    case IZQ:
        MoveToEx(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP, NULL);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
            serpiente[0].pos.y * TAMSERP + TAMSERP / 2);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP + TAMSERP);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP);
        break;
    case ARRIBA:
        MoveToEx(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP, NULL);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP/2,
            serpiente[0].pos.y * TAMSERP + TAMSERP);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
            serpiente[0].pos.y * TAMSERP);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP);
        break;
    case ABAJO:
        MoveToEx(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP + TAMSERP, NULL);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP/2,
            serpiente[0].pos.y * TAMSERP);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP + TAMSERP,
            serpiente[0].pos.y * TAMSERP + TAMSERP);
        LineTo(hdc, serpiente[0].pos.x * TAMSERP,
            serpiente[0].pos.y * TAMSERP + TAMSERP);
        break;
    }
    //se traza la cabeza según la dirección
    while (serpiente[i].tipo != CABEZA) {
        RoundRect(hdc, serpiente[i].pos.x * TAMSERP, 
            serpiente[i].pos.y * TAMSERP,
            serpiente[i].pos.x * TAMSERP + TAMSERP,
            serpiente[i].pos.y * TAMSERP + TAMSERP,
            5, 5);
        i++;
    }
    //se traza la cabeza según la dirección
    RoundRect(hdc, serpiente[i].pos.x * TAMSERP,
        serpiente[i].pos.y * TAMSERP,
        serpiente[i].pos.x * TAMSERP + TAMSERP,
        serpiente[i].pos.y * TAMSERP + TAMSERP,
        5, 5);

    switch (serpiente[i].dir)
    {
    case DER:
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
            serpiente[i].pos.y * TAMSERP,
            serpiente[i].pos.x * TAMSERP + TAMSERP/2,
            serpiente[i].pos.y * TAMSERP + TAMSERP/2);
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
            serpiente[i].pos.y * TAMSERP + TAMSERP/2,
            serpiente[i].pos.x * TAMSERP + TAMSERP/2,
            serpiente[i].pos.y * TAMSERP + TAMSERP);
        break;
    case IZQ:
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
            serpiente[i].pos.y * TAMSERP,
            serpiente[i].pos.x * TAMSERP + TAMSERP,
            serpiente[i].pos.y * TAMSERP + TAMSERP/2);
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
            serpiente[i].pos.y * TAMSERP + TAMSERP/2,
            serpiente[i].pos.x * TAMSERP + TAMSERP,
            serpiente[i].pos.y * TAMSERP + TAMSERP);
        break;
    case ARRIBA:
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
            serpiente[i].pos.y * TAMSERP + TAMSERP/2,
            serpiente[i].pos.x * TAMSERP + TAMSERP / 2,
            serpiente[i].pos.y * TAMSERP + TAMSERP);
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
            serpiente[i].pos.y * TAMSERP + TAMSERP / 2,
            serpiente[i].pos.x * TAMSERP + TAMSERP,
            serpiente[i].pos.y * TAMSERP + TAMSERP);
        break;
    case ABAJO:
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP,
            serpiente[i].pos.y * TAMSERP,
            serpiente[i].pos.x * TAMSERP + TAMSERP / 2,
            serpiente[i].pos.y * TAMSERP + TAMSERP / 2);
        Ellipse(hdc, serpiente[i].pos.x * TAMSERP + TAMSERP/2,
            serpiente[i].pos.y * TAMSERP,
            serpiente[i].pos.x * TAMSERP + TAMSERP,
            serpiente[i].pos.y * TAMSERP + TAMSERP/2);
        break;
    }
}

int MoverSerpiente(PEDACITOS *serpiente, int dir, RECT rect, int tams)
{
    int i = 0;

    while (serpiente[i].tipo != CABEZA){
        serpiente[i].dir = serpiente[i + 1].dir;
        serpiente[i].pos = serpiente[i + 1].pos;
        i++;
    }

    switch(serpiente[i].dir){
    case DER:
        if (dir != IZQ)
            serpiente[i].dir = dir;
        break;
    case IZQ:
        if (dir != DER)
            serpiente[i].dir = dir;
        break;
    case ARRIBA:
        if (dir != ABAJO)
            serpiente[i].dir = dir;
        break;
    case ABAJO:
        if (dir != ARRIBA)
            serpiente[i].dir = dir;
        break;
    }

    switch (serpiente[i].dir) {
    case DER:
        serpiente[i].pos.x = serpiente[i].pos.x + 1;
        if (serpiente[i].pos.x >= rect.right / TAMSERP)
            serpiente[i].pos.x = 0;
        break;
    case IZQ:
        serpiente[i].pos.x = serpiente[i].pos.x - 1;
        if (serpiente[i].pos.x < 0)
            serpiente[i].pos.x = rect.right/TAMSERP;
        break;
    case ARRIBA:
        serpiente[i].pos.y = serpiente[i].pos.y - 1;
        if (serpiente[i].pos.y < 0)
            serpiente[i].pos.y = rect.bottom/TAMSERP;
        break;
    case ABAJO:
        serpiente[i].pos.y = serpiente[i].pos.y + 1;
        if (serpiente[i].pos.y > rect.bottom/TAMSERP)
            serpiente[i].pos.y = 0;
        break;
    }
    return !Colisionar(serpiente, tams);
}

int Colisionar(PEDACITOS *serpiente, int tams)
{
    int i = 0;

    while (serpiente[i].tipo != CABEZA) {
        if (serpiente[i].pos.x == serpiente[tams-1].pos.x &&
            serpiente[i].pos.y == serpiente[tams-1].pos.y){
            return 1;
        }
        i++;
    }
    return 0;
}