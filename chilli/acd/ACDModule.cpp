#include "ACDModule.h"
#include "ACDDevice.h"
#include <log4cplus/loggingmacros.h>
#include "../tinyxml2/tinyxml2.h"
#include <json/config.h>
#include <json/json.h>


namespace chilli{
namespace ACD{

ACDModule::ACDModule(const std::string & id):ProcessModule(id)
{
	log =log4cplus::Logger::getInstance("chilli.ACDModule");
	log.setAppendName("." + this->getId());
	LOG4CPLUS_DEBUG(log, " Constuction a ACD module.");
}


ACDModule::~ACDModule(void)
{
	LOG4CPLUS_DEBUG(log, " Destruction a ACD module.");
}


bool ACDModule::LoadConfig(const std::string & configContext)
{
	using namespace tinyxml2;
	tinyxml2::XMLDocument config;
	if(config.Parse(configContext.c_str()) != XMLError::XML_SUCCESS) 
	{ 
		LOG4CPLUS_ERROR(log, " load config error:" << config.ErrorName() << ":" << config.GetErrorStr1());
		return false;
	}
	
	for (XMLElement *child = config.FirstChildElement("Device");
		child != nullptr;
		child = child->NextSiblingElement("Device"))
	{
		const char * num = child->Attribute("DeviceID");
		const char * sm = child->Attribute("StateMachine");
		num = num ? num : "";
		sm = sm ? sm : "";

		model::PerformElementPtr ext(new ACDDevice(this, num, sm));
		if (ext != nullptr && addPerformElement(num,ext)){
			ext->setVar("_extension.deviceID", num);
		}
		else {
			LOG4CPLUS_ERROR(log, " alredy had device:" << num);
		}
	}
	
	return true;
}


void ACDModule::fireSend(const std::string & strContent, const void * param)
{
	LOG4CPLUS_WARN(log, " fireSend not implement.");
}

}
}
