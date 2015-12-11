Item objects are used by the [Listbox2](Listbox2.md) and [Menu](Menu.md) classes to represent listbox items and menu items, respectively.

### Usage ###

```
item = Item(title, ...)
```

Creates a new Item instance.

  * **title** is the item title (unicode) and is mandatory; it can be read or changed after instantiation using the `title` object attribute,
  * If specified, additional keyword arguments are converted into object attributes.

### Example ###

```
menu = Menu()
menu.append(Item(u'Test1.txt', myfile=open('c:\\test1.txt')))
menu.append(Item(u'Test2.txt', myfile=open('c:\\test2.txt')))
item = menu.popup()
if item is not None:
    print item.myfile.read()
```
See the [Menu](Menu.md) class for explanation of its methods.

### Remarks ###

The ability to set any object attributes let you easily store your private data in the items. This is shown in the example above.

See the [Listbox2](Listbox2.md) and [Menu](Menu.md) classes for explanation of some specific attributes they may support or require in an Item object.