#include "JsonNode.h"

using namespace std;

namespace LeptJson
{
	JsonNode::JsonNode()
	{
		type = T_NULL;
		name = "";

		D_BOOL = false;
		D_NUMBER = 0.0;
		D_STRING = "";

		Father = nullptr;
	}
}