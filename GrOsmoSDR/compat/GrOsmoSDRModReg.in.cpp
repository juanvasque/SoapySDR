/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <SoapySDR/Registry.hpp>
#include "GrOsmoSDRInterface.hpp"
#include "arg_helpers.h"
@LOCAL_INCLUDES@

static std::vector<SoapySDR::Kwargs> find__@TARGET@(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;

    //call the get_devices() on the source or sink, but not both
    #if @HAS_SOURCE@
    std::vector<std::string> source_results = @TARGET@_source_c::get_devices();
    for (size_t i = 0; i < source_results.size(); i++)
    {
        results.push_back(params_to_dict(source_results[i]));
    }
    #elif @HAS_SINK@
    std::vector<std::string> sink_results = @TARGET@_sink_c::get_devices();
    for (size_t i = 0; i < sink_results.size(); i++)
    {
        results.push_back(params_to_dict(sink_results[i]));
    }
    #endif

    return results;
}

static SoapySDR::Device *make__@TARGET@(const SoapySDR::Kwargs &args)
{
    //convert args to param string
    std::string params;
    for (SoapySDR::Kwargs::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        if (not params.empty()) params += ",";
        params += it->first + "=" + it->second;
    }

    //shared pointer to interfaces
    boost::shared_ptr<source_iface> source;
    boost::shared_ptr<sink_iface> sink;

    //call factories when they exist
    #if @HAS_SOURCE@
    source = make_@TARGET@_source_c(params);
    #endif
    #if @HAS_SINK@
    sink = make_@TARGET@_sink_c(params);
    #endif

    //return a new interface wrapper
    return new GrOsmoSDRInterface(source, sink);
}

static SoapySDR::Registry register__@TARGET@("@TARGET@", &find__@TARGET@, &make__@TARGET@);
