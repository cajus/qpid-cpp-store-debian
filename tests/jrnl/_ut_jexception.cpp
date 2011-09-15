/*
 * Copyright (c) 2007, 2008 Red Hat, Inc.
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

#include "../unit_test.h"

#include <cstring>
#include <iostream>
#include "jrnl/jerrno.hpp"
#include "jrnl/jexception.hpp"

using namespace boost::unit_test;
using namespace mrg::journal;
using namespace std;

QPID_AUTO_TEST_SUITE(jexception_suite)

const string test_filename("_ut_jexception");

// === Helper functions ===

void throw_exception(const jexception& e, std::size_t what_len, std::size_t ai_len,
        std::size_t tc_len, std::size_t tf_len)
{
    try { throw e; }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(std::strlen(e.what()), what_len);
        BOOST_CHECK_EQUAL(e.additional_info().size(), ai_len);
        BOOST_CHECK_EQUAL(e.throwing_class().size(), tc_len);
        BOOST_CHECK_EQUAL(e.throwing_fn().size(), tf_len);
    }
}

void throw_exception(const jexception& e, std::size_t what_len, std::size_t ai_len)
{
    throw_exception(e, what_len, ai_len, 0, 0);
}

void throw_exception(const jexception& e, std::size_t what_len, std::size_t tc_len,
        std::size_t tf_len)
{
    throw_exception(e, what_len, 0, tc_len, tf_len);
}

// === Test suite ===

QPID_AUTO_TEST_CASE(constructor_1)
{
    cout << test_filename << ".constructor_1: " << flush;
    try
    {
        jexception e1;
        BOOST_CHECK_EQUAL(e1.err_code(), (u_int32_t)0);
        BOOST_CHECK(e1.additional_info().size() == 0);
        BOOST_CHECK(e1.throwing_class().size() == 0);
        BOOST_CHECK(e1.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e1.what()) > 0);
        throw e1;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), (u_int32_t)0);
        BOOST_CHECK(e.additional_info().size() == 0);
        BOOST_CHECK(e.throwing_class().size() == 0);
        BOOST_CHECK(e.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_2)
{
    cout << test_filename << ".constructor_2: " << flush;
    const u_int32_t err_code = 2;
    try
    {
        jexception e2(err_code);
        BOOST_CHECK_EQUAL(e2.err_code(), err_code);
        BOOST_CHECK(e2.additional_info().size() == 0);
        BOOST_CHECK(e2.throwing_class().size() == 0);
        BOOST_CHECK(e2.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e2.what()) > 0);
        throw e2;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), err_code);
        BOOST_CHECK(e.additional_info().size() == 0);
        BOOST_CHECK(e.throwing_class().size() == 0);
        BOOST_CHECK(e.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_3a)
{
    cout << test_filename << ".constructor_3a: " << flush;
    const char* err_msg = "exception3";
    try
    {
        jexception e3(err_msg);
        BOOST_CHECK_EQUAL(e3.err_code(), (u_int32_t)0);
        BOOST_CHECK(e3.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e3.throwing_class().size() == 0);
        BOOST_CHECK(e3.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e3.what()) > 0);
        throw e3;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), (u_int32_t)0);
        BOOST_CHECK(e.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e.throwing_class().size() == 0);
        BOOST_CHECK(e.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_3b)
{
    cout << test_filename << ".constructor_3b: " << flush;
    const string err_msg("exception3");
    try
    {
        jexception e3(err_msg);
        BOOST_CHECK_EQUAL(e3.err_code(), (u_int32_t)0);
        BOOST_CHECK(e3.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e3.throwing_class().size() == 0);
        BOOST_CHECK(e3.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e3.what()) > 0);
        throw e3;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), (u_int32_t)0);
        BOOST_CHECK(e.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e.throwing_class().size() == 0);
        BOOST_CHECK(e.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_4a)
{
    cout << test_filename << ".constructor_4a: " << flush;
    const u_int32_t err_code = 4;
    const char* err_msg = "exception4";
    try
    {
        jexception e4(err_code, err_msg);
        BOOST_CHECK_EQUAL(e4.err_code(), err_code);
        BOOST_CHECK(e4.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e4.throwing_class().size() == 0);
        BOOST_CHECK(e4.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e4.what()) > 0);
        throw e4;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), err_code);
        BOOST_CHECK(e.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e.throwing_class().size() == 0);
        BOOST_CHECK(e.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_4b)
{
    cout << test_filename << ".constructor_4b: " << flush;
    const u_int32_t err_code = 4;
    const string err_msg("exception4");
    try
    {
        jexception e4(err_code, err_msg);
        BOOST_CHECK_EQUAL(e4.err_code(), err_code);
        BOOST_CHECK(e4.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e4.throwing_class().size() == 0);
        BOOST_CHECK(e4.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e4.what()) > 0);
        throw e4;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), err_code);
        BOOST_CHECK(e.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e.throwing_class().size() == 0);
        BOOST_CHECK(e.throwing_fn().size() == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_5a)
{
    cout << test_filename << ".constructor_5a: " << flush;
    const u_int32_t err_code = 5;
    const char* err_class = "class5";
    const char* err_fn = "fn5";
    try
    {
        jexception e5(err_code, err_class, err_fn);
        BOOST_CHECK_EQUAL(e5.err_code(), err_code);
        BOOST_CHECK(e5.additional_info().size() == 0);
        BOOST_CHECK(e5.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e5.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e5.what()) > 0);
        throw e5;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), err_code);
        BOOST_CHECK(e.additional_info().size() == 0);
        BOOST_CHECK(e.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_5b)
{
    cout << test_filename << ".constructor_5b: " << flush;
    const u_int32_t err_code = 5;
    const string err_class("class5");
    const string err_fn("fn5");
    try
    {
        jexception e5(err_code, err_class, err_fn);
        BOOST_CHECK_EQUAL(e5.err_code(), err_code);
        BOOST_CHECK(e5.additional_info().size() == 0);
        BOOST_CHECK(e5.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e5.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e5.what()) > 0);
        throw e5;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), err_code);
        BOOST_CHECK(e.additional_info().size() == 0);
        BOOST_CHECK(e.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_6a)
{
    cout << test_filename << ".constructor_6a: " << flush;
    const u_int32_t err_code = 6;
    const char* err_msg = "exception6";
    const char* err_class = "class6";
    const char* err_fn = "fn6";
    try
    {
        jexception e6(err_code, err_msg, err_class, err_fn);
        BOOST_CHECK_EQUAL(e6.err_code(), err_code);
        BOOST_CHECK(e6.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e6.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e6.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e6.what()) > 0);
        throw e6;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), err_code);
        BOOST_CHECK(e.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(constructor_6b)
{
    cout << test_filename << ".constructor_6b: " << flush;
    const u_int32_t err_code = 6;
    const string err_msg("exception6");
    const string err_class("class6");
    const string err_fn("fn6");
    try
    {
        jexception e6(err_code, err_msg, err_class, err_fn);
        BOOST_CHECK_EQUAL(e6.err_code(), err_code);
        BOOST_CHECK(e6.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e6.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e6.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e6.what()) > 0);
        throw e6;
    }
    catch (const jexception& e)
    {
        BOOST_CHECK_EQUAL(e.err_code(), err_code);
        BOOST_CHECK(e.additional_info().compare(err_msg) == 0);
        BOOST_CHECK(e.throwing_class().compare(err_class) == 0);
        BOOST_CHECK(e.throwing_fn().compare(err_fn) == 0);
        BOOST_CHECK(std::strlen(e.what()) > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_CASE(msg_scope)
{
    cout << test_filename << ".msg_scope: " << flush;
    try
    {
        // These will go out of scope as soon as jexception is thrown...
        const string msg("Error message");
        const string cls("class");
        const string fn("function");
        throw jexception(100, msg, cls, fn);
    }
    catch (const jexception& e)
    {
        stringstream ss;
        ss << e;
        BOOST_CHECK(ss.str().size() > 0);
    }
    cout << "ok" << endl;
}

QPID_AUTO_TEST_SUITE_END()
