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
 * @file        Mac.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#define XSTEST_GTEST_COMPAT
#include <XSTest/XSTest.hpp>

#define DLIB_S( _s_ ) _s_

#include <dlib.hpp>
#include <mach/mach.h>

extern kern_return_t IOMasterPort( mach_port_t bootstrapPort, mach_port_t * masterPort );
extern kern_return_t IOMasterPortInvalid( mach_port_t bootstrapPort, mach_port_t * masterPort );

DLIB_FUNC_START( IOKit, kern_return_t, IOMasterPort, mach_port_t bootstrapPort, mach_port_t * masterPort )
DLIB_FUNC_RET(   IOKit, kern_return_t, IOMasterPort, bootstrapPort, masterPort )

DLIB_FUNC_START( IOKit, kern_return_t, IOMasterPortInvalid, mach_port_t bootstrapPort, mach_port_t * masterPort )
DLIB_FUNC_RET(   IOKit, kern_return_t, IOMasterPortInvalid, bootstrapPort, masterPort )

TEST( dlib, GetModule )
{
    dlib::Manager  manager;
    dlib::Module * mod1( manager.GetModule( "IOKit" ) );
    dlib::Module * mod2( manager.GetModule( "FooBar" ) );
    
    manager.AddSearchPath( "/System/Library/Frameworks/" );
    
    {
        dlib::Module * mod3( manager.GetModule( "IOKit" ) );
        
        ASSERT_EQ( mod1, nullptr );
        ASSERT_EQ( mod2, nullptr );
        ASSERT_NE( mod3, nullptr );
    }
}

TEST( dlib, GetSymbolAddress )
{
    dlib::Manager manager;
    
    manager.AddSearchPath( "/System/Library/Frameworks/" );
    
    {
        dlib::Module * mod( manager.GetModule( "IOKit" ) );
        
        ASSERT_NE( mod, nullptr );
        ASSERT_NE( mod->GetSymbolAddress( "IOMasterPort"  ),       nullptr );
        ASSERT_EQ( mod->GetSymbolAddress( "IOMasterPortInvalid" ), nullptr );
    }
}

TEST( dlib, Call )
{
    kern_return_t r1( 1 );
    kern_return_t r2( 1 );
    mach_port_t   p1( 0 );
    mach_port_t   p2( 0 );
    
    dlib::Manager::SharedInstance().AddSearchPath( "/System/Library/Frameworks/" );
    
    ASSERT_NO_THROW( r1 = IOMasterPort(        MACH_PORT_NULL, &p1 ) );
    ASSERT_NO_THROW( r2 = IOMasterPortInvalid( MACH_PORT_NULL, &p2 ) );
    
    ASSERT_TRUE( r1 == 0 );
    ASSERT_TRUE( r2 == 0 );
    
    ASSERT_TRUE( p1 != 0 );
    ASSERT_TRUE( p2 == 0 );
}
