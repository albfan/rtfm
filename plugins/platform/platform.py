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

    def do_cleanup(self, path, collection):
        # If we are in the root path, we want to move the items
        # info our category items.
        if path.is_empty():
            # Create our injected category nodes
            categories = {}
            for id in _TITLES.keys():
                category = Category(id)
                categories[id] = category

            matching = []

            # Look through all the loaded items
            for item in collection:
                idstr = item.props.id
                if idstr is None:
                    continue

                # If we find an item that is a dummy/gir node, then
                # we want to possibly reorder it.
                if idstr.startswith('dummy:') or idstr.startswith('gir:'):
                    category_id = findCategory(idstr)
                    if category_id in categories:
                        categories[category_id].append(item)
                        matching.append(item)

            # Now remove any matching items
            for item in matching:
                collection.remove_item(item)

            # Now add our categories
            for category in categories.values():
                collection.add(category)

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
        if id in _TITLES:
            return Category(id)
        return None

    def do_get_languages(self):
        return []
