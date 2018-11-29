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
 * @file        WIN.cpp
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#define XSTEST_MAIN
#define XSTEST_GTEST_COMPAT
#include <XSTest/XSTest.hpp>

#define DLIB_S( _s_ ) _s_

#include <dlib.hpp>

extern DWORD SymSetOptions( DWORD );
extern DWORD FooBar( DWORD );

DLIB_FUNC_START( DbgHelp, DWORD, SymSetOptions, DWORD i )
DLIB_FUNC_RET(   DbgHelp, DWORD, SymSetOptions, i )

DLIB_FUNC_START( DbgHelp, DWORD, FooBar, DWORD i )
DLIB_FUNC_RET(   DbgHelp, DWORD, FooBar, i )

TEST( dlib, GetModule )
{
    dlib::Manager  manager;
    dlib::Module * mod1( manager.GetModule( "DbgHelp" ) );
    dlib::Module * mod2( manager.GetModule( "FooBar" ) );
    
    manager.AddSearchPath( "C:\\Windows\\System32\\" );
    
    {
        dlib::Module * mod3( manager.GetModule( "DbgHelp" ) );
        
        ASSERT_EQ( mod1, nullptr );
        ASSERT_EQ( mod2, nullptr );
        ASSERT_NE( mod3, nullptr );
    }
}

TEST( dlib, GetSymbolAddress )
{
    dlib::Manager manager;
    
    manager.AddSearchPath( "C:\\Windows\\System32\\" );

    {
        dlib::Module * mod( manager.GetModule( "DbgHelp" ) );
        
        ASSERT_NE( mod, nullptr );
        ASSERT_NE( mod->GetSymbolAddress( "SymSetOptions"  ), nullptr );
        ASSERT_EQ( mod->GetSymbolAddress( "FooBar" ),  nullptr );
    }
}

TEST( dlib, Call )
{
    DWORD r1( 0 );
    DWORD r2( 1 );
    
    dlib::Manager::SharedInstance().AddSearchPath( "C:\\Windows\\System32\\" );
    
    ASSERT_NO_THROW( r1 = SymSetOptions( 42 ) );
    ASSERT_NO_THROW( r2 = FooBar( 42 ) );
    
    ASSERT_TRUE( r1 != 0 );
    ASSERT_TRUE( r2 == 0 );
}