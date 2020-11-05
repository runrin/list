list
====
`list` is an extremely basic tool for keeping/updating lists in a common
directory. Useful for shopping lists -- you can easily ammend items, make a new
list, edit your list in your favorite editor, print the list to stdout (for
piping to `lp`) without having to type the full path to your list.

Building list
-------------
Edit `list.c` to point to an empty directory on your machine, then build.
Makefile is extremely rudimentary, so you'll want to edit that as well.

Usage
-----
        list [OPTION] item1 item2 ...
          -n    create new list and append items
          -e    echo current list to stdout
          -o    open current list in EDITOR
          -v    display version information
          -h    display this help and exit
