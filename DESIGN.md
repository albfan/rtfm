# Rtfm Design

## Goals

 * Support GObject Introspection documentation in various languages.
 * Support for tutorials that are not bundled as part of a particular
   GObject Introspection document set.
 * Support for comments on documentation items.
 * Support for external code examples of how to use API
 * Support for acknowledging the quality of a piece of documentation.
   Thumbs up/down and a short blurb of why.
 * Support to take a user to where they should file a bug on the
   particular piece of code/API.
 * Support for real-time help such as IRC.
 * Fast full-text search with scoring.
 * Support for IDE integration including an assistant for clicking on
   types and displaying information.
 * Support to provide auto-completion information inside of IDEs.
   This is similar to the assistant.
 * It would be nice if this could provide documentation for some additional
   languages such as Python, Ruby, JavaScript/nodejs, and Vala.

## Non-Goals

 * Support for every documentation format created.
 * Support for every hobby programming language API docs.

## Overview

The Rtfm project is a documentation browser with extended capabilities for
editing and collaborating on the improvement of documentation.

While Rtfm supports plugins for data providers, it tries very hard to avoid
exposing "plug-in architecture" into the user interface. Where content comes
from should not be visible from the application. The goal is for everything
to fit together nicely, no matter which plugin provided the data.

## How this works

The central data item in Rtfm is an `RtfmItem`. These can be extended by
`RtfmProvider` plug-ins to load and provide additional information about
an `RtfmItem`.

Such an item might be a method of a class. The core provider for the method
might provide some information coming from a documentation store, such as
GObject Introspection. Additional providers might then extend information on
the object from `RtfmProvider::extend_item_async()`. Such a provider might
load comments, external code examples, and other information which could
be useful to the user.

## Documentation Search

Providers might also want to implement a search backend for the data they
provide. The `RtfmProvider::search_async()` virtual-method should be used
to perform this task.

## Browsing Documentation

Documentation is hierarchical in nature. This works well with browsing and
so the `RtfmProvider::load_children_async()` virtual-method can be used
to the child items as the user browses the documentation hierarchy.

## Displaying Documentation

Not all documentation systems are made equal. Some documentation systems will
need to resort to showing an HTML widget (such as WebKit) and others might
use native `GtkWidget`s to display information.

Because of this, we leave it up to the `RtfmItem` instance (as created by
the particular `RtfmProvider`) to create the `GtkWidget` for display. Some,
will simply return a `WebKitWebView` if they need to render HTML content.

It is up to this widget implementation to display the additional content
provided by the extension data. It's possible that some systems will not
be able to display everything, but this is the side-effect of supporting less
ideal documentation systems.

For those that cannot integrate the additional content into their UI,
we might make a common base class they can use as a `GtkWidget` to simplify
this process (without ruining the experience for those that can integrate it
tightly).




## API Design

### RtfmItem

This is the primary instance type returned by provider plug-ins. It contains
the collection of information about a particular topic.

class PathElement {
	string title;

	/* For popovers */
	void populate_menu (Menu);

	/* If clicked, go to this item */
	Item get_item ();
}

class Path {
	push_element (PathElement);
}

abstract class Item {
	string id;
	string title;
	string subtitle;
	string icon_name;

	Group[] groups;

	HashTable<string,Variant> metadata;

	/* To display the path at the top */
	Path get_path ();

	/* When the item is selected in the sidebar, we need
	 * to create the widget. The widget implementation will
	 * probably want to look at the widget to get more information.
	 */
	Widget create_widget ();

	/*
	 * Add a new group like "Methods" or "Comments".
	 */
	void add_group (Group group);
}

### RtfmGroup

This can be used for extra information on an Item. Things like "Comments"
would be a group that contains information that the view could choose
to add to the view.

interface Group {
	string id;
	string title;

	Widget create_view ();
}

### RtfmProvider

The root interface for plug-ins to implement.

```
interface Provider {
	void load_children_async  (Provider, Item? parent, Collection, Cancellable, Callback, data);
	bool load_children_finish (Provider, Result, out Error);

	void search_async (Provider, SearchQuery, Collection, Cancellable, Callback, data);
	bool search_finish (Provider, Result, out Error);

	void extend_item_async (Item, Cancellable, Callback, data);
	bool extend_item_finish (Item, Result, out Error);

	Item? handle_uri (string uri);
}
```

### RtfmView

This is the common widget used for views.

A view addin can add groups, items to groups, etc.

class View {
	string title;
	Item item;
}

### RtfmViewAddin

If we do ViewAddin's, we can avoid doing so much extensions on the
Item. However, you might want something that adds metadata to the
Item but does not do anything with it until the view is created (and
the ViewAddin can render the additional content).

interface ViewAddin {
	void load   (View);
	void unload (View);
}

### RtfmSearch

class Search {
	string search_text;
}

### RtfmCollection

This is a container class for `RtfmItem` that can be asynchronously populated
by the various provider plug-ins.

It might be nice if this could be thread-safe so that we don't have to deal
with copying items across to the main-loop before adding/removing them.





interface RtfmProvider {
}

class RtfmCollection<RtfmItem> : ListModel {
}

abstract class RtfmItem {
  string id;
  string title;
  string subtitle;
  HashTable<string,GVariant> metadata;
}

class RtfmView : GtkWidget {
  string title;
  void add_group (RtfmViewGroup);
}

interface RtfmViewAddin {
  void load (RtfmView);
  void unload (RtfmView);
}
