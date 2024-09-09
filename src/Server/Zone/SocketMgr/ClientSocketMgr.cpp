#include "ClientSocketMgr.hpp"
#include "Server/Zone/Zone.hpp"

Horizon::Zone::ZoneNetworkThread::ZoneNetworkThread()
: KernelComponent(sZone, Horizon::System::RUNTIME_NETWORKING),
_resource_manager(PrimaryResource(RESOURCE_PRIORITY_PRIMARY, std::make_shared<s_segment_storage<uint64_t, std::shared_ptr<ZoneSocket>>>())) 
{
}

bool Horizon::Zone::ClientSocketMgr::start(boost::asio::io_context &io_context, std::string const &listen_ip, uint16_t port, uint32_t threads, bool minimal)
{
	if (!BaseSocketMgr::start(io_context, listen_ip, port, threads, minimal))
		return false;
	
	for (auto i : get_thread_map()) {
		sZone->register_component(Horizon::System::RUNTIME_NETWORKING, (std::dynamic_pointer_cast<ZoneNetworkThread>(i.second)));
	}

	return true;
}

bool Horizon::Zone::ClientSocketMgr::stop()
{
	get_sockets().clear();
	
	for (auto i = get_thread_map().begin(); i != get_thread_map().end(); i++)
		sZone->deregister_component(Horizon::System::RUNTIME_NETWORKING, (std::static_pointer_cast<ZoneNetworkThread>(i->second))->get_segment_number());

	if (!BaseSocketMgr::stop_network())
		return false;

	return true;
}