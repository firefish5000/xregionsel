/* main.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "xregionsel.h"
#include "options.h"


/*typedef struct
{
	int rx;
	int ry;
	int rw;
	int rh;
}Selection;
*/
Selection NullRegion = {0, 0, 0, 0};

int
main(int argc,
     char **argv)
{
  Selection region = NullRegion;

  init_parse_options(argc, argv);

  init_x_and_imlib(NULL, 0);

  if (opt.focused)
    region = xregionsel_grab_focused();
  else if (opt.select)
    region = xregionsel_sel_and_grab_image();
//  else
//    region = xregionsel_sel_and_grab_image();
  else {
//    xregionsel_do_delay();
//    if (opt.multidisp) {
//      region = xregionsel_grab_shot_multi();
//    } else {
      region = xregionsel_grab_shot();
//    }
  }

  if ( region.rx == NullRegion.rx
		  && region.ry == NullRegion.ry
		  && region.rh == NullRegion.rh
		  && region.rw == NullRegion.rw)
    gib_eprintf("no region grabbed");

  printf("%dx%d+%d+%d\n", region.rw, region.rh, region.rx,region.ry);

//  if (opt.exec)
//    xregionsel_exec_app(image, tm, filename_im, filename_thumb);

  return 0;
}

Selection
xregionsel_grab_shot(void)
{
  Selection region;

  region.rx = 0;
  region.ry = 0;
  region.rw = scr->width;
  region.rh = scr->height;
  return region;
}
//void
//xregionsel_exec_app(Imlib_Image image, struct tm *tm,
//               char *filename_im, char *filename_thumb)
//{
//  char *execstr;
//
//  execstr = im_printf(opt.exec, tm, filename_im, filename_thumb, image);
//  system(execstr);
//  exit(0);
//}

Selection
xregionsel_grab_focused(void)
{
  Selection client_region = NullRegion;
  int rx = 0, ry = 0, rw = 0, rh = 0;
  Window target = None;
  int ignored;

  //xregionsel_do_delay();
  XGetInputFocus(disp, &target, &ignored);
  if (!xregionsel_get_geometry(target, &rx, &ry, &rw, &rh)) return NullRegion;
  xregionsel_nice_clip(&rx, &ry, &rw, &rh);
  client_region.rx=rx;
  client_region.ry=ry;
  client_region.rw=rw;
  client_region.rh=rh;
  return client_region;
}

