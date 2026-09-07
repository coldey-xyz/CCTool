#include <wx/wx.h>
#include "MainFrame.h"

class CCTool : public wxApp {
    public:
    virtual bool OnInit() override;
};
wxIMPLEMENT_APP(CCTool);
bool CCTool::OnInit() {
    MainFrame* frame = new MainFrame("coldey's compression tool");
    frame->Show(true);
    return true;
}