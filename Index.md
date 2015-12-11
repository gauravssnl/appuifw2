The appuifw2 module extends the appuifw module. This means that it contains all functions, constants and classes defined in it. However, some of them are replaced (or could be replaced in future versions) by a more functional equivalents. In such cases, the replacements can be used in exactly the same way as the originals but they usually provide additional functionalities or fixes for some bugs. There are also new functionalities not present in appuifw module.

This documentation describes the replaced/added functionalities only. Things that haven't been changed are **NOT** documented. Refer to the PyS60 documentation for more information about them. In general, it is expected that the reader already knows how to use the appuifw module.

### General ###

> The **Application** object available as `appuifw2.app` variable features some new properties.

  * [Application](Application.md)

### UI controls ###

> These are the new or improved UI controls that can be used with either `appuifw.app.body` or `appuifw2.app.body` properties. Refer to the PyS60 documentation to learn more about this property.

  * [Listbox](Listbox.md)
  * [Listbox2](Listbox2.md)
  * [Text](Text.md)
  * [Text\_display](Text_display.md)

### Classes ###

> Classes used to define data structures.

  * [Menu](Menu.md)
  * [Item](Item.md)
  * [View](View.md)

### Miscellaneous ###

> Various other functions.

  * [Misc](Misc.md)