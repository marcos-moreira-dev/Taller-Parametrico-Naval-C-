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

void MainWindow::onViewLeftPanel(wxCommandEvent& event)
{
    wxAuiPaneInfo& pane = auiManager_.GetPane(leftPanel_);
    pane.Show(event.IsChecked());
    auiManager_.Update();
    syncViewMenuChecks();
}

void MainWindow::onViewRightPanel(wxCommandEvent& event)
{
    wxAuiPaneInfo& pane = auiManager_.GetPane(rightPanel_);
    pane.Show(event.IsChecked());
    auiManager_.Update();
    syncViewMenuChecks();
}

void MainWindow::onViewBottomPanel(wxCommandEvent& event)
{
    wxAuiPaneInfo& pane = auiManager_.GetPane(bottomPanel_);
    pane.Show(event.IsChecked());
    auiManager_.Update();
    syncViewMenuChecks();
}

void MainWindow::onViewGrid(wxCommandEvent& event)
{
    if (canvas_) {
        canvas_->setShowGrid(event.IsChecked());
    }
}

void MainWindow::onViewField(wxCommandEvent& event)
{
    if (canvas_) {
        canvas_->setShowField(event.IsChecked());
    }
}

void MainWindow::onViewTrajectory(wxCommandEvent& event)
{
    if (canvas_) {
        canvas_->setShowTrajectory(event.IsChecked());
    }
}

void MainWindow::onModeTheory(wxCommandEvent& event)
{
    setMode(AppMode::Theory);
}

void MainWindow::onModeSimulation(wxCommandEvent& event)
{
    setMode(AppMode::Simulation);
}

void MainWindow::onModeEditor(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::STATE_CHANGE, "MainWindow", "onModeEditor");
    setMode(AppMode::Editor);
}

void MainWindow::onModeFieldEditor(wxCommandEvent& event)
{
    (void)event;
    TRACE_INFO(tp::shared::TraceEventType::STATE_CHANGE, "MainWindow", "onModeFieldEditor");
    setMode(AppMode::FieldEditor);
}


} // namespace tp::presentation
