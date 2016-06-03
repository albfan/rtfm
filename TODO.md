# Rtfm Todo

## Search

We have only created our search interface, we haven't implemented any of the
search indexes. Each plugin will probably want to implement a search index
(if necessary). For example, the G-I plugin will likely create a xapian
index in the background when new .gir files are added. The search query
will access this rather than parse .gir files and manually look. This way
we only parse gir files when necesary (to browse the hierarchy).

## Remove Path Bar

It looks like we won't be needing the path bar since we have the StackList
working relatively well now.

## Views

Only basic plumbing for views exist. We need to create data items when the
view language matches. So the gobject-introspection should add widgets to
the view when the current language is C. Otherwise, the Python version should
add the items to the tree.

## Node Visibility

We should add visibility to RtfmItem so that items can be hidden by plugins
that do not want the item visible. This can be bound to the
RtfmSidebarRow:visible property.

## Source Code Examples

Using the chergert/sightline work, we can start parsing modules from
git.gnome.org to get realistic code examples.

## Tutorials

We need to add tutorials from various yelp/mallard/developer sites as an
another plugin that attaches them to the data objects that matter.

## Editing

We'd like basic editing, which has not been prototyped yet. The goal here
is to be able to click on a text field and suggest an edit. We won't automate
the whole thing, but we should be able to provide developers with something
close to a patch that they can apply to their project.

Worst case, email codedocs@somewhere.

## Flatpak Integration

I'd like to have flatpak integration that lets us browse documentation from
runtime/sdk installed. Since we'll have access to the .gir files in the SDK we
can allow browsing something like:

  Library → SDK x.y → Group → Namespace

## Syntax Viewing

When viewing the documentation (that sometimes includes markdown or gtk-doc
inline code examples) we need to provide something readable. We'll probably
need to create a custom xml/cdata→buffer that can be used to view code.

Again, I'd like this to be editable.

## IDE Integration

We'd like something like "cards" for documentation in Builder. If you click
on a type in GtkSourceView, we should be able to provide some good information
for that type in a card. I'd like this widget to be part of Rtfm so it can
be reused in other places like Gedit.

## Ask for Help, File a bug

It would be nice if we could take people from documentation (where their eyes
will be when tracking down a bug, likely) to filing a bug or asking for more
help on IRC.

For many projects, we'll be able to send the user directly to bugzilla to
file a bug for the particular project.

## API Popularity

Since we'll be using sightline to generate statistics about code on
git.gnome.org, we should also track popularity of API. We can use this
information from Builder to help sort auto-completion results.

## Feedback

It could be as simple as a thumbs-up/thumbs-down for an API documentation
endpoint. Tracking thumbs-down might be useful to see where we need to
spend additional time documenting API.

## Manpages

It might be possible to generate manpages that can be used from VIM.
If we set ~/.manpath, then vim/emacs should be able to pick this up.

## Application Menu

We need to add an application menu with about/help/quit/etc.
