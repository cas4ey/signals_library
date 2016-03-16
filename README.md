`under construction...`

### signals_library (slib)
SignalsLibrary (a.k.a. **slib**) is simple "include and use" library that provides anonimous function pointer (`slib::delegate`),
function arguments list (`slib::args_list`) and simple messaging system (`slib::signal` and `slib::slot`).

- **slib::delegate** is a template anonimous pointer to class method or static function. Delegates can be copied
and stored in generic containers (for example, `std::vector`). Delegates are fast, small (it consists only
of two pointers) and does not use dynamic memory allocation. `#include "slib/delegate.hpp"` and you are ready to use delegates.
- **slib::args_list** is a function arguments list. It can be used to store arguments to call a lot of functions later.
Any argument in list can be changed at any time. It stores arguments in `std::tuple` and has methods for unpacking
arguments from tuple to call function/method/delegate/functor with these arguments. `#include "slib/args_list.hpp"` to
use `slib::args_list`.
- **slib::signal** and **slib::slot** is simple messaging system. When signal emits, all connected slots are invoked.
If you have ever used Qt and it's signal-slot system then you understand what is it. You don't need to inherit from
special base-class to use signals and slots (for example, in Qt you have to inherit from QObject class to be able
to use signals and slots). Multiple slots can be connected to one signal; one slot can be also connected to multiple signals;
one signal can be connected to multiple signals like a slot (when parent signal emits, all connected signals emits too).
Signal and slot automatically disconnects on destructor from all connected signals and slots, that's why signal and slot
can not be copied or copy-constructed - they fully belong to theirs owner-object.

### compiling
SignalsLibrary is using features of C++11 standard, so you have to use C++11 compatible compiler.

### adding slib into your project
1. Copy all files from `include/slib` to to your include directory (for example, `include/third_party/slib` or just `include/slib`). Please, note that `slib` is using [shared_allocator](https://github.com/cas4ey/shared_allocator/) as a third-party library (see **NOTE** below).
2. Include necessary headers (`slib/delegate.hpp` and/or `slib/args_list.hpp` and/or `slib/signals.hpp`) and you are ready for using delegates, args_lists, signals and slots.
**NOTE:** `slib::signal` and `slib::slot` requires dynamic linkage with `shared_allocator` (instructions can be found [here](https://github.com/cas4ey/shared_allocator/)). `slib::delegate` and `slib::args_list` does not need `shared_allocator`.

# LICENSE
SignalsLibrary is licensed under terms of GNU GPL v3 (see file LICENSE), but I plan to add more permissive licenses
in the near future (LGPL or MIT licenses maybe). Send me a message if you are interested.
