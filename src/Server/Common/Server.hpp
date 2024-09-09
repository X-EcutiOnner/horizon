/***************************************************
 *       _   _            _                        *
 *      | | | |          (_)                       *
 *      | |_| | ___  _ __ _ _______  _ __          *
 *      |  _  |/ _ \| '__| |_  / _ \| '_  \        *
 *      | | | | (_) | |  | |/ / (_) | | | |        *
 *      \_| |_/\___/|_|  |_/___\___/|_| |_|        *
 ***************************************************
 * This file is part of Horizon (c).
 *
 * Copyright (c) 2019 Sagun K. (sagunxp@gmail.com).
 * Copyright (c) 2019 Horizon Dev Team.
 *
 * Base Author - Sagun K. (sagunxp@gmail.com)
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 **************************************************/

#ifndef HORIZON_SERVER_HPP
#define HORIZON_SERVER_HPP

#include "Core/Logging/Logger.hpp"
#include "Core/Multithreading/LockedLookupTable.hpp"

#include "System.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <queue>

#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/tcp_ssl.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>

#include <boost/chrono.hpp>

#include "Server/Common/Configuration/ServerConfiguration.hpp"

#include <sol/sol.hpp>
#include <thread>

#define TERMINAL_STR "Horizon $> "

enum shutdown_stages
{
	SHUTDOWN_NOT_STARTED      = 0,
	SHUTDOWN_INITIATED        = 1,
	SHUTDOWN_CLEANUP_COMPLETE = 2,
	SHUTDOWN_COMPLETE         = 3
};

extern std::atomic<shutdown_stages> _shutdown_stage;
extern std::atomic<int> _shutdown_signal;

extern inline void set_shutdown_signal(int signal) { _shutdown_signal.exchange(signal); }
extern inline shutdown_stages get_shutdown_stage() { return _shutdown_stage.load(); };
extern inline void set_shutdown_stage(shutdown_stages new_stage) { _shutdown_stage.exchange(new_stage); };

enum kernel_resource_priority_type
{
	RESOURCE_PRIORITY_PRIMARY = 0,
	RESOURCE_PRIORITY_SECONDARY = 1,
	RESOURCE_PRIORITY_TERTIARY = 2,
	RESOURCE_PRIORITY_QUATERNARY = 3,
	RESOURCE_PRIORITY_QUINARY = 4,
	RESOURCE_PRIORITY_SENARY = 5,
	RESOURCE_PRIORITY_SEPTENARY = 6,
	RESOURCE_PRIORITY_OCTONARY = 7,
	RESOURCE_PRIORITY_NONARY = 8,
	RESOURCE_PRIORITY_DENARY = 9,
	MAX_KERNEL_SEGMENT_PRIORITIES = 10
};

template <typename Key, typename Value>
struct s_segment_storage {
	using key_type = Key;
	using value_type = Value;
	LockedLookupTable<Key, Value> _table;
	s_segment_storage() { }
	~s_segment_storage() { }
	// add value
	void add(Key key, Value value) { _table.insert(key, value); }
	// remove value
	void remove(Key key) { _table.erase(key); }
	// get value
	Value get(Key key, Value const &default_value = Value()) { return _table.at(key, default_value); }
	// get size
	int size() { return _table.size(); }
	// clear
	void clear() { _table.clear(); }
};

template <class Storage>
class SharedPriorityResourceMedium
{
public:
	SharedPriorityResourceMedium(int priority, std::shared_ptr<Storage> storage) : _priority(priority), _storage(storage) { }
	
	int64_t get_priority() { return _priority; }
	Storage get_storage() { return _storage; }
	// add
	void add(typename Storage::key_type key, typename Storage::value_type value) { _storage->add(key, value); }
	// remove
	void remove(typename Storage::key_type key) { _storage->remove(key); }
	// get
	typename Storage::value_type get(typename Storage::key_type key, typename Storage::value_type default_value = typename Storage::value_type()) { return _storage->get(key, default_value); }
	// size
	int size() { return _storage->size(); }
	// clear
	void clear() { _storage->clear(); }

	std::map<typename Storage::key_type, typename Storage::value_type> get_map() { return _storage->_table.get_map(); }

protected:
	int64_t _priority;
	std::shared_ptr<Storage> _storage;
};

template <typename... SharedPriorityResourceMediums>
class SharedPriorityResourceManager {
public:
	SharedPriorityResourceManager(SharedPriorityResourceMediums... mediums) : _resources(mediums...) { }

