#include "JsonTree.h"

using namespace std;

namespace LeptJson
{
	//Constructor
	JsonTree::JsonTree()
	{
		ROOT = new JsonNode;
		ROOT->type = T_OBJECT;
		ROOT->Father = nullptr;
	}

	JsonTree::JsonTree(JsonTree &ths)
	{
		this->ReadJsonFromString(ths.JsonReaded);
	}

	//Destructor
	JsonTree::~JsonTree()
	{
		for (auto &v : ROOT->Sons)
		{
			DeleteJsonTree(v);
		}
	}

	//Delete Json Tree by Recursion
	void DeleteJsonTree(JsonNode *Node)
	{
		if (Node->Sons.size() == 0)
		{
			delete Node;
		}
		else
		{
			for (auto &v : Node->Sons)
			{
				DeleteJsonTree(v);
			}
			delete Node;
		}
	}

	int JsonTree::ReadJsonFromFile(const string &FileName)
	{
		int returnNum = 0;
		fstream ss(FileName);
		if (!ss.is_open())
		{
			return FileCanNotOpen;
		}
		else
		{
			string JsonString;

			string hh;
			while (getline(ss, hh))
			{
				JsonString += '\n';
				JsonString += hh;
			}

			ss.close();

			returnNum = ReadJsonFromString(JsonString);
		}
		return returnNum;
	}

	int JsonTree::ReadJsonFromString(const string &JsonString)
	{
		JsonReaded = JsonString;

		auto iter = JsonString.begin();
		auto End = JsonString.end();

		if (!haveChar(iter, End, '{'))
		{
			return JsonWrongSyntax;
		}

		levelStack.push('{');

		//Now Father Node
		JsonNode *nowRoot = ROOT;
		//Now Status
		int status = 1;

		//Now Structuring Node
		JsonNode thisOne;

		while (true)
		{
			switch (status)
			{
				//0:Create a Empty JsonNode to Initialize ThisOne,then Switch Status to Waiting Node Name Input
			case 0:
			{
				JsonNode temp;
				thisOne = temp;

				status = 1;
			}
			break;

			//1:Waiting a New Node Name Inputing,for Example " Name " :
			case 1:
			{
				if (!getStringWithQuotes(iter, End, thisOne.name))
				{
					return JsonWrongNodeName;
				}

				if (!haveChar(iter, End, ':'))
				{
					return JsonWrongNodeName;
				}

				status = 2;
			}
			break;

			//2:Type Judgment,for Example " n t f - 0~9 { 
			case 2:
			{
				while (isspace(*iter))
				{
					++iter;
					if (iter == End)
					{
						return JsonWrongType;
					}
				}

				thisOne.Father = nowRoot;

				switch (*iter)
				{
				case 'n':
				{
					string pd(iter, iter + 4);
					if (pd != "null")
					{
						return JsonWrongType;
					}

					thisOne.type = T_NULL;

					JsonNode* Temp = new JsonNode;
					*Temp = thisOne;
					nowRoot->Sons.push_back(Temp);

					status = 4;
					iter += 4;
				}
				break;

				case 't':
				{
					string pd(iter, iter + 4);
					if (pd != "true")
					{
						return JsonWrongType;
					}

					thisOne.type = T_BOOL;
					thisOne.D_BOOL = true;

					JsonNode* Temp = new JsonNode;
					*Temp = thisOne;
					nowRoot->Sons.push_back(Temp);

					status = 4;
					iter += 4;
				}
				break;

				case 'f':
				{
					string pd(iter, iter + 5);
					if (pd != "false")
					{
						return JsonWrongType;
					}

					thisOne.type = T_BOOL;
					thisOne.D_BOOL = false;

					JsonNode* Temp = new JsonNode;
					*Temp = thisOne;
					nowRoot->Sons.push_back(Temp);

					status = 4;
					iter += 5;
				}
				break;

				case '\"':
				{
					if (!getStringWithQuotes(iter, End, thisOne.D_STRING))
					{
						return JsonWrongSyntax;
					}
					thisOne.type = T_STRING;

					JsonNode* Temp = new JsonNode;
					*Temp = thisOne;
					nowRoot->Sons.push_back(Temp);

					status = 4;
				}
				break;

				case '[':
				{
					thisOne.type = T_VECTOR;
					JsonNode* Temp = new JsonNode;
					*Temp = thisOne;
					nowRoot = Temp;

					nowRoot->Father->Sons.push_back(Temp);

					JsonNode newi;
					thisOne = newi;

					levelStack.push('[');

					++iter;
					if (iter == End)
					{
						return JsonWrongSyntax;
					}

					status = 2;
				}
				break;

				case '{':
				{
					levelStack.push('{');
					thisOne.type = T_OBJECT;
					JsonNode *Temp = new JsonNode;
					*Temp = thisOne;
					nowRoot = Temp;

					nowRoot->Father->Sons.push_back(Temp);

					status = 0;

					++iter;
					if (iter == End)
					{
						return JsonWrongSyntax;
					}
				}
				break;

				default:
				{
					if (isdigit(*iter) || *iter == '-')
					{
						thisOne.type = T_NUMBER;
						status = 3;
					}
					else
					{
						return JsonWrongType;
					}
				}
				break;
				}
			}
			break;

			//3:Enter Numeric Values
			case 3:
			{
				auto tempIter = iter;
				while (!(isspace(*iter) || *iter == '}' || *iter == ',' || *iter == ']'))
				{
					++iter;
					if (iter == End)
					{
						return JsonWrongSyntax;
					}
				}

				string numberString(tempIter, iter);
				stringstream ss;
				ss << numberString;
				if (ss >> thisOne.D_NUMBER)
				{
					ss.clear();

					JsonNode* Temp = new JsonNode;
					*Temp = thisOne;
					nowRoot->Sons.push_back(Temp);
				}
				else
				{
					return JsonWrongNumber;
				}

				status = 4;
			}
			break;

			//4:Enter Numeric Values Finished，Expecting Text is '}' or ']',then the Function will judgment Whether to End the Json Text Input or not
			case 4:
			{
				while (isspace(*iter))
				{
					++iter;
					if (iter == End)
					{
						return JsonWrongSyntax;
					}
				}

				if (*iter == '}')
				{
					if (levelStack.top() != '{')
					{
						return JsonWrongSyntax;
					}
					levelStack.pop();

					nowRoot = nowRoot->Father;
					if (levelStack.size() == 0)
					{
						++iter;
						if (iter == End)
						{
							return 0;
						}
						else
						{
							bool flag = false;
							while (true)
							{
								if (iter == End)
								{
									return 0;
								}

								if (isspace(*iter))
								{
									++iter;
								}
								else
								{
									return JsonWrongSyntax;
								}
							}
						}
					}
					else
					{
						++iter;
						if (iter == End)
						{
							return JsonWrongSyntax;
						}
						status = 4;
					}
				}
				else if (*iter == ']')
				{
					if (levelStack.top() != '[')
					{
						return JsonWrongSyntax;
					}
					levelStack.pop();

					++iter;
					if (iter == End)
					{
						return JsonWrongSyntax;
					}

					nowRoot = nowRoot->Father;
					status = 4;
				}
				else if (*iter == ',')
				{
					++iter;
					if (iter == End)
					{
						return JsonWrongSyntax;
					}

					if (levelStack.top() == '{')
					{
						status = 0;
					}
					else
					{
						status = 2;
					}
				}
			}
			break;
			}
		}

		return 0;
	}

