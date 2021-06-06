// Serpiente.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "Serpiente.h"

//librerías para socket
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "comctl32.lib")
//necesario para enlazar los dll's para el uso de socket
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "4200"

#define MAX_LOADSTRING 100
#define TAMSERP 20

#define CUERPO  1
#define CABEZA  2
#define COLA    3

#define IZQ 1
#define DER 2
#define ARRIBA  3
#define ABAJO   4

#define CRECE   1
#define ACHICA  2
#define NADA    3

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

struct comida {
    POS pos;
    int tipo;
};
typedef struct comida COMIDA;

COMIDA com = { {0,0}, NADA };

PEDACITOS* NuevaSerpiente(int);
void DibujarSerpiente(HDC, const PEDACITOS *);
int MoverSerpiente(PEDACITOS *, int, RECT, int);
PEDACITOS* AjustarSerpiente(PEDACITOS *, int *, int, RECT);
int Colisionar(const PEDACITOS *, int);
int Comer(const PEDACITOS *, int);

// Variables globales:
HINSTANCE hInst;                                // instancia actual
WCHAR szTitle[MAX_LOADSTRING];                  // Texto de la barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // nombre de clase de la ventana principal

char szMiIP[17] = "127.0.0.1"; /*Mi dirección IP*/
char szUsuario[32] = "User"; /*Mi nombre de Usuario actual*/

DWORD WINAPI Servidor(LPVOID argumento); /*función que va hacer lanzada como hilo*/
//int Cliente(HWND hChat, char* szDirIP, PSTR pstrMensaje);
int Cliente(char* szDirIP, const char *pstrMensaje);

