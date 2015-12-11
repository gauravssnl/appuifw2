Listbox2 represents a native S60 listbox control. Listbox2 objects can be used with either appuifw.app.body or appuifw2.app.body properties.

As opposite to the [Listbox](Listbox.md) and the original appuifw.Listbox, Listbox2 provides methods to dynamically add/replace/remove items and control the listbox view (item displayed at the top). As a `list`-derived class, it features an easy interface to access items. Since items are represented by [Item](Item.md) objects, user data can be easily associated with the items. The Listbox2 also supports empty lists.

For more information about usage of [Item](Item.md) objects in a Listbox2 control, see the **Items** section below.

### Usage ###

```
listbox = Listbox2(items=[], select_callback=None, double=False, icons=False, markable=False)
```

Creates a new Listbox2 instance.

  * **items** is a sequence of [Item](Item.md) objects representing the initial listbox contents,
  * **select\_callback** is a callable called when user presses the Select button,
  * **double**, if True, creates a double lined listbox instead of a single lined one,
  * **icons**, if True, adds icons displayed next to listbox entries (works with either single or double lined listboxes), in such case, the [Item](Item.md) objects must provide an [Icon](Icon.md) object in the `icon` attribute,
  * **markable**, if True, allows the user to mark the items using the Shift key.

### Methods ###

```
begin_update()
```

If a lot of items have to be added/updated/deleted and the listbox is currently visible, this method should be used to suppress multiple view updates. After all items have been processed, `end_update` should be called.


---


```
end_update()
```

Finishes processing started with `begin_update` and updates the listbox view.


---


```
clear()
```

Removes all items.


---


```
append(item)
```

Appends an item to the end of the list.

  * **item** must be an [Item](Item.md) object.


---


```
extend(items)
```

Appends many items to the end of the list.

  * **itmes** must be a sequence of [Item](Item.md) objects.


---


```
insert(position, item)
```

Inserts an item at given position.

  * **position** is the item position, 0 will insert the item at the top.
  * **item** must be an [Item](Item.md) object.


---


```
remove(item)
```

Removes given item from the listbox. If the item isn't found, a `ValueError` exception is raised.

  * **item** is an [Item](Item.md) object to remove.


---


```
pop([position])
```

Pops an item from the listbox and returns it. If position isn't specified, pops the last item. Raises an `IndexError` if the listbox is empty.

  * **position** is an optional item position, last item is popped if omitted; square brackets indicate that the argument is optional.


---


```
sort([cmpfunc])
```

Sorts the items in the listbox. If the compare function isn't provided, sorts alphabetically using a case-insensitive comparison of items titles and subtitles.

  * **cmpfunc** in an optional compare function receiving two [Item](Item.md) objects and returning an integer; for more information refer to Python list.sort function documentation.


---


```
reverse()
```

Reverses the order of items in the listbox.


---


```
current()
```

Returns the index of currently selected item.


---


```
set_current(position)
```

Selects an item.

  * **position** is an index of the item to select.


---


```
current_item()
```

Returns the currently selected [Item](Item.md) object. This is equivalent to `listbox[listbox.current()]`.


---


```
top()
```

Returns an index of the item visible at the top of the screen.


---


```
set_top(position)
```

Moves the list view so that the item at given position is visible at the top of the screen.

  * **position** is the position of the item.


---


```
top_item()
```

Returns the [Item](Item.md) object of the item visible at the top of the screen.


---


```
bottom()
```

Returns an index of the item visible at the bottom of the screen.


---


```
bottom_item()
```

Returns the [Item](Item.md) object of the item visible at the bottom of the screen.


---


```
make_visible(position)
```

Moves the list view so that the item at given position is visible.

  * **position** is the position of the item.


---


```
bind(event_code, callback)
```

Binds a callable to a key. The callable will be called when the key is pressed. The key press event is always passed to the control.

  * **event\_code** is the key code, you can use codes defined in `key_codes` module,
  * **callback** is a callable to be called when the key is pressed.


---


```
marked()
```

Returns a list of indexes of currently marked items.


---


```
marked_items()
```

Returns a list of currently marked [Item](Item.md) objects.


---


```
clear_marked()
```

Unmarks all marked items.


---


```
empty_list_text()
```

Returns the text displayed in the center of the screen when the list is empty.


---


```
set_empty_list_text(text)
```

Sets a new text displayed in the center of the screen when the list is empty.


---


```
highlight_rect()
```

Returns a `(left, top, width, height)` tuple representing the item selection box.

### Remarks ###

Listbox2 class derives from Python `list` type and as such behaves like a `list` object and provides all other `list` methods for manipulation of the items. One can access and replace the items by indexing for example.

### Items ###

Every item in a Listbox2 control is represented by an [Item](Item.md) object.
This kind of object allows the user to freely specify its attributes (mainly to provide a way to store user data in it). However, Listbox2 gives some attributes a special meaning. They are listed below.

  * **title** always represents the title of the item; in a single lined listbox it is the item text, in a double one it is the upper item text
  * **subtitle** is used in double lined listboxes to represent the lower item text; if it is not specified, an empty string is used
  * **icon** is used in icon listboxes and it has to be an [Icon](Icon.md) object; it is required by icon listboxes and ignored by non-icon ones
  * **marked** represents the state of item marking in a markable listbox; it is ignored by non-markable listboxes
  * **current** is True if the item is currently selected (highlighted) and False otherwise

These attributes can also be set to change the appearance of the listbox.