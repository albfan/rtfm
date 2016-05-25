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

class Category(Rtfm.Item):
    def __init__(self, category_id):
        super().__init__()
        self.props.id = category_id
        self.props.title = _TITLES[self.props.id]

class Categorizer(GObject.Object, Rtfm.Provider):
    def do_load_children_async(self, path, collection, cancellable, callback, data):
        if path.is_empty():
            for id in _TITLES.keys():
                collection.add(Category(id))

        task = Gio.Task.new(self, cancellable, callback)
        task.return_boolean(True)

    def do_load_children_finish(self, task):
        return task.propagate_boolean()

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
