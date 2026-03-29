// VideoExporter.cpp - Implementación del exportador de video

#include "VideoExporter.hpp"
#include <wx/mstream.h>
#include <wx/image.h>

namespace tp::presentation {

VideoExporter::VideoExporter()
    : isRecording_(false)
    , width_(0)
    , height_(0)
    , fps_(30)
    , cancelled_(false)
{
}

VideoExporter::~VideoExporter() {
    if (isRecording_) {
        cancelRecording();
    }
}

bool VideoExporter::startRecording(const wxString& outputPath, int width, int height, int fps) {
    if (isRecording_) {
        return false;
    }
    
    outputPath_ = outputPath;
    width_ = width;
    height_ = height;
    fps_ = fps;
    
    frames_.clear();
    cancelled_ = false;
    isRecording_ = true;
    
    return true;
}

bool VideoExporter::addFrame(const wxBitmap& frame) {
    if (!isRecording_ || cancelled_) {
        return false;
    }
    
    // Ensure frame has correct size
    wxBitmap scaledFrame;
    if (frame.GetWidth() != width_ || frame.GetHeight() != height_) {
        wxImage img = frame.ConvertToImage();
        img.Rescale(width_, height_);
        scaledFrame = wxBitmap(img);
    } else {
        scaledFrame = frame;
    }
    
    frames_.push_back(scaledFrame);
    return true;
}

bool VideoExporter::finishRecording() {
    if (!isRecording_) {
        return false;
    }
    
    isRecording_ = false;
    
    if (frames_.empty() || cancelled_) {
        frames_.clear();
        return false;
    }
    
    // Simple BMP sequence export (puede extenderse a AVI con Windows API)
    // Por ahora guardamos como secuencia de imágenes
    wxString basePath = outputPath_;
    if (!basePath.EndsWith(".avi") && !basePath.EndsWith(".mp4")) {
        basePath = basePath + ".avi";
    }
    
    // Guardar primer frame como ejemplo (AVI completo requiere Windows API)
    // Por ahora guardamos un PNG del primer frame
    wxString pngPath = basePath.BeforeLast('.') + ".png";
    if (!frames_.empty()) {
        wxImage img = frames_[0].ConvertToImage();
        img.SaveFile(pngPath, wxBITMAP_TYPE_PNG);
    }
    
    frames_.clear();
    return true;
}

void VideoExporter::cancelRecording() {
    cancelled_ = true;
    isRecording_ = false;
    frames_.clear();
}

float VideoExporter::getProgress() const {
    // Progress depends on expected duration - placeholder
    return frames_.empty() ? 0.0f : 1.0f;
}

} // namespace tp::presentation