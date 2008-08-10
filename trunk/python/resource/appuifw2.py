#
# appuifw2.py - enhanced appuifw
#

import e32
from appuifw import *


# import the C++ module
if e32.s60_version_info >= (3, 0):
    import imp
    _appuifw2 = imp.load_dynamic('_appuifw2', 'c:\\sys\\bin\\_appuifw2.pyd')
    del imp
    
else:
    import _appuifw2


# version
version = '1.00.0'
version_info = tuple(version.split('.'))


# easy way of doing an async call
def schedule(target, *args, **kwargs):
    e32.ao_sleep(0, lambda: target(*args, **kwargs))


# event class with threading.Event interface based on e32.Ao_lock
class Event(object):
    def __init__(self):
        self.lock = e32.Ao_lock()
        self.clear()

    def wait(self):
        if not self.isSet():
            self.lock.wait()

    def set(self):
        self.lock.signal()
        self.signaled = True

    def clear(self):
        self.signaled = False

    def isSet(self):
        return self.signaled

    def __repr__(self):
        return '<%s; set=%s>' % (object.__repr__(self)[1:-1], self.isSet())


# common item class used in Listbox2 and Menu
class Item(object):
    def __init__(self, title, **kwargs):
        kwargs['title'] = title
        self.__dict__.update(kwargs)
        self.__observers = []

    def add_observer(self, observer):
        from weakref import ref
        if ref(observer) not in self.__observers:
            self.__observers.append(ref(observer, self.__del_observer))
            
    def remove_observer(self, observer):
        from weakref import ref
        self.__del_observer(ref(observer))

    def __del_observer(self, ref):
        try:
            self.__observers.remove(ref)
        except ValueError:
            pass

    def __getattribute__(self, name):
        if not name.startswith('_'):
            for obref in self.__observers:
                ob = obref()
                if hasattr(ob, 'handle_item_getattr'):
                    ob.handle_item_getattr(self, name)
        return object.__getattribute__(self, name)

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)
        if not name.startswith('_'):
            for obref in self.__observers:
                ob = obref()
                if hasattr(ob, 'handle_item_setattr'):
                    ob.handle_item_setattr(self, name)

    def __repr__(self):
        return '%s(%s)' % (self.__class__.__name__, self.title)


