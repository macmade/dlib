/*******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2017 Jean-David Gadina - www.xs-labs.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @file        Tests.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#ifdef _WIN32
#include <gtest/gtest.h>
#else
#include <GoogleMock/GoogleMock.h>
#endif

#define DLIB_S( _s_ ) _s_

#include <dlib.hpp>

extern int hv_vcpu_run(  unsigned int cpu );
extern int hv_vm_foobar( uint64_t flags );

DLIB_FUNC_START( Hypervisor, int, hv_vcpu_run, unsigned int cpu )
DLIB_FUNC_RET(   Hypervisor, int, hv_vcpu_run, cpu )

DLIB_FUNC_START( Hypervisor, int, hv_vm_foobar, uint64_t flags )
DLIB_FUNC_RET(   Hypervisor, int, hv_vm_foobar, flags )

TEST( dlib, GetModule )
{
    dlib::Module * m1( dlib::Manager::SharedInstance().GetModule( "Hypervisor" ) );
    dlib::Module * m2( dlib::Manager::SharedInstance().GetModule( "FooBar" ) );
    
    dlib::Manager::SharedInstance().AddSearchPath( "/System/Library/Frameworks/" );
    
    {
        dlib::Module * m3( dlib::Manager::SharedInstance().GetModule( "Hypervisor" ) );
        
        ASSERT_EQ( m1, nullptr );
        ASSERT_EQ( m2, nullptr );
        ASSERT_NE( m3, nullptr );
    }
}

TEST( dlib, GetSymbolAddress )
{
    dlib::Manager::SharedInstance().AddSearchPath( "/System/Library/Frameworks/" );
    
    {
        dlib::Module * m( dlib::Manager::SharedInstance().GetModule( "Hypervisor" ) );
        
        ASSERT_NE( m, nullptr );
        ASSERT_NE( m->GetSymbolAddress( "hv_vcpu_run"  ), nullptr );
        ASSERT_EQ( m->GetSymbolAddress( "hv_vm_foobar" ), nullptr );
    }
}

TEST( dlib, Call )
{
    int r1( 0 );
    int r2( 1 );
    
    dlib::Manager::SharedInstance().AddSearchPath( "/System/Library/Frameworks/" );
    
    ASSERT_NO_THROW( r1 = hv_vcpu_run( 0 ) );
    ASSERT_NO_THROW( r2 = hv_vm_foobar( 0 ) );
    
    ASSERT_TRUE( r1 != 0 );
    ASSERT_TRUE( r2 == 0 );
}

