#pragma once

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

#include <wx/wx.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <memory>
#include <vector>
#include <cmath>
#include "Scenario.hpp"
#include "VectorField.hpp"
#include "Boat.hpp"
#include "SimulationCanvas.hpp"

namespace tp::presentation {

class Water3DCanvas : public wxPanel {
public:
    Water3DCanvas(wxWindow* parent, wxWindowID id = wxID_ANY,
                  const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = 0, const wxString& name = wxT("Water3DCanvas"));
    
    ~Water3DCanvas();
    
    void setScenario(const domain::Scenario* scenario);
    void setVectorField(const domain::VectorField* field);
    void setBoat(const domain::Boat* boat);
    void setBoatPosition(double x, double y);
    void setSimulationTime(double time);
    
    void setShowGrid(bool show);
    void setShowField(bool show);
    void setShowTrajectory(bool show);
    void setShowBoat(bool show);
    void setShowObstacles(bool show);
    
    void setTrajectory(const std::vector<TrajectoryPoint>& trajectory);
    void clearTrajectory();
    
    void startAnimation();
    void stopAnimation();
    void pauseAnimation();
    void resumeAnimation();
    bool isAnimating() const { return isAnimating_; }
    
    void refreshCanvas();
    void resetGL() { isInitialized_ = false; }
    
    struct Vec3 {
        float x, y, z;
        Vec3(float x_ = 0, float y_ = 0, float z_ = 0) : x(x_), y(y_), z(z_) {}
        Vec3 operator+(const Vec3& o) const { return Vec3(x + o.x, y + o.y, z + o.z); }
        Vec3 operator-(const Vec3& o) const { return Vec3(x - o.x, y - o.y, z - o.z); }
        Vec3 operator*(float s) const { return Vec3(x * s, y * s, z * s); }
        float dot(const Vec3& o) const { return x * o.x + y * o.y + z * o.z; }
        Vec3 normalize() const {
            float len = std::sqrt(x * x + y * y + z * z);
            if (len > 0.0001f) return Vec3(x / len, y / len, z / len);
            return Vec3(0, 1, 0);
        }
    };

private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnTimer(wxTimerEvent& event);
    
    bool initGL();
    void render();
    
    struct WaveParams {
        float amplitude;
        float frequency;
        float directionX;
        float directionZ;
        float speed;
        float phase;
    };
     
    float calculateWaterHeight(float x, float z, double time);
    float sampleVectorFieldMagnitude(float x, float z);
    void sampleVectorFieldDirection(float x, float z, float& dirX, float& dirZ);
    
    Vec3 computeNormal(float x, float z, double time);
    Vec3 computeBlinnPhong(float x, float y, float z, const Vec3& normal);
    
    HGLRC hRC_;
    HDC hDC_;
    bool isInitialized_;
    bool isAnimating_;
    bool isPaused_;
    double simulationTime_;
    double oceanAnimationTime_;
    
    const domain::Scenario* scenario_;
    const domain::VectorField* vectorField_;
    const domain::Boat* boat_;
    double boatX_, boatY_;
    
    bool showGrid_;
    bool showField_;
    bool showTrajectory_;
    bool showBoat_;
    bool showObstacles_;
    std::vector<TrajectoryPoint> trajectory_;
    
    float cameraAngleX_;
    float cameraAngleY_;
    float cameraDistance_;
    
    wxTimer* timer_;
    
    static constexpr int GRID_RESOLUTION = 56;
    static constexpr float OVERSCAN = 0.2f;
    static constexpr float VISIBLE_SIZE = 50.0f;
    
    static constexpr int NUM_WAVES = 4;
    WaveParams waves_[NUM_WAVES];
    
    wxDECLARE_EVENT_TABLE();
};

} // namespace tp::presentation
