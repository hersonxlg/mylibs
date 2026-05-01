/* * fenster_strict_size.h
 * Librería STB-style para forzar un tamaño estricto y bloquear el redimensionamiento
 * en Windows usando fenster.h
 */

#ifndef FENSTER_STRICT_SIZE_H
#define FENSTER_STRICT_SIZE_H

#include "fenster.h"
#include <windows.h>

// Declaración de la función pública
void fenster_strict_size(struct fenster *f);

#endif // FENSTER_STRICT_SIZE_H

// ============================================================================
// IMPLEMENTACIÓN
// ============================================================================
#ifdef FENSTER_STRICT_SIZE_IMPLEMENTATION

static WNDPROC fenster_original_proc;

// Hook para interceptar mensajes de Windows y bloquear el redimensionamiento
static LRESULT CALLBACK fenster_ss_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Si Windows pregunta por los límites de la ventana...
    if (msg == WM_GETMINMAXINFO) {
        MINMAXINFO *mmi = (MINMAXINFO*)lParam;
        RECT wr;
        GetWindowRect(hwnd, &wr);
        
        // Calculamos el tamaño total actual
        int win_w = wr.right - wr.left;
        int win_h = wr.bottom - wr.top;
        
        // Forzamos a que el tamaño mínimo y máximo sea el actual (bloqueando el resize)
        mmi->ptMinTrackSize.x = win_w;
        mmi->ptMinTrackSize.y = win_h;
        mmi->ptMaxTrackSize.x = win_w;
        mmi->ptMaxTrackSize.y = win_h;
        
        return 0; // Indicamos que hemos procesado el mensaje
    }
    // Pasamos cualquier otro mensaje al proceso original de la ventana
    return CallWindowProc(fenster_original_proc, hwnd, msg, wParam, lParam);
}

// Función interna que ajusta midiendo la diferencia real en pantalla
static void fenster_ss_fix(HWND hwnd, int target_w, int target_h) {
    RECT client, win;
    
    // Obtenemos el tamaño real de dibujo que Windows nos dio
    GetClientRect(hwnd, &client);
    int current_w = client.right - client.left;
    int current_h = client.bottom - client.top;

    // Calculamos los píxeles que nos faltan (o sobran)
    int diff_w = target_w - current_w;
    int diff_h = target_h - current_h;

    // Obtenemos el tamaño de la ventana completa y le sumamos la diferencia
    GetWindowRect(hwnd, &win);
    int new_win_w = (win.right - win.left) + diff_w;
    int new_win_h = (win.bottom - win.top) + diff_h;

    // Aplicamos el nuevo tamaño
    SetWindowPos(hwnd, NULL, 0, 0, new_win_w, new_win_h, SWP_NOMOVE | SWP_NOZORDER);
}

// Función principal que el usuario llamará en su main
void fenster_strict_size(struct fenster *f) {
    HWND hwnd = GetActiveWindow();
    if (!hwnd) return;

    // 1. Ajuste empírico: 
    // Repetimos 3 veces para asegurar que el DWM de Windows estabilice sus medidas internas.
    for (int i = 0; i < 3; i++) {
        fenster_ss_fix(hwnd, f->width, f->height);
    }

    // 2. Inyectamos nuestro Hook
    // Guardamos el proceso original para no romper la ventana y asignamos el nuestro
    fenster_original_proc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)fenster_ss_proc);
}

#endif // FENSTER_STRICT_SIZE_IMPLEMENTATION
