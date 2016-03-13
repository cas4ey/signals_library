/***************************************************************************************
* file        : connect_disconnect.hpp
* data        : 2016/03/08
* author      : Victor Zarubkin
* contact     : v.s.zarubkin@gmail.com
* copyright   : Copyright (C) 2015  Victor Zarubkin
*             :
* description : This header contains auxiliary functions to connect and disconnect signals and slots.
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

#ifndef SIGNALS_LIBRARY__CONNECT_DISCONNECT__HPP_
#define SIGNALS_LIBRARY__CONNECT_DISCONNECT__HPP_

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace slib {

    /** \brief Connects signal and slot.

    \param _signal Reference to the signal.
    \param _slot Reference to the slot. */
    template <typename function_signature>
    inline void connect(const ::slib::signal<function_signature>& _signal, ::slib::slot<function_signature>& _slot)
    {
        _signal.connect(_slot);
    }

    /** \brief Connects signal and slot.

    \param _signal Reference to the signal.
    \param _slot Reference to the slot. */
    template <typename function_signature>
    inline void connect(::slib::slot<function_signature>& _slot, const ::slib::signal<function_signature>& _signal)
    {
        _signal.connect(_slot);
    }

    /** \brief Connects two signals.

    \param _signal Reference to the first signal (which would be emitted).
    \param _slot Reference to the second signal (which would receive first signal's emittion and would emit itself after that). */
    template <typename function_signature>
    inline void connect(const ::slib::signal<function_signature>& _signal, ::slib::signal<function_signature>& _slot)
    {
        _signal.connect(_slot.to_slot());
    }

    //////////////////////////////////////////////////////////////////////////

    /** \brief Disconnects signal and slot.

    \param _signal Reference to the signal.
    \param _slot Reference to the slot. */
    template <typename function_signature>
    inline void disconnect(const ::slib::signal<function_signature>& _signal, ::slib::slot<function_signature>& _slot)
    {
        _slot.disconnect(_signal);
    }

    /** \brief Disconnects signal and slot.

    \param _signal Reference to the signal.
    \param _slot Reference to the slot. */
    template <typename function_signature>
    inline void disconnect(::slib::slot<function_signature>& _slot, const ::slib::signal<function_signature>& _signal)
    {
        _slot.disconnect(_signal);
    }

    /** \brief Disconnects two signals.

    \param _signal Reference to the first signal (which would be emitted).
    \param _slot Reference to the second signal (which would receive first signal's emittion and would emit itself after that). */
    template <typename function_signature>
    inline void disconnect(const ::slib::signal<function_signature>& _signal, ::slib::signal<function_signature>& _slot)
    {
        _slot.to_slot().disconnect(_signal);
    }

} // END namespace slib.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SIGNALS_LIBRARY__CONNECT_DISCONNECT__HPP_
