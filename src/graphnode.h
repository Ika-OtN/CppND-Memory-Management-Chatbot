#ifndef GRAPHNODE_H_
#define GRAPHNODE_H_

#include "chatbot.h"
#include <memory>
#include <string>
#include <vector>

using namespace std;

// forward declarations
class GraphEdge;

class GraphNode {
private:
  // data handles (owned)
  vector<unique_ptr<GraphEdge>> _childEdges; // edges to subsequent nodes

  // data handles (not owned)
  vector<GraphEdge *> _parentEdges; // edges to preceding nodes
  ChatBot _chatBot;

  // proprietary members
  int _id;
  vector<string> _answers;

public:
  // constructor / destructor
  GraphNode(int id);
  ~GraphNode();

  // getter / setter
  int GetID() { return _id; }
  
  int GetNumberOfChildEdges() { return _childEdges.size(); }
  
  GraphEdge *GetChildEdgeAtIndex(int index);
  
  vector<string> GetAnswers() { return _answers; }
  
  int GetNumberOfParents() { return _parentEdges.size(); }

  // proprietary functions
  void AddToken(string token); // add answers to list
  void AddEdgeToParentNode(GraphEdge *edge);
  void AddEdgeToChildNode(unique_ptr<GraphEdge> edge);

  // bot functions
  void MoveChatbotHere(ChatBot chatbot);
  void MoveChatbotToNewNode(GraphNode *newNode);
};

#endif /* GRAPHNODE_H_ */