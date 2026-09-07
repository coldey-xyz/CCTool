#include "MainFrame.h"
#include "ImageProcessor.h"

namespace {
    const wxString kInputWildcard =
    "images (*.png;*.jpeg;*.bmp;*.tga)|*.png;*.jpeg;*.bmp;*.tga";
    const wxString kFormatExtensions[] = { "png", "jpeg", "bmp", "tga" };
}

MainFrame::MainFrame(const wxString& title)
: wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(480, 380))
{
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* fileSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* chooseFileBtn = new wxButton(panel, wxID_ANY, "pick image...");
    m_inputFileLabel = new wxStaticText(panel, wxID_ANY, "no file selected");
    fileSizer->Add(chooseFileBtn, 0, wxALL, 5);
    fileSizer->Add(m_inputFileLabel, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    mainSizer->Add(fileSizer, 0, wxEXPAND);
    // resolution thing
    wxStaticBoxSizer* sliderSizer = new wxStaticBoxSizer(wxVERTICAL, panel, "output resolution");
    m_resolutionSlider = new wxSlider(panel, wxID_ANY, 100, 20, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_LABELS);
    sliderSizer->Add(m_resolutionSlider, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(sliderSizer, 0, wxEXPAND | wxALL, 10);
    
    // then make the user select the output folder himself and file type too
    wxBoxSizer* dirSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* chooseDirBtn = new wxButton(panel, wxID_ANY, "target folder...");
    m_outputDirLabel = new wxStaticText(panel, wxID_ANY, "no folder selected");
    dirSizer->Add(chooseDirBtn, 0, wxALL, 5);
    dirSizer->Add(m_outputDirLabel, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    mainSizer->Add(dirSizer, 0, wxEXPAND);
    wxBoxSizer* formatSizer = new wxBoxSizer(wxHORIZONTAL);
    formatSizer->Add(new wxStaticText(panel, wxID_ANY, "output format:"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    wxArrayString formats;
    formats.Add("png (.png)");
    formats.Add("jpeg (.jpeg)");
    formats.Add("bmp (.bmp)");
    formats.Add("tga (.tga)");
    m_formatChoice = new wxChoice(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, formats);
    m_formatChoice->SetSelection(0);
    formatSizer->Add(m_formatChoice, 0, wxALL, 5);
    mainSizer->Add(formatSizer, 0, wxEXPAND);
    wxButton* compressBtn = new wxButton(panel, wxID_ANY, "compress!");
    mainSizer->Add(compressBtn, 0, wxALIGN_CENTER | wxALL, 15);
    panel->SetSizer(mainSizer);
    CreateStatusBar();
    SetStatusText("ready!");

    // bind the buttons to the functions
    chooseFileBtn->Bind(wxEVT_BUTTON, &MainFrame::OnChooseFile, this);
    chooseDirBtn->Bind(wxEVT_BUTTON, &MainFrame::OnChooseOutputDir, this);
    compressBtn->Bind(wxEVT_BUTTON, &MainFrame::OnCompress, this);
}   
// now to the others shitlings
void MainFrame::OnChooseFile(wxCommandEvent& event) {
    wxFileDialog openDialog(this, "pick a image", "", "", kInputWildcard, wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openDialog.ShowModal() == wxID_CANCEL) return; // does nun if the user canceled
    m_inputFilePath = openDialog.GetPath();
    m_inputFileLabel->SetLabel(openDialog.GetFilename());
    SetStatusText("picked image (assuming it is one): " + m_inputFilePath);
}
void MainFrame::OnChooseOutputDir(wxCommandEvent& event) {
    wxDirDialog dirDialog(this, "choose the output dir (folder)", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDialog.ShowModal() == wxID_CANCEL) return;
    m_outputDir = dirDialog.GetPath();
    m_outputDirLabel->SetLabel(m_outputDir);
}
// compression
void MainFrame::OnCompress(wxCommandEvent& event) {
    // security check in-case someone fucks it up
    if (m_compressing) {
        static const wxString nagMessages[] = {"no"};
        int index = wxMin(m_busybutton, (int)WXSIZEOF(nagMessages) - 1);
        SetStatusText(nagMessages[index]);
        m_busybutton++;
        return;
    };
    // ^^ basically what that do is prevent the user from pressing the button again if the compression state is still going
    if (m_inputFilePath.IsEmpty()) {
        wxMessageBox("pick a image first (make sure its a image type of file)", "warning", wxOK | wxICON_WARNING);
        return;
    }
    if (m_outputDir.IsEmpty()) {
        wxMessageBox("pick a directory for the output first (make sure its a folder)", "warning", wxOK | wxICON_WARNING);
        return;
    }
    m_compressing = true;
    m_busybutton = 0;
    SetStatusText("compressing...");
    int quality = m_resolutionSlider->GetValue();
    wxString extension = kFormatExtensions[m_formatChoice->GetSelection()];

    // now to actually compressing
    ImageProcessor::CompressResult result = ImageProcessor::Compress(m_inputFilePath, m_outputDir, quality, extension);
    if (result.success) {
        SetStatusText("done: " + result.outputPath);
    } else {
        SetStatusText("error!");
        wxMessageBox(result.errorMessage, "error", wxOK | wxICON_ERROR);
    }
    m_compressing = false;
}