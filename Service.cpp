#include "Service.h"

#include "Helpers.h"

using namespace hap;

Service::Service(service_type _uuid) : uuid(_uuid) 
{
}

short Service::numberOfCharacteristics() 
{ 
	return _characteristics.size();
}

Characteristics *Service::characteristicsAtIndex(int index) 
{ 
	return _characteristics[index]; 
}

std::string Service::describe(net::ConnectionInfo *sender) {
	std::string keys[3] = { "iid", "type", "characteristics" };
	std::string values[3];
	{
		char temp[8];
		snprintf(temp, 8, "%d", serviceID);
		values[0] = temp;
	}
	{
		char temp[8];
		snprintf(temp, 8, "\"%X\"", uuid);
		values[1] = temp;
	}
	{
		int no = numberOfCharacteristics();
		std::string *chars = new std::string[no];
		for (int i = 0; i < no; i++) {
			chars[i] = _characteristics[i]->describe(sender);
		}
		values[2] = arrayWrap(chars, no);
		delete[] chars;
	}
	return dictionaryWrap(keys, values, 3);
}