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
 * @header      dlib-private-macros.hpp
 * @copyright   (c) 2017, Jean-David Gadina - www.xs-labs.com
 */

#ifndef DLIB_PRIVATE_MACROS_HPP
#define DLIB_PRIVATE_MACROS_HPP

#include <iostream>
#include <string>
#include <mutex>

#ifdef _MSC_VER

    #pragma section( ".CRT$XCU", read )
    
    #define DLIB_PRIVATE_CONSTRUCTOR( _name_ )  static void __cdecl _name_( void );														\
                                                 __declspec( allocate( ".CRT$XCU" ) ) void ( __cdecl * _name_ ## _ )( void ) = _name_;	\
                                                static void __cdecl _name_( void )

#else
    
    #define DLIB_PRIVATE_CONSTRUCTOR( _name_ )  static void _name_( void ) __attribute__( ( constructor ) );                            \
                                                static void _name_( void )
    
#endif

#ifdef DEBUG

#define DLIB_PRIVATE_WARN_MISSING_MODULE( _module_ )                        std::cerr << "!!! DLIB WARNING - Missing Module: " << _module_ << std::endl
#define DLIB_PRIVATE_WARN_MISSING_VAR( _type_, _module_, _name_ )           std::cerr << "!!! DLIB WARNING - Missing Symbol: " << _type_ << " " << _module_ << "." << _name_ << std::endl
#define DLIB_PRIVATE_WARN_MISSING_FUNC( _module_, _ret_, _name_, _args_ )   std::cerr << "!!! DLIB WARNING - Missing Symbol: " << _ret_ << " " << _module_ << "." << _name_ << "( " << _args_ << " )" << std::endl

#else

#define DLIB_PRIVATE_WARN_MISSING_MODULE( _module_ )                        
#define DLIB_PRIVATE_WARN_MISSING_VAR( _type_, _module_, _name_ )           
#define DLIB_PRIVATE_WARN_MISSING_FUNC( _type_, _module_, _name_, _args_ )  

#endif

#define DLIB_PRIVATE_VAR( _module_, _type_, _name_, _default_ ) _type_ _name_;                                                                              \
                                                                                                                                                            \
                                                                DLIB_CONSTRUCTOR( __DLib_Constructor_Var_ ## _module_ ## _ ## _type_ ## _ ## _name_ )       \
                                                                {                                                                                           \
                                                                    dlib::Module * module;                                                                  \
                                                                                                                                                            \
                                                                    module = dlib::Manager::SharedInstance().GetModule( # _module_ );                       \
                                                                                                                                                            \
                                                                    if( module == nullptr )                                                                 \
                                                                    {                                                                                       \
                                                                        DLIB_PRIVATE_WARN_MISSING_MODULE( # _module_ );                                     \
                                                                                                                                                            \
                                                                        _name_ = _default_;                                                                 \
                                                                    }                                                                                       \
                                                                    else if( module->GetSymbolAddress( # _name_ ) == nullptr )                              \
                                                                    {                                                                                       \
                                                                        DLIB_PRIVATE_WARN_MISSING_VAR( # _type_, # _module_, # _name_ );                    \
                                                                                                                                                            \
                                                                        _name_ = _default_;                                                                 \
                                                                    }                                                                                       \
                                                                    else                                                                                    \
                                                                    {                                                                                       \
                                                                        _name_ = *( reinterpret_cast< _type_ * >( module->GetSymbolAddress( # _name_ ) ) ); \
                                                                    }                                                                                       \
                                                                }
                                                                
#define DLIB_PRIVATE_FUNC_START( _module_, _ret_, _name_, ... ) _ret_ _name_( __VA_ARGS__ )                                                                         \
                                                                {                                                                                                   \
                                                                    static _ret_ ( * f )( __VA_ARGS__ ) = nullptr;                                                  \
                                                                    static std::once_flag once;                                                                     \
                                                                                                                                                                    \
                                                                    std::call_once                                                                                  \
                                                                    (                                                                                               \
                                                                        once,                                                                                       \
                                                                        [ & ]                                                                                       \
                                                                        {                                                                                           \
                                                                            dlib::Module * module;                                                                  \
                                                                                                                                                                    \
                                                                            module = dlib::Manager::SharedInstance().GetModule( # _module_ );                       \
                                                                                                                                                                    \
                                                                            if( module == nullptr )                                                                 \
                                                                            {                                                                                       \
                                                                                DLIB_PRIVATE_WARN_MISSING_MODULE( # _module_ );                                     \
                                                                            }                                                                                       \
                                                                            else                                                                                    \
                                                                            {                                                                                       \
                                                                                f = reinterpret_cast< _ret_ ( * )( __VA_ARGS__ ) >                                  \
                                                                                (                                                                                   \
                                                                                    module->GetSymbolAddress( # _name_ )                                            \
                                                                                );                                                                                  \
                                                                                                                                                                    \
                                                                                if( f == nullptr )                                                                  \
                                                                                {                                                                                   \
                                                                                    DLIB_PRIVATE_WARN_MISSING_FUNC( # _module_, # _ret_, # _name_, # __VA_ARGS__ ); \
                                                                                }                                                                                   \
                                                                            }                                                                                       \
                                                                        }                                                                                           \
                                                                    );
                                                                    
#define DLIB_PRIVATE_FUNC_RET( _module_, _ret_, _name_, ... )       if( f == nullptr )          \
                                                                    {                           \
                                                                        return {};              \
                                                                    }                           \
                                                                                                \
                                                                    return f( __VA_ARGS__ );    \
                                                                }
                                                                
#define DLIB_PRIVATE_FUNC_NORET( _module_, _ret_, _name_, ... )     if( f == nullptr )  \
                                                                    {                   \
                                                                        return;         \
                                                                    }                   \
                                                                                        \
                                                                    f( __VA_ARGS__ );   \
                                                                }

#endif /* DLIB_PRIVATE_MACROS_HPP */
