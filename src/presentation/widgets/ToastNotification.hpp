#pragma once

/**
 * @file ToastNotification.hpp
 * @brief Sistema de notificaciones temporales (toast)
 * @author Proyecto Taller Paramétrico
 * @version 0.2.3
 * 
 * Muestra notificaciones emergentes temporales en la UI:
 * - Info: información general
 * - Success: operación exitosa
 * - Warning: precaución
 * - Error: error detectado
 * 
 * Las notificaciones se muestran en cola y desaparecen automáticamente.
 */

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <queue>
#include <chrono>

namespace tp::presentation {

// Tipos de notificación
enum class NotificationType {
    Info,
    Success,
    Warning,
    Error
};

// Estructura de notificación
struct Notification {
    wxString message;
    NotificationType type;
    std::chrono::steady_clock::time_point created;
    int durationMs;
};

// Widget de notificación toast
class ToastNotification : public wxPanel {
public:
    ToastNotification(wxWindow* parent);
    
    void show(const wxString& message, NotificationType type = NotificationType::Info, int durationMs = 3000);
    void update();
    
private:
    void onPaint(wxPaintEvent& event);
    void drawNotification(wxDC& dc);
    wxColour getTypeColor(NotificationType type);
    wxString getTypeIcon(NotificationType type);
    
    std::queue<Notification> notifications_;
    bool showing_;
    
    wxDECLARE_EVENT_TABLE();
};

}
