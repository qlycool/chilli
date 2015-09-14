#pragma once

#ifndef _CHILLI_CTI_DEVICE_MODULE_HEADER_
#define _CHILLI_CTI_DEVICE_MODULE_HEADER_

#include "..\model\ProcessModule.h"
#include "..\CEventBuffer.h"
#include <log4cplus\logger.h>

namespace chilli{
namespace model{


class DevModule :
	public ProcessModule
{
public:
	DevModule(void);
	virtual ~DevModule(void);
	
private:

	//Only define a copy constructor and assignment function, these two functions can be disabled
	DevModule(const DevModule & other);
	DevModule & operator=(const DevModule &);

	log4cplus::Logger log;
public:
	static void addEventToBuffer(const std::string & strContent);
	//State machine executer, one instance per thread
	static helper::CEventBuffer<const std::string> recEvtBuffer;

};
}
}
#endif //end dev module header