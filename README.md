About
-----

Xregionsel is a tool developed from scrot which uses scrot's screen,
window, and rectangular selection features to print X11 regions.

Currently we follow the default formating of lolilolicon/xrectsel
, and thus output

  WIDTHxHEIGHT+X+Y

Where X,Y marks the upper left region of the rectangle and
X+WIDTH,Y+HEIGHT marks the lower right region of the rectangle.

In the future formatting options will be added.

The Difference
--------------

While our goals is very similar to xrectsel, we differ by the inclusion of a means to easily 
select a window. This sort of functionality may be added to xrectsel in the future, in which case we will be obsolete.

TODO
----

Code will likely shrink dramatically as we get rid of all image related code.
It is not unlikely that we will switch to the much simpler  xrectsel as the base,
and import the necessary code from scrot. At the least the end code should be of
comparable size.


Compiling
--------

  $ ./configure
  $ make
  $ su -c "make install"

(Red hat users, use $ ./configure --prefix=/usr)
