#include "main_window/MainWindow.hpp"

#include "main_window/MainWindowIds.hpp"
#include "main_window/MainWindowStyles.hpp"
#include "panels/BottomPanel.hpp"
#include "panels/LeftPanel.hpp"
#include "panels/RightPanel.hpp"
#include "panels/TheoryPanel.hpp"
#include "panels/EditorPanel.hpp"
#include "panels/FieldEditorPanel.hpp"
#include "widgets/SimulationCanvas.hpp"
#include "widgets/Water3DCanvas.hpp"
#include "widgets/SimpleGraph.hpp"

#include <wx/aboutdlg.h>
#include <wx/artprov.h>
#include <wx/filename.h>
#include <wx/sizer.h>
#include <wx/spinctrl.h>
#include <wx/stattext.h>

#include <fstream>
#include <iomanip>

#include "Logger.hpp"
#include "TraceLogger.hpp"
#include "ExpressionParser.hpp"
#include "Scenario.hpp"
#include "VectorField.hpp"
#include "MathExpression.hpp"
#include "Boat.hpp"
#include "Motor.hpp"
#include "integrators/Integrators.hpp"
#include "models/PhysicsModel.hpp"
#include "runner/SimulationRunner.hpp"

namespace tp::presentation {

void MainWindow::updateEditorBrushSize(int size) {
    if (canvas_) {
        canvas_->setBrushSize(size);
    }
}

void MainWindow::updateEditorSnap(bool snap) {
    if (canvas_) {
        canvas_->setSnapToGrid(snap);
    }
}

void MainWindow::setEditorMode(bool editor) {
    if (canvas_) {
        canvas_->setEditorMode(editor);
    }
}

void MainWindow::beginEditorStroke() {
    if (editorPanel_) {
        editorPanel_->beginStroke();
    }
}

void MainWindow::applyEditorToolAtCell(int x, int y) {
    if (editorPanel_) {
        editorPanel_->applyToolAtCell(x, y);
    }
}

void MainWindow::endEditorStroke() {
    if (editorPanel_) {
        editorPanel_->endStroke();
    }
}

bool MainWindow::confirmSaveChanges(const wxString& action)
{
    if (!isModified_) return true;
    
    int result = wxMessageBox(wxT("¿Desea guardar los cambios antes de ") + action + wxT("?"),
                              wxT("Confirmar"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
    if (result == wxCANCEL) return false;
    if (result == wxYES) {
        return saveScenarioWithDialog();
    }
    return true;
}

void MainWindow::stripExtension(wxString& filename, const wxString& ext)
{
    if (filename.EndsWith(ext)) {
        filename = filename.Left(filename.Length() - ext.Length());
    }
}

void MainWindow::onNew(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::FILE_OPERATION, "MainWindow", "onNew");
    
    if (!confirmSaveChanges(wxT("crear un nuevo escenario"))) return;
    
    experimentService_.createNewExperiment();
    currentScenarioName_ = wxString::FromUTF8(experimentService_.getConfig().scenario.getName().c_str());
    currentScenarioPath_.clear();
    simulationStartPosition_ = experimentService_.getConfig().boat.getPosition();
    simulationStartVelocity_ = experimentService_.getConfig().boat.getVelocity();
    simulationStartOrientation_ = experimentService_.getConfig().boat.getOrientation();
    simulationStatusText_ = wxT("Listo para simular");
    isModified_ = false;
    updateWindowTitle();
    resetSimulation();
    
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Nuevo escenario creado"));
    }
}

void MainWindow::onOpen(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::FILE_OPERATION, "MainWindow", "onOpen");
    
    if (!confirmSaveChanges(wxT("abrir otro escenario"))) return;
    
