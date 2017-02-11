#ifndef _JSON_NODE_H_
#define _JSON_NODE_H_

#include <string>
#include <list>

namespace LeptJson
{
	//Type of Json Node: type-NULL = 0, type-Bool = 1, type-Number = 2, type-String = 3, type-Array = 4,type-Object = 5
	enum DataType { T_NULL, T_BOOL, T_NUMBER, T_STRING, T_VECTOR, T_OBJECT };

	//Json Node Class
	struct JsonNode
	{
		//FUNCTION
		JsonNode();

		//DATA
		DataType type;
		std::string name;
		
		//DATA_VALUES
		bool D_BOOL;
		double D_NUMBER;
		std::string D_STRING;

		std::list <JsonNode*> Sons;
		JsonNode *Father;
	};
}

#endif