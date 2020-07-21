#pragma once

enum ANK_BUFFER_USAGE
{
	DYNAMIC
};

enum ANK_MAP_TYPE
{
	DISCARD
};

enum ANK_ACCESS_FLAG
{
	READ,
	WRITE
};

class Buffer
{
public:
	Buffer();
	virtual ~Buffer();
private:

};