	// copy constructor
	SharedPriorityResourceManager(const SharedPriorityResourceManager &other) : _resources(std::move(other._resources)) { }
	// move constructor
	SharedPriorityResourceManager(SharedPriorityResourceManager &&other) : _resources(std::move(other._resources)) { }
	// copy assignment
	SharedPriorityResourceManager &operator=(const SharedPriorityResourceManager &other) { _resources = other._resources; return *this; }
	// move assignment
	SharedPriorityResourceManager &operator=(SharedPriorityResourceManager &&other) { _resources = std::move(other._resources); return *this; }
	
    template <std::size_t Priority>
    auto& get_medium() {
        return std::get<Priority>(_resources);
    }

	template <std::size_t Index, typename Key, typename Value>
	void add(Key key, Value value) 
	{
		std::get<Index>(_resources).add(key, value);
	}

	template <std::size_t Index, typename Key>
	void remove(Key key) 
	{
		std::get<Index>(_resources).remove(key);
	}

	template <std::size_t Index, typename Key, typename Value>
	Value get_resource(Key key, Value const &default_value = Value()) 
	{
		return std::get<Index>(_resources).get(key, default_value);
	}

	template <std::size_t Index>
	int size() 
	{
		return std::get<Index>(_resources).size();
	}

	template <std::size_t Index>
	void clear() 
	{
		std::get<Index>(_resources).clear();
	}

private:
    std::tuple<SharedPriorityResourceMediums...> _resources;
};

class Kernel;
class KernelComponent : public std::enable_shared_from_this<KernelComponent>
{
public:
	KernelComponent(Kernel *kernel, Horizon::System::runtime_module_type module_type) 
	: _kernel(kernel), _module_type(module_type), _hsr_manager(module_type), _uuid(boost::uuids::random_generator()()) 
	{ }
	virtual void initialize(int segment_number = 1) 
	{
		set_segment_number(segment_number);
	}
	virtual void finalize() { }

	virtual bool is_initialized() { return false; }
	virtual bool is_finalized() { return false; }

	void set_segment_number(int64_t segment_number) { _segment_number = segment_number; }
	int64_t get_segment_number() { return _segment_number.load(); }

	void system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContext> context);
	void system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContextChain> context);
	void system_routine_process_queue();
	void system_routine_register(Horizon::System::runtime_module_type module_t, Horizon::System::runtime_synchronization_method sync_t, std::shared_ptr<Horizon::System::RuntimeContext> context);

	const std::string get_uuid_string() { return boost::uuids::to_string(_uuid); }

	const std::string get_type_string() 
	{ 
		switch (_module_type)
		{
			case Horizon::System::RUNTIME_MAIN: return "Main";
			case Horizon::System::RUNTIME_GAMELOGIC: return "Game-logic";
			case Horizon::System::RUNTIME_PERSISTENCE: return "Persistence";
			case Horizon::System::RUNTIME_DATABASE: return "Database";
			case Horizon::System::RUNTIME_COMMANDLINE: return "Command-Line";
			case Horizon::System::RUNTIME_SCRIPTVM: return "Script-VM";
			case Horizon::System::RUNTIME_NETWORKING: return "Networking";
			case Horizon::System::RUNTIME_RUNTIME: return "Runtime";
			case Horizon::System::RUNTIME_CLIENT_NETWORKING: return "Client-Networking"; 
			case Horizon::System::RUNTIME_HTTP_SERVICE: return "Http Service";
			case Horizon::System::RUNTIME_WEB_SOCKET: return "Web Socket Service";
			default: return "Unknown";
		}
	}
	
	Horizon::System::SystemRoutineManager &get_system_routine_manager() { return _hsr_manager; }

	Kernel *get_kernel() { return _kernel; }

	void set_thread_cpu_id(int cpu_id) { _thread_cpu_id.exchange(cpu_id); }
	int get_thread_cpu_id() { return _thread_cpu_id.load(); }

	void set_thread_update_rate(double rate) { _thread_update_rate.exchange(rate); }
	double get_thread_update_rate() { return _thread_update_rate.load(); }
	
	void set_total_execution_time(int time) 
	{
		_total_execution_time_aggregate += time;

		if (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _last_total_execution_time_update).count() >= 1) {
			_last_total_execution_time_update = std::chrono::steady_clock::now();
			_total_execution_time_average.exchange(_total_execution_time_aggregate / get_thread_update_rate());
			_total_execution_time_aggregate = 0;
		}
	}
	int get_total_execution_time() { return _total_execution_time_average.load(); }

	void calculate_and_set_cpu_load()
	{
		_update_count++;

        // Update thread update rate every second to determine the number of updates per second
        auto current_time = std::chrono::steady_clock::now();
        auto diff_time = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time - _last_thread_update_rate_time).count();
        if (diff_time >= 1e9) {
            double update_rate = _update_count / (diff_time / 1e9);
            set_thread_update_rate(update_rate);
            _update_count = 0;
            _last_thread_update_rate_time = current_time;
		}
	}