    wxFileDialog openDialog(this, wxT("Abrir escenario"), wxT(""), wxT(""),
                           wxT("Escenarios (*.scenario)|*.scenario|Todos los archivos (*.*)|*.*"),
                           wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openDialog.ShowModal() == wxID_OK) {
        wxString path = openDialog.GetPath();
        if (!experimentService_.loadExperiment(path.ToStdString())) {
            wxMessageBox(
                wxT("No se pudo cargar el archivo seleccionado.\n\n")
                wxT("Verifica que sea un archivo .scenario válido y compatible con esta versión."),
                wxT("Abrir escenario"),
                wxOK | wxICON_ERROR,
                this);
            return;
        }

        currentScenarioPath_ = path;
        const std::string loadedName = experimentService_.getConfig().scenario.getName();
        if (!loadedName.empty()) {
            currentScenarioName_ = wxString::FromUTF8(loadedName.c_str());
        } else {
            currentScenarioName_ = openDialog.GetFilename();
            stripExtension(currentScenarioName_, wxT(".scenario"));
        }
        
        simulationStartPosition_ = experimentService_.getConfig().boat.getPosition();
        simulationStartVelocity_ = experimentService_.getConfig().boat.getVelocity();
        simulationStartOrientation_ = experimentService_.getConfig().boat.getOrientation();
        simulationStatusText_ = wxT("Listo para simular");
        isModified_ = false;
        updateWindowTitle();
        resetSimulation();
        updateCanvas();
        if (leftPanel_) {
            leftPanel_->setLoadedFileScenarioName(currentScenarioName_);
            leftPanel_->setCurrentScenarioLabel(wxT("Archivo actual: ") + currentScenarioName_);
        }
        if (editorPanel_) {
            editorPanel_->syncFromDocument();
        }
        if (fieldEditorPanel_) {
            fieldEditorPanel_->syncFromDocument();
        }
        if (rightPanel_) {
            rightPanel_->updateProperties();
        }
        
        if (bottomPanel_) {
            bottomPanel_->addEvent(wxT("Escenario abierto: ") + path);
        }
    }
}

bool MainWindow::saveScenarioWithDialog()
{
    wxString defaultDir;
    if (!currentScenarioPath_.empty()) {
        wxFileName currentFile(currentScenarioPath_);
        defaultDir = currentFile.GetPath();
    }
    wxString defaultName = currentScenarioName_.empty()
        ? wxString::FromUTF8(experimentService_.getConfig().scenario.getName().c_str())
        : currentScenarioName_;
    wxFileDialog saveDialog(this, wxT("Guardar escenario"), defaultDir, defaultName,
                           wxT("Escenarios (*.scenario)|*.scenario"),
                           wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (saveDialog.ShowModal() != wxID_OK) {
        return false;
    }

    wxString path = saveDialog.GetPath();
    experimentService_.saveExperiment(path.ToStdString());

    currentScenarioPath_ = path;
    const std::string scenarioName = experimentService_.getConfig().scenario.getName();
    if (!scenarioName.empty()) {
        currentScenarioName_ = wxString::FromUTF8(scenarioName.c_str());
    } else {
        currentScenarioName_ = saveDialog.GetFilename();
        stripExtension(currentScenarioName_, wxT(".scenario"));
    }

    isModified_ = false;
    updateWindowTitle();
    simulationStatusText_ = wxT("Listo para simular");
    refreshStatusBar();

    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Escenario guardado: ") + path);
    }
    return true;
}

void MainWindow::onSave(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::FILE_OPERATION, "MainWindow", "onSave");
    saveScenarioWithDialog();
}

void MainWindow::onExportImage(wxCommandEvent& event)
{
    (void)event;
    wxFileDialog exportDialog(this, wxT("Exportar imagen"), wxT(""), wxT("captura.png"),
                             wxT("PNG (*.png)|*.png|JPEG (*.jpg)|*.jpg|Bitmap (*.bmp)|*.bmp"),
                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (exportDialog.ShowModal() == wxID_OK && canvas_) {
        wxString path = exportDialog.GetPath();
        wxBitmapType type = wxBITMAP_TYPE_PNG;
        
        if (path.EndsWith(wxT(".jpg")) || path.EndsWith(wxT(".jpeg"))) {
            type = wxBITMAP_TYPE_JPEG;
        } else if (path.EndsWith(wxT(".bmp"))) {
            type = wxBITMAP_TYPE_BMP;
        }
        
        // Capturar el canvas
        wxClientDC dc(canvas_);
        wxSize size = canvas_->GetClientSize();
        wxBitmap bitmap(size);
        wxMemoryDC memDC;
        memDC.SelectObject(bitmap);
        memDC.Blit(0, 0, size.GetWidth(), size.GetHeight(), &dc, 0, 0);
        memDC.SelectObject(wxNullBitmap);
        
        if (bitmap.SaveFile(path, type)) {
            wxMessageBox(wxT("Imagen exportada exitosamente a:\n") + path, 
                        wxT("Exportar Imagen"), wxOK | wxICON_INFORMATION);
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("Imagen exportada: ") + path);
            }
        } else {
            wxMessageBox(wxT("Error al guardar la imagen"), 
                        wxT("Error"), wxOK | wxICON_ERROR);
        }
    }
}

