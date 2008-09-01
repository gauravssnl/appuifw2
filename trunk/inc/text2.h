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
PyObject* Text2_clear(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_text(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_text(PyObject* /*self*/, PyObject *args);
PyObject* Text2_add_text(PyObject* /*self*/, PyObject *args);
PyObject* Text2_insert_text(PyObject* /*self*/, PyObject *args);
PyObject* Text2_delete_text(PyObject* /*self*/, PyObject *args);
PyObject* Text2_apply(PyObject* /*self*/, PyObject *args);
PyObject* Text2_text_length(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_pos(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_pos(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_focus(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_focus(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_style(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_style(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_color(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_color(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_highlight_color(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_highlight_color(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_font(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_font(PyObject* /*self*/, PyObject *args);
PyObject* Text2_select_all(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_read_only(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_read_only(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_selection(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_selection(PyObject* /*self*/, PyObject *args);
PyObject* Text2_clear_selection(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_word_wrap(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_limit(PyObject* /*self*/, PyObject *args);
PyObject* Text2_undo(PyObject* /*self*/, PyObject *args);
PyObject* Text2_clear_undo(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_allow_undo(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_allow_undo(PyObject* /*self*/, PyObject *args);
PyObject* Text2_can_undo(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_word_info(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_case(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_allowed_cases(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_input_mode(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_allowed_input_modes(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_editor_flags(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_undo_buffer(PyObject* /*self*/, PyObject *args);
PyObject* Text2_can_cut(PyObject* /*self*/, PyObject *args);
PyObject* Text2_cut(PyObject* /*self*/, PyObject *args);
PyObject* Text2_can_copy(PyObject* /*self*/, PyObject *args);
PyObject* Text2_copy(PyObject* /*self*/, PyObject *args);
PyObject* Text2_can_paste(PyObject* /*self*/, PyObject *args);
PyObject* Text2_paste(PyObject* /*self*/, PyObject *args);
PyObject* Text2_move(PyObject* /*self*/, PyObject *args);
PyObject* Text2_move_display(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_has_changed(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_has_changed(PyObject* /*self*/, PyObject *args);
PyObject* Text2_xy2pos(PyObject* /*self*/, PyObject *args);
PyObject* Text2_pos2xy(PyObject* /*self*/, PyObject *args);
PyObject* Text2_get_indicator_text(PyObject* /*self*/, PyObject *args);
PyObject* Text2_set_indicator_text(PyObject* /*self*/, PyObject *args);

#endif /* TEXT2_H */
