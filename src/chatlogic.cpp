#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <tuple>
#include <vector>

#include "chatbot.h"
#include "chatlogic.h"
#include "graphedge.h"
#include "graphnode.h"

using namespace std;

ChatLogic::ChatLogic() {}

ChatLogic::~ChatLogic() {}

template <typename T>
void ChatLogic::AddAllTokensToElement(string tokenID, tokenlist &tokens,
                                      T &element) {
  // find all occurences for current node
  auto token = tokens.begin();
  
  while (true) {
    token = find_if(
        token, tokens.end(), [&tokenID](const pair<string, string> &pair) {
          
          return pair.first == tokenID;
          // TODO
    });
    
    if (token != tokens.end()) {
      element.AddToken(token->second); // add new keyword to edge
      token++;                         // increment iterator to next element
    } else {
      
      break; // quit infinite while-loop
    }
  }
}

void ChatLogic::LoadAnswerGraphFromFile(string filename) {
  // load file with answer graph elements
  ifstream file(filename);

  // check for file availability and process it line by line
  if (file) {
    // loop over all lines in the file
    string lineStr;
    while (getline(file, lineStr)) {
      // extract all tokens from current line
      tokenlist tokens;
      while (lineStr.size() > 0) {
        // extract next token
        int posTokenFront = lineStr.find("<");
        int posTokenBack = lineStr.find(">");
        if (posTokenFront < 0 || posTokenBack < 0)
          
          break; // quit loop if no complete token has been found
        string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - 1);

        // extract token type and info
        int posTokenInfo = tokenStr.find(":");
        
        if (posTokenInfo != string::npos) {
          string tokenType = tokenStr.substr(0, posTokenInfo);
          string tokenInfo = tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

          // add token to vector
          tokens.push_back(make_pair(tokenType, tokenInfo));
        }

        // remove token from current line
        lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
      }

      // process tokens for current line
      auto type = find_if(tokens.begin(), tokens.end(), [](const pair<string, string> &pair) {
          return pair.first == "TYPE";
      });
      
      if (type != tokens.end()) {
        // check for id
        auto idToken =
            find_if(tokens.begin(), tokens.end(), [](const pair<string, string> &pair) {
            
            return pair.first == "ID";
        });
        
        if (idToken != tokens.end()) {
          // extract id from token
          int id = stoi(idToken->second);

          // node-based processing
          if (type->second == "NODE") {

            // check if node with this ID exists already
            // Comment: Replaced raw pointer by a REFERENCE of the unique
            //          pointer. Usage of pure unique pointers would not work
            //          because the function find_if would try to make copies of
            //          the unique pointers (forbidden!).
            auto newNode = find_if(_nodes.begin(), _nodes.end(), [&id](auto &node) { return node->GetID() == id; });

            // create new element if ID does not yet exist
            if (newNode == _nodes.end()) {
              // Usage of make_unique because copies of unique pointers are not
              // allowed per definition and emplace back tries to copy an
              // instance in order to add it at the back of the vector
              _nodes.emplace_back(make_unique<GraphNode>(id));
              auto newNode = _nodes.end() - 1; // get iterator to last element

              // add all answers to current node
              AddAllTokensToElement("ANSWER", tokens, **newNode);
            }
          }

          // edge-based processing
          if (type->second == "EDGE") {
            // find tokens for incoming (parent) and outgoing (child) node
            auto parentToken = find_if( tokens.begin(), tokens.end(), [](const pair<string, string> &pair) {
                  
              return pair.first == "PARENT";
            });
            
            auto childToken = find_if( tokens.begin(), tokens.end(), [](const pair<string, string> &pair) {
                  
              return pair.first == "CHILD";
            });

            if (parentToken != tokens.end() && childToken != tokens.end()) {
              // get iterator on incoming and outgoing node via ID search
              auto parentNode = find_if( _nodes.begin(), _nodes.end(), [&parentToken](auto &node) {
                    
                return node->GetID() == stoi(parentToken->second);
              });
              
              auto childNode = find_if( _nodes.begin(), _nodes.end(), [&childToken](auto &node) {
                    
                return node->GetID() == stoi(childToken->second);
              });

              // create new edge
              unique_ptr<GraphEdge> edge(new GraphEdge(id));
              edge->SetChildNode((*childNode).get());
              edge->SetParentNode((*parentNode).get());

              // find all keywords for current node
              AddAllTokensToElement("KEYWORD", tokens, *edge);

              // store reference in child node and parent node
              (*childNode)->AddEdgeToParentNode(edge.get());
              (*parentNode)->AddEdgeToChildNode(move(edge));
            }
          }
        } else {
          
          cout << "Error: ID missing. Line is ignored!" << endl;
        }
      }
    } // eof loop over all lines in the file

    file.close();

  } // eof check for file availability
  else {
    cout << "File could not be opened!" << endl;
    return;
  }

  // identify root node
  GraphNode *rootNode = nullptr;
  for (auto it = begin(_nodes); it != end(_nodes); ++it) {
    // search for nodes which have no incoming edges
    if ((*it)->GetNumberOfParents() == 0) {

      if (rootNode == nullptr) {
        
        rootNode = (*it).get(); // assign current node to root
      } else {
        
        cout << "ERROR : Multiple root nodes detected" << endl;
      }
    }
  }

  // create instance of chatbot
  ChatBot bot = ChatBot("../images/chatbot.png");
  // set _chatBot value for GUI communication
  _chatBot = &bot;

  // add pointer to chatlogic so that chatbot answers can be passed on to the
  // GUI
  bot.SetChatLogicHandle(this);

  // add chatbot to graph root node
  bot.SetRootNode(rootNode);
  rootNode->MoveChatbotHere(move(bot));
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog) { _panelDialog = panelDialog; }

void ChatLogic::SetChatbotHandle(ChatBot *chatbot) { _chatBot = chatbot; }

void ChatLogic::SendMessageToChatbot(string message) { _chatBot->ReceiveMessageFromUser(message); }

void ChatLogic::SendMessageToUser(string message) { _panelDialog->PrintChatbotResponse(message); }

wxBitmap *ChatLogic::GetImageFromChatbot() {
  
  return _chatBot->GetImageHandle();
}