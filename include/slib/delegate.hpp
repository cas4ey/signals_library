/***************************************************************************************
* file        : delegate.hpp
* data        : 2015/12/12
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2015 Victor Zarubkin
*             : 
* description : This header contains description of delegates with different number of arguments.
*             : Delegate is a template pointer to class method or static function.
*             : Delegates can be copied and stored in generic containers (for example, std::vector).
*             : Delegates are fast, small (it consists only of two pointers) and
*             : does not use dynamic memory allocation.
*             : 
*             : This is redesigned idea of Sergey Ryazanov's fast delegates.
*             : The original source code can be found at
*             : http://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
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

#ifndef SIGNALS_LIBRARY__DELEGATE__HPP_
#define SIGNALS_LIBRARY__DELEGATE__HPP_

#include <stdlib.h>
#include "slib/util/default_constructor.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Auxiliary macros

#ifdef BIND
# error delegate.hpp Macro BIND is already defined!
#else
// Macro for binding delegate and slot to non-static method of some object.
# define BIND(Class, Instance, Method) bind<Class, &Class::Method >(Instance)
#endif


#ifdef BIND_CONST
# error delegate.hpp Macro BIND_CONST is already defined!
#else
// Macro for binding delegate and slot to non-static const-method of some object.
# define BIND_CONST(Class, Instance, ConstMethod) bind_const<Class, &Class::ConstMethod >(Instance)
#endif


#ifdef FROM_METHOD
# error delegate.hpp Macro FROM_METHOD is already defined!
#else
// Macro for creating new delegate from non-static method of some object.
# define FROM_METHOD(Class, Instance, Method) from_method<Class, &Class::Method >(Instance)
#endif


#ifdef FROM_CMETHOD
# error delegate.hpp Macro FROM_CMETHOD is already defined!
#else
// Macro for creating new delegate from non-static const-method of some object.
# define FROM_CMETHOD(Class, Instance, ConstMethod) from_const_method<Class, &Class::ConstMethod >(Instance)
#endif


//////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER
// To optimize Delegate calls under Visual Studio __fastcall directive is used.
# define SLIB_VCCALLTYPE __fastcall
#else
# define SLIB_VCCALLTYPE 
#endif

//////////////////////////////////////////////////////////////////////////

namespace slib {

    //////////////////////////////////////////////////////////////////////////
    // Forward declarations
    template <typename function_signature> class delegate;
    template <typename function_signature> class args_list;
    template <typename function_signature> class slot;
    template <typename function_signature> class signal;

    //////////////////////////////////////////////////////////////////////////

    /** \brief Fast template pointer to class method or static function.

    \note It does not use dynamic memory allocation.

    \note It's size is equal to the size of 2 pointers.

    \note It can be copied and stored in STL (and alike) containers.

    \note It is totally safe to call an unbinded delegate.

    \warning Please, remember to unbind delegate if you are going to destroy instance of
    class to which method you have binded your delegate.

    \ingroup slib */
    template <typename return_type, typename ... Args>
    class delegate < return_type(Args...) >
    {
        typedef return_type(SLIB_VCCALLTYPE *inner_method_type)(void*, Args&&...);

        inner_method_type       m_method; ///< Pointer to one of static delegate's functions: method_stub, method_stub_const, function_stub.
        void*             m_instance_ptr; ///< Pointer to class instance which method will be called. It is nullptr for static/global functions.

    public:

        typedef ::slib::delegate< return_type(Args...) >   delegate_type;
        typedef ::slib::args_list< return_type(Args...) > args_list_type;
        typedef ::slib::slot< return_type(Args...) >           slot_type;
        typedef ::slib::signal< return_type(Args...) >       signal_type;

    private:

        typedef delegate_type this_type;

    public:

        /** \brief Constructor.

        Constructs an unbinded delegate. */
        delegate() : m_method(&function_stub<this_type::empty_method>), m_instance_ptr(nullptr)
        {
        }

        /** \brief Copying constructor.

        Creates delegate and copies pointers to function and object from another delegate. */
        delegate(const this_type& _delegate) : m_method(_delegate.m_method), m_instance_ptr(_delegate.m_instance_ptr)
        {
        }

        /** \brief Calls binded method/function. */
        return_type operator()(Args... _args) const
        {
            return (*m_method)(m_instance_ptr, ::std::forward<Args>(_args)...);
        }

        /** \brief Tests if delegate is unbinded.

        \retval true if delegate is unbinded

        \retval false if delegate is binded */
        inline bool operator!() const
        {
            return m_method == &function_stub<this_type::empty_method>;
        }

        /** \brief Tests if delegate is binded.

        \note It is opposite to empty() method.

        \retval true if delegate is binded

        \retval false if delegate is unbinded

        \sa empty */
        inline operator bool() const
        {
            return m_method != &function_stub<this_type::empty_method>;
        }

        /** \brief Returns pointer to binded class instance. */
        void* obj()
        {
            return m_instance_ptr;
        }

        /** \brief Returns Pointer to binded class instance. */
        const void* obj() const
        {
            return m_instance_ptr;
        }

        /** \brief Creates new delegate and binds it to class non-static method.

        \param _instance Pointer to class instance */
        template <class T, return_type(T::*METHOD)(Args...)>
        static this_type from_method(T* _instance)
        {
            this_type d;
            d.m_instance_ptr = _instance;
            d.m_method = &inner_method<T, METHOD>;
            return d;
        }

        /** \brief Creates new delegate and binds it to class non-static const-method.

        \param _instance Pointer to class instance */
        template <class T, return_type(T::*CONST_METHOD)(Args...) const>
        static this_type from_const_method(T* _instance)
        {
            this_type d;
            d.m_instance_ptr = _instance;
            d.m_method = &method_stub_const<T, CONST_METHOD>;
            return d;
        }

        /** \brief Creates new delegate and binds it to global/static function or class static method. */
        template <return_type(*FUNCTION)(Args...)>
        static this_type from_function()
        {
            this_type d;
            d.m_instance_ptr = nullptr;
            d.m_method = &function_stub<FUNCTION>;
            return d;
        }

        /** \brief Binds delegate to class non-static method.

        \param _instance Pointer to class instance */
        template <class T, return_type(T::*METHOD)(Args...)>
        void bind(T* _instance)
        {
            m_instance_ptr = _instance;
            m_method = &inner_method<T, METHOD>;
        }

        /** \brief Binds delegate to class non-static const-method.

        \param _instance Pointer to class instance */
        template <class T, return_type(T::*CONST_METHOD)(Args...) const>
        void bind_const(T* _instance)
        {
            m_instance_ptr = _instance;
            m_method = &method_stub_const<T, CONST_METHOD>;
        }

        /** \brief Binds delegate to global/static function or class static method. */
        template <return_type(*FUNCTION)(Args...)>
        void bind()
        {
            m_instance_ptr = nullptr;
            m_method = &function_stub<FUNCTION>;
        }

        /** \brief Binds delegate to the same object of another delegate.

        \param _delegate reference to another delegate */
        inline void bind(const this_type& _delegate)
        {
            m_instance_ptr = _delegate.m_instance_ptr;
            m_method = _delegate.m_method;
        }

        /** \brief Unbinds delegate from function/method.

        \note It is fully safe to call an unbinded delegate. */
        inline void unbind()
        {
            bind<this_type::empty_method>();
        }

        /** \brief Tests two Delegates for identity.

        \param _other reference to another delegate */
        bool operator==(const this_type& _other) const
        {
            return (m_method == _other.m_method && m_instance_ptr == _other.m_instance_ptr);
        }

        /** \brief Tests two Delegates for difference.

        \param _other reference to another delegate */
        bool operator!=(const this_type& _other) const
        {
            return (m_method != _other.m_method || m_instance_ptr != _other.m_instance_ptr);
        }

    private:

        /** \brief Calls non-static method.

        \param _instance_ptr m_instance_ptr

        \sa m_instance_ptr */
        template <class T, return_type(T::*METHOD)(Args...)>
        static return_type SLIB_VCCALLTYPE inner_method(void* _instance_ptr, Args&&... _args)
        {
            return (static_cast<T*>(_instance_ptr)->*METHOD)(::std::forward<Args>(_args)...);
        }

        /** \brief Calls non-static const-method.

        \param _instance_ptr m_instance_ptr

        \sa m_instance_ptr */
        template <class T, return_type(T::*CONST_METHOD)(Args...) const>
        static return_type SLIB_VCCALLTYPE method_stub_const(void* _instance_ptr, Args&&... _args)
        {
            return (static_cast<const T*>(_instance_ptr)->*CONST_METHOD)(::std::forward<Args>(_args)...);
        }

        /** \brief Calls global/static function or static class method. */
        template <return_type(*FUNCTION)(Args...)>
        static return_type SLIB_VCCALLTYPE function_stub(void*, Args&&... _args)
        {
            return (*FUNCTION)(::std::forward<Args>(_args)...);
        }

        /** \brief Secure method to make unbinded delegate's calls safe.

        \note When unbinding delegate it will be automatically binded to that function, so you will never call a null pointer.

        \sa unbind */
        static return_type empty_method(Args...)
        {
            return ::slib::util::default_constructor<return_type>();
        }

    }; // END class delegate.

} // END namespace slib.

#undef SLIB_VCCALLTYPE

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SIGNALS_LIBRARY__DELEGATE__HPP_
