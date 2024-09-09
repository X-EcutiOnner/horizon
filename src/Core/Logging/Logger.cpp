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

#include "Logger.hpp"
#include <iostream>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/core/null_deleter.hpp>

static inline std::string color(uint16_t color) { return "\033[" + std::to_string(color) + "m"; }
 
void Logger::colored_formatter(boost::log::record_view const& rec, boost::log::formatting_ostream& strm)
{
    static auto date_time_formatter = boost::log::expressions::stream << boost::log::expressions::format_date_time<boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");

    strm << "[";
    date_time_formatter(rec, strm);
    strm << "] [";

    auto severity = rec[boost::log::trivial::severity];

    switch (*severity) {
        case boost::log::trivial::trace:
        case boost::log::trivial::debug:
            strm << color(90);
        break;
        case boost::log::trivial::info:
            strm << color(36);
        break;
        case boost::log::trivial::warning:
            strm << color(33);
        break;
        case boost::log::trivial::error:
        case boost::log::trivial::fatal:
            strm << color(31);
        break;
        default:
        break;
    }

    strm << severity << color(0) << "]  " << rec[boost::log::expressions::message];
}

void Logger::initialize()
{   
    boost::log::core::get()->add_global_attribute("Scope",
        boost::log::attributes::named_scope());
    
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::trace
    );

    /* log formatter:
     * [TimeStamp] [Severity Level] Log message
     */
    auto fmtTimeStamp = boost::log::expressions::
        format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f");
    
    auto fmtSeverity = boost::log::expressions::
        attr<boost::log::trivial::severity_level>("Severity");
    
    auto fmtScope = boost::log::expressions::format_named_scope("Scope",
        boost::log::keywords::format = "%n(%f:%l)",
        boost::log::keywords::iteration = boost::log::expressions::reverse,
        boost::log::keywords::depth = 2);
    
    boost::log::formatter logFmt =
        boost::log::expressions::format("[%1%] (%2%) %3%")
        % fmtTimeStamp % fmtSeverity
        % boost::log::expressions::smessage;

    /* console sink */
    _consoleSink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>();
    boost::log::core::get()->add_sink(_consoleSink);
    
    boost::shared_ptr<std::ostream> stream{&std::clog, boost::null_deleter{}};
    _consoleSink->locked_backend()->add_stream(stream);
    
    _consoleSink->set_formatter(std::bind(&Logger::colored_formatter, this, std::placeholders::_1, std::placeholders::_2));

    /* fs sink */
    _fileSink = boost::make_shared<boost::log::sinks::asynchronous_sink<boost::log::sinks::text_file_backend>>(
        boost::log::keywords::target = "logs",
        boost::log::keywords::file_name = "logs/log_%Y-%m-%d_%H-%M-%S.%N.log",
        boost::log::keywords::rotation_size = 10 * 1024 * 1024,
        boost::log::keywords::min_free_space = 30 * 1024 * 1024,
        boost::log::keywords::open_mode = std::ios_base::app,
		boost::log::keywords::start_thread = false);
    
    _fileSink->set_formatter(logFmt);
    
    _fileSink->locked_backend()->auto_flush(true);

    // Add the file sink to the core
    boost::log::core::get()->add_sink(_fileSink);

    // Add common attributes
    boost::log::add_common_attributes();

    _initialized.store(true);
}

void Logger::shutdown() {
    // Remove and flush the console sink
    if (_consoleSink) {
		_consoleSink->flush();
        boost::log::core::get()->remove_sink(_consoleSink);
        _consoleSink.reset();
    }

    // Remove and flush the file sink
    if (_fileSink) {
		_fileSink->flush();
        boost::log::core::get()->remove_sink(_fileSink);
        _fileSink.reset();
    }
}