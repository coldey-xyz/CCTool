#pragma once
#include <wx/wx.h>

class MainFrame : public wxFrame {
    public:
    MainFrame(const wxString& title);

    private:
    // whole lotta bullshit tbh
    wxStaticText* m_inputFileLabel;
    wxSlider* m_resolutionSlider;
    wxStaticText* m_outputDirLabel;
    wxChoice* m_formatChoice;
    wxString m_inputFilePath;
    wxString m_outputDir;

    // button functions
    void OnChooseFile(wxCommandEvent& event);
    void OnChooseOutputDir(wxCommandEvent& event);
    void OnCompress(wxCommandEvent& event);

    bool m_compressing = false;
    int m_busybutton = 0;
};