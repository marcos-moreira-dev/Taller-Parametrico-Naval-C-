#include "widgets/ToastNotification.hpp"
#include <wx/dcclient.h>
#include <wx/dcgraph.h>

namespace tp::presentation {

wxBEGIN_EVENT_TABLE(ToastNotification, wxPanel)
    EVT_PAINT(ToastNotification::onPaint)
wxEND_EVENT_TABLE()

ToastNotification::ToastNotification(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(300, 60))
    , showing_(false)
{
    SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
    Hide();
}

void ToastNotification::show(const wxString& message, NotificationType type, int durationMs)
{
    Notification notif;
    notif.message = message;
    notif.type = type;
    notif.created = std::chrono::steady_clock::now();
    notif.durationMs = durationMs;
    
    notifications_.push(notif);
    
    if (!showing_) {
        showing_ = true;
        Show();
        Refresh();
    }
}

void ToastNotification::update()
{
    if (notifications_.empty()) {
        showing_ = false;
        Hide();
        return;
    }
    
    auto& current = notifications_.front();
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - current.created).count();
    
    if (elapsed > current.durationMs) {
        notifications_.pop();
        if (notifications_.empty()) {
            showing_ = false;
            Hide();
        } else {
            Refresh();
        }
    }
}

void ToastNotification::onPaint(wxPaintEvent& event)
{
    (void)event;
    wxPaintDC dc(this);
    drawNotification(dc);
}

void ToastNotification::drawNotification(wxDC& dc)
{
    if (notifications_.empty()) return;
    
    const auto& current = notifications_.front();
    
    wxSize size = GetClientSize();
    int width = size.GetWidth();
    int height = size.GetHeight();
    
    // Color según tipo
    wxColour bgColor = getTypeColor(current.type);
    
    // Fondo redondeado
    dc.SetBrush(wxBrush(bgColor));
    dc.SetPen(wxPen(bgColor));
    dc.DrawRoundedRectangle(0, 0, width, height, 8);
    
    // Icono
    dc.SetTextForeground(wxColour(255, 255, 255));
    dc.SetFont(wxFont(16, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    wxString icon = getTypeIcon(current.type);
    dc.DrawText(icon, 15, (height - 20) / 2);
    
    // Mensaje
    dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    wxString msg = current.message;
    // Truncar si es muy largo
    if (msg.Length() > 35) {
        msg = msg.Left(32) + wxT("...");
    }
    dc.DrawText(msg, 45, (height - 16) / 2);
}

wxColour ToastNotification::getTypeColor(NotificationType type)
{
    switch (type) {
        case NotificationType::Success:
            return wxColour(46, 204, 113);  // Verde
        case NotificationType::Warning:
            return wxColour(241, 196, 15);  // Amarillo
        case NotificationType::Error:
            return wxColour(231, 76, 60);   // Rojo
        case NotificationType::Info:
        default:
            return wxColour(52, 152, 219);  // Azul
    }
}

wxString ToastNotification::getTypeIcon(NotificationType type)
{
    switch (type) {
        case NotificationType::Success:
            return wxT("✓");
        case NotificationType::Warning:
            return wxT("⚠");
        case NotificationType::Error:
            return wxT("✕");
        case NotificationType::Info:
        default:
            return wxT("ℹ");
    }
}

}
