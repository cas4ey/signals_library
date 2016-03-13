/***************************************************************************************
* file        : signals.inl
* data        : 2016/03/12
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2016 Victor Zarubkin
*             :
* description : This inline file contains implementation of slot and signal methods.
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

#ifndef SIGNALS_LIBRARY_SIGNALS___INL__
# error signals.inl must be included only from signals.hpp!
#else

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace slib {

    template <typename return_type, typename ... Args>
    slot< return_type(Args...) >::slot()
        : parent_type()
        , m_first(nullptr)
    {
        m_allocator.reserve(1, 1); // reserve connection for one signal
    }

    template <typename return_type, typename ... Args>
    slot< return_type(Args...) >::slot(const parent_type& _handler)
        : parent_type(_handler)
        , m_first(nullptr)
    {
        m_allocator.reserve(1, 1); // reserve connection for one signal
    }

    template <typename return_type, typename ... Args>
    slot< return_type(Args...) >::slot(bool _is_threadsafe)
        : parent_type()
        , m_mutex(_is_threadsafe)
        , m_first(nullptr)
    {
        m_allocator.reserve(1, 1); // reserve connection for one signal
    }

    template <typename return_type, typename ... Args>
    slot< return_type(Args...) >::slot(const parent_type& _handler, bool _is_threadsafe)
        : parent_type(_handler)
        , m_mutex(_is_threadsafe)
        , m_first(nullptr)
    {
        m_allocator.reserve(1, 1); // reserve connection for one signal
    }

    template <typename return_type, typename ... Args>
    slot< return_type(Args...) >::~slot()
    {
        m_deleted = true;

        m_mutex.lock();

        subscriber_type* current;
        while (m_first != nullptr)
        {
            // remove pointer to current signal's slots list:
            current = m_first;
            m_first = current->slot_list_link.next;

            current->slot_unbind();
            current->signal->remove(current);

            m_allocator.destroy(current);
            m_allocator.deallocate_force(current);
        }

        m_mutex.unlock();
    }

    template <typename return_type, typename ... Args>
    inline bool slot< return_type(Args...) >::threadsafe() const
    {
        return m_mutex.threadsafe();
    }

    template <typename return_type, typename ... Args>
    inline void slot< return_type(Args...) >::set_threadsafe(bool _is_threadsafe)
    {
        m_mutex.set_threadsafe(_is_threadsafe);
    }

    template <typename return_type, typename ... Args>
    inline typename slot< return_type(Args...) >::subscriber_type* slot< return_type(Args...) >::get_new_subscriber()
    {
        lock_guard lg(m_mutex);

        auto subscriber = m_allocator.allocate();
        if (subscriber == nullptr)
        {
            return nullptr;
        }

        m_allocator.construct(subscriber, this);

        // put new slot into subscribers list
        subscriber->slot_list_link.next = m_first;
        if (m_first != nullptr)
        {
            m_first->slot_list_link.prev = subscriber;
        }
        m_first = subscriber;
        lg.unlock();

        return subscriber;
    }

    template <typename return_type, typename ... Args>
    void slot< return_type(Args...) >::disconnect()
    {
        subscriber_type* current;

        lock_guard lg(m_mutex);
        while (m_first != nullptr)
        {
            // remove pointer to current signal's slots list:
            current = m_first;
            m_first = current->slot_list_link.next;

            current->slot_unbind();
            current->signal->remove(current);

            m_allocator.destroy(current);
            m_allocator.deallocate(current);
        }
    }

    template <typename return_type, typename ... Args>
    void slot< return_type(Args...) >::connect(const signal_type& _signal)
    {
        subscriber_type* subscriber = get_new_subscriber();
        if (subscriber != nullptr)
        {
            _signal.insert(subscriber);
        }
    }

    template <typename return_type, typename ... Args>
    void slot< return_type(Args...) >::disconnect(const signal_type& _signal)
    {
        lock_guard lg(m_mutex);

        subscriber_type* current = m_first;
        while (current != nullptr)
        {
            if (current->signal == &_signal)
            {
                if (current == m_first)
                {
                    m_first = current->slot_list_link.next;
                }

                current->slot_unbind();
                _signal.remove(current);

                m_allocator.destroy(current);
                m_allocator.deallocate(current);

                return;
            }

            current = current->slot_list_link.next;
        }
    }

    template <typename return_type, typename ... Args>
    inline void slot< return_type(Args...) >::reserve(unsigned int _number)
    {
        lock_guard lg(m_mutex);
        m_allocator.reserve(1, _number);
    }

    template <typename return_type, typename ... Args>
    void slot< return_type(Args...) >::detach(subscriber_type* _that)
    {
        if (m_deleted)
        {
            return;
        }

        lock_guard lg(m_mutex);

        if (_that == m_first)
        {
            m_first = _that->slot_list_link.next;
        }

        _that->slot_unbind();

        m_allocator.destroy(_that);
        m_allocator.deallocate(_that);
    }

    template <typename return_type, typename ... Args>
    inline bool slot< return_type(Args...) >::connected() const
    {
        lock_guard lg(m_mutex);
        return m_first != nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    template <typename return_type, typename ... Args>
    signal< return_type(Args...) >::signal()
        : parent_type(delegate_type::template from_const_method<this_type, &this_type::private_invoke>(this))
        , m_head(this)
    {
    }

    template <typename return_type, typename ... Args>
    signal< return_type(Args...) >::signal(bool _is_threadsafe)
        : parent_type(delegate_type::template from_const_method<this_type, &this_type::private_invoke>(this), _is_threadsafe)
        , m_head(this)
        , m_mutex(_is_threadsafe)
    {
    }

    template <typename return_type, typename ... Args>
    signal< return_type(Args...) >::~signal()
    {
        m_deleted = true;
        disconnect();
    }

    template <typename return_type, typename ... Args>
    inline bool signal< return_type(Args...) >::threadsafe() const
    {
        return m_mutex.threadsafe();
    }

    template <typename return_type, typename ... Args>
    inline void signal< return_type(Args...) >::set_threadsafe(bool _is_threadsafe)
    {
        parent_type::set_threadsafe(_is_threadsafe);
        m_mutex.set_threadsafe(_is_threadsafe);
    }

    template <typename return_type, typename ... Args>
    inline slot< return_type(Args...) >& signal< return_type(Args...) >::to_slot()
    {
        return static_cast<parent_type&>(*this);
    }

    template <typename return_type, typename ... Args>
    void signal< return_type(Args...) >::disconnect() const
    {
        lock_guard lg(m_mutex);

        subscriber_type* current = m_head.signal_list_link.next;
        while (current != nullptr)
        {
            subscriber_type* next = current->signal_list_link.next;

            current->signal_unbind();
            current->slot->detach(current);

            current = next;
        }

        m_head.signal_list_link.prev = nullptr;
        m_head.signal_list_link.next = nullptr;
    }

    template <typename return_type, typename ... Args>
    void signal< return_type(Args...) >::insert(subscriber_type* _subscriber) const
    {
        lock_guard lg(m_mutex);

        _subscriber->signal_list_link.next = m_head.signal_list_link.next;
        m_head.signal_list_link.next = _subscriber;

        if (_subscriber->signal_list_link.next != nullptr)
        {
            _subscriber->signal_list_link.next->signal_list_link.prev = _subscriber;
        }

        _subscriber->signal_list_link.prev = &m_head;
        _subscriber->signal = this;
    }

    template <typename return_type, typename ... Args>
    inline void signal< return_type(Args...) >::remove(subscriber_type* _subscriber) const
    {
        if (_subscriber->signal == this && !m_deleted)
        {
            lock_guard lg(m_mutex);
            _subscriber->signal_unbind();
        }
    }

    template <typename return_type, typename ... Args>
    inline void signal< return_type(Args...) >::connect(slot_type& _slot) const
    {
        subscriber_type* subscriber = _slot.get_new_subscriber();
        if (subscriber != nullptr)
        {
            insert(subscriber);
        }
    }

    template <typename return_type, typename ... Args>
    inline void signal< return_type(Args...) >::disconnect(slot_type& _slot) const
    {
        _slot.disconnect(*this);
    }

    template <typename return_type, typename ... Args>
    void signal< return_type(Args...) >::private_emit(Args&&... _args) const
    {
        lock_guard lg(m_mutex);

        subscriber_type* current = m_head.signal_list_link.next;
        while (current != nullptr)
        {
            subscriber_type* next = current->signal_list_link.next;
            current->slot->operator()(::std::forward<Args>(_args)...); // call signal handler
            current = next;
        }
    }

    template <typename return_type, typename ... Args>
    inline void signal< return_type(Args...) >::emit_(Args... _args) const
    {
        private_emit(::std::forward<Args>(_args)...);
    }

    template <typename return_type, typename ... Args>
    inline void signal< return_type(Args...) >::operator ()(Args... _args) const
    {
        private_emit(::std::forward<Args>(_args)...);
    }

    template <typename return_type, typename ... Args>
    inline bool signal< return_type(Args...) >::connected() const
    {
        lock_guard lg(m_mutex);
        return m_head.signal_list_link.next != nullptr;
    }

    template <typename return_type, typename ... Args>
    inline return_type signal< return_type(Args...) >::private_invoke(Args... _args) const
    {
        private_emit(::std::forward<Args>(_args)...);
        return ::slib::util::default_constructor<return_type>();
    }

} // END namespace slib.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SIGNALS_LIBRARY_SIGNALS___INL__
