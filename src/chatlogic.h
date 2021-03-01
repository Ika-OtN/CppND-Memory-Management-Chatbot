#ifndef CHATLOGIC_H_
#define CHATLOGIC_H_

#include "chatgui.h"
#include <string>
#include <vector>

using namespace std;

// forward declarations
class ChatBot;
class GraphEdge;
class GraphNode;

class ChatLogic {
private:
  // data handles (owned)
  vector<unique_ptr<GraphNode>> _nodes;

  // data handles (not owned)
  GraphNode *_currentNode;
  ChatBot *_chatBot;
  ChatBotPanelDialog *_panelDialog;

  // proprietary type definitions
  typedef vector<pair<string, string>> tokenlist;

  // proprietary functions
  template <typename T>
  void AddAllTokensToElement(string tokenID, tokenlist &tokens, T &element);

public:
  // constructor / destructor
  ChatLogic();
  ~ChatLogic();

  // getter / setter
  void SetPanelDialogHandle(ChatBotPanelDialog *panelDialog);
  void SetChatbotHandle(ChatBot *chatbot);

  // proprietary functions
  void LoadAnswerGraphFromFile(string filename);
  void SendMessageToChatbot(string message);
  void SendMessageToUser(string message);
  wxBitmap *GetImageFromChatbot();
};

#endif /* CHATLOGIC_H_ */