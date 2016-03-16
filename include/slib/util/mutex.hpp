/***************************************************************************************
* file        : mutex.hpp
* data        : 2015/12/12
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2015-2016 Victor Zarubkin
*             :
* description : This header contains declaration and definition for dynamic_mutex, lock_guard and
*             : atomic_boolean classes which are used by signal and slot for multithreading protection.
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

#ifndef SIGNALS_LIBRARY__SIGNALS__HPP_

#error mutex.hpp must be included only from signals.hpp!

#elif !defined(SIGNALS_LIBRARY__MUTEX__HPP_)

#define SIGNALS_LIBRARY__MUTEX__HPP_

#include <mutex>
#include <atomic>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace slib {

    namespace util {

        //////////////////////////////////////////////////////////////////////////

        /** \brief Dynamic mutex.

        Used to dynamically change behavior of a mutex.
        By default lock and unlock methods does nothing, but works very fast.
        They can be made thread-safe if set_threadsafe would be called.
        Then lock and unlock methods will work properly (locking and unlocking mutex).
        This is done to provide unified interface without virtual methods
        (vtable call is slower than if-else).
        
        \ingroup util */
        class dynamic_mutex final
        {
            ::std::mutex         m_mutex; ///< Mutex (used only if m_is_threadsafe == true)
            bool         m_is_threadsafe; ///< Thread-safety flag (false by default). Changes behavior of lock and unlock methods.

        public:

            dynamic_mutex(bool _is_threadsafe = false) : m_is_threadsafe(_is_threadsafe)
            {
            }

            /** \brief Returns thread-safety flag.
            
            \sa m_is_threadsafe */
            inline bool threadsafe() const
            {
                return m_is_threadsafe;
            }

            /** \brief Locks mutex.
            
            \note Does nothing if m_is_threadsafe == false.
            
            \sa m_is_threadsafe, set_threadsafe, unlock */
            inline void lock()
            {
                // if-else works equal (under Release build) or faster (under other builds) than virtual function call
                if (m_is_threadsafe)
                {
                    m_mutex.lock();
                }
            }

            /** \brief Unlocks mutex.

            \note Does nothing if m_is_threadsafe == false.

            \sa m_is_threadsafe, set_threadsafe, lock */
            inline void unlock()
            {
                // if-else works equal (under Release build) or faster (under other builds) than virtual function call
                if (m_is_threadsafe)
                {
                    m_mutex.unlock();
                }
            }

            /** \brief Changes behavior of lock and unlock methods.

            \warning This method is NOT thread-safe! Use this on initialization.
            
            \param _is_threadsafe Thread-safety flag. If true, then lock and unlock will lock/unlock mutex.
            
            \sa m_is_threadsafe, lock, unlock */
            inline void set_threadsafe(bool _is_threadsafe)
            {
                m_is_threadsafe = _is_threadsafe;
            }

        }; // END class dynamic_mutex.

        //////////////////////////////////////////////////////////////////////////

        /** \brief Generic lock-guard.
        
        It's purpose is to lock mutext on constructor and unlock it on destructor.
        
        \ingroup util */
        template <class mutex_type>
        class lock_guard final
        {
            mutex_type&     m_mutex; ///< Reference to mutex
            bool        m_is_locked; ///< Lock status. Used to unlock mutex safely.

            lock_guard() = delete;
            lock_guard(const lock_guard&) = delete;
            lock_guard(lock_guard&&) = delete;

        public:

            /** \brief Constructor.

            Automatically locks mutex.

            \param _mutex_reference Reference to mutex. */
            lock_guard(mutex_type& _mutex_reference) : m_mutex(_mutex_reference), m_is_locked(true)
            {
                m_mutex.lock();
            }

            /** \brief Constructor.

            Automatically locks mutex.

            \param _mutex_reference Reference to mutex. */
            lock_guard(const mutex_type& _mutex_reference) : m_mutex(const_cast<mutex_type&>(_mutex_reference)), m_is_locked(true)
            {
                m_mutex.lock();
            }

            /** \brief Destructor.

            Automatically unblocks mutex. */
            ~lock_guard()
            {
                unlock();
            }

            /** \brief Lock mutex. */
            inline void lock()
            {
                m_mutex.lock();
                m_is_locked = true;
            }

            /** \brief Unlock mutex. */
            inline void unlock()
            {
                if (m_is_locked)
                {
                    m_is_locked = false;
                    m_mutex.unlock();
                }
            }

            /** \brief Returns lock status. */
            inline bool locked() const
            {
                return m_is_locked;
            }

        }; // END class lock_guard.

        //////////////////////////////////////////////////////////////////////////

        /** \brief Atomic bool value.
        
        Simple wrapper for std::atomic<bool> with overloaded operators.
        
        \ingroup util */
        class atomic_boolean final
        {
            ::std::atomic<bool> m_value; ///< Boolean value

        public:

            atomic_boolean() : m_value(false)
            {
            }

            inline operator bool() const
            {
                return m_value.load();
            }

            inline bool operator !() const
            {
                return !m_value.load();
            }

            inline bool operator ==(bool _value) const
            {
                return m_value.load() == _value;
            }

            inline bool operator !=(bool _value) const
            {
                return m_value.load() != _value;
            }

            inline void operator =(bool _value)
            {
                m_value.store(_value);
            }

        }; // END class atomic_boolean.

        //////////////////////////////////////////////////////////////////////////

    } // END namespace util.

} // END namespace slib.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ifdef SIGNALS_LIBRARY__SIGNALS__HPP_ && !defined(SIGNALS_LIBRARY__MUTEX__HPP_)
