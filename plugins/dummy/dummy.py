#!/usr/bin/env python3

# this file is just a dummy loader that lets us play with
# pretend data before we write all the plugin backends.
# nothing should be done like it is in here, this is strictly
# dummy data access.

import gi

gi.require_version('Gtk', '3.0')
gi.require_version('Rtfm', '0.0')

from gi.repository import GObject
from gi.repository import Gio
from gi.repository import Rtfm
from gi.repository import Gtk

import os

# used for path lookups
ALL_ITEMS_BY_ID = {}

def path_helper(self):
    parent = self
    parts = []
    while parent is not None:
        parts.append(parent)
        parent = parent.parent
    parts.reverse()

    path = Rtfm.Path()
    for part in parts:
        id = part.props.id or ''
        if not id.startswith('dummy:'):
            id = 'dummy:' + id
        element = Rtfm.PathElement.new(id, part.props.icon_name, part.props.title)
        path.push_element(element)

    return path

class MethodItem(Rtfm.Item):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.props.icon_name = 'lang-function-symbolic'

    def get_children(self):
        return []

    def do_get_path(self):
        return path_helper(self)

class MethodsItem(Rtfm.Item):
    def __init__(self, parent):
        super().__init__()
        self.props.id = parent+':methods'
        self.props.title = 'Methods'
        #self.props.icon_name = 'lang-function-symbolic'

    def get_children(self):
        def create(title):
            return MethodItem(id=self.props.id+':methods:'+title,
                              title=title,
                              icon_name='lang-function-symbolic')
        return [
            create('get_width_chars'),
            create('set_width_chars'),
        ]

    def do_get_path(self):
        return path_helper(self)

class PropertiesItem(Rtfm.Item):
    def __init__(self, parent):
        super().__init__()
        self.props.id = parent+':properties'
        self.props.title = 'Properties'
        #self.props.icon_name = 'lang-property-symbolic'

    def get_children(self):
        def create(title):
            return PropertyItem(id=self.props.id+':properties:'+title,
                                title=title,
                                icon_name='lang-property-symbolic')

        return [
            create('label'),
            create('markup'),
            create('use-markup'),
            create('width-chars'),
            create('max-width-chars'),
            create('justification'),
            create('xalign'),
            create('xpad'),
            create('yalign'),
            create('ypad'),
        ]

    def do_get_path(self):
        return path_helper(self)

class PropertyItem(Rtfm.Item):
    def get_children(self):
        return []

    def do_get_path(self):
        return path_helper(self)

class ClassItem(Rtfm.Item):
    def __init__(self, namespace, title):
        super().__init__()
        self.props.id = 'dummy:'+namespace+'.'+title
        self.props.title = title
        self.props.subtitle = namespace + '.' + title
        self.props.icon_name = 'lang-class-symbolic'

    def get_children(self):
        return [PropertiesItem(self.props.id),
                MethodsItem(self.props.id),
                Rtfm.Item(title='Tasks'),
                Rtfm.Item(title='Examples')]

    def do_get_path(self):
        return path_helper(self)

class ClassesItem(Rtfm.Item):
    def __init__(self, namespace):
        super().__init__()
        self.props.id = 'dummy:'+namespace+':classes'
        self.props.title = 'Classes'
        self.namespace = namespace

    def get_children(self):
        return [ClassItem(self.namespace, 'Box'),
                ClassItem(self.namespace, 'Image'),
                ClassItem(self.namespace, 'Label'),
                ClassItem(self.namespace, 'TextView'),
                ClassItem(self.namespace, 'TreeView'),
                ClassItem(self.namespace, 'Widget'),
                ClassItem(self.namespace, 'Window')]

    def do_get_path(self):
        return path_helper(self)

class FlagsItem(Rtfm.Item):
    def __init__(self, namespace):
        super().__init__()
        self.props.id = 'dummy:'+namespace+':flags'
        self.props.title = 'Flags'
        #self.props.icon_name = 'lang-enum-symbolic'

    def get_children(self):
        return []

    def do_get_path(self):
        return path_helper(self)

class EnumsItem(Rtfm.Item):
    def __init__(self, namespace):
        super().__init__()
        self.props.id = 'dummy:'+namespace+':enums'
        self.props.title = 'Enumerations'
        #self.props.icon_name = 'lang-enum-symbolic'

    def get_children(self):
        return []

    def do_get_path(self):
        return path_helper(self)

class FunctionsItem(Rtfm.Item):
    def __init__(self, namespace):
        super().__init__()
        self.props.id = 'dummy:'+namespace+':functions'
        self.props.title = 'Global Functions'
        #self.props.icon_name = 'lang-function-symbolic'

    def get_children(self):
        return []

    def do_get_path(self):
        return path_helper(self)

