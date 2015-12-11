There is only one instance of the Application class. It is always available in the `app` attribute of the `appuifw2` module. It provides all methods and properties of the same class from the `appuifw` module.

This means that you can use the `appuifw2.app` object in exactly the same way as you used the `appuifw.app` one. Look into the PyS60 documentation for explanation of inherited functionalities.

### Additional and changed properties ###

```
menu
```

Already known from `appuifw` module. In `appuifw2` it can also be set to a [Menu](Menu.md) object or `None` (the latter to disable the menu, same as `[]`).


---


```
init_menu_handler
```

If set to a callable, it is called shortly before the Options menu is opened. The callable can then modify or replace the menu (using the `menu` property). The display is already prepared for the menu when the callback is called. If you don't need a menu and want the Menu key to trigger a different action, use the `menu_key_handler` property instead.

If this property is not used, it is set to `None`.


---


```
menu_key_handler
```

If set to a callable, it is called when the Menu key is pressed. Use this property if you don't need a menu and want the Menu key to trigger a different action. In such case, set the `menu` property to `None` or `[]`. If you want to modify the menu shortly before it is opened, use the `init_menu_handler` property instead.

If this property is not used, it is set to `None`.


---


```
menu_key_text
```

Contains the text (unicode) shown above the left command key (defaults to `u'Options'` in English locale).


---


```
exit_key_text
```

Contains the text (unicode) shown above the right command key (defaults to `u'Exit'` in English locale).


---


```
navi_text
```

Contains the text (unicode) shown in the navi pane (where Calendar application displays the date). The text is only visible when the area is not occupied by other controls.


---


```
left_navi_arrow
```

Enables (`True`) or disables (`False`) the display of the left arrow in the navi pane (such arrow can be seen in the Calendar application next to the date). The arrow is only visible when the area is not occupied by other controls. May be used together with `navi_text` property.


---


```
right_navi_arrow
```

Enables (`True`) or disables (`False`) the display of the right arrow in the navi pane (such arrow can be seen in the Calendar application next to the date). The arrow is only visible when the area is not occupied by other controls. May be used together with `navi_text` property.


---


```
view
```

Contains the current application view object (an instance of the [View](View.md) class or a class derived from it) or `None` (if no views are set). Can be set to a new view object to overlap the current one. When a view is later closed, this property automatically regains the previous value.

See the [View](View.md) class description for more information about application views.