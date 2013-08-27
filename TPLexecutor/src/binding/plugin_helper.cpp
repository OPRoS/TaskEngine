/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2011 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#ifdef USE_PLUGIN
#include "rpf/rpf.h"
#include "binding/PluginBinder.h"

rpf::rpf_object_helper object_helper;

extern "C" PLUGIN_API
pf_exit_func pf_init_plugin(pf_platform_services params)
{
	object_helper.init(params);
	
	// keep in mind that the template parameter ("Hello") should be exactly the same with the class name
	object_helper.register_object<PluginBinder>("PluginBinder");

   return object_helper.get_result();
}

#endif //USE_PLUGIN