class NamespaceItem(Rtfm.Item):
    parent = None

    def get_children(self):
        return [ClassesItem(self.props.title),
                EnumsItem(self.props.title),
                FlagsItem(self.props.title),
                FunctionsItem(self.props.title),
                Rtfm.Item(title='Tutorials')]

    def do_get_path(self):
        path = Rtfm.Path()
        path.push_element(Rtfm.PathElement.new(self.props.id, 'lang-namespace-symbolic', self.props.title))
        return path

class DummyProvider(GObject.Object, Rtfm.Provider):
    def do_load_children_async(self, path, collection, cancellable, callback, data):
        if not path.is_empty():
            index = path.get_length() - 1
            element = path.get_element(index)
            parent = ALL_ITEMS_BY_ID.get(element.props.id)
            if hasattr(parent, 'get_children'):
                for child in parent.get_children():
                    child.parent = parent
                    collection.add(child)
                    ALL_ITEMS_BY_ID[child.props.id] = child
        else:
            files = os.listdir('/usr/share/gir-1.0')
            files.sort()

            for filename in files:
                short = filename[:-4]
                namespace, version = short.split('-')
                item = NamespaceItem(id='dummy:'+short, icon_name='lang-namespace-symbolic', title=namespace, subtitle=short)
                collection.add(item)
                ALL_ITEMS_BY_ID[item.props.id] = item

        task = Gio.Task.new(self, cancellable, callback)
        task.return_boolean(True)

    def do_load_children_finish(self, result):
        return result.propagate_boolean()

    def do_extend_item_async(self, item, cancellable, callback, data):
        task = Gio.Task.new(self, cancellable, callback)
        task.return_boolean(True)

    def do_extend_item_finish(self, result):
        return result.propagate_boolean()

    def do_search_async(self, search_settings, cancellable, callback, data):
        task = Gio.Task.new(self, cancellable, callback)
        task.return_boolean(True)

    def do_search_finish(self, result):
        return result.propagate_boolean()

    def do_load_item(self, id):
        ret = ALL_ITEMS_BY_ID.get(id, None)
        #print("%s %r" % (id, ret))
        return ret

    def do_get_languages(self):
        return ['C', 'JavaScript', 'Lua', 'Python']

class BasicRow(Gtk.ListBoxRow):
    def __init__(self, title):
        super().__init__()
        self.set_visible(True)

        box = Gtk.Box(visible=True)
        self.add(box)

        label = Gtk.Label(visible=True,
                          label=title,
                          xalign=0.0)
        box.add(label)

class ClassRow(BasicRow):
    def __init__(self, klass):
        super().__init__(klass.props.title)
        self.klass = klass

class PropertyRow(BasicRow):
    def __init__(self, prop):
        super().__init__(prop.props.title)
        self.prop = prop

class MethodRow(BasicRow):
    def __init__(self, method):
        super().__init__(method.props.title)
        self.method = method

class DummyViewAddin(GObject.Object, Rtfm.ViewAddin):
    classes_group = None
    methods_group = None
    view = None

    def do_load(self, view):
        self.view = view

        self.classes_group = Rtfm.ViewGroup(title='Classes')
        self.view.add_group(self.classes_group)

        self.properties_group = Rtfm.ViewGroup(title='Properties')
        self.view.add_group(self.properties_group)

        self.methods_group = Rtfm.ViewGroup(title='Methods')
        self.view.add_group(self.methods_group)

    def do_unload(self, view):
        self.view = None

        self.classes_group.destroy()
        self.classes_group = None

        self.methods_group.destroy()
        self.methods_group = None

        self.properties_group.destroy()
        self.properties_group = None

    def do_set_item(self, item):
        self.classes_group.clear()
        self.properties_group.clear()
        self.methods_group.clear()

        self.classes_group.set_visible(isinstance(item, (NamespaceItem, ClassesItem)))
        self.methods_group.set_visible(isinstance(item, ClassItem))
        self.properties_group.set_visible(isinstance(item, ClassItem))

        if not item:
            return

        if not hasattr(item, 'get_children'):
            return

        for child in item.get_children():
            if isinstance(child, ClassesItem):
                for klass in child.get_children():
                    row = ClassRow(klass)
                    self.classes_group.add_row(row)
            elif isinstance(child, ClassItem):
                row = ClassRow(child)
                self.classes_group.add_row(row)
            elif isinstance(child, PropertiesItem):
                for prop in child.get_children():
                    row = PropertyRow(prop)
                    self.properties_group.add_row(row)
            elif isinstance(child, MethodsItem):
                for method in child.get_children():
                    row = MethodRow(method)
                    self.methods_group.add_row(row)

