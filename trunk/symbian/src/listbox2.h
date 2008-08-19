/* Copyright 2008 Arkadiusz Wahlig (arkadiusz.wahlig@gmail.com)
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
** http://www.apache.org/licenses/LICENSE-2.0
** 
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License. 
*/

#ifndef LISTBOX2_H
#define LISTBOX2_H

#include <Python.h>

PyObject* Listbox2_create(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_count(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_insert(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_finish_insert(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_delete(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_finish_delete(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_top(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_bottom(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_current(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_make_visible(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_select(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_clear_selection(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_selection(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_highlight_rect(PyObject* /*self*/, PyObject *args);
PyObject* Listbox2_empty_text(PyObject* /*self*/, PyObject *args);

#endif /* LISTBOX2_H */
