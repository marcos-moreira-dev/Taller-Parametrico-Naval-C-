#include "IRenderingContext.hpp"
#include "Rendering2D.hpp"
#include "Rendering3D.hpp"
#include <wx/wx.h>

namespace tp::presentation {

std::unique_ptr<IRenderingContext> RenderingFactory::create2D(wxWindow* parent) {
    return std::make_unique<Rendering2DContext>(parent);
}

std::unique_ptr<IRenderingContext> RenderingFactory::create3D(wxWindow* parent) {
    return std::make_unique<Rendering3DContext>(parent);
}

} // namespace tp::presentation
