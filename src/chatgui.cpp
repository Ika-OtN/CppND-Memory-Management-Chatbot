#include "chatgui.h"
#include "chatbot.h"
#include "chatlogic.h"
#include <memory>
#include <string>
#include <wx/colour.h>
#include <wx/filename.h>
#include <wx/image.h>

const int width = 414;
const int height = 736;

IMPLEMENT_APP(ChatBotApp);

std::string dataPath = "../";
std::string imgBasePath = dataPath + "images/";

bool ChatBotApp::OnInit() {

  ChatBotFrame *chatBotFrame = new ChatBotFrame(wxT("Udacity ChatBot"));
  chatBotFrame->Show(true);

  return true;
}

// wxWidgets FRAME
ChatBotFrame::ChatBotFrame(const wxString &title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(width, height)) {

  ChatBotFrameImagePanel *ctrlPanel = new ChatBotFrameImagePanel(this);

  _panelDialog = new ChatBotPanelDialog(ctrlPanel, wxID_ANY);

  int idTextXtrl = 1;
  _userTextCtrl = new wxTextCtrl(ctrlPanel, idTextXtrl, "", wxDefaultPosition, wxSize(width, 50), wxTE_PROCESS_ENTER, wxDefaultValidator, wxTextCtrlNameStr);
  Connect(idTextXtrl, wxEVT_TEXT_ENTER, wxCommandEventHandler(ChatBotFrame::OnEnter));

  wxBoxSizer *vertBoxSizer = new wxBoxSizer(wxVERTICAL);
  vertBoxSizer->AddSpacer(90);
  vertBoxSizer->Add(_panelDialog, 6, wxEXPAND | wxALL, 0);
  vertBoxSizer->Add(_userTextCtrl, 1, wxEXPAND | wxALL, 5);
  ctrlPanel->SetSizer(vertBoxSizer);

  this->Centre();
  
}

void ChatBotFrame::OnEnter(wxCommandEvent &WXUNUSED(event)) {
  wxString userText = _userTextCtrl->GetLineText(0);

  _panelDialog->AddDialogItem(userText, true);

  _userTextCtrl->Clear();
  _panelDialog->GetChatLogicHandle()->SendMessageToChatbot( std::string(userText.mb_str() ));
}

BEGIN_EVENT_TABLE(ChatBotFrameImagePanel, wxPanel)
EVT_PAINT(ChatBotFrameImagePanel::paintEvent) // catch paint events
END_EVENT_TABLE()

ChatBotFrameImagePanel::ChatBotFrameImagePanel(wxFrame *parent) : wxPanel(parent) {}

void ChatBotFrameImagePanel::paintEvent(wxPaintEvent &evt) {
  wxPaintDC dc(this);
  render(dc);
}

void ChatBotFrameImagePanel::paintNow() {
  wxClientDC dc(this);
  render(dc);
}

void ChatBotFrameImagePanel::render(wxDC &dc) {
  wxString imgFile = imgBasePath + "sf_bridge.jpg";
  wxImage image;
  image.LoadFile(imgFile);

  wxSize sz = this->GetSize();
  wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);
  _image = wxBitmap(imgSmall);

  dc.DrawBitmap(_image, 0, 0, false);
}

BEGIN_EVENT_TABLE(ChatBotPanelDialog, wxPanel)
EVT_PAINT(ChatBotPanelDialog::paintEvent) 
END_EVENT_TABLE()

ChatBotPanelDialog::ChatBotPanelDialog(wxWindow *parent, wxWindowID id)
    : wxScrolledWindow(parent, id) {
 
  _dialogSizer = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(_dialogSizer);

  wxInitAllImageHandlers();
  this->_chatLogic = std::make_unique<ChatLogic>();

  _chatLogic->SetPanelDialogHandle(this);
      
  _chatLogic->LoadAnswerGraphFromFile(dataPath + "src/answergraph.txt");
}

ChatBotPanelDialog::~ChatBotPanelDialog() {}

void ChatBotPanelDialog::AddDialogItem(wxString text, bool isFromUser) {
 
  ChatBotPanelDialogItem *item =
      new ChatBotPanelDialogItem(this, text, isFromUser);
  _dialogSizer->Add(
      item, 0, wxALL | (isFromUser == true ? wxALIGN_LEFT : wxALIGN_RIGHT), 8);
  _dialogSizer->Layout();

 
  this->FitInside(); 
  this->SetScrollRate(5, 5);
  this->Layout();

 
  int dx, dy;
  this->GetScrollPixelsPerUnit(&dx, &dy);
  int sy = dy * this->GetScrollLines(wxVERTICAL);
  this->DoScroll(0, sy);
}

void ChatBotPanelDialog::PrintChatbotResponse(std::string response) {
  wxString botText(response.c_str(), wxConvUTF8);
  AddDialogItem(botText, false);
}

void ChatBotPanelDialog::paintEvent(wxPaintEvent &evt) {
  wxPaintDC dc(this);
  render(dc);
}

void ChatBotPanelDialog::paintNow() {
  wxClientDC dc(this);
  render(dc);
}

void ChatBotPanelDialog::render(wxDC &dc) {
  wxImage image;
  image.LoadFile(imgBasePath + "sf_bridge_inner.jpg");

  wxSize sz = this->GetSize();
  wxImage imgSmall = image.Rescale(sz.GetWidth(), sz.GetHeight(), wxIMAGE_QUALITY_HIGH);

  _image = wxBitmap(imgSmall);
  dc.DrawBitmap(_image, 0, 0, false);
}

ChatBotPanelDialogItem::ChatBotPanelDialogItem(wxPanel *parent, wxString text, bool isFromUser) : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_NONE) {
  wxBitmap *bitmap = isFromUser == true ? nullptr : ((ChatBotPanelDialog *)parent)
  ->GetChatLogicHandle()
  ->GetImageFromChatbot();

  _chatBotImg = new wxStaticBitmap( this, wxID_ANY, (isFromUser ? wxBitmap(imgBasePath + "user.png", wxBITMAP_TYPE_PNG) : *bitmap), wxPoint(-1, -1), wxSize(-1, -1));
  
  _chatBotTxt = new wxStaticText(this, wxID_ANY, text, wxPoint(-1, -1), wxSize(150, -1), wxALIGN_CENTRE | wxBORDER_NONE);
  
  _chatBotTxt->SetForegroundColour(isFromUser == true ? wxColor(*wxBLACK) : wxColor(*wxWHITE));

  wxBoxSizer *horzBoxSizer = new wxBoxSizer(wxHORIZONTAL);
  horzBoxSizer->Add(_chatBotTxt, 8, wxEXPAND | wxALL, 1);
  horzBoxSizer->Add(_chatBotImg, 2, wxEXPAND | wxALL, 1);
  this->SetSizer(horzBoxSizer);

  _chatBotTxt->Wrap(150);

  this->SetBackgroundColour((isFromUser == true ? wxT("YELLOW") : wxT("BLUE")));
}