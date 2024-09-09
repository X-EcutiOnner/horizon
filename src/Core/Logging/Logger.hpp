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

#ifndef HORIZON_LOGGER_H
#define HORIZON_LOGGER_H

#include <cstring>
#include <atomic>
#include <mutex>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks.hpp>

class Logger
{
private:
    typedef boost::log::sources::severity_logger<boost::log::trivial::severity_level> logtype;

public:
	static Logger *getInstance()
	{
		static Logger instance;

        if (!instance._initialized)
            instance.initialize();
        
		return &instance;
	}

    void initialize();
    
    logtype &get_core_log() { return _core_log; }

	boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>> get_console_sink() { return _consoleSink; }
    
    void colored_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm);

    void shutdown();
protected:
    logtype _core_log;
    
    std::atomic<bool> _initialized;
	
    boost::shared_ptr<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>> _consoleSink;
    boost::shared_ptr<boost::log::sinks::asynchronous_sink<boost::log::sinks::text_file_backend>> _fileSink;
};

class HLogStream
{
public:
    HLogStream(boost::log::trivial::severity_level level)
        : level_(level) {}

    template <typename T>
    HLogStream& operator<<(const T& msg)
    {
        oss_ << msg;
        return *this;
    }

    // Overload for std::ostream manipulators like std::hex, std::endl
    HLogStream& operator<<(std::ostream& (*manip)(std::ostream&))
    {
        oss_ << manip;
        return *this;
    }

    // Overload for std::string
    HLogStream& operator<<(const std::string& msg)
    {
        oss_ << msg;
        return *this;
    }

    // Overload for const char*
    HLogStream& operator<<(const char* msg)
    {
        oss_ << msg;
        return *this;
    }

    ~HLogStream()
    {
		auto &lg = Logger().getInstance()->get_core_log();
		lg.lock();
        BOOST_LOG_SEV(lg, level_) << oss_.str();
		lg.unlock();
    }

private:
    boost::log::trivial::severity_level level_;
    std::ostringstream oss_;
};
#define HLog(type) HLogStream(boost::log::trivial::type)
#define HLogShutdown Logger().getInstance()->shutdown()
#endif //HORIZON_LOGGER_H