void MainWindow::onExportData(wxCommandEvent& event)
{
    (void)event;
    wxFileDialog exportDialog(this, wxT("Exportar datos"), wxT(""), wxT("datos.csv"),
                             wxT("CSV (*.csv)|*.csv|Texto (*.txt)|*.txt"),
                             wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (exportDialog.ShowModal() == wxID_OK) {
        wxString path = exportDialog.GetPath();
        
        std::ofstream file(path.ToStdString());
        if (file.is_open()) {
            // Encabezados CSV
            file << "Tiempo(s),PosX,PosY,VelX,VelY,Velocidad(m/s),Energia(J)\n";
            
            // Datos de trayectoria
            for (const auto& point : trajectoryPoints_) {
                file << std::fixed << std::setprecision(3)
                     << point.time << ","
                     << point.x << ","
                     << point.y << ","
                     << "0.000,0.000,"
                     << currentSpeed_ << ","
                     << currentEnergy_ << "\n";
            }
            
            file.close();
            
            wxMessageBox(wxT("Datos exportados exitosamente a:\n") + path + 
                        wxT("\n\nTotal de puntos: ") + wxString::Format(wxT("%d"), (int)trajectoryPoints_.size()), 
                        wxT("Exportar Datos"), wxOK | wxICON_INFORMATION);
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("Datos exportados: ") + path);
            }
        } else {
            wxMessageBox(wxT("No se pudo crear el archivo"), 
                        wxT("Error"), wxOK | wxICON_ERROR);
        }
    }
}

void MainWindow::onExit(wxCommandEvent& event)
{
    (void)event;
    Close(true);
}

void MainWindow::openCustomFieldDialog()
{
    TRACE_INFO(tp::shared::TraceEventType::USER_ACTION, "MainWindow", "openCustomFieldDialog");
    wxCommandEvent event;
    onCustomField(event);
}

void MainWindow::requestSimulationReset()
{
    resetSimulation();
}

bool MainWindow::requestSaveScenario()
{
    return saveScenarioWithDialog();
}

void MainWindow::requestLoadScenario()
{
    wxCommandEvent event;
    onOpen(event);
}

bool MainWindow::requestReloadCurrentScenarioFile()
{
    TRACE_INFO(tp::shared::TraceEventType::FILE_OPERATION, "MainWindow", "requestReloadCurrentScenarioFile");
    if (currentScenarioPath_.empty()) {
        return false;
    }

    if (!experimentService_.loadExperiment(currentScenarioPath_.ToStdString())) {
        return false;
    }

    const std::string loadedName = experimentService_.getConfig().scenario.getName();
    if (!loadedName.empty()) {
        currentScenarioName_ = wxString::FromUTF8(loadedName.c_str());
    }

    simulationStartPosition_ = experimentService_.getConfig().boat.getPosition();
    simulationStartVelocity_ = experimentService_.getConfig().boat.getVelocity();
    simulationStartOrientation_ = experimentService_.getConfig().boat.getOrientation();
    simulationStatusText_ = wxT("Archivo recargado");
    isModified_ = false;
    updateWindowTitle();
    resetSimulation();
    updateCanvas();
    if (leftPanel_) {
        leftPanel_->setLoadedFileScenarioName(currentScenarioName_);
        leftPanel_->setCurrentScenarioLabel(wxT("Archivo actual: ") + currentScenarioName_);
        leftPanel_->applyLayerVisibility();
    }
    if (editorPanel_) {
        editorPanel_->syncFromDocument();
    }
    if (fieldEditorPanel_) {
        fieldEditorPanel_->syncFromDocument();
    }
    if (rightPanel_) {
        rightPanel_->updateProperties();
    }
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Archivo recargado: ") + currentScenarioName_);
    }
    return true;
}

