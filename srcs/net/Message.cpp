#include <net/Message.h>

#include <cstring>

using namespace hap::net;

Message::Message(const char *rawData, size_t length)
{
	// Get HTTP request method
	size_t methodEnd = strchr(rawData, ' ') - rawData;

	_method = std::string(rawData, methodEnd);

	rawData += methodEnd + 1;

	// Get HTTP request path

	// Skip first slash of request path
	rawData++;

	size_t pathEnd = strchr(rawData, ' ') - rawData;

	_path = std::string(rawData, pathEnd);


	// Get content length
	size_t contentLength = strtoull(strstr(rawData, "Content-Length: ") + 16, NULL, 10);
	const char* typeBegin = strstr(rawData, "Content-Type: ") + 14;

	// Get content type
	size_t typeLength = strchr(typeBegin, '\r') - typeBegin;
	_contentType = std::string(typeBegin, typeLength);

	// Get content inside relative container
	_content = std::make_shared<MessageData>(std::string(strstr(rawData, "\r\n\r\n") + 4, contentLength));
}

Message::Message(const Message& copy)
{
	_method = copy._method;
	_path = copy._path;
	_contentType = copy._contentType;
	_content = std::make_shared<MessageData>(*copy._content);
}

Message::~Message()
{
}

const std::string &Message::getMethod() const
{
	return _method;
}

const std::string &Message::getPath() const
{
	return _path;
}

const std::string &Message::getContentType() const
{
	return _contentType;
}

ConstMessageData_ptr Message::getContent() const
{
	return _content;
}

std::string Message::getOriginalRequest()
{
	std::string request;
	request += _method;
	request += " ";
	request += _path;
	request += " HTTP/1.1\r\n";
	request += "Content-Type: ";
	request += _contentType;
	request += "\r\nContent-Length: ";

	std::string content = _content->rawData();

	request += std::to_string(content.length());
	request += "\r\n\r\n";
	request += content;

	return request;
}