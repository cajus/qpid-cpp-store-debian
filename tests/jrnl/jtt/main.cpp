/*
 * Copyright (c) 2008 Red Hat, Inc.
 *
 * This file is part of the Qpid async store library msgstore.so.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 *
 * The GNU Lesser General Public License is available in the file COPYING.
 */

#include "test_mgr.hpp"

#include "args.hpp"
#include <csignal>
#include <iostream>

#define PACKAGE_NAME "Journal Test Tool"
#define VERSION "0.1"

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    std::signal(SIGINT, mrg::jtt::test_mgr::signal_handler);
    std::signal(SIGTERM, mrg::jtt::test_mgr::signal_handler);

    std::cout << PACKAGE_NAME << " v." << VERSION << std::endl;

    std::ostringstream oss;
    oss << PACKAGE_NAME << " options";
    mrg::jtt::args args(oss.str());
    if (args.parse(argc, argv)) return 1;

    try
    {
        mrg::jtt::test_mgr tm(args);
        tm.run();
        if (tm.error()) return 2; // One or more tests threw exceptions
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 3;
    }
    return 0;
}