void MainWindow::requestLoadScenarioPreset(size_t index)
{
    TRACE_INFO(tp::shared::TraceEventType::FILE_OPERATION, "MainWindow", "requestLoadScenarioPreset", std::to_string(index));
    auto& service = getExperimentService();
    if (index >= service.getScenarioPresetCount()) {
        return;
    }

    service.applyScenarioPreset(index);
    if (leftPanel_) {
        leftPanel_->setLoadedFileScenarioName(wxEmptyString);
        leftPanel_->setSelectedScenarioIndex(static_cast<int>(index));
        leftPanel_->setCurrentScenarioLabel(wxT("Archivo actual: preset interno"));
    }
    currentScenarioPath_.clear();
    currentScenarioName_ = wxString::FromUTF8(service.getScenarioPresetName(index).c_str());
    simulationStartPosition_ = service.getConfig().boat.getPosition();
    simulationStartVelocity_ = service.getConfig().boat.getVelocity();
    simulationStartOrientation_ = service.getConfig().boat.getOrientation();
    simulationStatusText_ = wxT("Preset cargado");
    isModified_ = false;
    updateWindowTitle();
    resetSimulation();
    updateCanvas();
    if (leftPanel_) {
        leftPanel_->applyLayerVisibility();
    }
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Preset cargado: ") + currentScenarioName_);
    }
}

void MainWindow::onTheoryScenarioRequest(const wxString& scenarioName)
{
    TRACE_INFO(tp::shared::TraceEventType::USER_ACTION, "TheoryPanel", "onTheoryScenarioRequest", std::string(scenarioName.ToUTF8()));
    setMode(AppMode::Simulation);
    
    int index = -1;
    if (scenarioName.Contains(wxT("Río")) || scenarioName.Contains(wxT("1."))) {
        index = 0;
    } else if (scenarioName.Contains(wxT("Remolino")) || scenarioName.Contains(wxT("2."))) {
        index = 1;
    } else if (scenarioName.Contains(wxT("Canal")) || scenarioName.Contains(wxT("3."))) {
        index = 2;
    } else if (scenarioName.Contains(wxT("Turbulenta")) || scenarioName.Contains(wxT("4."))) {
        index = 3;
    } else if (scenarioName.Contains(wxT("Laberinto")) || scenarioName.Contains(wxT("5."))) {
        index = 4;
    } else if (scenarioName.Contains(wxT("Rápidos")) || scenarioName.Contains(wxT("6."))) {
        index = 5;
    }
    
    if (index >= 0) {
        requestLoadScenarioPreset(static_cast<size_t>(index));
        simulationStatusText_ = wxT("Escenario cargado desde teoría");
        refreshStatusBar();
    }
}

void MainWindow::onAbout(wxCommandEvent& event)
{
    (void)event;
    wxAboutDialogInfo info;
    info.SetName(wxT("Taller Paramétrico de Navegación y Campos 2D"));
    info.SetVersion(wxT("0.1.0"));
    info.SetDescription(wxT("Una aplicación educativa para explorar campos vectoriales, ")
                       wxT("trayectorias y métodos numéricos en escenarios 2D."));
    info.SetCopyright(wxT("© 2025 - Proyecto de C++"));
    wxAboutBox(info);
}

void MainWindow::onHelp(wxCommandEvent& event)
{
    (void)event;
    wxMessageBox(
        wxT("Guía Rápida:\n\n")
        wxT("F1 - Modo Teoría\n")
        wxT("F2 - Modo Simulación\n")
        wxT("F3 - Editor de escenario\n")
        wxT("F7 - Editor de campo\n")
        wxT("F4 - Mostrar/ocultar panel izquierdo\n")
        wxT("F5 - Mostrar/ocultar panel derecho\n")
        wxT("F6 - Mostrar/ocultar panel inferior\n")
        wxT("Espacio - Iniciar simulación\n")
        wxT("P - Pausar simulación\n")
        wxT("Ctrl+R - Reiniciar simulación"),
        wxT("Ayuda"),
        wxOK | wxICON_INFORMATION
    );
}

void MainWindow::onResetView(wxCommandEvent& event)
{
    (void)event;
    // Restaurar layout por defecto
    if (auiManager_.GetPane(leftPanel_).IsOk()) {
        auiManager_.GetPane(leftPanel_).Show(true);
    }
    if (auiManager_.GetPane(rightPanel_).IsOk()) {
        auiManager_.GetPane(rightPanel_).Show(true);
    }
    if (auiManager_.GetPane(bottomPanel_).IsOk()) {
        auiManager_.GetPane(bottomPanel_).Show(true);
    }
    auiManager_.Update();
    syncViewMenuChecks();
    
    // Actualizar checks del menú Ver
    wxMenuBar* menuBar = GetMenuBar();
    if (menuBar) {
        menuBar->Check(ID_MENU_VIEW_LEFT, true);
        menuBar->Check(ID_MENU_VIEW_RIGHT, true);
        menuBar->Check(ID_MENU_VIEW_BOTTOM, true);
    }
    
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Vista reiniciada a valores por defecto"));
    }
}

