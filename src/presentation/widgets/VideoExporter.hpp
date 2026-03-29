#pragma once

/**
 * @file VideoExporter.hpp
 * @brief Exportador de video de simulaciones
 * @author Proyecto Taller Paramétrico
 * @version 0.2.4
 * 
 * Captura frames de la simulación y los exporta como video:
 * - Formatos soportados: AVI (usando Windows AVI API)
 * - Captura de canvas de simulación
 * - Configuración de fps y calidad
 * 
 * @note Requiere Windows API para AVI o FFmpeg externo
 */

#include <wx/wx.h>
#include <wx/bitmap.h>
#include <vector>
#include <string>

namespace tp::presentation {

/**
 * @brief Exportador de video de simulaciones
 * 
 * Captura frames del canvas y los codifica en formato AVI.
 */
class VideoExporter {
public:
    VideoExporter();
    ~VideoExporter();
    
    /**
     * @brief Inicia una nueva grabación
     * @param outputPath Ruta del archivo de salida
     * @param width Ancho del video
     * @param height Alto del video
     * @param fps Frames por segundo
     * @return true si se inició correctamente
     */
    bool startRecording(const wxString& outputPath, int width, int height, int fps = 30);
    
    /**
     * @brief Agrega un frame al video
     * @param frame Bitmap del frame a agregar
     * @return true si se agregó correctamente
     */
    bool addFrame(const wxBitmap& frame);
    
    /**
     * @brief Finaliza la grabación y guarda el archivo
     * @return true si se guardó correctamente
     */
    bool finishRecording();
    
    /**
     * @brief Cancela la grabación actual
     */
    void cancelRecording();
    
    /**
     * @brief Verifica si está grabando
     * @return true si está grabando
     */
    bool isRecording() const { return isRecording_; }
    
    /**
     * @brief Obtiene el número de frames grabados
     * @return Cantidad de frames
     */
    int getFrameCount() const { return (int)frames_.size(); }
    
    /**
     * @brief Obtiene el progreso de la grabación
     * @return Porcentaje de frames grabados
     */
    float getProgress() const;
    
private:
    bool isRecording_;
    wxString outputPath_;
    int width_;
    int height_;
    int fps_;
    std::vector<wxBitmap> frames_;
    bool cancelled_;
};

} // namespace tp::presentation