Selection
xregionsel_sel_and_grab_image(void)
{
  Selection sel_region = NullRegion;
  static int xfd = 0;
  static int fdsize = 0;
  XEvent ev;
  fd_set fdset;
  int count = 0, done = 0;
  int rx = 0, ry = 0, rw = 0, rh = 0, btn_pressed = 0;
  int rect_x = 0, rect_y = 0, rect_w = 0, rect_h = 0;
  Cursor cursor, cursor2;
  Window target = None;
  GC gc;
  XGCValues gcval;

  xfd = ConnectionNumber(disp);
  fdsize = xfd + 1;

  cursor = XCreateFontCursor(disp, XC_left_ptr);
  cursor2 = XCreateFontCursor(disp, XC_lr_angle);

  gcval.foreground = XWhitePixel(disp, 0);
  gcval.function = GXxor;
  gcval.background = XBlackPixel(disp, 0);
  gcval.plane_mask = gcval.background ^ gcval.foreground;
  gcval.subwindow_mode = IncludeInferiors;

  gc =
    XCreateGC(disp, root,
              GCFunction | GCForeground | GCBackground | GCSubwindowMode,
              &gcval);

  if ((XGrabPointer
       (disp, root, False,
        ButtonMotionMask | ButtonPressMask | ButtonReleaseMask, GrabModeAsync,
        GrabModeAsync, root, cursor, CurrentTime) != GrabSuccess))
    gib_eprintf("couldn't grab pointer:");

  if ((XGrabKeyboard
       (disp, root, False, GrabModeAsync, GrabModeAsync,
        CurrentTime) != GrabSuccess))
    gib_eprintf("couldn't grab keyboard:");

	/* TODO Show CrossHair cursor (like xprop) to show we are active.
	 * Switch to RightAngle on drag. */
  while (1) {
    /* handle events here */
    while (!done && XPending(disp)) {
      XNextEvent(disp, &ev);
      switch (ev.type) {
        case MotionNotify:
          if (btn_pressed) {
            if (rect_w) {
              /* re-draw the last rect to clear it */
              XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
            } else {
              /* Change the cursor to show we're selecting a region */
              XChangeActivePointerGrab(disp,
                                       ButtonMotionMask | ButtonReleaseMask,
                                       cursor2, CurrentTime);
            }

            rect_x = rx;
            rect_y = ry;
            rect_w = ev.xmotion.x - rect_x;
            rect_h = ev.xmotion.y - rect_y;

            if (rect_w < 0) {
              rect_x += rect_w;
              rect_w = 0 - rect_w;
            }
            if (rect_h < 0) {
              rect_y += rect_h;
              rect_h = 0 - rect_h;
            }
            /* draw rectangle */
            XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
            XFlush(disp);
          }
          break;
        case ButtonPress:
          btn_pressed = 1;
          rx = ev.xbutton.x;
          ry = ev.xbutton.y;
          target =
            xregionsel_get_window(disp, ev.xbutton.subwindow, ev.xbutton.x,
                             ev.xbutton.y);
          if (target == None)
            target = root;
          break;
        case ButtonRelease:
          done = 1;
          break;
        case KeyPress:
          fprintf(stderr, "Key was pressed, aborting shot\n");
          done = 2;
          break;
        case KeyRelease:
          /* ignore */
          break;
        default:
          break;
      }
    }
    if (done)
      break;

    /* now block some */
    FD_ZERO(&fdset);
    FD_SET(xfd, &fdset);
    errno = 0;
    count = select(fdsize, &fdset, NULL, NULL, NULL);
    if ((count < 0)
        && ((errno == ENOMEM) || (errno == EINVAL) || (errno == EBADF)))
      gib_eprintf("Connection to X display lost");
  }
  if (rect_w) {
    XDrawRectangle(disp, root, gc, rect_x, rect_y, rect_w, rect_h);
    XFlush(disp);
  }
  XUngrabPointer(disp, CurrentTime);
  XUngrabKeyboard(disp, CurrentTime);
  XFreeCursor(disp, cursor);
  XFreeGC(disp, gc);
  XSync(disp, True);


  if (done < 2) {
    if (rect_w > 5) {
      /* if a rect has been drawn, it's an area selection */
      rw = ev.xbutton.x - rx;
      rh = ev.xbutton.y - ry;

      if (rw < 0) {
        rx += rw;
        rw = 0 - rw;
      }
      if (rh < 0) {
        ry += rh;
        rh = 0 - rh;
      }
    } else {
      /* else it's a window click */
      if (!xregionsel_get_geometry(target, &rx, &ry, &rw, &rh)) return NullRegion;
    }
    xregionsel_nice_clip(&rx, &ry, &rw, &rh);

    if (! opt.silent) XBell(disp, 0);
	sel_region.rx=rx;
	sel_region.ry=ry;
	sel_region.rw=rw;
	sel_region.rh=rh;
  }
  return sel_region;
}

/* clip rectangle nicely */
void
xregionsel_nice_clip(int *rx, 
		int *ry, 
		int *rw, 
		int *rh)
{
  if (*rx < 0) {
    *rw += *rx;
    *rx = 0;
  }
  if (*ry < 0) {
    *rh += *ry;
    *ry = 0;
  }
  if ((*rx + *rw) > scr->width)
    *rw = scr->width - *rx;
  if ((*ry + *rh) > scr->height)
    *rh = scr->height - *ry;
}

