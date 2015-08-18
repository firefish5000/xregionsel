/* xregionsel.h

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

#ifndef SCROT_H
#define SCROT_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <X11/cursorfont.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <giblib/giblib.h>


#include "config.h"
#include "structs.h"
#include "getopt.h"
#include "debug.h"

#ifndef __GNUC__
# define __attribute__(x)
#endif

typedef struct
{
	int rx;
	int ry;
	int rw;
	int rh;
}Selection;


typedef void (*sighandler_t) (int);

void show_usage(void);
void show_version(void);
void show_mini_usage(void);
void init_x_and_imlib(char *dispstr, int screen_num);
char *chop_file_from_full_path(char *str);
Selection xregionsel_grab_shot(void);
void xregionsel_exec_app(Imlib_Image image, struct tm *tm,
                    char *filename_im, char *filename_thumb);
void xregionsel_do_delay(void);
Selection xregionsel_sel_and_grab_image(void);
Selection xregionsel_grab_focused(void);
void xregionsel_sel_area(int *x, int *y, int *w, int *h);
void xregionsel_nice_clip(int *rx, int *ry, int *rw, int *rh);
int xregionsel_get_geometry(Window target, int *rx, int *ry, int *rw, int *rh);
Window xregionsel_get_window(Display *display,Window window,int x,int y);
Window xregionsel_get_client_window(Display * display, Window target);
Window xregionsel_find_window_by_property(Display * display, const Window window,
                                     const Atom property);
char *im_printf(char *str, struct tm *tm,
                char *filename_im, char *filename_thumb,
                Imlib_Image im);
Selection xregionsel_grab_shot_multi(void);
Selection stalk_image_concat(gib_list *images);

/* Imlib stuff */
extern Display *disp;
extern Visual *vis;
extern Colormap cm;
extern int depth;

/* Thumbnail sizes */
extern Window root;
extern Screen *scr;

#endif