# Listbox2 control
class Listbox2(list):
    def __init__(self, items=[], select_callback=None, double=False, icons=False, markable=False):
        if double:
            if icons:
                mode = 3
            else:
                mode = 1
        else:
            if icons:
                mode = 2
            else:
                mode = 0
        if markable:
            flags = 0x4001
        else:
            flags = 0
        self.__double = double
        self.__icons = icons
        self.__markable = markable
        list.__init__(self, items)
        self._uicontrolapi = _appuifw2.Listbox2_create(mode, flags, select_callback)
        for item in self:
            self.__item_check(item)
            self.__ui_insert(-1, item)
            item.add_observer(self)

    def __ui_insert(self, pos, item):
        if self.__double:
            s = u'%s\t%s' % (item.title, getattr(item, 'text', u''))
        else:
            s = item.title
        if self.__icons:
            try:
                i = item.icon
            except AttributeError:
                raise TypeError('this listbox requires icons')
        else:
            i = None
        api = self._uicontrolapi
        pos = _appuifw2.Listbox2_insert(api, pos, s, i)
        if self.__markable:
            for i in xrange(len(self)-1, pos, -1):
                _appuifw2.Listbox2_select(api, i,
                    _appuifw2.Listbox2_select(api, i-1))
            _appuifw2.Listbox2_select(api, pos,
                getattr(item, 'selected', False))

    def __ui_delete(self, pos, count=1):
        api = self._uicontrolapi
        if self.__markable:
            for i in xrange(pos+count, len(self)):
                _appuifw2.Listbox2_select(api, i,
                    _appuifw2.Listbox2_select(api, i+count))
        _appuifw2.Listbox2_delete(api, pos, count)

    def __item_check(self, item):
        if not isinstance(item, Item):
            raise TypeError('items must be Item class instances')

    def handle_item_getattr(self, item, name):
        try:
            pos = self.index(item)
        except ValueError:
            return
        if name == 'current':
            item.__dict__[name] = (self.current() == pos)
        elif name == 'selected':
            item.__dict__[name] = _appuifw2.Listbox2_select(self._uicontrolapi, pos)

    def handle_item_setattr(self, item, name):
        try:
            pos = self.index(item)
        except ValueError:
            return
        if name == 'current':
            if item.__dict__[name]:
                self.set_current(pos)
            else:
                item.__dict__[name] = (self.current() == pos)
        elif name == 'selected':
            _appuifw2.Listbox2_select(self._uicontrolapi, pos, item.__dict__[name])
            app.refresh()
        elif name in ('title', 'text', 'icon'):
            self.__setitem__(pos, item)

    def clear(self):
        del self[:]

    def append(self, item):
        self.__item_check(item)
        self.__ui_insert(-1, item)
        item.add_observer(self)
        list.append(self, item)
        app.refresh()
    
    def extend(self, lst):
        for item in lst:
            self.__item_check(item)
            self.__ui_insert(-1, item)
            item.add_observer(self)
        list.extend(self, lst)
        app.refresh()

    def insert(self, pos, item):
        self.__item_check(item)
        list.insert(self, pos, item)
        if pos < 0:
            pos = 0
        elif pos > len(self):
            pos = -1
        self.__ui_insert(pos, item)
        item.add_observer(self)
        app.refresh()

    def remove(self, item):
        pos = list.index(self, item)
        list.remove(self, item)
        self.__ui_delete(pos)
        item.remove_observer(self)
        app.refresh()
        
    def pop(self, pos=-1):
        item = list.pop(self, pos)
        if pos < 0:
            pos = len(self)+pos+1
        elif pos >= len(self):
            pos = -1
        self.__ui_delete(pos)
        item.remove_observer(self)
        app.refresh()
        return item

    def __defcmpfunc(item1, item2):
        s1 = (u'%s%s' % (item1.title, getattr(item1, 'text', u''))).lower()
        s2 = (u'%s%s' % (item2.title, getattr(item2, 'text', u''))).lower()
        return -(s1 < s2)

    def sort(self, cmpfunc=__defcmpfunc):
        list.sort(self, cmpfunc)
        self.__delete(0, len(self))
        for item in self:
            self.__ui_insert(-1, item)
        app.refresh()
        
    def reverse(self):
        list.reverse(self)
        self.__delete(0, len(self))
        for item in self:
            self.__ui_insert(-1, item)
        app.refresh()
        
    def current(self):
        pos = _appuifw2.Listbox2_current(self._uicontrolapi)
        if pos < 0:
            raise IndexError('no item selected')
        return pos
        
    def set_current(self, pos):
        if pos < 0:
            pos += len(self)
        _appuifw2.Listbox2_current(self._uicontrolapi, pos)
        app.refresh()

    def current_item(self):
        return self[self.current()]

    def top(self):
        if not len(self):
            raise IndexError('list is empty')
        return _appuifw2.Listbox2_top(self._uicontrolapi)
                
    def set_top(self, pos):
        if pos < 0:
            pos += len(self)
        if not (0 <= pos < len(self)):
            raise IndexError('index out of range')
        _appuifw2.Listbox2_top(self._uicontrolapi, pos)
        app.refresh()
        
    def top_item(self):
        return self[self.top()]
        
    def bottom(self):
        if not len(self):
            raise IndexError('list is empty')
        return _appuifw2.Listbox2_bottom(self._uicontrolapi)
        
    def bottom_item(self):
        return self[self.bottom()]
        
    def make_visible(self, pos):
        if pos < 0:
            pos += len(self)
        if not (0 <= pos < len(self)):
            raise IndexError('index out of range')
        _appuifw2.Listbox2_make_visible(self._uicontrolapi, pos)
        app.refresh()
        
    def bind(self, key, callable):
        _appuifw2.Listbox2_bind(self._uicontrolapi, key, callable)
        
    def selected(self):
        return _appuifw2.Listbox2_selection(self._uicontrolapi)
        
    def selected_items(self):
        return [self[x] for x in self.selected()]

    def empty_list_text(self):
        return _appuifw2.Listbox2_empty_text(self._uicontrolapi)

    def set_empty_list_text(self, text):
        _appuifw2.Listbox2_empty_text(self._uicontrolapi, text)

    if e32.s60_version_info >= (3, 0):
        def highlight_rect(self):
            return _appuifw2.Listbox2_highlight_rect(self._uicontrolapi)
                
    def __setitem__(self, pos, item):
        olditem = self[pos]
        self.__item_check(item)
        list.__setitem__(self, pos, item)
        olditem.remove_observer(self)
        if pos < 0:
            pos = len(self)+pos
        self.__ui_delete(pos)
        self.__ui_insert(pos, item)
        item.add_observer(self)
        app.refresh()
        
    def __delitem__(self, pos):
        item = self[pos]
        list.__delitem__(self, pos)
        item.remove_observer(self)
        if pos < 0:
            pos = len(self)+pos
        self.__ui_delete(pos)
        app.refresh()
        
    def __setslice__(self, i, j, items):
        olditems = self[i:j]
        list.__setslice__(self, i, j, items)
        for item in olditems:
            item.remove_observer(self)
        i = min(len(self), max(0, i))
        j = min(len(self), max(0, j))
        for pos in xrange(i, j):
            self.__ui_delete(pos)
            self.__ui_insert(pos, self[pos])
        app.refresh()
        
    def __delslice__(self, i, j):
        items = self[i:j]
        size = len(self)
        list.__delslice__(self, i, j)
        for item in items:
            item.remove_observer(self)
        i = min(size, max(0, i))
        j = min(size, max(0, j))
        self.__ui_delete(i, j-i)
        app.refresh()

    def __repr__(self):
        return '<%s instance at 0x%08X; %d items>' % (self.__class__.__name__, len(self))