void MainWindow::onAuiPaneClose(wxAuiManagerEvent& event)
{
    event.Skip();
    CallAfter([this]() {
        syncViewMenuChecks();
    });
}

void MainWindow::onClearTrajectory(wxCommandEvent& event)
{
    (void)event;
    trajectoryPoints_.clear();
    if (canvas_) {
        canvas_->clearTrajectory();
    }
    if (bottomPanel_) {
        bottomPanel_->addEvent(wxT("Trayectoria limpiada"));
    }
}

void MainWindow::onCaptureScreen(wxCommandEvent& event)
{
    (void)event;
    // Usar la misma función que Exportar Imagen
    wxCommandEvent dummyEvent;
    onExportImage(dummyEvent);
}

void MainWindow::OnClose(wxCloseEvent& event) {
    if (canvas3D_) {
        canvas3D_->stopAnimation();
    }
    event.Skip();
}

void MainWindow::onToggle3D(wxCommandEvent& event)
{
    (void)event;
    is3DMode_ = !is3DMode_;
    
    if (is3DMode_) {
        auiManager_.GetPane(canvas_).Hide();
        auiManager_.GetPane(canvas3D_).Show();

        updateCanvas();
        if (leftPanel_) {
            leftPanel_->applyLayerVisibility();
        }
        
        auiManager_.Update();
        Layout();
        canvas3D_->Show();
        canvas3D_->Refresh();
        canvas3D_->Update();
        canvas3D_->startAnimation();
    } else {
        auiManager_.GetPane(canvas3D_).Hide();
        auiManager_.GetPane(canvas_).Show();
        canvas3D_->stopAnimation();
    }
    
    auiManager_.Update();
    Refresh();
}

