Menu class is an alternative way to represent a menu in Python. The original way used by the appuifw module is a list of tuples, each defining the item's title and callback with the callback optionally being replaced by a tuple representing a submenu.

The advantage of this model was the lack of need for a new data type. The disadvantages are poor readability, bad expandability and complexity in more advanced usages.

The Menu class tries to fix this.

The class is based on Python `list` type and as such provides all its methods to append/insert/remove the items. There's no point in explaining them here, refer to the Python documentation for more information.

The items in a menu are represented by [Item](Item.md) objects. See the **Items** section below for more information.

### Usage ###

```
menu = Menu(title=u'', items=[])
```

Creates a new Menu instance.

  * **title** is the menu title,
  * **items** can be a list of [Item](Item.md) objects.

### Methods ###

The methods inherited from the Python `list` type are **NOT** explained here.

```
find(...)
```

Returns a list of items whose specific attributes are set to specific values. Accepts only keyword arguments which are used as the attributes to look for.

```
popup(full_screen=False, search_field=False)
```

Pops up the menu on the screen. Uses the title specified during the menu creation. If `full_screen` is `True`, displays a listbox instead of a menu. In this case, the `search_field` can be used to enable a search box.

Returns the selected [Item](Item.md) object or None if the menu was canceled.

```
multi_selection(style='checkbox', search_field=False)
```

Pops up a multi selection listbox on the screen. Uses the title specified during the menu creation. Refer to the `multi_selection_list()` function explanation in the PyS60 documentation for more information about the parameters.

Returns a list of selected [Item](Item.md) objects or None if the listbox was canceled.

```
copy()
```

Returns a copy of the menu, including copied submenus. Item attributes values are not copied.

### Items ###

Every item in a menu is represented by an [Item](Item.md) object. This kind of object allows the user to freely specify its attributes (mainly to provide a way to store user data in it). However, menu gives some attributes a special meaning. They are listed below.

  * **title** always represents the title of the item,
  * **submenu** can be set to a Menu object to make the item open a submenu; note that the application menu supports only one level of submenus whereas popup menus simply popup the submenu after the parent menu is closed,
  * **hidden** if `True`, hides the menu item,
  * **callback** can be set to a callable in application menu; it will be called when the item is selected,
  * **dimmed** if `True`, dimms the menu item (currently in S60 this actually hides the item),
  * **checked** if `True`, a checkmark is displayed next to the item title (works only in application submenus).