/***************************************************************************************
* file        : signals.hpp
* data        : 2016/03/12
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2016 Victor Zarubkin
*             :
* description : This header contains description of signals and slots with different number of arguments.
*             : This version of signal, slot has a user-controlled thread-safety behavior.
*             : By default, signal and slot are not thread-safe (for speed-up single thread usage), but
*             : you can invoke method set_threadsafe(true) when you really need thread-safety - you will
*             : got protected but slower version of signal and slot.
*             : 
*             : Slot is a delegate which uses signal subscription system.
*             : Slots can not be copied (neigher copy constructible nor copy assignable).
*             : It is because slot automatically disconnects from all connected signals on destructor.
*             : Slots uses dynamic memory allocation on first connect to signal, and does not use
*             : dynamic memory allocation on secondary connects after disconnect.
*             : 
*             : Signal is inherited from slot and has the same syntax. Signal is used to call all
*             : connected Slots when user calls it's emit_() method.
*             : One signal can be connected to another signal.
*             : Signals also diconnects all connected slots and signals on destructor.
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
#define SIGNALS_LIBRARY__SIGNALS__HPP_

#include "slib/delegate.hpp"
#include "slib/util/mutex.hpp"
#include "shared_allocator/cached_allocator.hpp"
#include "slib/details/signal_slot_subscriber.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace slib {

    //////////////////////////////////////////////////////////////////////////

    /** \brief Slot class. It has a pointer to method (handler) and keeps pointer to self position in
    signal's slot list for safe disconnection when owner of slot is being destroyed.

    \warning It CAN'T be stored in STL containers, both it can't be copyed, because it can have ONLY ONE owner.

    \ingroup slib */
    template <typename return_type, typename ... Args>
    class slot < return_type(Args...) > : public delegate < return_type(Args...) >
    {
    public:

        typedef ::slib::delegate< return_type(Args...) >   delegate_type;
        typedef ::slib::args_list< return_type(Args...) > args_list_type;
        typedef ::slib::slot< return_type(Args...) >           slot_type;
        typedef ::slib::signal< return_type(Args...) >       signal_type;

    private:

        typedef delegate_type                              parent_type;
        typedef slot_type                                    this_type;
        typedef ::slib::util::dynamic_mutex              dynamic_mutex;
        typedef ::slib::util::lock_guard<dynamic_mutex>     lock_guard;
        typedef ::slib::util::atomic_boolean            atomic_boolean;

        typedef ::slib::util::subscriber<slot_type, signal_type> subscriber_type;
        typedef ::salloc::cached_allocator<subscriber_type, ::salloc::shared_allocator<subscriber_type> > allocator_type;

        dynamic_mutex        m_mutex; ///< Mutex for multi-threading protection (it is not thread-safe by default)
        subscriber_type*     m_first; ///< Pointer to the first binded signal in list
        atomic_boolean     m_deleted; ///< Equals to true if deleted
        allocator_type   m_allocator; ///< Allocator for safe cross-library allocations and reuse of deallocated memory

    public:

        // Public methods

        /** \brief Constructs an unbinded slot. */
        explicit slot();

        /** \brief Constructs slot binded to specified handler method.

        \param _handler Reference to binded delegate */
        explicit slot(const parent_type& _handler);

        /** \brief Constructs an unbinded slot. */
        explicit slot(bool _is_threadsafe);

        /** \brief Constructs slot binded to specified handler method.

        \param _handler reference to binded delegate */
        explicit slot(const parent_type& _handler, bool _is_threadsafe);

        /** \brief Destructor.

        \note Disconnects this slot from all connected signals. */
        ~slot();

        /** \brief Returns thread-safety flag.

        \warning This method is NOT thread-safe by itself. */
        inline bool threadsafe() const;

        /** \brief Set this slot's thread-safe protection on/off.

        \warning This method is NOT thread-safe.

        \note Slots which are not thread-safe works faster.

        \param _is_threadsafe thread-safe protection flag. */
        inline void set_threadsafe(bool _is_threadsafe);

        /** \brief Connects this slot to specified signal.

        \note This method is thread-safe if set_threadsafe(true).

        \param _signal reference to the signal */
        void connect(const signal_type& _signal);

        /** \brief Disconnects slot from specified signal.

        \note This method is thread-safe if set_threadsafe(true).

        \param _signal reference to the signal */
        void disconnect(const signal_type& _signal);

        /** \brief Disconnects slot from all connected signals.

        \note This method is thread-safe if set_threadsafe(true). */
        void disconnect();

        /** \brief Reserve memory for concrete number of signal connections.

        \note Will do nothing if current reserved number is equal or greater than desired number of connections.

        \note This method is thread-safe if set_threadsafe(true).

        \param _number Desired number of signal connections */
        inline void reserve(unsigned int _number);

        /** \brief Tests if slot is connected at least to one signal.

        \note This method is thread-safe if set_threadsafe(true).

        \retval true if slot has been connected at least to one signal */
        inline bool connected() const;

    private:

        // Restricted methods

        /** \brief To prevent slot from copying.

        \warning this constructor has no implementation! */
        slot(slot const&) = delete;

        /** \brief To prevent slot from copying.

        \warning this method has no implementation! */
        slot const& operator =(slot const&) = delete;

    private:

        // Private methods to be used only by signal

        /** \brief Removes specified subscriber element from list.

        \note This method is thread-safe if set_threadsafe(true).

        \param _that pointer to the element */
        inline void detach(subscriber_type* _that);

        /** \brief Returns new subscriber object which can be connected to signal.

        \note This method is thread-safe if set_threadsafe(true). */
        inline subscriber_type* get_new_subscriber();

        friend signal_type;

    }; // END class slot.

    //////////////////////////////////////////////////////////////////////////

    /** \brief A signal. It's purpose is to call connected slots when signal emits.

    One signal can be connected to another. For that purpose use to_slot() method to convert signal to slot
    and then use connect() method as usual.

    \ingroup slib */
    template <typename return_type, typename ... Args>
    class signal < return_type(Args...) > : private slot < return_type(Args...) >
    {
    public:

        typedef ::slib::delegate< return_type(Args...) >   delegate_type;
        typedef ::slib::args_list< return_type(Args...) > args_list_type;
        typedef ::slib::slot< return_type(Args...) >           slot_type;
        typedef ::slib::signal< return_type(Args...) >       signal_type;

    private:

        typedef signal_type this_type;
        typedef slot_type parent_type;

        typedef ::slib::util::dynamic_mutex              dynamic_mutex;
        typedef ::slib::util::lock_guard<dynamic_mutex>     lock_guard;
        typedef ::slib::util::atomic_boolean            atomic_boolean;

        typedef ::slib::util::subscriber<slot_type, signal_type> subscriber_type;

        mutable subscriber_type    m_head; ///< The head of slots list
        dynamic_mutex             m_mutex; ///< Mutex for multithreading protection (it is not multithreated by default)
        atomic_boolean          m_deleted; ///< Equals to true if deleted

    public:

        // Public methods

        /** \brief Constructs signal. */
        explicit signal();

        explicit signal(bool _is_threadsafe);

        ~signal();

        /** \brief Returns thread-safe token.

        \warning This method is NOT thread-safe by itself. */
        inline bool threadsafe() const;

        /** \brief Set this signal's thread-safe protection on/off.

        \warning This method is NOT thread-safe.

        \note Signals which are not thread-safe works faster.

        \param _is_threadsafe thread-safe protection token. */
        inline void set_threadsafe(bool _is_threadsafe);

        /** \brief Convert this signal to slot.

        Use this method to be able to connect one signal to another.

        \retval Reference to parent class */
        inline slot_type& to_slot();

        /** \brief Connects specified slot with this signal.

        \note This method is thread-safe if set_threadsafe(true).

        \param _slot reference to the slot */
        inline void connect(slot_type& _slot) const;

        /** \brief Disconnects specified slot from this signal.

        \note This method is thread-safe if set_threadsafe(true).

        \param _slot reference to the slot */
        inline void disconnect(slot_type& _slot) const;

        /** \brief Disconnects all connected slots.

        \note This method is thread-safe if set_threadsafe(true). */
        void disconnect() const;

        /** \brief Emits signal with specified parameters.

        \note This method is thread-safe if set_threadsafe(true). */
        inline void emit_(Args... _args) const;

        /** \brief Emits signal with specified parameters.

        \note This method is thread-safe if set_threadsafe(true). */
        inline void operator()(Args... _args) const;

        /** \brief Test if signal is connected at least to one slot.

        \note This method is thread-safe if set_threadsafe(true). */
        inline bool connected() const;

    private:

        // Restricted methods

        /** \brief To prevent signal from copying.

        \warning this constructor has no implementation! */
        signal(signal const&) = delete;

        /** \brief To prevent signal from copying.

        \warning this method has no implementation! */
        signal const& operator =(signal const&) = delete;

    private:

        // Private methods to be used only by signal and slot

        /** \brief Inserts new slot into slots list.

        \note This method is thread-safe if set_threadsafe(true).

        \param _subscriber pointer to subscriber object */
        void insert(subscriber_type* _subscriber) const;

        /** \brief Removes slot from slots list.

        \note This method is thread-safe if set_threadsafe(true).

        \param _subscriber pointer to subscriber object */
        inline void remove(subscriber_type* _subscriber) const;

    private:

        // Self private methods

        /** \brief Private invoker method. */
        void private_emit(Args&&... _args) const;

        /** \brief This is emit_.

        It is used when binding one signal to another.

        \note This method is thread-safe if set_threadsafe(true).

        \sa emit_ */
        inline return_type private_invoke(Args... _args) const;

        friend slot_type;

    }; // END class signal.

    //////////////////////////////////////////////////////////////////////////

} // END namespace slib.

//////////////////////////////////////////////////////////////////////////

#define SIGNALS_LIBRARY_SIGNALS___INL__
#include "slib/details/signals.inl"
#undef SIGNALS_LIBRARY_SIGNALS___INL__

#include "slib/details/connect_disconnect.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SIGNALS_LIBRARY__SIGNALS__HPP_