void MainWindow::onCustomField(wxCommandEvent& event)
{
    (void)event;
    
    wxDialog dialog(this, wxID_ANY, wxT("Campo Vectorial Personalizado"), 
                    wxDefaultPosition, wxSize(500, 400));
    
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    
    // Instrucciones
    mainSizer->Add(new wxStaticText(&dialog, wxID_ANY, 
        wxT("Ingrese las componentes del campo vectorial F(x,y) = (Fx, Fy)")), 
        0, wxALL, 10);
    
    auto& currentConfig = experimentService_.getConfig();
    const bool isCustom = currentConfig.fieldView.type == tp::application::FieldPresetType::Custom;
    const bool hasCustomExpr = isCustom && !(currentConfig.fieldView.customFx == "0" && currentConfig.fieldView.customFy == "0");
    const wxString initialFx = hasCustomExpr ? wxString::FromUTF8(currentConfig.fieldView.customFx.c_str()) : wxT("-0.08*(x-25)-0.25*(y-25)");
    const wxString initialFy = hasCustomExpr ? wxString::FromUTF8(currentConfig.fieldView.customFy.c_str()) : wxT("-0.08*(y-25)+0.25*(x-25)");

    // Fx
    wxBoxSizer* fxSizer = new wxBoxSizer(wxHORIZONTAL);
    fxSizer->Add(new wxStaticText(&dialog, wxID_ANY, wxT("Fx(x,y) = ")), 0, wxALIGN_CENTER_VERTICAL);
    wxTextCtrl* fxCtrl = new wxTextCtrl(&dialog, wxID_ANY, initialFx);
    fxSizer->Add(fxCtrl, 1, wxEXPAND);
    mainSizer->Add(fxSizer, 0, wxEXPAND | wxALL, 5);
    
    // Fy
    wxBoxSizer* fySizer = new wxBoxSizer(wxHORIZONTAL);
    fySizer->Add(new wxStaticText(&dialog, wxID_ANY, wxT("Fy(x,y) = ")), 0, wxALIGN_CENTER_VERTICAL);
    wxTextCtrl* fyCtrl = new wxTextCtrl(&dialog, wxID_ANY, initialFy);
    fySizer->Add(fyCtrl, 1, wxEXPAND);
    mainSizer->Add(fySizer, 0, wxEXPAND | wxALL, 5);
    
    // Ejemplos predefinidos
    mainSizer->Add(new wxStaticText(&dialog, wxID_ANY, wxT("Ejemplos:")), 0, wxALL, 5);
    wxListBox* examples = new wxListBox(&dialog, wxID_ANY);
    examples->Append(wxT("Uniforme: Fx=1, Fy=0"));
    examples->Append(wxT("Rotacional: Fx=-y, Fy=x"));
    examples->Append(wxT("Radial: Fx=x, Fy=y"));
    examples->Append(wxT("Gradiente: Fx=x, Fy=-y"));
    examples->Append(wxT("Caracol: Fx=-0.08*(x-25)-0.25*(y-25), Fy=-0.08*(y-25)+0.25*(x-25)"));
    examples->Bind(wxEVT_LISTBOX, [fxCtrl, fyCtrl, examples](wxCommandEvent& evt) {
        int sel = evt.GetSelection();
        if (sel == wxNOT_FOUND) return;
        switch (sel) {
            case 0: fxCtrl->SetValue(wxT("1")); fyCtrl->SetValue(wxT("0")); break;
            case 1: fxCtrl->SetValue(wxT("-(y-25)")); fyCtrl->SetValue(wxT("x-25")); break;
            case 2: fxCtrl->SetValue(wxT("x-25")); fyCtrl->SetValue(wxT("y-25")); break;
            case 3: fxCtrl->SetValue(wxT("x")); fyCtrl->SetValue(wxT("-y")); break;
            case 4: fxCtrl->SetValue(wxT("-0.08*(x-25)-0.25*(y-25)")); fyCtrl->SetValue(wxT("-0.08*(y-25)+0.25*(x-25)")); break;
            default: break;
        }
    });
    mainSizer->Add(examples, 1, wxEXPAND | wxALL, 5);
    
    // Botones
    wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okBtn = new wxButton(&dialog, wxID_OK, wxT("Aplicar"));
    wxButton* cancelBtn = new wxButton(&dialog, wxID_CANCEL, wxT("Cancelar"));
    btnSizer->Add(okBtn, 0, wxALL, 5);
    btnSizer->Add(cancelBtn, 0, wxALL, 5);
    mainSizer->Add(btnSizer, 0, wxALIGN_CENTER | wxALL, 10);
    
    dialog.SetSizer(mainSizer);
    
    if (dialog.ShowModal() == wxID_OK) {
        wxString fxStr = fxCtrl->GetValue();
        wxString fyStr = fyCtrl->GetValue();
        
        std::string fxExpr = std::string(fxStr.mb_str());
        std::string fyExpr = std::string(fyStr.mb_str());
        
        tp::domain::MathExpression parser;
        
        if (parser.parse(fxExpr, fyExpr)) {
            auto& config = experimentService_.getConfig();
            config.fieldView.type = tp::application::FieldPresetType::Custom;
            config.fieldView.customFx = fxExpr;
            config.fieldView.customFy = fyExpr;

            // Aplicamos el campo personalizado directamente para evitar que
            // un flujo de resincronización de UI deje el campo activo en cero.
            const tp::domain::VectorField customField([parser](double x, double y) {
                return parser.evaluate(x, y);
            });
            experimentService_.setField(customField);

            const auto boatPos = config.boat.getPosition();
            const auto sampleBoat = config.field.getValue(boatPos.x, boatPos.y);
            const double sampleMagnitude = sampleBoat.magnitude();

            simulationStatusText_ = wxT("Campo personalizado aplicado");
            refreshStatusBar();
            resetSimulation();
            markAsModified(true);
            
            wxMessageBox(wxT("Campo personalizado aplicado:\nFx = ") + fxStr + wxT("\nFy = ") + fyStr +
                         wxString::Format(wxT("\n\nMuestra en bote: (%.3f, %.3f)\nMagnitud: %.3f"),
                                          sampleBoat.x, sampleBoat.y, sampleMagnitude),
                         wxT("Campo Vectorial"), wxOK | wxICON_INFORMATION);
            
            if (bottomPanel_) {
                bottomPanel_->addEvent(wxT("Campo personalizado aplicado"));
            }
            
            updateCanvas();
        } else {
            wxMessageBox(wxT("Error: No se pudo parsear la expresión.\nVerifique la sintaxis."),
                         wxT("Error"), wxOK | wxICON_ERROR);
        }
    }
}


} // namespace tp::presentation
