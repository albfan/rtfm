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
    'gir:namespace[Caribou-1.0]':            'platform:a11y',
    'gir:namespace[Cally-1.0]':              'platform:a11y',
    'gir:namespace[Atk-1.0]':                'platform:a11y',
    'gir:namespace[Atspi-2.0]':              'platform:a11y',

    'gir:namespace[GObject-2.0]':            'platform:core',
    'gir:namespace[GLib-2.0]':               'platform:core',
    'gir:namespace[GModule-2.0]':            'platform:core',
    'gir:namespace[Gio-2.0]':                'platform:core',

    'gir:namespace[Ggit-1.0]':               'platform:devel',
    'gir:namespace[Gladeui-2.0]':            'platform:devel',
    'gir:namespace[Vte-2.91]':               'platform:devel',

    'gir:namespace[GtkSource-3.0]':          'platform:text',
    'gir:namespace[GtkSpell-3.0]':           'platform:text',
    'gir:namespace[Pango-1.0]':              'platform:text',
    'gir:namespace[PangoCairo-1.0]':         'platform:text',
    'gir:namespace[PangoXft-1.0]':           'platform:text',
    'gir:namespace[PangoFT2-1.0]':           'platform:text',

    'gir:namespace[Gst-0.10]':               'platform:legacy',
    'gir:namespace[GstBase-0.10]':           'platform:legacy',
    'gir:namespace[GstNet-0.10]':            'platform:legacy',
    'gir:namespace[GstController-0.10]':     'platform:legacy',
    'gir:namespace[GstCheck-0.10]':          'platform:legacy',
    'gir:namespace[GstAudio-0.10]':          'platform:legacy',
    'gir:namespace[GstFft-0.10]':            'platform:legacy',
    'gir:namespace[GstInterfaces-0.10]':     'platform:legacy',
    'gir:namespace[GstNetbuffer-0.10]':      'platform:legacy',
    'gir:namespace[GstPbutils-0.10]':        'platform:legacy',
    'gir:namespace[GstRiff-0.10]':           'platform:legacy',
    'gir:namespace[GstRtp-0.10]':            'platform:legacy',
    'gir:namespace[GstRtsp-0.10]':           'platform:legacy',
    'gir:namespace[GstSdp-0.10]':            'platform:legacy',
    'gir:namespace[GstTag-0.10]':            'platform:legacy',
    'gir:namespace[GstVideo-0.10]':          'platform:legacy',
    'gir:namespace[Cogl-1.0]':               'platform:legacy',

    'gir:namespace[Gtk-2.0]':                'platform:legacy',
    'gir:namespace[Gdk-2.0]':                'platform:legacy',
    'gir:namespace[WebKit-3.0]':             'platform:legacy',
    'gir:namespace[JavaScriptCore-3.0]':     'platform:legacy',
    'gir:namespace[GooCanvas-1.0]':          'platform:legacy',

    'gir:namespace[Goa-1.0]':                'platform:comm',
    'gir:namespace[Camel-1.2]':              'platform:comm',
    'gir:namespace[EBook-1.2]':              'platform:comm',
    'gir:namespace[EBookContext-1.2]':       'platform:comm',
    'gir:namespace[EDataService-1.2]':       'platform:comm',

    'gir:namespace[Cogl-2.0]':               'platform:multimedia',
    'gir:namespace[Gst-1.0]':                'platform:multimedia',
    'gir:namespace[GstAllocators-1.0]':      'platform:multimedia',
    'gir:namespace[GstApp-1.0]':             'platform:multimedia',
    'gir:namespace[GstAudio-1.0]':           'platform:multimedia',
    'gir:namespace[GstBase-1.0]':            'platform:multimedia',
    'gir:namespace[GstCheck-1.0]':           'platform:multimedia',
    'gir:namespace[GstController-1.0]':      'platform:multimedia',
    'gir:namespace[GstFft-1.0]':             'platform:multimedia',
    'gir:namespace[GstGL-1.0]':              'platform:multimedia',
    'gir:namespace[GstInsertBin-1.0]':       'platform:multimedia',
    'gir:namespace[GstMpegts-1.0]':          'platform:multimedia',
    'gir:namespace[GstNet-1.0]':             'platform:multimedia',
    'gir:namespace[GstPbutils-1.0]':         'platform:multimedia',
    'gir:namespace[GstPlayer-1.0]':          'platform:multimedia',
    'gir:namespace[GstRtp-1.0]':             'platform:multimedia',
    'gir:namespace[GstRtsp-1.0]':            'platform:multimedia',
    'gir:namespace[GstSdp-1.0]':             'platform:multimedia',
    'gir:namespace[GstTag-1.0]':             'platform:multimedia',
    'gir:namespace[GstVideo-1.0]':           'platform:multimedia',

    'gir:namespace[Rsvg-2.0]':               'platform:graphics',
    'gir:namespace[Poppler-0.18]':           'platform:graphics',
    'gir:namespace[Gdl-3]':                  'platform:graphics',
    'gir:namespace[Gdk-3.0]':                'platform:graphics',
    'gir:namespace[GdkX11-3.0]':             'platform:graphics',
    'gir:namespace[GdkPixbuf-2.0]':          'platform:graphics',
    'gir:namespace[Gtk-3.0]':                'platform:graphics',
    'gir:namespace[xlib-2.0]':               'platform:graphics',
    'gir:namespace[xrandr-1.3]':             'platform:graphics',
    'gir:namespace[xft-2.0]':                'platform:graphics',
    'gir:namespace[xfixes-4.0]':             'platform:graphics',
    'gir:namespace[freetype2-2.0]':          'platform:graphics',
    'gir:namespace[fontconfig-2.0]':         'platform:graphics',
    'gir:namespace[cairo-1.0]':              'platform:graphics',
    'gir:namespace[GL-1.0]':                 'platform:graphics',
    'gir:namespace[Clutter-1.0]':            'platform:graphics',
    'gir:namespace[ClutterGdk-1.0]':         'platform:graphics',
    'gir:namespace[ClutterX11-1.0]':         'platform:graphics',
    'gir:namespace[GooCanvas-2.0]':          'platform:graphics',

    'gir:namespace[GnomeBluetooth-1.0]':     'platform:networking',
    'gir:namespace[DBus-1.0]':               'platform:networking',
    'gir:namespace[DBusGLib-1.0]':           'platform:networking',
    'gir:namespace[Soup-2.4]':               'platform:networking',
    'gir:namespace[SoupGNOME-2.4]':          'platform:networking',
    'gir:namespace[WebKit2-4.0]':            'platform:networking',
    'gir:namespace[WebKit2WebExtension-4.0': 'platform:networking',
    'gir:namespace[TelepathyGLib-0.12]':     'platform:networking',
    'gir:namespace[GMime-2.6]':              'platform:networking',

    'gir:namespace[JavaScriptCore-4.0]':     'platform:languages',
    'gir:namespace[GIRepository-2.0]':       'platform:languages',
    'gir:namespace[Gee-0.8]':                'platform:languages',

    'gir:namespace[Gck-1]':                  'platform:security',
    'gir:namespace[CryptUI-0.0]':            'platform:security',
    'gir:namespace[Secret-1]':               'platform:security',
    'gir:namespace[Polkit-1.0]':             'platform:security',
    'gir:namespace[PolkitAgent-1.0]':        'platform:security',
    'gir:namespace[Gcr-3]':                  'platform:security',
    'gir:namespace[GcrUi-3]':                'platform:security',
    'gir:namespace[GnomeKeyring-1.0]':       'platform:security',

    'gir:namespace[Json-1.0]':               'platform:formats',
    'gir:namespace[libxml2-2.0]':            'platform:formats',
    'gir:namespace[GCab-1.0]':               'platform:formats',

    'gir:namespace[Peas-1.0]':               'platform:plugins',
    'gir:namespace[PeasGtk-1.0]':            'platform:plugins',

    'gir:namespace[GSystem-1.0]':            'platform:os',
    'gir:namespace[OSTree-1.0]':             'platform:os',
    'gir:namespace[win32-1.0]':              'platform:os',
    'gir:namespace[GUdev-1.0]':              'platform:os',

    'gir:namespace[XdgApp-1.0]':             'platform:pkg',
    'gir:namespace[Flatpak-1.0]':            'platform:pkg',
    'gir:namespace[AppStream-1.0]':          'platform:pkg',
    'gir:namespace[AppStreamBuilder-1.0]':   'platform:pkg',
    'gir:namespace[AppStreamGlib-1.0]':      'platform:pkg',
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
