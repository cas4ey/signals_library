/***************************************************************************************
* file        : default_constructor.hpp
* data        : 2016/01/13
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2016  Victor Zarubkin
*             : 
* description : This headers contains template classes used as default constructors
*             : mainly for standard types (char, int, bool, etc.).
*             : 
* license     : This file is part of SignalsLibrary.
*             :
*             : SignalsLibrary is free software: you can redistribute it and/or modify
*             : it under the terms of the GNU General Public License as published by
*             : the Free Software Foundation, either version 3 of the License, or
*             : (at your option) any later version.
*             :
*             : SignalsLibrary is distributed in the hope that it will be useful,
*             : but WITHOUT ANY WARRANTY; without even the implied warranty of
*             : MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*             : GNU General Public License for more details.
*             :
*             : You should have received a copy of the GNU General Public License
*             : along with SignalsLibrary. If not, see <http://www.gnu.org/licenses/>.
*             :
*             : A copy of the GNU General Public License can be found in file LICENSE.
****************************************************************************************/

#ifndef SIGNALS_LIBRARY__DEFAULT_CONSTRUCTOR__HPP_
#define SIGNALS_LIBRARY__DEFAULT_CONSTRUCTOR__HPP_

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace slib {

    namespace util {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        namespace {

            //*******************************************************************************************************************

            template <class T> struct _static_instancer {
                static T& instance() {
                    static T s_instance;
                    return s_instance;
                }
            };

            template <class T> struct _static_zero_instancer {
                static T& instance() {
                    static T s_instance = 0;
                    return s_instance;
                }
            };

            //*******************************************************************************************************************

            template <class T> struct _default_constructor {
                static T construct() {
                    return T();
                }
            };

            template <class T> struct _default_constructor <T&> {
                static T& construct() {
                    return _static_instancer<T>::instance();
                }
            };

            template <class T> struct _default_constructor <const T&> {
                static const T& construct() {
                    return _static_instancer<T>::instance();
                }
            };

            template <class T> struct _default_constructor <T*> {
                static T* construct() {
                    return nullptr;
                }
            };

            template <class T> struct _default_constructor <const T*> {
                static const T* construct() {
                    return nullptr;
                }
            };

            //*******************************************************************************************************************

#define _SLIB_STD_DEFAULT_CONSTRUCTOR(STD_TYPE, DEFAULT_VALUE)           \
            template <> struct _default_constructor <STD_TYPE> {         \
                static STD_TYPE construct() {                            \
                    return DEFAULT_VALUE;                                \
                }                                                        \
            };                                                           \
            template <> struct _default_constructor <STD_TYPE&> {        \
                static STD_TYPE& construct() {                           \
                    return _static_zero_instancer<STD_TYPE>::instance(); \
                }                                                        \
            };                                                           \
            template <> struct _default_constructor <const STD_TYPE&> {  \
                static const STD_TYPE& construct() {                     \
                    return _static_zero_instancer<STD_TYPE>::instance(); \
                }                                                        \
            }

            _SLIB_STD_DEFAULT_CONSTRUCTOR(bool, false);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(char, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(unsigned char, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(short, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(unsigned short, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(int, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(unsigned int, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(long, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(unsigned long, 0);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(long long, 0LL);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(unsigned long long, 0ULL);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(float, 0.f);
            _SLIB_STD_DEFAULT_CONSTRUCTOR(double, 0.0);

#undef _SLIB_STD_DEFAULT_CONSTRUCTOR

            //*******************************************************************************************************************

        } // END namespace <noname>.

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        template <class T> inline T default_constructor()
        {
            return _default_constructor<T>::construct();
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    } // END namespace util.

} // END namespace slib.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SIGNALS_LIBRARY__DEFAULT_CONSTRUCTOR__HPP_