	string JsonTree::GetJsonToString()
	{
		string JSONStr = "{\n";
		for (auto &v : ROOT->Sons)
		{
			DFSLoop(v, JSONStr);
		}
		JSONStr.pop_back();
		JSONStr.pop_back();
		JSONStr += "\n}";

		return JSONStr;
	}

	int JsonTree::GetJsonToFile(const string &FileName)
	{
		ofstream fout(FileName);

		if (fout.is_open())
		{
			string theJson = GetJsonToString();
			fout << theJson;
			fout.close();
		}
		else
		{
			return FileCanNotCreate;
		}

		return 0;
	}

	JsonNode *JsonTree::FindNode(const string &NodeName)
	{
		JsonNode *returnNode;
		DFSFound(ROOT, NodeName, returnNode);
		return returnNode;
	}

	bool JsonTree::deleteNode(JsonNode *JsonN)
	{
		if (JsonN == nullptr)
		{
			return false;
		}
		else
		{
			for (auto iter = JsonN->Father->Sons.begin(); iter != JsonN->Father->Sons.end(); ++iter)
			{
				if (*iter == JsonN)
				{
					JsonN->Father->Sons.erase(iter);
					break;
				}
			}

			DeleteJsonTree(JsonN);
		}
	}

	bool JsonTree::operator==(JsonTree &That)
	{
		list<string> thisOne, thatOne;

		DFSStringCollector(ROOT, thisOne);
		DFSStringCollector(That.ROOT, thatOne);

		thisOne.sort(); thatOne.sort();

		if (thisOne.size() != thatOne.size())
		{
			return false;
		}

		bool AllSame = true;
		auto thisIter = thisOne.begin(), thatIter = thatOne.begin();
		while (thisIter == thisOne.end())
		{
			if (*thisIter != *thatIter)
			{
				AllSame = false;
				break;
			}

			++thisIter; ++thatIter;
		}

		return AllSame;
	}

