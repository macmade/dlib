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

extern void * AMRestoreCreateDefaultOptions( void * allocator );
extern void * AMRestoreCreateDefaultOptions_Invalid( void * allocator );

DLIB_FUNC_START( MobileDevice, void *, AMRestoreCreateDefaultOptions, void * allocator )
DLIB_FUNC_RET(   MobileDevice, void *, AMRestoreCreateDefaultOptions, allocator )

DLIB_FUNC_START( MobileDevice, void *, AMRestoreCreateDefaultOptions_Invalid, void * allocator )
DLIB_FUNC_RET(   MobileDevice, void *, AMRestoreCreateDefaultOptions_Invalid, allocator )

TEST( dlib, GetModule )
{
    dlib::Manager  manager;
    dlib::Module * mod1( manager.GetModule( "MobileDevice" ) );
    dlib::Module * mod2( manager.GetModule( "FooBar" ) );
    
    manager.AddSearchPath( "/System/Library/PrivateFrameworks/" );
    
    {
        dlib::Module * mod3( manager.GetModule( "MobileDevice" ) );
        
        ASSERT_EQ( mod1, nullptr );
        ASSERT_EQ( mod2, nullptr );
        ASSERT_NE( mod3, nullptr );
    }
}

TEST( dlib, GetSymbolAddress )
{
    dlib::Manager manager;
    
    manager.AddSearchPath( "/System/Library/PrivateFrameworks/" );
    
    {
        dlib::Module * mod( manager.GetModule( "MobileDevice" ) );
        
        ASSERT_NE( mod, nullptr );
        ASSERT_NE( mod->GetSymbolAddress( "AMRestoreCreateDefaultOptions"  ),        nullptr );
        ASSERT_EQ( mod->GetSymbolAddress( "AMRestoreCreateDefaultOptions_Invalid" ), nullptr );
    }
}

TEST( dlib, Call )
{
    void * r1( 0 );
    void * r2( r1 );
    
    dlib::Manager::SharedInstance().AddSearchPath( "/System/Library/PrivateFrameworks/" );
    
    ASSERT_NO_THROW( r1 = AMRestoreCreateDefaultOptions(         NULL ) );
    ASSERT_NO_THROW( r2 = AMRestoreCreateDefaultOptions_Invalid( NULL ) );
    
    ASSERT_TRUE( r1 != 0 );
    ASSERT_TRUE( r2 == 0 );
}
