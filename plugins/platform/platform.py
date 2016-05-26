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
    ('platform:crypto',     _("Cryptography")),
    ('platform:multimedia', _("Multimedia")),
    ('platform:networking', _("Networking")),
    ('platform:graphics',   _("Graphics")),
])

_MAPPING = {
    'dummy:Gst-1.0':             'platform:multimedia',
    'dummy:GstAllocators-1.0':   'platform:multimedia',
    'dummy:GstApp-1.0':          'platform:multimedia',
    'dummy:GstAudio-1.0':        'platform:multimedia',
    'dummy:GstBase-1.0':         'platform:multimedia',
    'dummy:GstCheck-1.0':        'platform:multimedia',
    'dummy:GstController-1.0':   'platform:multimedia',
    'dummy:GstFft-1.0':          'platform:multimedia',
    'dummy:GstGL-1.0':           'platform:multimedia',
    'dummy:GstInsertBin-1.0':    'platform:multimedia',
    'dummy:GstMpegts-1.0':       'platform:multimedia',
    'dummy:GstNet-1.0':          'platform:multimedia',
    'dummy:GstPbutils-1.0':      'platform:multimedia',
    'dummy:GstPlayer-1.0':       'platform:multimedia',
    'dummy:GstRtp-1.0':          'platform:multimedia',
    'dummy:GstRtsp-1.0':         'platform:multimedia',
    'dummy:GstSdp-1.0':          'platform:multimedia',
    'dummy:GstTag-1.0':          'platform:multimedia',
    'dummy:GstVideo-1.0':        'platform:multimedia',

    'dummy:Gdk-3.0':             'platform:graphics',
    'dummy:Gtk-3.0':             'platform:graphics',
    'dummy:xlib-2.0':            'platform:graphics',
    'dummy:xrandr-1.3':          'platform:graphics',
}

class Category(Rtfm.Item):
    def __init__(self, category_id):
        super().__init__()
        self.props.id = category_id
        self.props.title = _TITLES[self.props.id]

def findCategory(idstr):
    return _MAPPING.get(idstr, None)

class Categorizer(GObject.Object, Rtfm.Provider):

    # XXX: Add cleanup phase to provider

    def do_postprocess(self, collection):
        path = collection.get_path()

        # if this is the root item then we want to add our nodes to it
        # and then move the other children into the respective categories.
        if path.is_empty():
            categories = {}
            # Requires https://bugzilla.gnome.org/show_bug.cgi?id=766907
            for child in collection:
                category = findCategory(child.props.id)
                if category is not None:
                    if category not in categories:
                        categories[category] = Category(category)
                    categories[category].append(child)
            if categories:
                categories = list(categories.values())
                categories.sort(key=lambda x: x.get_title())
                for category in categories:
                    collection.prepend(category)

    def do_load_item(self, id):
        if id in _TITLES:
            return Category(id)
        return None
