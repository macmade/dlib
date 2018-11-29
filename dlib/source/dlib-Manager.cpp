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
 * @file        dlib-Manager.cpp
 * @copyright   (c) 2017, Jean-David Gadina - www.xs-labs.com
 */

#include "dlib-Manager.hpp"
#include "dlib-Module.hpp"
#include <mutex>
#include <algorithm>
#include <map>
#include <vector>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace dlib
{
    class Manager::IMPL
    {
        public:
            
            IMPL( void );
            IMPL( const IMPL & o );
            ~IMPL( void );
            
            Module * CreateAndAddModule( const std::string & name, const std::string & path );
            
            mutable std::recursive_mutex      _rmtx;
            std::vector< std::string >        _paths;
            std::map< std::string, Module * > _modules;
    };
    
    static Manager * sharedManager = nullptr;
    
    #ifdef _WIN32
    Manager & __cdecl Manager::SharedInstance( void )
    #else
    Manager & Manager::SharedInstance( void )
    #endif
    {
        static std::once_flag once;
        
        std::call_once
        (
            once,
            [ & ]
            {
                sharedManager = new Manager();
            }
        );
        
        return *( sharedManager );
    }
    
    Manager::Manager( void ): impl( new IMPL() )
    {}
    
    Manager::Manager( const Manager & o ): impl( new IMPL( *( o.impl ) ) )
    {}
    
    Manager::~Manager( void )
	{
        delete this->impl;
    }
	
    Manager & Manager::operator =( Manager o )
    {
        swap( *( this ), o );
        
        return *( this );
    }
    
    void Manager::AddSearchPath( const std::string & path )
    {
        std::lock_guard< std::recursive_mutex > l( this->impl->_rmtx );

        #ifdef _WIN32
        {
            wchar_t * ws;
            int       n;

            n  = MultiByteToWideChar( CP_UTF8, 0, path.c_str(), -1, NULL, 0 );
            ws = static_cast< wchar_t * >( malloc( ( ( size_t )n * sizeof( wchar_t ) ) + sizeof( wchar_t ) ) );

            if( ws != NULL )
            {
                MultiByteToWideChar( CP_UTF8, 0, path.c_str(), -1, ws, n );

                /*
                * AddDllDirectory doesn't exist on Windows XP
                * AddDllDirectory( ws );
                */
                SetDllDirectory( ws );
                free( ws );
            }
        }
        #endif

        this->impl->_paths.push_back( path );
    }
    
    Module * Manager::GetModule( const std::string & name )
    {
        std::lock_guard< std::recursive_mutex > l( this->impl->_rmtx );
        
        for( const auto & p: this->impl->_modules )
        {
            if( p.first == name )
            {
                return p.second;
            }
        }
        
        for( const auto & path: this->impl->_paths )
        {
            {
                std::string lib;
                Module    * module;
                
                #ifdef _WIN32
                
                lib  = path;
                lib += "/";
                lib += name;
                lib += ".dll";
                
                module = this->impl->CreateAndAddModule( name, lib );
                
                if( module )
                {
                    return module;
                }
                
                #else
                
                lib  = path;
                lib += "/";
                lib += name;
                lib += ".dylib";
                
                module = this->impl->CreateAndAddModule( name, lib );
                
                if( module != nullptr )
                {
                    return module;
                }
                
                lib  = path;
                lib += "/";
                lib += name;
                lib += ".framework";
                lib += "/";
                lib += name;
                
                module = this->impl->CreateAndAddModule( name, lib );
                
                if( module != nullptr )
                {
                    return module;
                }
                
                #endif
            }
        }
        
        return nullptr;
    }
    
    void swap( Manager & o1, Manager & o2 )
    {
        std::lock( o1.impl->_rmtx, o2.impl->_rmtx );
        
        std::lock_guard< std::recursive_mutex > l1( o1.impl->_rmtx, std::adopt_lock );
        std::lock_guard< std::recursive_mutex > l2( o2.impl->_rmtx, std::adopt_lock );
        
        using std::swap;
        
        swap( o1.impl, o2.impl );
    }
    
    Manager::IMPL::IMPL( void )
    {}
    
    Manager::IMPL::IMPL( const IMPL & o )
    {
        std::lock_guard< std::recursive_mutex > l( o._rmtx );
        
        this->_paths = o._paths;
        
        for( const auto & p: o._modules )
        {
            this->_modules[ p.first ] = new Module( *( p.second ) );
        }
    }
    
    Manager::IMPL::~IMPL( void )
    {
        for( const auto & p: this->_modules )
        {
            delete p.second;
        }
    }
    
    Module * Manager::IMPL::CreateAndAddModule( const std::string & name, const std::string & path )
    {
        std::lock_guard< std::recursive_mutex > l( this->_rmtx );
        std::ifstream                           file( path.c_str() );
        Module                                * module;
        
        if( file.good() )
        {
            module = new Module( path );
            
            this->_modules[ name ] = module;
            
            return module;
        }
        
        return nullptr;
    }
}
