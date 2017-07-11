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
 * @file        dlib-Module.cpp
 * @copyright   (c) 2017, Jean-David Gadina - www.xs-labs.com
 */

#include "dlib-Module.hpp"
#include <mutex>
#include <algorithm>
#include <map>

#ifndef WIN32
#include <dlfcn.h>
#else
#include <locale>
#include <codecvt>
#include <Windows.h>
#endif

namespace dlib
{
    class Module::IMPL
    {
        public:
            
            IMPL( const std::string & path );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            void * LoadAndAddSymbol( const std::string & name );
            
            mutable std::recursive_mutex    _rmtx;
            std::string                     _path;
            std::map< std::string, void * > _symbols;
            
            #ifdef _WIN32
            HMODULE _handle;
            #else
            void * _handle;
            #endif
    };
    
    Module::Module( const std::string & path ): impl( new IMPL( path ) )
    {}
    
    Module::Module( const Module & o ): impl( new IMPL( *( o.impl ) ) )
    {}
    
    Module::~Module( void )
    {
        delete this->impl;
    }
    
    Module & Module::operator =( Module o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    void * Module::GetSymbolAddress( const std::string & name )
    {
        std::lock_guard< std::recursive_mutex > l( this->impl->_rmtx );
        
        for( const auto & p: this->impl->_symbols )
        {
            if( p.first == name )
            {
                return p.second;
            }
        }
        
        return this->impl->LoadAndAddSymbol( name );
    }
    
    void swap( Module & o1, Module & o2 )
    {
        std::lock( o1.impl->_rmtx, o2.impl->_rmtx );
        
        std::lock_guard< std::recursive_mutex > l1( o1.impl->_rmtx, std::adopt_lock );
        std::lock_guard< std::recursive_mutex > l2( o2.impl->_rmtx, std::adopt_lock );
        
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }
    
    Module::IMPL::IMPL( const std::string & path ):
        _path( path ),
        _handle( nullptr )
    {}
    
    Module::IMPL::IMPL( const IMPL & o ):
        _path( o._path ),
        _handle( nullptr )
    {}
    
    Module::IMPL::~IMPL( void )
    {
        if( this->_handle != nullptr )
        {
            #ifdef _WIN32
            
            CloseHandle( this->_handle );
        
            #else
            
            dlclose( this->_handle );
            
            #endif
        }
    }
    
    #ifdef _WIN32
    
    void * Module::IMPL::LoadAndAddSymbol( const std::string & name )
    {
        std::lock_guard< std::recursive_mutex > l( this->_rmtx );
        void                                  * address;
        
        if( this->_handle == nullptr )
        {
            {
                std::wstring_convert< std::codecvt_utf8_utf16< wchar_t > > c;
                std::wstring                                               wname;
                
                wname = c.from_bytes( name );
                
                this->_handle = LoadLibrary( wname.c_str() );
            }
            
            if( this->_handle == nullptr )
            {
                return nullptr;
            }
        }
        
        address = GetProcAddress( this->_handle, name.c_str() );
        
        this->_symbols[ name ] = address;
        
        return address;
    }
    
    #else
    
    void * Module::IMPL::LoadAndAddSymbol( const std::string & name )
    {
        std::lock_guard< std::recursive_mutex > l( this->_rmtx );
        void                                  * address;
        
        if( this->_handle == nullptr )
        {
            this->_handle = dlopen( this->_path.c_str(), RTLD_LOCAL );
            
            if( this->_handle == nullptr )
            {
                return nullptr;
            }
        }
        
        address = dlsym( this->_handle, name.c_str() );
        
        this->_symbols[ name ] = address;
        
        return address;
    }
    
    #endif
}
