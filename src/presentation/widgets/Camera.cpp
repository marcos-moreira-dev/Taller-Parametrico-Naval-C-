#include "Camera.hpp"
#include <cmath>
#include <algorithm>

namespace tp::presentation {

Camera::Camera(float aspectRatio)
    : posX_(0.0f), posY_(30.0f), posZ_(30.0f),
      targetX_(0.0f), targetY_(0.0f), targetZ_(0.0f),
      upX_(0.0f), upY_(1.0f), upZ_(0.0f),
      fov_(45.0f), aspect_(aspectRatio), nearClip_(0.1f), farClip_(100.0f),
      matricesDirty_(true) {
    computeViewMatrix();
    computeProjectionMatrix();
}

void Camera::setPosition(float x, float y, float z) {
    posX_ = x; posY_ = y; posZ_ = z;
    matricesDirty_ = true;
}

void Camera::setTarget(float x, float y, float z) {
    targetX_ = x; targetY_ = y; targetZ_ = z;
    matricesDirty_ = true;
}

void Camera::setUp(float x, float y, float z) {
    upX_ = x; upY_ = y; upZ_ = z;
    matricesDirty_ = true;
}

void Camera::setFOV(float fovDegrees) {
    fov_ = fovDegrees;
    matricesDirty_ = true;
}

void Camera::setAspectRatio(float aspect) {
    aspect_ = aspect;
    matricesDirty_ = true;
}

void Camera::setClipping(float nearPlane, float farPlane) {
    nearClip_ = nearPlane;
    farClip_ = farPlane;
    matricesDirty_ = true;
}

void Camera::setTiltedTopDownView() {
    posX_ = 0.0f;
    posY_ = 35.0f;
    posZ_ = 35.0f;
    targetX_ = 0.0f;
    targetY_ = 0.0f;
    targetZ_ = 0.0f;
    upX_ = 0.0f;
    upY_ = 1.0f;
    upZ_ = 0.0f;
    fov_ = 45.0f;
    matricesDirty_ = true;
    computeViewMatrix();
    computeProjectionMatrix();
}

void Camera::setTopDownView() {
    posX_ = 0.0f;
    posY_ = 50.0f;
    posZ_ = 0.0f;
    targetX_ = 0.0f;
    targetY_ = 0.0f;
    targetZ_ = 0.0f;
    upX_ = 0.0f;
    upY_ = 0.0f;
    upZ_ = -1.0f;
    fov_ = 30.0f;
    matricesDirty_ = true;
    computeViewMatrix();
    computeProjectionMatrix();
}

void Camera::setAngledView(float angleDegrees) {
    float angleRad = angleDegrees * 3.14159265f / 180.0f;
    float dist = 40.0f;
    posX_ = dist * std::sin(angleRad);
    posY_ = dist * 0.7f;
    posZ_ = dist * std::cos(angleRad);
    targetX_ = 0.0f;
    targetY_ = 0.0f;
    targetZ_ = 0.0f;
    upX_ = 0.0f;
    upY_ = 1.0f;
    upZ_ = 0.0f;
    fov_ = 45.0f;
    matricesDirty_ = true;
    computeViewMatrix();
    computeProjectionMatrix();
}

void Camera::computeViewMatrix() {
    float ex = posX_, ey = posY_, ez = posZ_;
    float tx = targetX_, ty = targetY_, tz = targetZ_;
    float ux = upX_, uy = upY_, uz = upZ_;
    
    float zx = ex - tx;
    float zy = ey - ty;
    float zz = ez - tz;
    float zLen = std::sqrt(zx*zx + zy*zy + zz*zz);
    if (zLen > 0.0f) { zx /= zLen; zy /= zLen; zz /= zLen; }
    
    float xx = uy * zz - uz * zy;
    float xy = uz * zx - ux * zz;
    float xz = ux * zy - uy * zx;
    float xLen = std::sqrt(xx*xx + xy*xy + xz*xz);
    if (xLen > 0.0f) { xx /= xLen; xy /= xLen; xz /= xLen; }
    
    float yx = zy * xz - zz * xy;
    float yy = zz * xx - zx * xz;
    float yz = zx * xy - zy * xx;
    
    viewMatrix_ = {
        xx, yx, zx, 0.0f,
        xy, yy, zy, 0.0f,
        xz, yz, zz, 0.0f,
        -(xx*ex + xy*ey + xz*ez),
        -(yx*ex + yy*ey + yz*ez),
        -(zx*ex + zy*ey + zz*ez),
        1.0f
    };
    
    matricesDirty_ = false;
}

void Camera::computeProjectionMatrix() {
    float f = 1.0f / std::tan(fov_ * 3.14159265f / 360.0f);
    float nf = 1.0f / (nearClip_ - farClip_);
    
    projMatrix_ = {
        f / aspect_, 0.0f, 0.0f, 0.0f,
        0.0f, f, 0.0f, 0.0f,
        0.0f, 0.0f, (farClip_ + nearClip_) * nf, -1.0f,
        0.0f, 0.0f, 2.0f * farClip_ * nearClip_ * nf, 0.0f
    };
}

std::array<float, 16> Camera::getViewMatrix() const {
    if (matricesDirty_) {
        const_cast<Camera*>(this)->computeViewMatrix();
    }
    return viewMatrix_;
}

std::array<float, 16> Camera::getProjectionMatrix() const {
    if (matricesDirty_) {
        const_cast<Camera*>(this)->computeProjectionMatrix();
    }
    return projMatrix_;
}

} // namespace tp::presentation
