#include "Water3DCanvas.hpp"
#include <wx/timer.h>
#include <vector>
#include <iostream>

#if !defined(ssize_t) && !defined(_SSIZE_T_DEFINED)
#define ssize_t long
#define _SSIZE_T_DEFINED
#endif

namespace {

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
    Vec3 cross(const Vec3& o) const {
        return Vec3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }
};

}

namespace tp::presentation {

wxBEGIN_EVENT_TABLE(Water3DCanvas, wxPanel)
    EVT_PAINT(Water3DCanvas::OnPaint)
    EVT_SIZE(Water3DCanvas::OnSize)
    EVT_TIMER(wxID_ANY, Water3DCanvas::OnTimer)
wxEND_EVENT_TABLE()

Water3DCanvas::Water3DCanvas(wxWindow* parent, wxWindowID id,
                             const wxPoint& pos, const wxSize& size,
                             long style, const wxString& name)
    : wxPanel(parent, id, pos, size, style, name),
      hRC_(nullptr),
      hDC_(nullptr),
      isInitialized_(false),
      isAnimating_(false),
      isPaused_(false),
      simulationTime_(0.0),
      oceanAnimationTime_(0.0),
      scenario_(nullptr),
      vectorField_(nullptr),
      boat_(nullptr),
      boatX_(10.0),
      boatY_(10.0),
      showGrid_(false),
      showField_(true),
      showTrajectory_(true),
      showBoat_(true),
      showObstacles_(true),
      cameraAngleX_(0.0f),
      cameraAngleY_(0.0f),
      cameraDistance_(0.0f),
      timer_(nullptr) {
    
    waves_[0] = {0.15f, 2.0f, 1.0f, 0.0f, 4.8f, 0.0f};
    waves_[1] = {0.12f, 1.5f, 0.0f, 1.0f, 3.9f, 1.57f};
    waves_[2] = {0.08f, 3.0f, 1.0f, 1.0f, 5.6f, 3.14f};
    waves_[3] = {0.05f, 4.0f, 1.0f, -1.0f, 6.4f, 0.5f};
    
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}

Water3DCanvas::~Water3DCanvas() {
    if (timer_) {
        timer_->Stop();
        delete timer_;
    }
    
    if (hRC_ && hDC_) {
        wglMakeCurrent(hDC_, hRC_);
        wglDeleteContext(hRC_);
        ::ReleaseDC((HWND)GetHandle(), hDC_);
    }
}

void Water3DCanvas::setScenario(const domain::Scenario* scenario) {
    scenario_ = scenario;
    refreshCanvas();
}

void Water3DCanvas::setVectorField(const domain::VectorField* field) {
    vectorField_ = field;
    refreshCanvas();
}

void Water3DCanvas::setBoat(const domain::Boat* boat) {
    boat_ = boat;
    if (boat_) {
        auto pos = boat_->getPosition();
        boatX_ = pos.x;
        boatY_ = pos.y;
    }
}

void Water3DCanvas::setBoatPosition(double x, double y) {
    boatX_ = x;
    boatY_ = y;
}

void Water3DCanvas::setSimulationTime(double time) {
    simulationTime_ = time;
}

void Water3DCanvas::setShowGrid(bool show) {
    showGrid_ = show;
    refreshCanvas();
}

void Water3DCanvas::setShowField(bool show) {
    showField_ = show;
    refreshCanvas();
}

void Water3DCanvas::setShowTrajectory(bool show) {
    showTrajectory_ = show;
    refreshCanvas();
}

void Water3DCanvas::setShowBoat(bool show) {
    showBoat_ = show;
    refreshCanvas();
}

void Water3DCanvas::setShowObstacles(bool show) {
    showObstacles_ = show;
    refreshCanvas();
}

void Water3DCanvas::setTrajectory(const std::vector<TrajectoryPoint>& traj) {
    trajectory_ = traj;
    refreshCanvas();
}

void Water3DCanvas::clearTrajectory() {
    trajectory_.clear();
    refreshCanvas();
}

void Water3DCanvas::startAnimation() {
    if (isAnimating_) return;
    
    isAnimating_ = true;
    isPaused_ = false;
    
    if (!timer_) {
        timer_ = new wxTimer(this);
    }
    timer_->Start(16);
}

void Water3DCanvas::stopAnimation() {
    isAnimating_ = false;
    isPaused_ = false;
    oceanAnimationTime_ = 0.0;
    
    if (timer_) {
        timer_->Stop();
    }
}

void Water3DCanvas::pauseAnimation() {
    if (!isAnimating_) return;
    isPaused_ = true;
    if (timer_) timer_->Stop();
}

void Water3DCanvas::resumeAnimation() {
    if (!isAnimating_) return;
    isPaused_ = false;
    if (timer_) timer_->Start(16);
}

void Water3DCanvas::refreshCanvas() {
    wxPanel::Refresh(false);
}

void Water3DCanvas::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    (void)dc;
    
