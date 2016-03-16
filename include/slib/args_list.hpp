/***************************************************************************************
* file        : args_list.hpp
* data        : 2016/03/12
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2016 Victor Zarubkin
*             :
* description : This header contains description of delegates with different number of arguments.
*             : Delegate is template a pointer to class method or static function.
*             : Delegates can be copied and stored in STL (and alike) containers.
*             : Delegates are fast, small (it consists only of two pointers) and
*             : does not use dynamic memory allocation.
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

#ifndef SIGNALS_LIBRARY__ARGS_LIST__HPP_
#define SIGNALS_LIBRARY__ARGS_LIST__HPP_

#include <stdlib.h>
#include <tuple>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace slib {

    //////////////////////////////////////////////////////////////////////////

    namespace util {

        template <int ...> struct args_sequence { };

        template <int N, int ... S> struct args_sequence_generator : args_sequence_generator<N - 1, N - 1, S...> { };

        template <int ... S> struct args_sequence_generator<0, S...>
        {
            typedef args_sequence<S...> type;
        };

    } // END namespace util.

    //////////////////////////////////////////////////////////////////////////

    /** \brief args_list class.

    It's goal is to store predetermined set of arguments
    and invoke custom delegate with these arguments.

    \warning Please, note that if you have references in arguments list, it must be initialized on constructor!
    After that it can not be changed, even if you call "set" methods.

    \warning Please, note that "set" methods does not change references and their values!

    \warning Be careful with references. Referenced objects must exist when you will invoke delegate.

    \ingroup slib */
    template <typename return_type, typename ... Args>
    class args_list < return_type(Args...) >
    {
    public:

        typedef ::slib::delegate< return_type(Args...) >   delegate_type;
        typedef ::slib::args_list< return_type(Args...) > args_list_type;
        typedef ::slib::slot< return_type(Args...) >           slot_type;
        typedef ::slib::signal< return_type(Args...) >       signal_type;

    private:

        typedef args_list_type this_type;
        typedef ::std::tuple<Args...> real_args_list;

        real_args_list m_args; ///< Arguments

    public:

        /** \brief Constructs args_list with specified set of arguments. */
        args_list(Args&&... _args) : m_args(::std::forward<Args>(_args)...)
        {
        }

        args_list(real_args_list&& _args) : m_args(_args)
        {
        }

        args_list(const real_args_list& _args) : m_args(_args)
        {
        }

        /** \brief Copying constructor.

        \param f reference to filled args_list to copy from */
        args_list(const this_type& f) : m_args(f.m_args)
        {
        }

        /** \brief Returns reference to argument with specified index.
        
        \param argument_index Index of the argument. */
        template <int argument_index>
        inline auto arg() -> decltype(::std::get<argument_index>(m_args))
        {
            return ::std::get<argument_index>(m_args);
        }

        /** \brief Returns const-reference to argument with specified index.

        \param argument_index Index of the argument. */
        template <int argument_index>
        inline auto arg() const -> decltype(::std::get<argument_index>(m_args))
        {
            return ::std::get<argument_index>(m_args);
        }

        /** \brief Returns reference to arguments list. */
        inline real_args_list& args()
        {
            return m_args;
        }

        /** \brief Returns const-reference to arguments list. */
        inline const real_args_list& args() const
        {
            return m_args;
        }

        /** \brief Invoke specified delegate with predetermined set of arguments.

        \param _delegate Reference to delegate to call */
        template <class some_delegate_type>
        inline return_type operator()(const some_delegate_type& _delegate) const
        {
            return private_invoke(_delegate,
                                  typename ::slib::util::args_sequence_generator<sizeof...(Args)>::type());
        }

        /** \brief Direct invoke some class method with predetermined set of arguments.

        \param _instance Reference to the instance of certain class
        \param _method Pointer to the method to be called */
        template <class T, typename TMethod>
        inline return_type operator()(T& _instance, TMethod _method) const
        {
            return private_invoke(_instance, _method,
                                  typename ::slib::util::args_sequence_generator<sizeof...(Args)>::type());
        }

    private:

        /** \brief Auxiliary method for unpacking variadic arguments list. */
        template <class some_delegate_type, int ...S>
        return_type private_invoke(const some_delegate_type& _delegate, ::slib::util::args_sequence<S...>) const
        {
            return _delegate(::std::get<S>(m_args) ...);
        }

        /** \brief Auxiliary method for unpacking variadic arguments list. */
        template <class T, typename TMethod, int ...S>
        return_type private_invoke(T& _instance, TMethod _method, ::slib::util::args_sequence<S...>) const
        {
            return (_instance.*_method)(::std::get<S>(m_args) ...);
        }

    }; // END class args_list.

    //////////////////////////////////////////////////////////////////////////

    namespace {

        /** \brief Auxiliary method for unpacking variadic arguments list. */
        template <typename return_type, class some_delegate_type, class tuple_type, int ...S>
        return_type private_invoke(const some_delegate_type& _delegate, const tuple_type& _arguments, ::slib::util::args_sequence<S...>)
        {
            return _delegate(::std::get<S>(_arguments) ...);
        }

        /** \brief Auxiliary method for unpacking variadic arguments list. */
        template <typename return_type, class T, typename TMethod, class tuple_type, int ...S>
        return_type private_invoke(T& _instance, TMethod _method, const tuple_type& _arguments, ::slib::util::args_sequence<S...>)
        {
            return (_instance.*_method)(::std::get<S>(_arguments) ...);
        }

    } // END namespace <noname>.

    /** \brief Invoke some delegate/function with predetermined set of arguments.

    \param _delegate Reference to the delegate/function
    \param _arguments Const-reference to the tuple with arguments */
    template <typename return_type, class some_delegate_type, typename ... Args>
    inline return_type invoke(const some_delegate_type& _delegate, const ::std::tuple<Args...>& _arguments)
    {
        return private_invoke<return_type>(_delegate, _arguments,
                                           typename ::slib::util::args_sequence_generator<sizeof...(Args)>::type());
    }

    /** \brief Direct invoke some class method with predetermined set of arguments.

    \param _instance Reference to the instance of certain class
    \param _method Pointer to the method to be called
    \param _arguments Const-reference to the tuple with arguments */
    template <typename return_type, class T, typename TMethod, typename ... Args>
    inline return_type invoke(T& _instance, TMethod _method, const ::std::tuple<Args...>& _arguments)
    {
        return private_invoke<return_type>(_instance, _method, _arguments,
                                           typename ::slib::util::args_sequence_generator<sizeof...(Args)>::type());
    }

    //////////////////////////////////////////////////////////////////////////

} // END namespace slib.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SIGNALS_LIBRARY__ARGS_LIST__HPP_