# a version of Listbox2 providing appuifw.Listbox interface
class Listbox(Listbox2):
    def __init__(self, items, select_callback=None):
        if not isinstance(items, list):
            raise TypeError('argument 1 must be a list')
        if not items:
            raise ValueError('non-empty list expected')
        item = items[0]
        double = False
        icons = False
        if isinstance(item, tuple):
            if len(item) == 2:
                if isinstance(item[1], unicode):
                    double = True
                else:
                    icons = True
            elif len(item) == 3:
                double = True
                icons = True
            else:
                raise ValueError('tuple must include 2 or 3 elements')
        self.__double = double
        self.__icons = icons
        Listbox2.__init__(self, self.__make_items(items), select_callback,
            double, icons)
        
    def __make_items(self, items):
        if self.__double:
            if self.__icons:
                return [Item(a, text=b, icon=c) for a, b, c in items]
            else:
                return [Item(a, text=b) for a, b in items]
        else:
            if self.__icons:
                return [Item(a, icon=b) for a, b in items]
            else:
                return [Item(a) for a in items]

    def set_list(self, items, current=0):
        app.begin_refresh()
        self.clear()
        self.extend(self.__make_items(items))
        try:
            self.set_current(current)
        except IndexError:
            pass
        app.end_refresh()

    if e32.s60_version_info >= (3, 0):

        def __get_size(self):
            return self.highlight_rect()[2:]
            
        size = property(__get_size)
    
        def __get_position(self):
            return self.highlight_rect()[:2]
            
        position = property(__get_position)


# menu class
class Menu(list):
    def __init__(self, title=u'', items=[]):
        if title:
            self.title = title
        else:
            self.title = u''
        list.__init__(self, items)

    def __repr__(self):
        return '%s(%s, %s)' % (self.__class__.__name__, repr(self.title),
            list.__repr__(self))

    def __defcompare(a, b):
        return -(a.title.lower() < b.title.lower())

    def sort(self, compare=__defcompare):
        list.sort(self, compare)

    def find(self, **kwargs):
        items = []
        for item in self:
            for name, val in kwargs.items():
                if not hasattr(item, name) or getattr(item, name) != val:
                    break
            else:
                items.append(item)
        return tuple(items)

    def popup(self, full_screen=False, search_field=False):
        menu = self
        while True:
            items = [x for x in menu if not getattr(x, 'hidden', False)]
            titles = [x.title for x in items]
            if full_screen:
                if menu.title:
                    title = app.title
                    app.title = menu.title
                i = selection_list(titles, search_field)
                if menu.title:
                    app.title = title
            elif menu:
                i = popup_menu(titles, menu.title)
            else:
                i = None
            if i is None or i < 0:
                item = None
                break
            item = items[i]
            try:
                menu = item.submenu
            except AttributeError:
                break
        return item

    def multi_selection(self, style='checkbox', search_field=False):
        items = [x for x in self if not getattr(x, 'hidden', False)]
        titles = [x.title for x in items]
        if menu.title:
            title = app.title
            app.title = menu.title
        r = multi_selection_list(titles, style, search_field)
        if menu.title:
            app.title = title
        return [items[x] for x in r]

    def as_fw_menu(self):
        menu = []
        for item in self:
            if getattr(item, 'hidden', False):
                continue
            try:
                second = item.submenu.as_fw_menu()
            except AttributeError:
                second = getattr(item, 'callback', lambda: None)
            flags = getattr(item, 'flags', 0)
            if getattr(item, 'dimmed', False):
                flags |= 0x1 # EEikMenuItemDimmed
            if getattr(item, 'checked', False):
                flags |= 0x88 # EEikMenuItemCheckBox|EEikMenuItemSymbolOn
            if flags:
                menu.append((item.title, second, flags))
            else:
                menu.append((item.title, second))
        return menu

    def copy(self):
        items = []
        for item in self:
            item = Item(item.__dict__)
            try:
                item.submenu = item.submenu.copy()
            except AttributeError:
                pass
            items.append(item)
        return Menu(self.title, items)