    if (!IsShown()) return;
    
    HWND hwnd = (HWND)GetHandle();
    hDC_ = ::GetDC(hwnd);
    if (!hDC_) return;
    
    if (!isInitialized_) {
        initGL();
        isInitialized_ = true;
    }

    wglMakeCurrent(hDC_, hRC_);
    
    int w, h;
    GetClientSize(&w, &h);
    if (w > 0 && h > 0) {
        glViewport(0, 0, w, h);
    }

    render();
    wglMakeCurrent(nullptr, nullptr);
    ::SwapBuffers(hDC_);
    ::ReleaseDC(hwnd, hDC_);
}

void Water3DCanvas::OnSize(wxSizeEvent& event) {
    if (IsShown() && isInitialized_ && hRC_) {
        HWND hwnd = (HWND)GetHandle();
        HDC tempDC = ::GetDC(hwnd);
        if (tempDC) {
            wglMakeCurrent(tempDC, hRC_);
            int w, h;
            GetClientSize(&w, &h);
            glViewport(0, 0, w, h);
            wglMakeCurrent(nullptr, nullptr);
            ::ReleaseDC(hwnd, tempDC);
        }
    }
    event.Skip();
}

void Water3DCanvas::OnTimer(wxTimerEvent& event) {
    (void)event;
    if (isAnimating_ && !isPaused_) {
        oceanAnimationTime_ += 0.033;
        refreshCanvas();
    }
}