	//Expecting Text is Space(Optional)-Specified Character-Space(Optional),End this Function,Iter Point a Not-A-Blank Character after the Last Space
	bool haveChar(string::const_iterator &iter, string::const_iterator &end, char character)
	{
		while (isspace(*iter))
		{
			++iter;
			if (iter == end)
			{
				return false;
			}
		}

		if (*iter != character)
		{
			return false;
		}

		++iter;
		if (iter == end)
		{
			return false;
		}

		while (isspace(*iter))
		{
			++iter;
			if (iter == end)
			{
				return false;
			}
		}

		return true;
	}

	//Excepting a String,End this Function,Iter Point the first Character after String
	bool getString(string::const_iterator &iter, string::const_iterator &end, std::string &theString)
	{
		auto BeginIter = iter;
		while (*iter != '\"' || (*iter == '\"'&&*(iter - 1) == '\\'))
		{
			++iter;
			if (iter == end)
			{
				return false;
			}
		}

		string STR(BeginIter, iter);
		theString = STR;

		return true;
	}

	//Excepting a String With Quotes and Spaces on both sides,End this Function,Iter Point the first Character after String
	bool getStringWithQuotes(std::string::const_iterator &iter, std::string::const_iterator &end, std::string &theStr)
	{
		if (!haveChar(iter, end, '\"'))
		{
			return false;
		}

		if (!getString(iter, end, theStr))
		{
			return false;
		}

		if (!haveChar(iter, end, '\"'))
		{
			return false;
		}

		return true;
	}

	//DFS Ergodic JsonTree
	void DFSLoop(JsonNode *Root, string &ss)
	{
		static int level = 0;
		for (int i = 0; i<(level + 5); ++i)
		{
			ss.push_back(' ');
		}

		if (Root->Father->type != T_VECTOR)
		{
			ss += "\"";
			ss += Root->name;
			ss += "\":";
		}


		if (Root->Sons.size() == 0)
		{
			switch (Root->type)
			{
			case T_BOOL:
				if (Root->D_BOOL)
				{
					ss += "true,\n";
				}
				else
				{
					ss += "false,\n";
				}
				break;

			case T_NULL:
				ss += "null,\n";
				break;

			case T_NUMBER:
			{
				stringstream sa;
				string temp;
				sa << Root->D_NUMBER;
				sa >> temp;
				sa.clear();
				ss += temp;
				ss += ",\n";
			}
			break;

			case T_STRING:
				ss += "\"";
				ss += Root->D_STRING;
				ss += "\",\n";
				break;
			}
		}
		else
		{
			if (Root->type == T_VECTOR)
			{
				level += 5;
				ss += "[\n";
			}
			else if (Root->type == T_OBJECT)
			{
				level += 5;
				ss += "{\n";
			}

			for (auto &v : Root->Sons)
			{
				DFSLoop(v, ss);
			}

			if (Root->type == T_VECTOR)
			{
				ss.pop_back();
				ss.pop_back();
				ss.push_back('\n');

				for (int i = 0; i<(level + 5); ++i)
				{
					ss.push_back(' ');
				}
				ss += "],\n";
				level -= 5;
			}
			else if (Root->type == T_OBJECT)
			{
				ss.pop_back();
				ss.pop_back();
				ss.push_back('\n');

				for (int i = 0; i<(level + 5); ++i)
				{
					ss.push_back(' ');
				}
				ss += "},\n";
				level -= 5;
			}
		}
	}

	void DFSFound(JsonNode *TreeNode, const string &Name, JsonNode *&Result)
	{
		static bool continu = true;

		if (continu)
		{
			if (TreeNode->Sons.size() == 0)
			{
				if (TreeNode->name == Name)
				{
					continu = false;
					Result = TreeNode;
					return;
				}
			}
			else
			{
				if (TreeNode->name == Name)
				{
					continu = false;
					Result = TreeNode;
					return;
				}

				for (auto &v : TreeNode->Sons)
				{
					DFSFound(v, Name, Result);
				}
			}
		}
		else
		{
			return;
		}
	}

	void DFSStringCollector(JsonNode *Root, list<string> &SS)
	{
		static list<string>prefix(1, "ROOT");

		if (Root->Sons.size() == 0)
		{
			string temp;
			for (auto &v : prefix)
			{
				temp += v;
			}
			temp += "\"";

			temp += Root->name;

			switch (Root->type)
			{
			case T_BOOL:
				if (Root->D_BOOL)
				{
					temp += "true";
				}
				else
				{
					temp += "false";
				}
				break;

			case T_NULL:
				temp += "null";
				break;

			case T_NUMBER:
			{
				stringstream sa;
				string t;
				sa << Root->D_NUMBER;
				sa >> t;
				sa.clear();
				temp += t;
			}
			break;

			case T_STRING:
				temp += Root->D_STRING;
				break;
			}

			SS.push_back(temp);
		}
		else
		{
			prefix.push_back(Root->name);

			for (auto &v : Root->Sons)
			{
				DFSStringCollector(v, SS);
			}

			prefix.pop_back();
		}
	}
}
