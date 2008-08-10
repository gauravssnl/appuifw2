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

#ifndef TEXT2_H
#define TEXT2_H

#include <Python.h>

PyObject* Text2_create(PyObject* /*self*/, PyObject *args);

//PyObject* text_enable_scrollbars(PyObject* /*self*/, PyObject *args);
PyObject* text_select_all(PyObject* /*self*/, PyObject *args);
PyObject* text_set_read_only(PyObject* /*self*/, PyObject *args);
PyObject* text_is_read_only(PyObject* /*self*/, PyObject *args);
PyObject* text_get_selection(PyObject* /*self*/, PyObject *args);
PyObject* text_set_selection(PyObject* /*self*/, PyObject *args);
PyObject* text_get_selection_len(PyObject* /*self*/, PyObject *args);
PyObject* text_clear_selection(PyObject* /*self*/, PyObject *args);
PyObject* text_count_words(PyObject* /*self*/, PyObject *args);
PyObject* text_set_wordwrap(PyObject* /*self*/, PyObject *args);
PyObject* text_set_limit(PyObject* /*self*/, PyObject *args);
PyObject* text_undo(PyObject* /*self*/, PyObject *args);
PyObject* text_clear_undo(PyObject* /*self*/, PyObject *args);
PyObject* text_set_allow_undo(PyObject* /*self*/, PyObject *args);
PyObject* text_supports_undo(PyObject* /*self*/, PyObject *args);
PyObject* text_can_undo(PyObject* /*self*/, PyObject *args);
PyObject* text_get_word_info(PyObject* /*self*/, PyObject *args);
PyObject* text_set_case(PyObject* /*self*/, PyObject *args);
PyObject* text_set_allowed_cases(PyObject* /*self*/, PyObject *args);
PyObject* text_set_input_mode(PyObject* /*self*/, PyObject *args);
PyObject* text_set_allowed_input_modes(PyObject* /*self*/, PyObject *args);
PyObject* text_set_editor_flags(PyObject* /*self*/, PyObject *args);
//PyObject* text_set_alignment(PyObject* /*self*/, PyObject *args);
PyObject* text_set_undoable(PyObject* /*self*/, PyObject *args);
PyObject* text_can_cut(PyObject* /*self*/, PyObject *args);
PyObject* text_cut(PyObject* /*self*/, PyObject *args);
PyObject* text_can_copy(PyObject* /*self*/, PyObject *args);
PyObject* text_copy(PyObject* /*self*/, PyObject *args);
PyObject* text_can_paste(PyObject* /*self*/, PyObject *args);
PyObject* text_paste(PyObject* /*self*/, PyObject *args);
//PyObject* text_set_background_color(PyObject* /*self*/, PyObject *args);
PyObject* text_set_edit_callback(PyObject* /*self*/, PyObject *args);
PyObject* text_set_pos_callback(PyObject* /*self*/, PyObject *args);
PyObject* text_move(PyObject* /*self*/, PyObject *args);
// same as appuifw.Text.set_pos but sets select to true
PyObject* text_sel_pos(PyObject* /*self*/, PyObject *args);
PyObject* text_set_has_changed(PyObject* /*self*/, PyObject *args);
PyObject* text_has_changed(PyObject* /*self*/, PyObject *args);
PyObject* text_test(PyObject* /*self*/, PyObject *args);

#endif /* TEXT2_H */
