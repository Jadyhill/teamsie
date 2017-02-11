#ifndef _JSON_TREE_H_
#define _JSON_TREE_H_

//Macros : Some Error Code
#define FileCanNotOpen        1
#define FileCanNotCreate      2
#define JsonWrongSyntax       3
#define JsonWrongNodeName     4
#define JsonWrongType         5
#define JsonWrongNumber       6

#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include <algorithm>
#include <cctype>

#include "JsonNode.h"

namespace LeptJson
{
	//Json Tree
	class JsonTree
	{
	public:
		JsonTree();
		JsonTree(JsonTree &ths);
		~JsonTree();

		int ReadJsonFromFile(const std::string &FileName);
		int ReadJsonFromString(const std::string &JsonString);

		std::string GetJsonToString();
		int GetJsonToFile(const std::string &FileName);

		JsonNode *FindNode(const std::string &NodeName);
		bool deleteNode(JsonNode *JsonN);
		bool operator==(JsonTree &That);

		//Root Node of the Json Tree
		JsonNode *ROOT;

	private:
		
		//Json String Readed
		std::string JsonReaded;

		//stack with '{' and '['
		std::stack<char> levelStack;
	};

	void DeleteJsonTree(JsonNode *Node);
	
	bool haveChar(std::string::const_iterator &iter, std::string::const_iterator &end, char character);
	bool getString(std::string::const_iterator &iter, std::string::const_iterator &end, std::string &theString);
	bool getStringWithQuotes(std::string::const_iterator &iter, std::string::const_iterator &end, std::string &theStr);
	void DFSLoop(JsonNode *Root, std::string &ss);
    void DFSFound(JsonNode *TreeNode, const std::string &Name,JsonNode *&Result);
	void DFSStringCollector(JsonNode *Root, std::list<std::string> &SS);
}

#endif
