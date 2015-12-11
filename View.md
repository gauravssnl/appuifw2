Every more advanced PyS60 application (or any application for that matter) needs more than one view. You pretty fast get into situation where you have to change the contents of the screen and you know you will have to restore them later. Normally in such case you have to back up all `appuifw.app` attributes, replace them with new values and restore the backed up values later. Appuifw2's views were designed to simplify such situations.

A view in appuifw2 is an object of a View class or a class derived from it. It has most of the attributes the `appuifw2.app` object provides but they are private to that one application view. May views can be created simultaneously and easily switched.

The application manages a stack of views. When a new view is set, it is added to the stack. This makes it easy to return to the previous view, simply by closing the current one. Close is always the default function of the Exit key (the Exit key label can easily be changed using the `exit_key_text` property).

The views are controlled by a single property of the [Application](Application.md) object, the `appuifw2.app.view`. Assigning a view object to it adds it to the views stack and displays the view on screen. Reading yields the current view object. If no views are set, `None` is returned.

When the first view is set, the current application state is saved. It is then restored when the last view is closed.

### Usage ###

You can instantiate the View class directly but usually you will want to subclass it. This will allow you to initialize the view's properties in its `__init__` method and use the object as a container for callbacks and other view specific functions (and variables).

Example subclass could look like this.

```
import appuifw2

class Main(appuifw2.View):
    def __init__(self):
        # call the superclass __init__()
        appuifw2.View.__init__(self)
        # initialize the properties
        self.title = u'Main view'
```

The init method sets the view title to "Main view".

After the class is defined, we can create the view object.

```
myview = Main()
```

To make the view visible on the screen, we pass it to the `view` property of the [Application](Application.md) object.

```
appuifw2.app.view = myview
```

To block the script until the view is closed, we can use the `wait_for_close` method.

```
myview.wait_for_close()
```

### Methods ###

```
set_tabs(tab_texts, callback=None)
activate_tab(index)
```

Same as respective methods of the [Application](Application.md) object but private to the view.


---


```
close()
```

Closes the view and removes it from the application views stack. This will reveal the previous view or restore the original values of [Application](Application.md) object properties.

This method is the default `exit_key_handler` property value which means that the view will be closed when the Exit key is pressed. Set the `exit_key_handler` property to `None` or other callable to disable this feature.


---


```
wait_for_close()
```

Sleeps until the `close()` method is called.


---


```
shown()
```

Called by the framework when the view appears on the screen. This happens when the view is passed to the `appuifw2.app.view` property or when it reappears after being overlapped by another view.

The default implementation does nothing.


---


```
hidden()
```

Called by the framework when the view disappears from the screen. This happens when  the view is overlapped by another view or when it is being closed.

The default implementation does nothing.


---


```
init_menu()
```

The default `init_menu_handler` property value. Provided for subclass convenience.

The default implementation does nothing.


---


```
handle_menu_key()
```

The default `menu_key_handler` property value. Provided for subclass convenience.

The default implementation does nothing.

### Properties ###

```
body
exit_key_handler
menu
screen
title
init_menu_handler
menu_key_handler
menu_key_text
exit_key_text
navi_text
left_navi_arrow
right_navi_arrow
```

Same as respective properties of the [Application](Application.md) object but private to the view.