#include <rtm/CorbaNaming.h>
#include "SimulationExecutionContext.h"

namespace RTC
{
  ReturnCode_t SimulationExecutionContext::start() throw (CORBA::SystemException)
  {
    ReturnCode_t ret = OpenHRPExecutionContext::start();
    if (ret == RTC_OK){
      m_cg->subscribe(this->_this(), 1.0/get_rate());
    }
    return ret;
  }


  ReturnCode_t SimulationExecutionContext::stop() throw (CORBA::SystemException)
  {
    if (!m_running) return RTC::PRECONDITION_NOT_MET;

    m_cg->unsubscribe(this->_this());

    // stop thread
    m_running = false;

    // invoke on_shutdown for each comps.
    std::for_each(m_comps.begin(), m_comps.end(), invoke_on_shutdown());
    
    // change EC thread state
    m_state = false;
    
    return RTC::RTC_OK;
  }

  OpenHRP::ClockGenerator_var SimulationExecutionContext::m_cg;
};

void SimulationECInit(RTC::Manager* manager)
{
  RTC::Properties &props = manager->getConfig();
  RTC::CorbaNaming cn
    = RTC::CorbaNaming(manager->getORB(), props["corba.nameservers"].c_str());
  try{
    CORBA::Object_ptr obj = cn.resolve("ClockGenerator");
    RTC::SimulationExecutionContext::m_cg = OpenHRP::ClockGenerator::_narrow(obj);

    manager->registerECFactory("SimulationEC",
			       RTC::ECCreate<RTC::SimulationExecutionContext>,
			       RTC::ECDelete<RTC::OpenHRPExecutionContext>);
  }catch(RTC::CorbaNaming::NotFound& ex){
    std::cerr << "SimultationExecutionContext: can not find ClockGenerator"
	      << std::endl;
  }
}
