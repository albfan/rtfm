#!/usr/bin/env python3

#
# platform.py
#
# Copyright (C) 2016 Christian Hergert <chris@dronelabs.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import collections
import gi

gi.require_version('Rtfm', '0.0')

from gettext import gettext as _
from gi.repository import Gio
from gi.repository import GObject
from gi.repository import Rtfm

_TITLES = collections.OrderedDict([
    ('platform:a11y',       _("Accessability")),
    ('platform:core',       _("Core")),
    ('platform:devel',      _("Developer Tools")),
    ('platform:formats',    _("File Formats")),
    ('platform:multimedia', _("Multimedia")),
    ('platform:languages',  _("Languages")),
    ('platform:networking', _("Networking")),
    ('platform:os',         _("Operating System")),
    ('platform:pkg',        _("Packaging")),
    ('platform:plugins',    _("Plugins")),
    ('platform:graphics',   _("Graphics")),
    ('platform:security',   _("Security")),
    ('platform:text',       _("Text and Spelling")),
    ('platform:other',      _("Other")),
])

_MAPPING = {
    'dummy:Atk-1.0':                 'platform:a11y',
    'dummy:Atspi-2.0':               'platform:a11y',

    'dummy:GObject-2.0':             'platform:core',
    'dummy:GLib-2.0':                'platform:core',
    'dummy:GModule-2.0':             'platform:core',
    'dummy:Gio-2.0':                 'platform:core',

    'dummy:Ggit-1.0':                'platform:devel',
    'dummy:Gladeui-2.0':             'platform:devel',
    'dummy:Vte-2.91':                'platform:devel',

    'dummy:GtkSource-3.0':           'platform:text',
    'dummy:GtkSpell-3.0':            'platform:text',
    'dummy:Pango-1.0':               'platform:text',
    'dummy:PangoCairo-1.0':          'platform:text',
    'dummy:PangoXft-1.0':            'platform:text',
    'dummy:PangoFT2-1.0':            'platform:text',

    'dummy:Gst-1.0':                 'platform:multimedia',
    'dummy:GstAllocators-1.0':       'platform:multimedia',
    'dummy:GstApp-1.0':              'platform:multimedia',
    'dummy:GstAudio-1.0':            'platform:multimedia',
    'dummy:GstBase-1.0':             'platform:multimedia',
    'dummy:GstCheck-1.0':            'platform:multimedia',
    'dummy:GstController-1.0':       'platform:multimedia',
    'dummy:GstFft-1.0':              'platform:multimedia',
    'dummy:GstGL-1.0':               'platform:multimedia',
    'dummy:GstInsertBin-1.0':        'platform:multimedia',
    'dummy:GstMpegts-1.0':           'platform:multimedia',
    'dummy:GstNet-1.0':              'platform:multimedia',
    'dummy:GstPbutils-1.0':          'platform:multimedia',
    'dummy:GstPlayer-1.0':           'platform:multimedia',
    'dummy:GstRtp-1.0':              'platform:multimedia',
    'dummy:GstRtsp-1.0':             'platform:multimedia',
    'dummy:GstSdp-1.0':              'platform:multimedia',
    'dummy:GstTag-1.0':              'platform:multimedia',
    'dummy:GstVideo-1.0':            'platform:multimedia',

    'dummy:Gdl-3':                   'platform:graphics',
    'dummy:Gdk-3.0':                 'platform:graphics',
    'dummy:GdkX11-3.0':              'platform:graphics',
    'dummy:GdkPixbuf-2.0':           'platform:graphics',
    'dummy:Gtk-3.0':                 'platform:graphics',
    'dummy:xlib-2.0':                'platform:graphics',
    'dummy:xrandr-1.3':              'platform:graphics',
    'dummy:xft-2.0':                 'platform:graphics',
    'dummy:xfixes-4.0':              'platform:graphics',
    'dummy:freetype2-2.0':           'platform:graphics',
    'dummy:fontconfig-2.0':          'platform:graphics',
    'dummy:cairo-1.0':               'platform:graphics',
    'dummy:GL-1.0':                  'platform:graphics',

    'dummy:DBus-1.0':                'platform:networking',
    'dummy:DBusGLib-1.0':            'platform:networking',
    'dummy:Soup-2.4':                'platform:networking',
    'dummy:SoupGNOME-2.4':           'platform:networking',
    'dummy:WebKit2-4.0':             'platform:networking',
    'dummy:WebKit2WebExtension-4.0': 'platform:networking',
    'dummy:TelepathyGLib-0.12':      'platform:networking',

    'dummy:JavaScriptCore-4.0':      'platform:languages',
    'dummy:GIRepository-2.0':        'platform:languages',
    'dummy:Gee-0.8':                 'platform:languages',

    'dummy:Gck-1':                   'platform:security',
    'dummy:CryptUI-0.0':             'platform:security',
    'dummy:Secret-1':                'platform:security',
    'dummy:Polkit-1.0':              'platform:security',
    'dummy:PolkitAgent-1.0':         'platform:security',
    'dummy:Gcr-3':                   'platform:security',
    'dummy:GcrUi-3':                 'platform:security',

    'dummy:Json-1.0':                'platform:formats',
    'dummy:libxml2-2.0':             'platform:formats',
    'dummy:GCab-1.0':                'platform:formats',

    'dummy:Peas-1.0':                'platform:plugins',
    'dummy:PeasGtk-1.0':             'platform:plugins',

    'dummy:GSystem-1.0':             'platform:os',
    'dummy:OSTree-1.0':              'platform:os',
    'dummy:win32-1.0':               'platform:os',
    'dummy:GUdev-1.0':               'platform:os',

    'dummy:XdgApp-1.0':              'platform:pkg',
    'dummy:Flatpak-1.0':             'platform:pkg',
    'dummy:AppStream-1.0':           'platform:pkg',
    'dummy:AppStreamBuilder-1.0':    'platform:pkg',
    'dummy:AppStreamGlib-1.0':       'platform:pkg',
}

class Category(Rtfm.Item):
    def __init__(self, category_id):
        super().__init__()
        self.props.id = category_id
        self.props.title = _TITLES[self.props.id]

def findCategory(idstr):
    return _MAPPING.get(idstr, 'platform:other')

def itermodel(model):
    for i in range(model.get_n_items()):
        yield model.get_item(i)

class Categorizer(GObject.Object, Rtfm.Provider):

    def do_postprocess(self, collection):
        path = collection.get_path()

        # if this is the root item then we want to add our nodes to it
        # and then move the other children into the respective categories.
        if path.is_empty():
            matched = []
            categories = {}
            for child in itermodel(collection):
                category = findCategory(child.props.id)
                if category is not None:
                    if category not in categories:
                        categories[category] = Category(category)
                    categories[category].append(child)
                    matched.append(child)
            for match in matched:
                collection.remove_item(match)
            if categories:
                categories = list(categories.values())
                categories.sort(key=lambda x: x.get_title(), reverse=True)
                for category in categories:
                    collection.prepend(category)

    def do_load_item(self, id):
        # TODO: Not really sufficient for loading from an item when restoring
        #       a previous item at startup. We need to ensure that the first
        #       layer is loaded and that we have cached the items.
        if id in _TITLES:
            return Category(id)
        return None