//void EnviarMensaje(HWND hChat, HWND hEscribir, HWND hIP);
void EnviarMensaje(char *szMiIP, const char* mensaje);
void Mostrar_Mensaje(HWND hChat, char* szMiIP, char* szUsuario, char* szMsg, COLORREF color);
void Colorear_texto(HWND szChat, char* szUsuario, long ilength, COLORREF color);

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
      // TODO: Colocar código aquí.
    TCHAR tchIP[17], tchszUsuario[32];
    if (_tcslen(lpCmdLine) > 10) {
        swscanf_s(lpCmdLine, L"%s %s", tchIP, tchszUsuario);
        wcstombs(szMiIP, tchIP, 17);
        wcstombs(szUsuario, tchszUsuario, 32);
    }

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
    static int cuenta = 0;
    int wmId = LOWORD(wParam);

    //variables utilizadas para el hilo servidor
    //static HWND hChat, hEscribir, hEnviar, hIP;
    static char msj[17] = "iniciar servidor";
    HFONT hFont;
    static HANDLE hHiloServidor;
    static DWORD idHiloServidor;

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
                EnviarMensaje(szMiIP, "TIMER");
                GetClientRect(hWnd, &rect);
                if (!MoverSerpiente(serpiente, serpiente[tams - 1].dir, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                cuenta++;
                if (cuenta == 15) {
                    if (rand() % 100 < 80) {
                        com.tipo = CRECE;
                    }
                    else {
                        com.tipo = ACHICA;
                    }
                    com.pos.x = rand() % rect.right / TAMSERP;
                    com.pos.y = rand() % rect.bottom / TAMSERP;
                    cuenta = 0;
                }

                if (Comer(serpiente, tams)){
                    serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                    com.tipo = NADA;
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
                EnviarMensaje(szMiIP, "ARRIBA");
                {
                if (!MoverSerpiente(serpiente, ARRIBA, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                if (Comer(serpiente, tams)) {
                    serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                    com.tipo = NADA;
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            case VK_DOWN:
                EnviarMensaje(szMiIP, "ABAJO");
                {
                if (!MoverSerpiente(serpiente, ABAJO, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                if (Comer(serpiente, tams)) {
                    serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                    com.tipo = NADA;
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            case VK_LEFT:
                EnviarMensaje(szMiIP, "IZQUIERDA");
                {
                if (!MoverSerpiente(serpiente, IZQ, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                if (Comer(serpiente, tams)) {
                    serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                    com.tipo = NADA;
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            case VK_RIGHT:
                EnviarMensaje(szMiIP, "DERECHA");
                {
                if (!MoverSerpiente(serpiente, DER, rect, tams)) {
                    KillTimer(hWnd, IDT_TIMER1);
                    MessageBox(hWnd, L"Ya se murió, F", L"Fin del juego",
                        MB_OK | MB_ICONINFORMATION);
                }
                if (Comer(serpiente, tams)) {
                    serpiente = AjustarSerpiente(serpiente, &tams, com.tipo, rect);
                    com.tipo = NADA;
                }
                InvalidateRect(hWnd, NULL, TRUE);
                break;
                }
            }
        }
        break;
    case WM_COMMAND:
        {
            // Analizar las selecciones de menú:
            switch (wmId)
            {
            case IDM_NUEVO:
                if (serpiente != NULL) {
                    KillTimer(hWnd, IDT_TIMER1);
                    free(serpiente);
                    tams = 5;
                    cuenta = 0;
                    serpiente = NuevaSerpiente(tams);
                    SetTimer(hWnd, IDT_TIMER1, 500, NULL);
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                break;
            case IDM_NUEVOCOMPARTIDO:
                hHiloServidor = CreateThread
                (NULL, //atributos de seguridad por defecto
                    0, // usar el tamaño por defecto de la pila
                    Servidor, //nombre de la función hilo
                    (LPVOID)msj, //argumento de la función hilo
                    0, //usar la creación por defecto de bandera
                    &idHiloServidor); //retornar el identificador del hilo

                if (hHiloServidor == NULL) {
                    MessageBox(hWnd, L"Error al crear el hilo para el servidor", L"Error", MB_OK | MB_ICONERROR);
                }
                break;
            case IDM_CONECTARSE:
                hHiloServidor = CreateThread
                (NULL, //atributos de seguridad por defecto
                    0, // usar el tamaño por defecto de la pila
                    Servidor, //nombre de la función hilo
                    (LPVOID)msj, //argumento de la función hilo
                    0, //usar la creación por defecto de bandera
                    &idHiloServidor); //retornar el identificador del hilo

                if (hHiloServidor == NULL) {
                    MessageBox(hWnd, L"Error al crear el hilo para el servidor", L"Error", MB_OK | MB_ICONERROR);
                }
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_SALIR:
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
            if (com.tipo == CRECE) {
                RoundRect(hdc, com.pos.x * TAMSERP,
                    com.pos.y * TAMSERP,
                    com.pos.x * TAMSERP + TAMSERP,
                    com.pos.y * TAMSERP + TAMSERP,
                    7, 7);
            }
            else {
                if (com.tipo == ACHICA) {
                    Ellipse(hdc, com.pos.x * TAMSERP,
                        com.pos.y * TAMSERP,
                        com.pos.x * TAMSERP + TAMSERP,
                        com.pos.y * TAMSERP + TAMSERP);
                }
            }

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        free(serpiente);
        CloseHandle(hHiloServidor);
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

int Colisionar(const PEDACITOS *serpiente, int tams)
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

PEDACITOS *AjustarSerpiente(PEDACITOS *serpiente, int *tams, int comida, RECT rect)
{
    int i;
    PEDACITOS cabeza = serpiente[*tams - 1];
    
    switch (comida) {
    case CRECE:
        (*tams)++;
        serpiente = (PEDACITOS*)realloc(serpiente, sizeof(PEDACITOS) * (*tams));
        serpiente[*tams - 2].tipo = CUERPO;
        serpiente[*tams - 1] = cabeza;
        i = *tams - 1;

        switch (serpiente[i].dir) {
        case DER:
            serpiente[i].pos.x = serpiente[i].pos.x + 1;
            if (serpiente[i].pos.x >= rect.right / TAMSERP)
                serpiente[i].pos.x = 0;
            break;
        case IZQ:
            serpiente[i].pos.x = serpiente[i].pos.x - 1;
            if (serpiente[i].pos.x < 0)
                serpiente[i].pos.x = rect.right / TAMSERP;
            break;
        case ARRIBA:
            serpiente[i].pos.y = serpiente[i].pos.y - 1;
            if (serpiente[i].pos.y < 0)
                serpiente[i].pos.y = rect.bottom / TAMSERP;
            break;
        case ABAJO:
            serpiente[i].pos.y = serpiente[i].pos.y + 1;
            if (serpiente[i].pos.y > rect.bottom / TAMSERP)
                serpiente[i].pos.y = 0;
            break;
        }
        break;
    case ACHICA:
        if (*tams > 2) {
            i = 0;
            while (serpiente[i].tipo != CABEZA) {
                serpiente[i] = serpiente[i + 1];
                i++;
            }
            (*tams)--;
            serpiente = (PEDACITOS*)realloc(serpiente, sizeof(PEDACITOS) * (*tams));
            serpiente[*tams - 1] = cabeza;
        }
        break;
    }
    return serpiente;
}
int Comer(const PEDACITOS* serpiente, int tams)
{
    if (serpiente[tams - 1].pos.x == com.pos.x &&
        serpiente[tams - 1].pos.y == com.pos.y) {
        return 1;
    }
    return 0;
}

//int Cliente(HWND hChat, char* szDirIP, PSTR pstrMensaje) {
int Cliente(char* szDirIP, const char *pstrMensaje) {
    //Varaibles utilizadas para manejar sockets
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    ////////////////////////////////////

    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char szMsg[256]; //Guarda la cadena de mensaje de entrada y salida
    char localhost[] = "localhost";
    char chat[] = "chat";
    TCHAR msgFalla[256];

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wsprintf(msgFalla, L"WSAStartup failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
        return 1;
    }

    //Se limpia la memoria e inicializa la estructura
    //se designa el tipo de direcciones con las que el socket puede comunicarse 
    //(en este caso, la familia de direcciones no está especificada)
    //socket orientado a flujo
    //TCP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    //Resolver la dirección y el puerto del servidor
    iResult = getaddrinfo(szDirIP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        wsprintf(msgFalla, L"getaddrinfo failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeds
    //Intente conectarse a una dirección hasta que uno tenga éxito
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (ConnectSocket == INVALID_SOCKET) {
            wsprintf(msgFalla, L"socket failed with error: %d\n", WSAGetLastError());
            MessageBox(NULL, msgFalla, L"Error en cliente", MB_OK | MB_ICONERROR);
            WSACleanup();
            return 1;
        }

        // Connect to server
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        MessageBox(NULL, L"Unable to connect to server!\n", L"Error en  cliente", MB_OK | MB_ICONERROR);
        sprintf_s(szMsg, "Error en la llamada a connect\nla dirección %s no es válida", szDirIP);
        //Mostrar_Mensaje(hChat, localhost, chat, szMsg, RGB(255, 0, 0));
        WSACleanup();
        return 1;
    }

    // Envío de mensajes al servidor
    sprintf_s(szMsg, "%s %s ", szDirIP, szUsuario);
    iResult = send(ConnectSocket, szMsg, sizeof(char) * 256, 0);
    iResult = recv(ConnectSocket, szMsg, sizeof(char) * 256, 0);

    strcpy_s(szMsg, pstrMensaje);

    iResult = send(ConnectSocket, szMsg, sizeof(char) * 256, 0);
    iResult = shutdown(ConnectSocket, SD_SEND);
    iResult = recv(ConnectSocket, szMsg, sizeof(char) * 256, 0);

    //Mostrar_Mensaje(hChat, szMiIP, szUsuario, szMsg, RGB(0, 0, 255));
    printf("%s %s %s\n", szMiIP, szUsuario, szMsg);

    closesocket(ConnectSocket);
    WSACleanup();

    return 1;
}

DWORD WINAPI Servidor(LPVOID argumento) {

    //HWND hChat = (HWND)argumento;
    char *msj = (char*)argumento;
    int aux;
    TCHAR msgFalla[256];

    //variables para el manejo de socket
    WSADATA wsaData;
    int iResult;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char szBuffer[256], szIP[16], szNN[32]; //Guarda la cadena de mensajes para entrada y salida

    //Inicializar API winsock (similar a las de POSIX)
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        wsprintf(msgFalla, L"WSAStartup failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error en Servidor", MB_OK | MB_ICONERROR);
        return 1;
    }

    //Se limpia la memoria e inicializa las estructuras
    //se designa el tipo de direcciones con las que el socket puede comunicarse 
    //(en este caso, direcciones de Protocolo de Internet v4)
    //socket orientado a flujo
    //TCP
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Resolver la dirección y el puerto del servidor
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        wsprintf(msgFalla, L"getaddrinfo failed with error: %d\n", iResult);
        MessageBox(NULL, msgFalla, L"Error en Servidor", MB_OK | MB_ICONERROR);
        WSACleanup();
        return 1;
    }

    //crear un socket para conectar con el servidor
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        wsprintf(msgFalla, L"socket failed with error: %ld\n", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en Servidor", MB_OK | MB_ICONERROR);
        freeaddrinfo(result);
        return 1;
    }

    //configurar el socket de escucha TCP
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        wsprintf(msgFalla, L"bind failed with error: %d\n", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en Servidor", MB_OK | MB_ICONERROR);
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        wsprintf(msgFalla, L"listen failed with error: %d\n", WSAGetLastError());
        MessageBox(NULL, msgFalla, L"Error en Servidor", MB_OK | MB_ICONERROR);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    while (TRUE) {
        //Aceptar un socket cliente
        //MessageBox(NULL, L"Esperando conexión", L"Depuración", MB_OK);
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            wsprintf(msgFalla, L"accept failed with error: %d\n", WSAGetLastError());
            MessageBox(NULL, msgFalla, L"Error en Servidor", MB_OK | MB_ICONERROR);
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        /**Envío de mensajes al cliente*/
        //Recibe hasta que el par cierre la conexión
        iResult = recv(ClientSocket, szBuffer, sizeof(char) * 256, 0);
        aux = sscanf(szBuffer, "%s %s ", szIP, szNN);
        sprintf_s(szBuffer, "OK");
        //Hacer eco del bufer de nuevo al remitente
        iSendResult = send(ClientSocket, szBuffer, sizeof(char) * 256, 0);
        iResult = recv(ClientSocket, szBuffer, sizeof(char) * 256, 0);
        iSendResult = send(ClientSocket, szBuffer, sizeof(char) * 256, 0);

        //Mostrar_Mensaje(hChat, szIP, szNN, szBuffer, RGB(34, 177, 76));
        printf("%s %s %s\n", szIP, szNN, szBuffer);

        iResult = shutdown(ClientSocket, SD_SEND);
    }
    //limpieza
    closesocket(ClientSocket);
    WSACleanup();

    return 1;
}

/*
void Mostrar_Mensaje(HWND hChat, char* ip, char* szUsuario, char* szMsg, COLORREF color) {
    long iLength;
    char* pstrBuffer = NULL;
    TCHAR* ptchBuffer = NULL;
    DWORD iIni = 0, iFin = 0;
    POINT posScroll;
    int r = 0;
    size_t i;

    int tam = strlen(szUsuario) + strlen(ip) + strlen(szMsg);

    if (NULL == (pstrBuffer = (PSTR)malloc(sizeof(char) * (tam + 10))) ||
        NULL == (ptchBuffer = (TCHAR*)malloc(sizeof(TCHAR) * (tam + 10))))
        MessageBox(NULL, L"Error al reservar memoria", L"Error", MB_OK | MB_ICONERROR);


    sprintf_s(pstrBuffer, tam + 9, "%s -> %s\n%s\n", szUsuario, ip, szMsg);
    tam = strlen(pstrBuffer);
    mbstowcs_s(&i, ptchBuffer, tam + 1, pstrBuffer, tam + 1);


    iLength = GetWindowTextLength(hChat);
    r = SendMessage(hChat, EM_GETLINECOUNT, 0, 0) - 1;

    SendMessage(hChat, EM_SETSEL, (WPARAM)iLength, (LPARAM)iLength + tam);
    SendMessage(hChat, EM_REPLACESEL, FALSE, (LPARAM)ptchBuffer);

    Colorear_texto(hChat, szUsuario, iLength - r, color);

    SetFocus(hChat); //Posiciona el cursor en el editor grande
    SendMessage(hChat, EM_GETSEL, (WPARAM)&iIni, (LPARAM)&iFin);
    SendMessage(hChat, EM_GETSCROLLPOS, 0L, (LPARAM)&posScroll);
    SendMessage(hChat, EM_SETSEL, (WPARAM)iIni, (LPARAM)iFin);
    SendMessage(hChat, EM_SETSCROLLPOS, 0L, (LPARAM)&posScroll);

    free(pstrBuffer);
    free(ptchBuffer);
}

void Colorear_texto(HWND hChat, char* szUsuario, long iLength, COLORREF color) {
    CHARFORMAT2 cf; //Formato del texto
    size_t i;
    // El siguiente segmento de código da formato reemplazando texto
    // normal por texto con formato.
    memset(&cf, 0, sizeof cf); //Se limpia la estructura del formato
    cf.cbSize = sizeof(CHARFORMAT2); //Se fija el tamaño de la estructura
    cf.dwMask = CFM_COLOR; //se establece la máscara para que sea posible aplicar color al texto
    cf.crTextColor = color;

    TCHAR auxiliar[35];

    int tam = strlen(szUsuario);
    mbstowcs_s(&i, auxiliar, szUsuario, sizeof(szUsuario) + 2);

    SendMessage(hChat, EM_SETSEL, (WPARAM)iLength, (LPARAM)iLength + tam);
    SendMessage(hChat, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    SendMessage(hChat, EM_REPLACESEL, FALSE, (LPARAM)auxiliar);
    cf.crTextColor = RGB(0, 0, 0);
}

*/
//void EnviarMensaje(HWND hChat, HWND hEscribir, HWND hIP) {
void EnviarMensaje(char* szDirIP ,const char *mensaje) {
    /*TCHAR tchDirIP[16];
    char szDirIP[16];
    int tam = 0;
    size_t i;

    GetWindowText(hIP, tchDirIP, 16);
    tam = GetWindowTextLength(hIP);
    wcstombs(szDirIP, tchDirIP, tam);
    szDirIP[tam] = '\0';

    long iLength;
    PSTR pstrBuffer;
    TCHAR* ptchBuffer;

    iLength = GetWindowTextLength(hEscribir);
    if (NULL == (pstrBuffer = (PSTR)malloc(sizeof(char) * (iLength + 2))) ||
        NULL == (ptchBuffer = (TCHAR*)malloc(sizeof(TCHAR) * (iLength + 2))))
        MessageBox(NULL, L"Error al reservar memoria", L"Error", MB_OK | MB_ICONERROR);
    else
    {
        GetWindowText(hEscribir, ptchBuffer, iLength + 1);
        wcstombs_s(&i, pstrBuffer, (iLength + 1), ptchBuffer, (iLength + 1));
        pstrBuffer[iLength + 1] = '\0';

        Cliente(hChat, szDirIP, pstrBuffer);

        SetWindowText(hEscribir, L"");
        free(pstrBuffer);
        free(ptchBuffer);
    }*/

    Cliente(szDirIP, mensaje);

}