private:
	std::atomic<int64_t> _segment_number{0};
	Horizon::System::runtime_module_type _module_type{Horizon::System::RUNTIME_MAIN};
	Horizon::System::SystemRoutineManager _hsr_manager;
	boost::uuids::uuid _uuid;

	std::atomic<int> _thread_cpu_id{0};
	Kernel *_kernel{nullptr};
	int _update_count{0};
	std::chrono::steady_clock::time_point _last_thread_update_rate_time;
	std::chrono::steady_clock::time_point _last_total_execution_time_update;
	std::atomic<double> _thread_update_rate{0.0};
	int _total_execution_time_aggregate{0};
	std::atomic<int> _total_execution_time_average{0};
};

class CLICommand
{
public:
	typedef std::function<void(CLICommand, bool)> FinishFunc;

	CLICommand() { };
	
	CLICommand(char *command, FinishFunc finish_func)
	: m_command(command), m_finish_func(finish_func)
	{
		//
	}

	~CLICommand()
	{
	}

	CLICommand(CLICommand &command)
	{
		m_command = command.m_command;
		m_finish_func = command.m_finish_func;
	}
	
	CLICommand(CLICommand &&command)
	{
		m_command = command.m_command;
		m_finish_func = command.m_finish_func;
	}

	CLICommand operator=(CLICommand &command)
	{
		return CLICommand(command);
	}

	std::string m_command;            ///< Command string.
	FinishFunc m_finish_func;         ///< Completion handler function.
};

class CommandLineProcess : public KernelComponent
{
public:
	CommandLineProcess(Kernel *kernel) : KernelComponent(kernel, Horizon::System::RUNTIME_COMMANDLINE) { }
	void process();
	void queue(CLICommand &&cmdMgr) { _cli_cmd_queue.push(std::move(cmdMgr)); }
	void add_function(std::string cmd, std::function<bool(std::string)> func) { _cli_function_map.insert(std::make_pair(cmd, func)); };

	/* CLI Function getter */
	std::function<bool(std::string)> find(std::string &cmd)
	{
		auto it = _cli_function_map.find(cmd);
		return (it != _cli_function_map.end()) ? it->second : nullptr;
	}

	void initialize(int segment_number = 1) override;
	void finalize() override;

	void command_complete(CLICommand /*cmd*/, bool /*success*/)
	{
		fflush(stdout);
	}

	void cli_thread_start();

	/**
	 * CLI Commands
	 */
	bool clicmd_shutdown(std::string /*cmd*/);
	bool clicmd_kernel_info(std::string /*cmd*/);

	bool is_initialized() override { return _is_initialized; }
	bool is_finalized() override { return _is_finalized; }

private:
	std::unordered_map<std::string, std::function<bool(std::string)>> _cli_function_map;
	// CLI command holder to be thread safe
	std::queue<CLICommand> _cli_cmd_queue;
	std::thread _cli_thread;
	std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};
	std::atomic<bool> _is_running_command{false};
};

class DatabaseProcess : public KernelComponent
{
public:
	DatabaseProcess() : KernelComponent(nullptr, Horizon::System::RUNTIME_DATABASE) { }
	// KernelComponent dispatch module type is set to Main because DatabaseProcess doesn't run on its own thread.
	DatabaseProcess(Kernel *kernel) : KernelComponent(kernel, Horizon::System::RUNTIME_DATABASE) { }
	~DatabaseProcess() 
	{ 
		_connection.reset();
		_ssl_ctx.reset();
	}
	
	void initialize(int segment_number = 1) override 
	{
		set_segment_number(segment_number);
		HLog(error) << "Database not configured"; 
		_is_initialized.exchange(true);
	}

	void initialize(boost::asio::io_context &io_context, int segment_number, std::string host, int port, std::string user, std::string pass, std::string database);

	void finalize() override 
	{
		// Close connection object.
		if (_connection != nullptr) {
			_connection->close();
		}
		
		_is_finalized.exchange(true);
	}

	std::shared_ptr<boost::mysql::tcp_ssl_connection> get_connection() { return _connection; }

	bool is_initialized() override { return _is_initialized.load(); }
	bool is_finalized() override { return _is_finalized.load(); }

protected:
	std::shared_ptr<boost::asio::ssl::context> _ssl_ctx{nullptr};
    std::shared_ptr<boost::mysql::tcp_ssl_connection> _connection{nullptr};
	std::atomic<bool> _is_initialized{false};
	std::atomic<bool> _is_finalized{false};
};

struct kernel_component_state_holder
{
	~kernel_component_state_holder() { ptr.reset(); }
	Horizon::System::runtime_module_type type;
	std::shared_ptr<KernelComponent> ptr;
	int segment_number;
};

typedef std::map<std::string, kernel_component_state_holder> KernelComponents;

