#include <net/Response.h>

using namespace hap::net;

Response::Response(uint16_t responseCode, ConstMessageData_ptr content) 
	: _responseCode(responseCode), _content(content)
{
}

Response::Response(const Response& copy) : _responseCode(copy._responseCode)
{
	_content = std::make_shared<const MessageData>(*copy._content);
}

std::string Response::getResponse()
{
	std::string content = _content->rawData();

	std::string response;
	response += "HTTP/1.1 ";
	response += std::to_string(_responseCode);
	if (_responseCode == 200)
		response += " OK";
	response += "\r\nContent-Type: application/pairing+tlv8\r\n"
		"Content-Length: ";
	response += std::to_string(content.length());
	response += "\r\n\r\n";
	response += content;

	return response;
}