/* get geometry of window and use that */
int
xregionsel_get_geometry(Window target,
		   int *rx, 
		   int *ry, 
		   int *rw, 
		   int *rh)
{
  Window child;
  XWindowAttributes attr;
  int stat;

  /* get windowmanager frame of window */
  if (target != root) {
    unsigned int d;
    int x;
    int status;
    
    status = XGetGeometry(disp, target, &root, &x, &x, &d, &d, &d, &d);
    if (status != 0) {
      Window rt, *children, parent;
      
      for (;;) {
	/* Find window manager frame. */
	status = XQueryTree(disp, target, &rt, &parent, &children, &d);
	if (status && (children != None))
	  XFree((char *) children);
	if (!status || (parent == None) || (parent == rt))
	  break;
	target = parent;
      }
      /* Get client window. */
      if (!opt.border)
	target = xregionsel_get_client_window(disp, target);
      XRaiseWindow(disp, target);
    }
  }
  stat = XGetWindowAttributes(disp, target, &attr);
  if ((stat == False) || (attr.map_state != IsViewable))
    return 0;
  *rw = attr.width;
  *rh = attr.height;
  XTranslateCoordinates(disp, target, root, 0, 0, rx, ry, &child);
  return 1;
}

Window
xregionsel_get_window(Display * display,
                 Window window,
                 int x,
                 int y)
{
  Window source, target;

  int status, x_offset, y_offset;

  source = root;
  target = window;
  if (window == None)
    window = root;
  while (1) {
    status =
      XTranslateCoordinates(display, source, window, x, y, &x_offset,
                            &y_offset, &target);
    if (status != True)
      break;
    if (target == None)
      break;
    source = window;
    window = target;
    x = x_offset;
    y = y_offset;
  }
  if (target == None)
    target = window;
  return (target);
}


Window
xregionsel_get_client_window(Display * display,
                        Window target)
{
  Atom state;
  Atom type = None;
  int format, status;
  unsigned char *data;
  unsigned long after, items;
  Window client;

  state = XInternAtom(display, "WM_STATE", True);
  if (state == None)
    return target;
  status =
    XGetWindowProperty(display, target, state, 0L, 0L, False,
                       (Atom) AnyPropertyType, &type, &format, &items, &after,
                       &data);
  if ((status == Success) && (type != None))
    return target;
  client = xregionsel_find_window_by_property(display, target, state);
  if (!client)
    return target;
  return client;
}

Window
xregionsel_find_window_by_property(Display * display,
                              const Window window,
                              const Atom property)
{
  Atom type = None;
  int format, status;
  unsigned char *data;
  unsigned int i, number_children;
  unsigned long after, number_items;
  Window child = None, *children, parent, root;

  status =
    XQueryTree(display, window, &root, &parent, &children, &number_children);
  if (!status)
    return None;
  for (i = 0; (i < number_children) && (child == None); i++) {
    status =
      XGetWindowProperty(display, children[i], property, 0L, 0L, False,
                         (Atom) AnyPropertyType, &type, &format,
                         &number_items, &after, &data);
    if (data)
      XFree(data);
    if ((status == Success) && (type != (Atom) NULL))
      child = children[i];
  }
  for (i = 0; (i < number_children) && (child == None); i++)
    child = xregionsel_find_window_by_property(display, children[i], property);
  if (children != None)
    XFree(children);
  return (child);
}
/* // TODO Multi Region Support
Selection
xregionsel_grab_shot_multi(void)
{
  int screens;
  int i;
  char *dispstr, *subdisp;
  char newdisp[255];
  gib_list *images = NULL;
  Selection ret_region = NullRegion;

  screens = ScreenCount(disp);
  if (screens < 2)
    return xregionsel_grab_shot();

  dispstr = DisplayString(disp);

  subdisp = gib_estrdup(DisplayString(disp));

  for (i = 0; i < screens; i++) {
    dispstr = strchr(subdisp, ':');
    if (dispstr) {
      dispstr = strchr(dispstr, '.');
      if (NULL != dispstr)
        *dispstr = '\0';
    }
    snprintf(newdisp, sizeof(newdisp), "%s.%d", subdisp, i);
    init_x_and_imlib(newdisp, i);
    ret_region = { 0, 0, scr->width, scr->height };
    images = gib_list_add_end(images, ret);
  }
  free(subdisp);

  ret = stalk_image_concat(images);

  return ret;
}
*/