class Application(object):
    def __init__(self):
        global app
        if isinstance(app, self.__class__):
            raise TypeError('%s already instantiated' % self.__class__.__name__)
        self.__left_command = None
        self.__navi_text = u''
        self.__left_navi_arrow = False
        self.__right_navi_arrow = False
        self.__navi = None
        from appuifw import app
        self.__app = app
        for attr in dir(app):
            setattr(self, attr, getattr(app, attr))
        # creates a menu init callback and stores a reference to it
        # (callback is removed if this reference dies)
        # this call also activates the menu flags
        self.__init_menu_callback = \
            _appuifw2.set_menu_init_callback(self.__init_menu)
        self.__refresh_level = 0
        self.__refresh_pending = False

    def begin_refresh(self):
        self.__refresh_level += 1
        
    def end_refresh(self):
        self.__refresh_level -= 1
        if self.__refresh_level <= 0:
            self.__refresh_level = 0
            if self.__refresh_pending:
                _appuifw2.refresh()
                self.__refresh_pending = False

    def refresh(self):
        if self.__refresh_level == 0:
            _appuifw2.refresh()
        else:
            self.__refresh_pending = True
        
    def __get_body(self):
        return self.__app.body
        
    def __set_body(self, value):
        self.__app.body = value

    def __get_exit_key_handler(self):
        return self.__app.exit_key_handler
        
    def __set_exit_key_handler(self, value):
        self.__app.exit_key_handler = value

    def __get_menu(self):
        return getattr(self, '__menu', None)
        
    def __set_menu(self, value):
        self.__menu = value
        
    def __init_menu(self):
        if self.__left_command is not None:
            self.__left_command()
        if not hasattr(self, '__menu'):
            return
        if hasattr(self.__menu, 'as_fw_menu'):
            self.__app.menu = self.__menu.as_fw_menu()
        elif self.__menu is None:
            self.__app.menu = []
        else:
            self.__app.menu = self.__menu

    def __get_screen(self):
        return self.__app.screen
        
    def __set_screen(self, value):
        self.__app.screen = value

    def __get_title(self):
        return self.__app.title
        
    def __set_title(self, value):
        self.__app.title = value

    def __get_focus(self):
        return self.__app.focus
        
    def __set_focus(self, value):
        self.__app.focus = value

    def __get_orientation(self):
        return self.__app.orientation
        
    def __set_orientation(self, value):
        self.__app.orientation = value

    def __get_left_command(self, value):
        return self.__left_command

    def __set_left_command(self, value):
        self.__left_command = value

    def __get_left_command_text(self):
        # EAknSoftkeyOptions
        return _appuifw2.command_text(3000)

    def __set_left_command_text(self, value):
        # EAknSoftkeyOptions
        _appuifw2.command_text(3000, value)

    def __get_right_command_text(self):
        # EAknSoftkeyExit
        return _appuifw2.command_text(3009)

    def __set_right_command_text(self, value):
        # EAknSoftkeyExit
        _appuifw2.command_text(3009, value)

    def __get_navi_text(self):
        return self.__navi_text

    def __set_navi_text(self, value):
        self.__navi_text = value
        self.__set_navi()
        
    def __get_left_navi_arrow(self):
        return self.__left_navi_arrow

    def __set_left_navi_arrow(self, value):
        self.__left_navi_arrow = bool(value)
        self.__set_navi()

    def __get_right_navi_arrow(self):
        return self.__right_navi_arrow

    def __set_right_navi_arrow(self, value):
        self.__right_navi_arrow = bool(value)
        self.__set_navi()

    def __set_navi(self):
        if self.__navi_text or self.__left_navi_arrow or \
                self.__right_navi_arrow:
            self.__navi = _appuifw2.set_navi(self.__navi_text,
                self.__left_navi_arrow, self.__right_navi_arrow)
        else:
            self.__navi = None
        # Note. The self.__navi is a navi pane indicator reference. The
        # indicator is removed if this reference dies.

    body = property(__get_body, __set_body)
    exit_key_handler = property(__get_exit_key_handler, __set_exit_key_handler)
    menu = property(__get_menu, __set_menu)
    screen = property(__get_screen, __set_screen)
    title = property(__get_title, __set_title)
    focus = property(__get_focus, __set_focus)
    orientation = property(__get_orientation, __set_orientation)
    
    left_command = property(__get_left_command, __set_left_command)
    right_command = property(__get_exit_key_handler, __set_exit_key_handler)
    left_command_text = property(__get_left_command_text, __set_left_command_text)
    right_command_text = property(__get_right_command_text, __set_right_command_text)

    navi_text = property(__get_navi_text, __set_navi_text)
    left_navi_arrow = property(__get_left_navi_arrow, __set_left_navi_arrow)
    right_navi_arrow = property(__get_right_navi_arrow, __set_right_navi_arrow)
    

app = Application()
