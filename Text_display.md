Text\_display represents a native S60 text viewer control. Text\_display objects can be used with either appuifw.app.body or appuifw2.app.body properties.

Text\_display is based on [Text](Text.md) class. As such it provides all its methods and properties but limits the user input capabilities. The cursor and input mode indicators are not visible and the user cannot change the text. Additionally, the up/down arrow keys are used to scroll the text.

### Usage ###

```
text = Text_display(text=u'', skinned=False, scrollbar=False, scroll_by_line=False)
```

Creates a new Text\_display instance.

  * **text** is the text to view,
  * **skinned** if `True`, enables a skinned background, this also changes the text color from dark green to a skin dependent color best suited for the skin in use,
  * **scrollbar** if `True`, enables a vertical scrollbar (see the [Text](Text.md) class remarks),
  * **scroll\_by\_line** if `True`, the up/down arrow keys scroll the text by line instead of page.

### Remarks ###

If you need special formatting, don't use the `text` argument but add the text after creation of the control using the methods and properties provided by the [Text](Text.md) class. Remember to set the cursor position to 0 before displaying the control on the screen.