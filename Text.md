Text represents a native S60 text editor control. Text objects can be used with either appuifw.app.body or appuifw2.app.body properties.

The Text class accepts the same parameters and provides the same methods as the original appuifw.Text class so it can be used as a direct replacement for it. This documentation describes only the new methods, refer to the PyS60 documentation for the description of the inherited ones.

### Usage ###

```
text = Text(text=u'', move_callback=None, edit_callback=None, skinned=False,
    scrollbar=False, word_wrap=True, t9=True, indicator=True, fixed_case=False)
```

Creates a new Text instance.

  * **text** is the initial contents of the editor, the cursor remains at the 0 position,
  * **move\_callback** may be a callable which will be called every time the user moves the cursor (moves caused by edit operations are excluded),
  * **edit\_callback** may be a callable accepting two arguments, it will be called every time the contents of the editor change, the arguments will be the position of the change and number of chars inserted (positive) or removed (negative),
  * **skinned** if `True`, enables a skinned background, this also changes the default text color from dark green to a skin dependent color best suited for the skin in use,
  * **scrollbar** if `True`, enables a vertical scrollbar (see remarks),
  * **word\_wrap** if `False`, disables the word wrapping,
  * **t9** if `False`, disables the dictionary input mode,
  * **indicator** if `False`, disables the input mode indicator in the navi pane; this enables the use of navi pane for other purposes (see [Application](Application.md)),
  * **fixed\_case** if `True`, forbids changing of the input case.

### Methods ###

Unchanged methods are **NOT** documented. See PyS60 documentation for more information.


---


```
insert(pos, text)
```

Inserts the text at given position. This is different than moving the cursor and using the `add` method in that it doesn't change the cursor and display position.


---


```
apply(pos=0, length=-1)
```

Applies the current text attributes (color, font, style) to the given text. If arguments are ommited, applies to the whole text.


---


```
get_selection()
```

Returns the current selection as a tuple of three values, `(pos, anchor, text)`, where `pos` is the current cursor position, `anchor` is the position of the other end of the selection (may be higher or lower than the cursor position) and `text` is the selected text. If no text is selected the `anchor` is equal the `pos` and the `text` is equal `u''`.


---


```
set_selection(pos=0, anchor=-1)
```

Sets the current selection, `pos` is the new cursor position and `anchor` is the position of the other end of the selection. By default selects the whole text.


---


```
set_word_wrap(word_wrap)
```

Enables (`True`) or disables (`False`) the word wrapping.


---


```
set_limit(limit)
```

Sets the maximal number of characters that can be entered into the control. Specify `0` to disable the limit.


---


```
get_word_info()
```

Returns the position and length (as a tuple) of the word the cursor is currently in.


---


```
set_allowed_cases(cases)
```

Sets allowed input case modes. Supported cases are:
```
EUpperCase
ELowerCase
ETextCase
EAllCases
```


---


```
set_case(case)
```

Sets the current input case mode. See the `set_allowed_cases()` method for possible values.


---


```
set_allowed_input_modes(modes)
```

Sets allowed input modes. Supported modes are:
```
ENullInputMode
# All text input modes that are available in current language.
ETextInputMode
ENumericInputMode
ESecretAlphaInputMode # not supported yet
# Japanese input modes - only effective in Japanese variant.
EKatakanaInputMode # half-width Katakana
EFullWidthTextInputMode # full-width latin alphabet
EFullWidthNumericInputMode # full-width numeric (0-9)
EFullWidthKatakanaInputMode # full-width Katakana
EHiraganaKanjiInputMode # Hiragana/Kanji
EHiraganaInputMode # only Hiragana
EHalfWidthTextInputMode # half-width Latin alphabet
EAllInputModes
```


---


```

set_input_mode(mode)
```

Sets the current input mode. See the `set_allowed_input_modes()` method for possible values.


---


```
set_undo_buffer(pos=0, length=-1)
```

Sets the cut/paste undo buffer. By default sets the whole text.


---


```
move(direction, select=False)
```

Moves the cursor in one of the given directions. If `select` is `True`, the text between the current and new cursor position is selected.

The directions are:
```
EFNoMovement
EFLeft
EFRight
EFLineUp
EFLineDown
EFPageUp
EFPageDown
EFLineBeg
EFLineEnd
```


---


```
move_display(direction)
```

Moves the display in one of the given directions. See the `move()` method for possible directions.


---


```
xy2pos(coords)
```

Converts the given screen coordinates (relative to the control top-left corner) into the text position. The `coords` is a tuple of X and Y values.


---


```
pos2xy(pos)
```

Converts the given text position into screen coordinates (relative to the control top-left corner). Returns a tuple of X and Y values. Raises a `ValueError` if the given position is out of the view.


---


```
select_all()
```

Selects the whole text.


---


```
clear_selection()
```

Clears the text selection.


---


```
clear_undo()
```

Clears the undo buffer.


---


```
can_undo()
```

Returns `True` if the undo buffer contains a change that can be undone. Otherwise returns `False`.


---


```
can_cut()
```

Returns `True` if text can be cut. Otherwise returns `False`.


---


```
can_copy()
```

Returns `True` if text can be copied. Otherwise returns `False`.


---


```
can_paste()
```

Returns `True` if text can be pasted. Otherwise returns `False`.


---


```
undo()
```

Undoes the last cut/paste change, if any.


---


```
cut()
```

Cuts the selected text (if any) into the clipboard.


---


```
copy()
```

Copies the selected text (if any) into the clipboard.


---


```
paste()
```

Pasts the text from the clipboard (if any) at the current cursor position.

### Properties ###

Unchanged properties are **NOT** documented. See PyS60 documentation for more information.


---


```
read_only
```

If `True`, the text can not be changed (cursor can still be moved). Otherwise `False`.


---


```
has_changed
```

Tells if the text has changed (`True`) or not (`False`) since last time this property was set to `False`.


---


```
allow_undo
```

If `True`, undo operations are allowed. Otherwise `False`.


---


```
indicator_text
```

Text (unicode) displayed next to the input mode indicator in the navi pane. Works only if the indicator was not disabled during object construction (using the `indicator` argument).

New in version 1.0.1.

### Remarks ###

Tests showed that the scrollbar works as long as the text is not longer than ca. 2000 chars. With longer texts the scrollbar can behave unexpectedly.