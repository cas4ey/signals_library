/***************************************************************************************
* file        : signal_slot_subscriber.hpp
* data        : 2016/03/12
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2016 Victor Zarubkin
*             :
* description : This header file contains declaration and definition of an auxiliary class
*             : subscriber used by slot and signal to connect one to another.
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

#ifndef SIGNALS_LIBRARY__SIGNAL_SLOT_SUBSCRIBER__HPP_
#define SIGNALS_LIBRARY__SIGNAL_SLOT_SUBSCRIBER__HPP_

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace slib {

    namespace util {

        /** \brief An auxiliary data struct, which keeps pointer to this slot
        and pointers to previous and next elements in signal's slot list. */
        template <class slot_type, class signal_type>
        class subscriber final
        {
            typedef subscriber this_type;
            typedef ::salloc::cached_allocator<this_type, ::salloc::shared_allocator<this_type> > allocator_type;

            struct link {
                this_type* prev;
                this_type* next;
                link(this_type* _prev = nullptr, this_type* _next = nullptr) : prev(_prev), next(_next) { }
            };

        private:

            slot_type*           slot; ///< Pointer to slot to make disconnect which is used by signal
            const signal_type* signal; ///< Pointer to signal to make disconnect which is used by both slot and signal

            link       slot_list_link; ///< Pointers to prev and next elements in slot's list // used by slot
            link     signal_list_link; ///< Pointers to prev and next elements in signal's list // used by signal

            subscriber(slot_type* _slot) : slot(_slot), signal(nullptr)
            {
            }

            subscriber(const signal_type* _signal) : slot(nullptr), signal(_signal)
            {
            }

            /** \brief Unbinds signal's private parts.

            Used to detach this subscriber object from slots list of current signal. */
            inline void signal_unbind()
            {
                if (signal_list_link.prev != nullptr)
                {
                    signal_list_link.prev->signal_list_link.next = signal_list_link.next;
                }

                if (signal_list_link.next != nullptr)
                {
                    signal_list_link.next->signal_list_link.prev = signal_list_link.prev;
                }

                signal_list_link.prev = nullptr;
                signal_list_link.next = nullptr;
                signal = nullptr;
            }

            /** \brief Unbinds slot's private parts.

            Used to detach this subscriber object from internal list of subscriber objects of current slot. */
            inline void slot_unbind()
            {
                if (slot_list_link.prev != nullptr)
                {
                    slot_list_link.prev->slot_list_link.next = slot_list_link.next;
                }

                if (slot_list_link.next != nullptr)
                {
                    slot_list_link.next->slot_list_link.prev = slot_list_link.prev;
                }

                slot_list_link.prev = nullptr;
                slot_list_link.next = nullptr;
            }

            friend slot_type;
            friend signal_type;
            friend allocator_type;

        }; // END struct subscriber.

    } // END namespace util.

} // END namespace slib.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SIGNALS_LIBRARY__SIGNAL_SLOT_SUBSCRIBER__HPP_