class Kernel
{
public:
	Kernel(general_server_configuration &config);
	~Kernel();

	virtual void initialize() = 0; //< Kernel initialization routine
	virtual void finalize() = 0;   //< Kernel finalization routine
	virtual void post_initialize() = 0; //< Post initialization routine
	virtual void post_finalize() = 0; //< Post finalization routine

	struct general_server_configuration &general_conf() { return this->_config; }

	template <typename T>
	std::shared_ptr<T> get_component(std::string uuid) 
	{ 
		auto it = _components.find(uuid);
		if (it == _components.end()) 
			return nullptr;

		return std::static_pointer_cast<T>(it->second.ptr);
	}

	template <typename T>
	std::shared_ptr<T> get_component_of_type(Horizon::System::runtime_module_type type, int segment_number = 1)
	{
		for (auto c : _components) {
			if (c.second.type == type && c.second.segment_number == segment_number) {
				return std::static_pointer_cast<T>(c.second.ptr);
			}
		}

		return nullptr;
	}

	template <typename T>
	void register_component(Horizon::System::runtime_module_type type, T &&component) { 
		kernel_component_state_holder holder;
		holder.segment_number = get_registered_component_count_of_type(type) + 1;
		holder.type = type;
		holder.ptr = std::make_shared<T>(std::move(component));
		_components.insert(std::pair(component.get_uuid_string(), holder)); 
	}

	template <typename T>
	void register_component(Horizon::System::runtime_module_type type, std::shared_ptr<T> component) 
	{
		kernel_component_state_holder holder;
		holder.segment_number = get_registered_component_count_of_type(type) + 1;
		holder.type = type;
		holder.ptr = std::static_pointer_cast<KernelComponent>(component);
		_components.insert(std::pair(component->get_uuid_string(), holder)); 
	}

	void deregister_component(Horizon::System::runtime_module_type type, int segment_number = 1)
	{
		for (auto c : _components) {
			if (c.second.type == type && c.second.segment_number == segment_number) {
				_components.erase(c.first);
				break;
			}
		}
	}

	int get_registered_component_count_of_type(Horizon::System::runtime_module_type type)
	{
		int count = 0;
		for (auto c : _components) {
			if (c.second.type == type)
				count++;
		}
		return count;
	}
	
	template <typename ComponentType, std::size_t Priority, typename Key, typename Value>
	int get_segment_number_for_resource(Horizon::System::runtime_module_type module_t, Key resource_key, Value resource_not_found_value)
	{
		
		for (int i = 0; i < get_registered_component_count_of_type(module_t); i++) {
			auto component = get_component_of_type<ComponentType>(module_t, i + 1);
			if (component->get_resource_manager().template get_resource<Priority, Key, Value>(resource_key, resource_not_found_value) != resource_not_found_value)
				return i + 1;
		}

		return 0;
	}

	int get_component_count() { return _components.size(); }

	KernelComponents &get_components() { return _components; }

	void system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContext> context);
	void system_routine_queue_push(std::shared_ptr<Horizon::System::RuntimeContextChain> context);
	void system_routine_process_queue();
	void system_routine_register(Horizon::System::runtime_module_type module_t, Horizon::System::runtime_synchronization_method sync_t, std::shared_ptr<Horizon::System::RuntimeContext> context);

	Horizon::System::SystemRoutineManager &get_system_routine_manager() { return _hsr_manager; }

	/* Core I/O Service*/
	boost::asio::io_context &get_io_context();
	
	void set_signal_interrupt_command_line_loop(bool signal) { _signal_interrupt_command_line_loop.exchange(signal); }
	bool get_signal_interrupt_command_line_loop() { return _signal_interrupt_command_line_loop.load(); }
	
protected:
	boost::asio::io_context _io_context_global;
	KernelComponents _components;
	general_server_configuration _config;
private:
	Horizon::System::SystemRoutineManager _hsr_manager;

	std::atomic<bool> _signal_interrupt_command_line_loop{false};
};

class Server : public Kernel
{
public:
	Server();
	~Server();

	void parse_exec_args(const char *argv[], int argc);

	virtual void initialize();
	virtual void finalize();
	
	virtual void post_initialize();
	virtual void post_finalize();
	
	void print_help();

	/* General Configuration */
	struct general_server_configuration &general_conf() { return this->general_config; }
	/* Common Configuration */
	bool parse_common_configs(sol::table &cfg);

	std::shared_ptr<boost::mysql::tcp_ssl_connection> get_database_connection() 
	{ 
		return get_component_of_type<DatabaseProcess>(Horizon::System::RUNTIME_DATABASE)->get_connection();
	}

	bool test_database_connection();

protected:
	/* General Configuration */
	struct general_server_configuration general_config;

};

#endif /* HORIZON_SERVER_HPP */