bool Water3DCanvas::initGL() {
    HWND hwnd = (HWND)GetHandle();
    hDC_ = ::GetDC(hwnd);
    if (!hDC_) return false;
    
    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    
    int pixelFormat = ChoosePixelFormat(hDC_, &pfd);
    if (!pixelFormat) return false;
    
    SetPixelFormat(hDC_, pixelFormat, &pfd);
    
    hRC_ = wglCreateContext(hDC_);
    if (!hRC_) return false;
    
    wglMakeCurrent(hDC_, hRC_);
    
    glClearColor(0.05f, 0.08f, 0.12f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    
    float lightPos[4] = {8.0f, 12.0f, 8.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
    float lightAmbient[4] = {0.25f, 0.25f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    
    float lightDiffuse[4] = {0.8f, 0.85f, 0.9f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    
    float lightSpecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    
    float matAmbient[4] = {0.1f, 0.15f, 0.25f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, matAmbient);
    
    float matDiffuse[4] = {0.2f, 0.4f, 0.6f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, matDiffuse);
    
    float matSpecular[4] = {0.8f, 0.8f, 0.9f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpecular);
    
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 64.0f);
    
    wglMakeCurrent(nullptr, nullptr);
    return true;
}

float Water3DCanvas::sampleVectorFieldMagnitude(float x, float z) {
    if (!vectorField_) return 0.0f;
    
    double vx, vz;
    try {
        Vec2d val = vectorField_->getValue(x, z);
        vx = val.x;
        vz = val.y;
    } catch (...) {
        return 0.0f;
    }
    
    return std::sqrt(static_cast<float>(vx * vx + vz * vz));
}

void Water3DCanvas::sampleVectorFieldDirection(float x, float z, float& dirX, float& dirZ) {
    if (!vectorField_) {
        dirX = 1.0f;
        dirZ = 0.0f;
        return;
    }
    
    try {
        Vec2d val = vectorField_->getValue(x, z);
        float mag = std::sqrt(static_cast<float>(val.x * val.x + val.y * val.y));
        if (mag > 0.001f) {
            dirX = static_cast<float>(val.x / mag);
            dirZ = static_cast<float>(val.y / mag);
        } else {
            dirX = 1.0f;
            dirZ = 0.0f;
        }
    } catch (...) {
        dirX = 1.0f;
        dirZ = 0.0f;
    }
}

float Water3DCanvas::calculateWaterHeight(float x, float z, double time) {
    float height = 0.0f;
    float fieldDirX, fieldDirZ;
    sampleVectorFieldDirection(x, z, fieldDirX, fieldDirZ);
    float fieldMag = sampleVectorFieldMagnitude(x, z);

    float advectedX = x;
    float advectedZ = z;
    if (vectorField_) {
        Vec2d v = vectorField_->getValue(x, z);
        const float advectionStrength = 0.35f;
        advectedX = x - static_cast<float>(v.x) * static_cast<float>(time) * advectionStrength;
        advectedZ = z - static_cast<float>(v.y) * static_cast<float>(time) * advectionStrength;
    }
    
    float alpha = 0.5f;
    float beta = 0.6f;
    float gamma = 2.0f;
    
    for (int i = 0; i < NUM_WAVES; ++i) {
        float baseAmp = waves_[i].amplitude;
        float amp = baseAmp * (1.0f + alpha * fieldMag);
        
        float baseDirX = waves_[i].directionX;
        float baseDirZ = waves_[i].directionZ;
        float len = std::sqrt(baseDirX * baseDirX + baseDirZ * baseDirZ);
        if (len > 0.001f) {
            baseDirX /= len;
            baseDirZ /= len;
        }
        
        float dirX = (1.0f - beta) * baseDirX + beta * fieldDirX;
        float dirZ = (1.0f - beta) * baseDirZ + beta * fieldDirZ;
        len = std::sqrt(dirX * dirX + dirZ * dirZ);
        if (len > 0.001f) {
            dirX /= len;
            dirZ /= len;
        }
        
        float k = waves_[i].frequency;
        float omega = waves_[i].speed;
        float phase = waves_[i].phase + gamma * (fieldDirX * baseDirX + fieldDirZ * baseDirZ);
        
        float dotProduct = dirX * advectedX + dirZ * advectedZ;
        height += amp * sinf(k * dotProduct - omega * static_cast<float>(time) + phase);
    }

    float travelingRipple = 0.06f * sinf(0.24f * advectedX + 0.17f * advectedZ - 2.4f * static_cast<float>(time));
    float localizedRipple = 0.03f * fieldMag * sinf(0.12f * advectedX + 0.09f * advectedZ - 1.9f * static_cast<float>(time));
    height += travelingRipple + localizedRipple;
    
    return height;
}

Water3DCanvas::Vec3 Water3DCanvas::computeNormal(float x, float z, double time) {
    float eps = 0.05f;
    float hL = calculateWaterHeight(x - eps, z, time);
    float hR = calculateWaterHeight(x + eps, z, time);
    float hD = calculateWaterHeight(x, z - eps, time);
    float hU = calculateWaterHeight(x, z + eps, time);
    
    Water3DCanvas::Vec3 normal;
    normal.x = (hL - hR) / (2.0f * eps);
    normal.z = (hD - hU) / (2.0f * eps);
    normal.y = 1.0f;
    
    return normal.normalize();
}

Water3DCanvas::Vec3 Water3DCanvas::computeBlinnPhong(float x, float y, float z, const Water3DCanvas::Vec3& normal) {
    const float domainWidth = scenario_ ? static_cast<float>(scenario_->getWidth()) : VISIBLE_SIZE;
    const float domainHeight = scenario_ ? static_cast<float>(scenario_->getHeight()) : VISIBLE_SIZE;
    const float centerX = domainWidth * 0.5f;
    const float centerZ = domainHeight * 0.5f;
    const float domainSize = std::max(domainWidth, domainHeight);
    Water3DCanvas::Vec3 lightPos(centerX + domainSize * 0.18f, domainSize * 0.68f, centerZ - domainSize * 0.16f);
    Water3DCanvas::Vec3 lightDir = (lightPos - Water3DCanvas::Vec3(x, y, z)).normalize();

    Water3DCanvas::Vec3 cameraPos(centerX, domainSize * 1.8f, centerZ);
    Water3DCanvas::Vec3 viewDir = (cameraPos - Water3DCanvas::Vec3(x, y, z)).normalize();
    
    Water3DCanvas::Vec3 halfDir = (lightDir + viewDir).normalize();
    
    float NdotL = std::max(0.0f, normal.dot(lightDir));
    float NdotH = std::max(0.0f, normal.dot(halfDir));
    
    Water3DCanvas::Vec3 waterDeep(0.0f, 0.15f, 0.35f);
    Water3DCanvas::Vec3 waterShallow(0.0f, 0.45f, 0.65f);
    float depthFactor = std::clamp((y + 0.3f) / 0.6f, 0.0f, 1.0f);
    Water3DCanvas::Vec3 baseColor = waterDeep * (1.0f - depthFactor) + waterShallow * depthFactor;
    
    float Ka = 0.42f;
    float Kd = 0.72f;
    float Ks = 0.45f;
    float shininess = 24.0f;

    Water3DCanvas::Vec3 ambient = baseColor * Ka;
    Water3DCanvas::Vec3 diffuse = baseColor * Kd * NdotL;
    float spec = Ks * std::pow(NdotH, shininess);
    Water3DCanvas::Vec3 specular = Water3DCanvas::Vec3(1.0f, 0.95f, 0.9f) * spec;

    const float dx = x - lightPos.x;
    const float dz = z - lightPos.z;
    const float radius = domainSize * 0.42f;
    const float dist = std::sqrt(dx * dx + dz * dz);
    const float sunPool = std::clamp(1.0f - dist / radius, 0.0f, 1.0f);
    const float sunGlow = sunPool * sunPool;
    Water3DCanvas::Vec3 projectedSun = Water3DCanvas::Vec3(0.16f, 0.18f, 0.12f) * sunGlow;
    
    Water3DCanvas::Vec3 result = ambient + diffuse + specular + projectedSun;
    
    result.x = std::clamp(result.x, 0.0f, 1.0f);
    result.y = std::clamp(result.y, 0.0f, 1.0f);
    result.z = std::clamp(result.z, 0.0f, 1.0f);
    
    return result;
}

void Water3DCanvas::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    int w, h;
    GetClientSize(&w, &h);
    float aspect = (float)w / (float)h;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const float domainWidth = scenario_ ? static_cast<float>(scenario_->getWidth()) : VISIBLE_SIZE;
    const float domainHeight = scenario_ ? static_cast<float>(scenario_->getHeight()) : VISIBLE_SIZE;
    const float domainSize = std::max(domainWidth, domainHeight);
    float halfSpan = domainSize * 0.55f;
    if (aspect >= 1.0f) {
        glOrtho(-halfSpan * aspect, halfSpan * aspect, -halfSpan, halfSpan, -200.0, 200.0);
    } else {
        glOrtho(-halfSpan, halfSpan, -halfSpan / aspect, halfSpan / aspect, -200.0, 200.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const float centerX = domainWidth * 0.5f;
    const float centerZ = domainHeight * 0.5f;
    const float camX = centerX;
    const float camY = domainSize * 1.8f;
    const float camZ = centerZ;

    gluLookAt(camX, camY, camZ, centerX, 0.0, centerZ, 0.0, 0.0, -1.0);
    
    int res = GRID_RESOLUTION;
    float stepX = domainWidth / res;
    float stepZ = domainHeight / res;
    
    glDisable(GL_LIGHTING);
    glBegin(GL_TRIANGLES);
    
    for (int z = 0; z < res; z++) {
        for (int x = 0; x < res; x++) {
            float x1 = x * stepX;
            float x2 = (x + 1) * stepX;
            float z1 = z * stepZ;
            float z2 = (z + 1) * stepZ;
            
            float y11 = calculateWaterHeight(x1, z1, oceanAnimationTime_);
            float y12 = calculateWaterHeight(x1, z2, oceanAnimationTime_);
            float y21 = calculateWaterHeight(x2, z1, oceanAnimationTime_);
            
            Vec3 normal11 = computeNormal(x1, z1, oceanAnimationTime_);
            Vec3 color11 = computeBlinnPhong(x1, y11, z1, normal11);
            glColor3f(color11.x, color11.y, color11.z);
            glNormal3f(normal11.x, normal11.y, normal11.z);
            glVertex3f(x1, y11, z1);
            
            Vec3 normal12 = computeNormal(x1, z2, oceanAnimationTime_);
            Vec3 color12 = computeBlinnPhong(x1, y12, z2, normal12);
            glColor3f(color12.x, color12.y, color12.z);
            glNormal3f(normal12.x, normal12.y, normal12.z);
            glVertex3f(x1, y12, z2);
            
            Vec3 normal21 = computeNormal(x2, z1, oceanAnimationTime_);
            Vec3 color21 = computeBlinnPhong(x2, y21, z1, normal21);
            glColor3f(color21.x, color21.y, color21.z);
            glNormal3f(normal21.x, normal21.y, normal21.z);
            glVertex3f(x2, y21, z1);
            
            float y22 = calculateWaterHeight(x2, z2, oceanAnimationTime_);
            
            Vec3 normal12b = computeNormal(x1, z2, oceanAnimationTime_);
            Vec3 color12b = computeBlinnPhong(x1, y12, z2, normal12b);
            glColor3f(color12b.x, color12b.y, color12b.z);
            glNormal3f(normal12b.x, normal12b.y, normal12b.z);
            glVertex3f(x1, y12, z2);
            
            Vec3 normal22 = computeNormal(x2, z2, oceanAnimationTime_);
            Vec3 color22 = computeBlinnPhong(x2, y22, z2, normal22);
            glColor3f(color22.x, color22.y, color22.z);
            glNormal3f(normal22.x, normal22.y, normal22.z);
            glVertex3f(x2, y22, z2);
            
            Vec3 normal21b = computeNormal(x2, z1, oceanAnimationTime_);
            Vec3 color21b = computeBlinnPhong(x2, y21, z1, normal21b);
            glColor3f(color21b.x, color21b.y, color21b.z);
            glNormal3f(normal21b.x, normal21b.y, normal21b.z);
            glVertex3f(x2, y21, z1);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);

    if (scenario_ && showObstacles_) {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);

        const int scenarioWidth = scenario_->getWidth();
        const int scenarioHeight = scenario_->getHeight();

        for (int cellY = 0; cellY < scenarioHeight; ++cellY) {
            for (int cellX = 0; cellX < scenarioWidth; ++cellX) {
                const auto cell = scenario_->getCell(cellX, cellY);
                if (cell == tp::shared::CellType::Water) {
                    continue;
                }

                const float x1 = static_cast<float>(cellX);
                const float x2 = static_cast<float>(cellX + 1);
                const float z1 = static_cast<float>(cellY);
                const float z2 = static_cast<float>(cellY + 1);
                const float y1 = calculateWaterHeight(x1, z1, oceanAnimationTime_);
                const float y2 = calculateWaterHeight(x2, z1, oceanAnimationTime_);
                const float y3 = calculateWaterHeight(x2, z2, oceanAnimationTime_);
                const float y4 = calculateWaterHeight(x1, z2, oceanAnimationTime_);

                if (cell == tp::shared::CellType::Land) {
                    const float top = std::max(std::max(y1, y2), std::max(y3, y4)) + 0.18f;
                    glBegin(GL_TRIANGLES);
                    glColor3f(0.88f, 0.78f, 0.54f);
                    glVertex3f(x1, top, z1);
                    glVertex3f(x2, top, z1);
                    glVertex3f(x2, top, z2);
                    glColor3f(0.80f, 0.69f, 0.45f);
                    glVertex3f(x1, top, z1);
                    glVertex3f(x2, top, z2);
                    glVertex3f(x1, top, z2);
                    glEnd();

                    glLineWidth(1.2f);
                    glColor3f(0.95f, 0.87f, 0.67f);
                    glBegin(GL_LINE_LOOP);
                    glVertex3f(x1, top + 0.002f, z1);
                    glVertex3f(x2, top + 0.002f, z1);
                    glVertex3f(x2, top + 0.002f, z2);
                    glVertex3f(x1, top + 0.002f, z2);
                    glEnd();
                } else {
                    const float top = std::max(std::max(y1, y2), std::max(y3, y4)) + 0.34f;
                    glBegin(GL_TRIANGLES);
                    glColor3f(0.40f, 0.43f, 0.48f);
                    glVertex3f(x1, top, z1);
                    glVertex3f(x2, top, z1);
                    glVertex3f(x2, top, z2);
                    glColor3f(0.30f, 0.33f, 0.38f);
                    glVertex3f(x1, top, z1);
                    glVertex3f(x2, top, z2);
                    glVertex3f(x1, top, z2);
                    glEnd();

                    glColor3f(0.62f, 0.66f, 0.71f);
                    glBegin(GL_LINES);
                    glVertex3f(x1 + 0.2f, top + 0.003f, z1 + 0.2f);
                    glVertex3f(x2 - 0.2f, top + 0.003f, z2 - 0.2f);
                    glVertex3f(x2 - 0.2f, top + 0.003f, z1 + 0.2f);
                    glVertex3f(x1 + 0.2f, top + 0.003f, z2 - 0.2f);
                    glEnd();
                }
            }
        }

        glEnable(GL_LIGHTING);
    }

    if (showGrid_) {
        glDisable(GL_LIGHTING);
        glColor4f(1.0f, 1.0f, 1.0f, 0.12f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (float gx = 0.0f; gx <= domainWidth; gx += 5.0f) {
            glVertex3f(gx, 0.06f, 0.0f);
            glVertex3f(gx, 0.06f, domainHeight);
        }
        for (float gz = 0.0f; gz <= domainHeight; gz += 5.0f) {
            glVertex3f(0.0f, 0.06f, gz);
            glVertex3f(domainWidth, 0.06f, gz);
        }
        glEnd();
        glEnable(GL_LIGHTING);
    }
    
    float maxVisibleX = domainWidth;
    float maxVisibleZ = domainHeight;
    
    if (showField_ && vectorField_) {
        const int arrowStep = 3;
        const float arrowBody = 1.6f;
        const float arrowHead = 0.72f;

        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glLineWidth(1.6f);

        for (int cellY = 1; cellY < static_cast<int>(domainHeight); cellY += arrowStep) {
            for (int cellX = 1; cellX < static_cast<int>(domainWidth); cellX += arrowStep) {
                if (scenario_ && scenario_->getCell(cellX, cellY) != tp::shared::CellType::Water) {
                    continue;
                }

                const float gx = static_cast<float>(cellX) + 0.5f;
                const float gz = static_cast<float>(cellY) + 0.5f;
                Vec2d field = vectorField_->getValue(gx, gz);
                const float magnitude = static_cast<float>(field.magnitude());
                if (magnitude < 0.05f) {
                    continue;
                }

                const float invMag = 1.0f / magnitude;
                const float dirX = static_cast<float>(field.x) * invMag;
                const float dirZ = static_cast<float>(field.y) * invMag;
                const float animX = gx;
                const float animZ = gz;

                const float py = calculateWaterHeight(animX, animZ, oceanAnimationTime_) + 0.28f;
                const float normalized = std::clamp(magnitude / 10.0f, 0.0f, 1.0f);
                const float scale = std::clamp(0.65f + normalized * 0.55f, 0.65f, 1.2f);
                const float body = arrowBody * scale;
                const float head = arrowHead * scale;
                const float tailX = animX - dirX * body * 0.5f;
                const float tailZ = animZ - dirZ * body * 0.5f;
                const float tipX = animX + dirX * body * 0.5f;
                const float tipZ = animZ + dirZ * body * 0.5f;
                const float sideX = -dirZ;
                const float sideZ = dirX;

                const float bodyRed = 0.24f + normalized * 0.66f;
                const float bodyGreen = 0.70f + normalized * 0.26f;
                const float bodyBlue = 1.0f;
                const float headRed = 0.40f + normalized * 0.58f;
                const float headGreen = 0.80f + normalized * 0.18f;
                const float headBlue = 1.0f;

                glColor3f(bodyRed, bodyGreen, bodyBlue);
                glBegin(GL_LINES);
                glVertex3f(tailX, py, tailZ);
                glVertex3f(tipX, py, tipZ);
                glEnd();

                glColor3f(headRed, headGreen, headBlue);
                glBegin(GL_TRIANGLES);
                glVertex3f(tipX, py, tipZ);
                glVertex3f(tipX - dirX * head + sideX * head * 0.58f, py, tipZ - dirZ * head + sideZ * head * 0.58f);
                glVertex3f(tipX - dirX * head - sideX * head * 0.58f, py, tipZ - dirZ * head - sideZ * head * 0.58f);
                glEnd();
            }
        }

        glLineWidth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }
    
    if (boat_ && showBoat_) {
        float bx = static_cast<float>(boatX_);
        float bz = static_cast<float>(boatY_);
        
        if (bx >= 0 && bx <= maxVisibleX && bz >= 0 && bz <= maxVisibleZ) {
            float boatHeight = calculateWaterHeight(bx, bz, oceanAnimationTime_);
            float boatSize = static_cast<float>(std::max(0.8, boat_->getSize() * 0.45));
            float angle = static_cast<float>(boat_->getOrientation());
            float dirX = std::cos(angle);
            float dirZ = std::sin(angle);
            float sideX = -dirZ;
            float sideZ = dirX;
            float prowX = bx + dirX * boatSize;
            float prowZ = bz + dirZ * boatSize;
            float sternLeftX = bx - dirX * boatSize * 0.6f + sideX * boatSize * 0.55f;
            float sternLeftZ = bz - dirZ * boatSize * 0.6f + sideZ * boatSize * 0.55f;
            float sternRightX = bx - dirX * boatSize * 0.6f - sideX * boatSize * 0.55f;
            float sternRightZ = bz - dirZ * boatSize * 0.6f - sideZ * boatSize * 0.55f;
            float deckX = bx + dirX * boatSize * 0.18f;
            float deckZ = bz + dirZ * boatSize * 0.18f;
            
            glDisable(GL_LIGHTING);
            glDisable(GL_DEPTH_TEST);
            glBegin(GL_TRIANGLES);
            glColor3f(1.0f, 0.45f, 0.12f);
            glVertex3f(prowX, boatHeight + 0.12f, prowZ);
            glVertex3f(sternLeftX, boatHeight + 0.12f, sternLeftZ);
            glVertex3f(sternRightX, boatHeight + 0.12f, sternRightZ);

            glColor3f(1.0f, 0.82f, 0.2f);
            glVertex3f(deckX + sideX * boatSize * 0.18f, boatHeight + 0.13f, deckZ + sideZ * boatSize * 0.18f);
            glVertex3f(deckX - sideX * boatSize * 0.18f, boatHeight + 0.13f, deckZ - sideZ * boatSize * 0.18f);
            glVertex3f(deckX + dirX * boatSize * 0.25f, boatHeight + 0.13f, deckZ + dirZ * boatSize * 0.25f);
            glEnd();
            glBegin(GL_LINE_LOOP);
            glColor3f(1.0f, 0.95f, 0.75f);
            glVertex3f(prowX, boatHeight + 0.14f, prowZ);
            glVertex3f(sternLeftX, boatHeight + 0.14f, sternLeftZ);
            glVertex3f(sternRightX, boatHeight + 0.14f, sternRightZ);
            glEnd();
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
        }
    }
    
    if (showTrajectory_ && !trajectory_.empty()) {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glLineWidth(3.6f);
        glBegin(GL_LINE_STRIP);
        glColor3f(0.92f, 1.0f, 0.35f);
        
        for (const auto& point : trajectory_) {
            float px = static_cast<float>(point.x);
            float pz = static_cast<float>(point.y);
            
            if (px >= 0 && px <= maxVisibleX && pz >= 0 && pz <= maxVisibleZ) {
                float py = calculateWaterHeight(px, pz, oceanAnimationTime_);
                glVertex3f(px, py + 0.03f, pz);
            }
        }
        glEnd();
        glLineWidth(1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }
}

} // namespace tp::presentation
