#include <bitset>
#include <map>
#include "readFile.h"

using namespace std;

void readCodetoJson(const std::string &filename)
{
	map<int, string> deviceNameMap;
	deviceNameMap.insert(make_pair(1, "temperature"));
	deviceNameMap.insert(make_pair(2, "humidity"));
	deviceNameMap.insert(make_pair(3, "air pressure"));
	deviceNameMap.insert(make_pair(4, "light intensity(inside)"));
	deviceNameMap.insert(make_pair(5, "light intensity(outside)"));
	deviceNameMap.insert(make_pair(6, "water yield of cistern"));
	deviceNameMap.insert(make_pair(7, "PM2.5"));
	deviceNameMap.insert(make_pair(8, "PM10"));
	deviceNameMap.insert(make_pair(9, "formaldehyde"));
	deviceNameMap.insert(make_pair(10, "carbon dioxide"));
	deviceNameMap.insert(make_pair(11, "rainwater"));
	deviceNameMap.insert(make_pair(12, "resident location"));
	deviceNameMap.insert(make_pair(13, "air conditioner running situation"));
	deviceNameMap.insert(make_pair(14, "water pump running situation"));
	deviceNameMap.insert(make_pair(15, "battery power"));
	deviceNameMap.insert(make_pair(16, "charging pile data"));
	deviceNameMap.insert(make_pair(17, "electric automobile location"));
	deviceNameMap.insert(make_pair(18, "hot water temperature"));
	deviceNameMap.insert(make_pair(19, "generating capacity"));
	deviceNameMap.insert(make_pair(20, "PV panel back temperature"));
	deviceNameMap.insert(make_pair(21, "solar meteorological data"));
	deviceNameMap.insert(make_pair(22, "inverter"));
	deviceNameMap.insert(make_pair(23, "electricity consumption"));

	JsonTree info;

	fstream file(filename);
	string s;

	if (file.is_open())
	{
		int i = 1;
		while (getline(file, s))
		{
			auto iter = s.begin();

			string roomNumStr(iter, iter + 4);
			string devTypeNumStr(iter + 4, iter + 12);
			string hardwareCountNumStr(iter + 12, iter + 16);
			string dataNumStr(iter + 16, iter + 32);
			string decimalCountStr(iter + 32, iter + 34);

			bitset<4> roomNumBit(roomNumStr);
			bitset<8> devTypeNumBit(devTypeNumStr);
			bitset<4> hardwareCountBit(hardwareCountNumStr);
			bitset<16> dataNumBit(dataNumStr);
			bitset<2> decimalCountBit(decimalCountStr);

			int roomNum = roomNumBit.to_ulong();
			int devTypeNum = devTypeNumBit.to_ulong();
			int hardwareCount = hardwareCountBit.to_ulong();
			double dataNum = dataNumBit.to_ulong();
			int decimalCount = decimalCountBit.to_ulong();

			for (int i = 0; i<decimalCount; ++i)
			{
				dataNum /= 10;
			}

			JsonNode *temp = new JsonNode;
			temp->type = T_OBJECT;
			stringstream ss; string s; ss << i; ss >> s; ss.clear();
			temp->name = s;

			JsonNode *tempRMN = new JsonNode;
			tempRMN->type = T_NUMBER;
			tempRMN->name = "Room Number";
			tempRMN->Father = temp;
			tempRMN->D_NUMBER = roomNum;
			temp->Sons.push_back(tempRMN);

			JsonNode *tempDTN = new JsonNode;
			tempDTN->type = T_STRING;
			tempDTN->name = "Device Type Name";
			tempDTN->Father = temp;
			if (devTypeNum >= 1 && devTypeNum <= 23)
			{
				tempDTN->D_STRING = deviceNameMap[devTypeNum];
			}
			else
			{
				tempDTN->D_STRING = "NoName";
			}
			temp->Sons.push_back(tempDTN);

			JsonNode *tempHWC = new JsonNode;
			tempHWC->type = T_NUMBER;
			tempHWC->name = "Serial Number";
			tempHWC->Father = temp;
			tempHWC->D_NUMBER = hardwareCount;
			temp->Sons.push_back(tempHWC);

			JsonNode *tempDN = new JsonNode;
			tempDN->type = T_NUMBER;
			tempDN->name = "Data";
			tempDN->Father = temp;
			tempDN->D_NUMBER = dataNum;
			temp->Sons.push_back(tempDN);

			temp->Father = info.ROOT;
			info.ROOT->Sons.push_back(temp);

			++i;
		}

		info.GetJsonToFile("RPi-1.json");
	}
}