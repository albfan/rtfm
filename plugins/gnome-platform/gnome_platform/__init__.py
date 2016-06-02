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
    ('platform:comm',       _("Communication")),
    ('platform:os',         _("Operating System")),
    ('platform:pkg',        _("Packaging")),
    ('platform:plugins',    _("Plugins")),
    ('platform:graphics',   _("Graphics")),
    ('platform:security',   _("Security")),
    ('platform:text',       _("Text and Spelling")),
    ('platform:legacy',     _("Legacy")),
    ('platform:other',      _("Other")),
])

_MAPPING = {
    'gir:Caribou-1.0':             'platform:a11y',
    'gir:Cally-1.0':               'platform:a11y',
    'gir:Atk-1.0':                 'platform:a11y',
    'gir:Atspi-2.0':               'platform:a11y',

    'gir:GObject-2.0':             'platform:core',
    'gir:GLib-2.0':                'platform:core',
    'gir:GModule-2.0':             'platform:core',
    'gir:Gio-2.0':                 'platform:core',

    'gir:Ggit-1.0':                'platform:devel',
    'gir:Gladeui-2.0':             'platform:devel',
    'gir:Vte-2.91':                'platform:devel',

    'gir:GtkSource-3.0':           'platform:text',
    'gir:GtkSpell-3.0':            'platform:text',
    'gir:Pango-1.0':               'platform:text',
    'gir:PangoCairo-1.0':          'platform:text',
    'gir:PangoXft-1.0':            'platform:text',
    'gir:PangoFT2-1.0':            'platform:text',

    'gir:Gst-0.10':                'platform:legacy',
    'gir:GstBase-0.10':            'platform:legacy',
    'gir:GstNet-0.10':             'platform:legacy',
    'gir:GstController-0.10':      'platform:legacy',
    'gir:GstCheck-0.10':           'platform:legacy',
    'gir:GstAudio-0.10':           'platform:legacy',
    'gir:GstFft-0.10':             'platform:legacy',
    'gir:GstInterfaces-0.10':      'platform:legacy',
    'gir:GstNetbuffer-0.10':       'platform:legacy',
    'gir:GstPbutils-0.10':         'platform:legacy',
    'gir:GstRiff-0.10':            'platform:legacy',
    'gir:GstRtp-0.10':             'platform:legacy',
    'gir:GstRtsp-0.10':            'platform:legacy',
    'gir:GstSdp-0.10':             'platform:legacy',
    'gir:GstTag-0.10':             'platform:legacy',
    'gir:GstVideo-0.10':           'platform:legacy',
    'gir:Cogl-1.0':                'platform:legacy',

    'gir:Gtk-2.0':                 'platform:legacy',
    'gir:Gdk-2.0':                 'platform:legacy',
    'gir:WebKit-3.0':              'platform:legacy',
    'gir:JavaScriptCore-3.0':      'platform:legacy',
    'gir:GooCanvas-1.0':           'platform:legacy',

    'gir:Goa-1.0':                 'platform:comm',
    'gir:Camel-1.2':               'platform:comm',
    'gir:EBook-1.2':               'platform:comm',
    'gir:EBookContext-1.2':        'platform:comm',
    'gir:EDataService-1.2':        'platform:comm',

    'gir:Cogl-2.0':                'platform:multimedia',
    'gir:Gst-1.0':                 'platform:multimedia',
    'gir:GstAllocators-1.0':       'platform:multimedia',
    'gir:GstApp-1.0':              'platform:multimedia',
    'gir:GstAudio-1.0':            'platform:multimedia',
    'gir:GstBase-1.0':             'platform:multimedia',
    'gir:GstCheck-1.0':            'platform:multimedia',
    'gir:GstController-1.0':       'platform:multimedia',
    'gir:GstFft-1.0':              'platform:multimedia',
    'gir:GstGL-1.0':               'platform:multimedia',
    'gir:GstInsertBin-1.0':        'platform:multimedia',
    'gir:GstMpegts-1.0':           'platform:multimedia',
    'gir:GstNet-1.0':              'platform:multimedia',
    'gir:GstPbutils-1.0':          'platform:multimedia',
    'gir:GstPlayer-1.0':           'platform:multimedia',
    'gir:GstRtp-1.0':              'platform:multimedia',
    'gir:GstRtsp-1.0':             'platform:multimedia',
    'gir:GstSdp-1.0':              'platform:multimedia',
    'gir:GstTag-1.0':              'platform:multimedia',
    'gir:GstVideo-1.0':            'platform:multimedia',

    'gir:Rsvg-2.0':                'platform:graphics',
    'gir:Poppler-0.18':            'platform:graphics',
    'gir:Gdl-3':                   'platform:graphics',
    'gir:Gdk-3.0':                 'platform:graphics',
    'gir:GdkX11-3.0':              'platform:graphics',
    'gir:GdkPixbuf-2.0':           'platform:graphics',
    'gir:Gtk-3.0':                 'platform:graphics',
    'gir:xlib-2.0':                'platform:graphics',
    'gir:xrandr-1.3':              'platform:graphics',
    'gir:xft-2.0':                 'platform:graphics',
    'gir:xfixes-4.0':              'platform:graphics',
    'gir:freetype2-2.0':           'platform:graphics',
    'gir:fontconfig-2.0':          'platform:graphics',
    'gir:cairo-1.0':               'platform:graphics',
    'gir:GL-1.0':                  'platform:graphics',
    'gir:Clutter-1.0':             'platform:graphics',
    'gir:ClutterGdk-1.0':          'platform:graphics',
    'gir:ClutterX11-1.0':          'platform:graphics',
    'gir:GooCanvas-2.0':           'platform:graphics',

    'gir:GnomeBluetooth-1.0':      'platform:networking',
    'gir:DBus-1.0':                'platform:networking',
    'gir:DBusGLib-1.0':            'platform:networking',
    'gir:Soup-2.4':                'platform:networking',
    'gir:SoupGNOME-2.4':           'platform:networking',
    'gir:WebKit2-4.0':             'platform:networking',
    'gir:WebKit2WebExtension-4.0': 'platform:networking',
    'gir:TelepathyGLib-0.12':      'platform:networking',
    'gir:GMime-2.6':               'platform:networking',

    'gir:JavaScriptCore-4.0':      'platform:languages',
    'gir:GIRepository-2.0':        'platform:languages',
    'gir:Gee-0.8':                 'platform:languages',

    'gir:Gck-1':                   'platform:security',
    'gir:CryptUI-0.0':             'platform:security',
    'gir:Secret-1':                'platform:security',
    'gir:Polkit-1.0':              'platform:security',
    'gir:PolkitAgent-1.0':         'platform:security',
    'gir:Gcr-3':                   'platform:security',
    'gir:GcrUi-3':                 'platform:security',
    'gir:GnomeKeyring-1.0':        'platform:security',

    'gir:Json-1.0':                'platform:formats',
    'gir:libxml2-2.0':             'platform:formats',
    'gir:GCab-1.0':                'platform:formats',

    'gir:Peas-1.0':                'platform:plugins',
    'gir:PeasGtk-1.0':             'platform:plugins',

    'gir:GSystem-1.0':             'platform:os',
    'gir:OSTree-1.0':              'platform:os',
    'gir:win32-1.0':               'platform:os',
    'gir:GUdev-1.0':               'platform:os',

    'gir:XdgApp-1.0':              'platform:pkg',
    'gir:Flatpak-1.0':             'platform:pkg',
    'gir:AppStream-1.0':           'platform:pkg',
    'gir:AppStreamBuilder-1.0':    'platform:pkg',
    'gir:AppStreamGlib-1.0':       'platform:pkg',
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
