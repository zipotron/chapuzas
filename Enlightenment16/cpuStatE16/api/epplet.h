/*****************************************************************************/
/* Enlightenment - The Window Manager that dares to do what others don't     */
/*****************************************************************************/
/* Copyright (C) 1997 - 2000 Carsten Haitzler (The Rasterman)                */
/*                                                                           */
/* This program and utilites is free software; you can redistribute it       */
/* and/or modify it under the terms of the License shown in COPYING          */
/*                                                                           */
/* This software is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                      */
/*****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <Imlib2.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <pwd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#endif

#ifdef HAVE_GLX
#include <GL/glx.h>
#endif

#if defined(sun) && !defined(__sun__)
#define __sun__
#endif

#define HAVE_SNPRINTF 1
#if HAVE_SNPRINTF
#define Evsnprintf vsnprintf
#define Esnprintf snprintf
#else
int                 Evsnprintf(char *str, size_t count, const char *fmt,
			       va_list args);
int                 Esnprintf(char *str, size_t count, const char *fmt, ...);
#endif /* HAVE_SNPRINTF */

/****************************************************************************/
/* Data structures & primitives                                             */
/****************************************************************************/
typedef struct _etimer ETimer;
typedef void       *Epplet_gadget;
typedef struct _rgb_buf
  {
     Imlib_Image        *im;
  }
                   *RGB_buf;

typedef struct _configitem
  {
     char               *key;
     char               *value;
  }
ConfigItem;
typedef enum gad_type
{
   E_BUTTON,
   E_DRAWINGAREA,
   E_TEXTBOX,
   E_HSLIDER,
   E_VSLIDER,
   E_TOGGLEBUTTON,
   E_POPUPBUTTON,
   E_POPUP,
   E_IMAGE,
   E_LABEL,
   E_HBAR,
   E_VBAR
} GadType;

/****************************************************************************/
/* Initialization call                                                      */
/****************************************************************************/
/* This sets everything up - connects to X, finds E, creates a window...    */
/* Parameters are: a name, version, info, then width and height in w, h.    */
/* width and height are multiples of 16 pixels so (2, 2) will make a 32x32  */
/* sized window. You also need to pass your argc and argv parameters your   */
/* main() function gets. vertical is a flag as to if the app is vertical.   */
/* Apps that are more horizontal than vertical should set this to 0.        */
void                Epplet_Init(char *name, char *version, char *info, int w, int h,
				int argc, char **argv, char vertical);

/****************************************************************************/
/* Cleanup call                                                             */
/****************************************************************************/
/* You HAVE to call this before exiting your epplet!                        */

void                Epplet_cleanup(void);

/* actualy display the app */
void                Epplet_show(void);

/* ask E to remember stuff about it - you don't need to do this at startup */
/* or whenver the Epplet moves etc.- this is done for you, but if you need */
/* to for some special reason - call it */
void                Epplet_remember(void);

/* if you dont want E to remember anything abotu you or start you up anymore */
/* call this - this is a good call to make if you want to exit and never */
/* have yourself started up again by E */
void                Epplet_unremember(void);

/* return the window id of the main epplet window */
Window              Epplet_get_main_window(void);

/* return the X display connection used */
Display            *Epplet_get_display(void);

/* Get locations and sizes of gadgets */
int Epplet_gadget_get_x(Epplet_gadget gad);
int Epplet_gadget_get_y(Epplet_gadget gad);
int Epplet_gadget_get_width(Epplet_gadget gad);
int Epplet_gadget_get_height(Epplet_gadget gad);
int Epplet_gadget_get_type(Epplet_gadget gad);

/****************************************************************************/
/* IPC calls                                                                */
/****************************************************************************/
/* send the string "s" to Enlightenment */
void                Epplet_send_ipc(char *s);

/* sit and wait for an IPc message - nothing happens whilst waiting no */
/* timers run, no events or anything else is handled. */
char               *Epplet_wait_for_ipc(void);

/* take the imageclass called iclass in state state ("normal", "hilited", */
/* "clicked") and set it as the backgorund pixmap to window ww and have */
/* its shape mask be the shape of the window ww */
void                Epplet_imageclass_apply(char *iclass, char *state, Window ww);

/* paste the imageclass iclass in state state in window ww at (x,y) at a */
/* size of (w x h) */
void                Epplet_imageclass_paste(char *iclass, char *state, Window ww,
					    int x, int y, int w, int h);

/* return pixmaps of imageclass iclass in the state state and place the */
/* pixmap and mask ID's in the Pixmaps Id's poitned to by p and m, and have */
/* the pximaps be of size (w x h) */
void                Epplet_imageclass_get_pixmaps(char *iclass, char *state, Pixmap * p,
						  Pixmap * m, int w, int h);

/* draw the text class tclass in state state on window ww at x, y with the */
/* text "txt" */
void                Epplet_textclass_draw(char *tclass, char *state, Window ww, int x,
					  int y, char *txt);

/* get the size text for textclass tclass will be using the text "txt" and */
/* return widht and height to the int's pointed to by e and h */
void                Epplet_textclass_get_size(char *tclass, int *w, int *h, char *txt);

/* the epplet main loop - once you've set up and showed your epplet window */
/* call this */
void                Epplet_Loop(void);

/* call the function func with data as its data param whenever an expose */
/* happens and needs to be handled */
void                Epplet_register_expose_handler(void (*func)
                         (void *data, Window win, int x, int y, int w, int h),
						   void *data);

/* call func whenever the epplet is moved */
void                Epplet_register_move_resize_handler(void (*func)
                         (void *data, Window win, int x, int y, int w, int h),
							void *data);

/* call func whenever a button is pressed */
void                Epplet_register_button_press_handler(void (*func)
	                        (void *data, Window win, int x, int y, int b),
							 void *data);

/* call func whenever a button is released */
void                Epplet_register_button_release_handler(void (*func)
	                        (void *data, Window win, int x, int y, int b),
							   void *data);

/* call func whenever a key is pressed (pass a string version of the key */
/* pressed to the regsitsered function) */
void                Epplet_register_key_press_handler(void (*func)
		                          (void *data, Window win, char *key),
						      void *data);

/* call func whenever a key is released (pass a string version of the key */
/* pressed to the regsitsered function) */
void                Epplet_register_key_release_handler(void (*func)
		                          (void *data, Window win, char *key),
							void *data);

/* call func whenever a the mouse is moved in a window */
void                Epplet_register_mouse_motion_handler(void (*func)
		                       (void *data, Window win, int x, int y),
							 void *data);

/* call func whenever a the mouse enters a window */
void                Epplet_register_mouse_enter_handler(void (*func)
				                     (void *data, Window win),
							void *data);

/* call func whenever a the mouse leaves a window */
void                Epplet_register_mouse_leave_handler(void (*func)
				                     (void *data, Window win),
							void *data);

/* call func whenever focus is active on your epplet window */
void                Epplet_register_focus_in_handler(void (*func)
				                     (void *data, Window win),
						     void *data);

/* call func whenever leaves your epplet window */
void                Epplet_register_focus_out_handler(void (*func)
				                     (void *data, Window win),
						      void *data);

/* call func and pass a pointer to n XEvent on every event that happens */
void                Epplet_register_event_handler(void (*func)
				                    (void *data, XEvent * ev),
						  void *data);

/* call func and pass a string (that you dont have to free) with the IPC */
/* message whenever e sends you an IPC message */
void                Epplet_register_comms_handler(void (*func)
				                        (void *data, char *s),
						  void *data);

/* call func when a window is closed by the wm. This func must return a
 * value. Return 1 and the window will be destroyed for you, return 0 and it
 * will not */
void                Epplet_register_delete_event_handler(int (*func)
				                     (void *data, Window win),
							 void *data);

/****************************************************************************/
/* Timer timeout functions - this lets you appear to have threads when you  */
/* dont have any at all                                                     */
/****************************************************************************/
/* run function func and pass data data to it in in seconds (in is double */
/* so you can for exmaple use 0.5 to have that function called in 0.5 */
/* seconds from now ). You also attach the name to the timeout of name */
void                Epplet_timer(void (*func) (void *data), void *data, double in,
				 char *name);

/* delete any timeout of name name in the queue. you should use unique */
/* names for different timeouts in the queue */
void                Epplet_remove_timer(char *name);

/* get the data passed to a timer */
void               *Epplet_timer_get_data(char *name);

/* get the current time as a double (time is in seconds since Jan 1, 1970 */
double              Epplet_get_time(void);

/****************************************************************************/
/* widgets available from the epplet api that use images from E to define   */
/* their look. if you change themes in E all the widgets will change too    */
/****************************************************************************/
/* create a button with either label label or image file image at (x,y) with */
/* with a size of (w x h), OR make a standard button of name std whihc is */
/* (12x12) pixels in size. Valid standard butotns are: "ARROW_UP", */
/* "ARROW_DOWN", "ARROW_LEFT", "ARROW_RIGHT", "PLAY", "STOP", "PAUSE", */
/* "PREVIOUS", "NEXT", "EJECT", "CLOSE", "FAST_FORWARD", "REWIND", "REPEAT", */
/* "SKIP", "HELP", "CONFIGURE" */
/* parent is the parent window - normally you only need to use 0 for this */
/* unless you want a special parent (only buttons can be speically parented */
/* and the function func si called whne the button is clicked and data is */
/* passed to that function */
Epplet_gadget       Epplet_create_button(char *label, char *image, int x, int y,
				       int w, int h, char *std, Window parent,
					 Epplet_gadget pop_parent,
			void                (*func) (void *data), void *data);

/* A cut down version for text-only buttons */
Epplet_gadget       Epplet_create_text_button(char *label, int x, int y,
					      int w, int h,
			void                (*func) (void *data), void *data);

/* A cut down version for std-image-only buttons */
Epplet_gadget       Epplet_create_std_button(char *std, int x, int y,
			void                (*func) (void *data), void *data);

/* A cut down version for image-only buttons */
Epplet_gadget       Epplet_create_image_button(char *image, int x, int y,
					       int w, int h,
			void                (*func) (void *data), void *data);

/* create a textbox at coordinates (x, y), with 'image' as bg, 'contents' as
 * the default contents, w, h, and 'size' as the font size. When ENTER is
 * pressed in the textbox, 'func' is executed.
 * */
Epplet_gadget       Epplet_create_textbox(char *image, char *contents, int x,
					  int y, int w, int h, char size,
				     void                (*func) (void *data),
					  void *data);

/* Retrieve the current contents of the textbox */
char               *Epplet_textbox_contents(Epplet_gadget g);

/* Reset the textbox */
void                Epplet_reset_textbox(Epplet_gadget eg);

/* Change the contents of a textbox */
void                Epplet_change_textbox(Epplet_gadget eg, char *new_contents);
void                Epplet_textbox_insert(Epplet_gadget eg, char *new_contents);

/* create drawing area at (x,y) of size (w x h) */
Epplet_gadget       Epplet_create_drawingarea(int x, int y, int w, int h);

/* create horizontal slider at x, y of length len. the minimum length is 9 */
/* pixels, and the width is always 8 pixels. min is the minimum value and */
/* max is the maximum value. max should always > min. the slider can move */
/* by step units as a minimum step, and moves by jump whenever you click */
/* either side of the slider. whenever the slider changed func is called */
Epplet_gadget       Epplet_create_hslider(int x, int y, int len, int min, int max,
					  int step, int jump, int *val,
			void                (*func) (void *data), void *data);

/* same as horizontal slider except vertical */
Epplet_gadget       Epplet_create_vslider(int x, int y, int len, int min, int max,
					  int step, int jump, int *val,
			void                (*func) (void *data), void *data);

/* create a button (like normal buttons) except it toggles the value */
/* pointed to by val between 1 and 0. func is called whenever it changes */
Epplet_gadget       Epplet_create_togglebutton(char *label, char *pixmap, int x,
					       int y, int w, int h, int *val,
				     void                (*func) (void *data),
					       void *data);

/* creates a button just like normal button except it pops up the popup */
/* when clicked */
Epplet_gadget       Epplet_create_popupbutton(char *label, char *image, int x,
					      int y, int w, int h, char *std,
					      Epplet_gadget popup);

/* creates an empty popup */
Epplet_gadget       Epplet_create_popup(void);

/* adds an image file pixmaps or label to the popup gadget and calls */
/* func when it is selected */
void                Epplet_add_popup_entry(Epplet_gadget gadget, char *label,
					   char *pixmap,
			void                (*func) (void *data), void *data);
/* same as above, but specify a size */
void                Epplet_add_sized_popup_entry(Epplet_gadget gadget, char *label,
                                                 char *pixmap, int w, int h,
                                                 void (*func) (void *data), void *data);

/* remove an entry from a popup gadget */
/* If entry num is positive or zero then it is relateive to elements */
/* least recently added to the popup gadget, zero is the element first */
/* added to the list. If entry_num is negative then is is relateive to */
/* elements most recently added, -1 is the elemet most recently */
/* added to the popup */
void                Epplet_remove_popup_entry(Epplet_gadget gadget, int entry_num);

/* get the number of entries in a popup */
int                 Epplet_popup_entry_num(Epplet_gadget gadget);

/* get the data of a callback of a popup entry */
void               *Epplet_popup_entry_get_data(Epplet_gadget gadget,
						int entry_num);

/* creates an image widget of the file image at (x,y) of size (w x h) */
Epplet_gadget       Epplet_create_image(int x, int y, int w, int h, char *image);

/* puts a label widget of text label at (x,y) of size size. sizes are 0, 1 */
/* 2 and 3. 0 is normal , 1 is tiny, 2 is medium and 3 is big. experiment */
Epplet_gadget       Epplet_create_label(int x, int y, char *label, char size);

/* creates a horizontal progress bar at (x,y) of size (w x h) displaying the */
/* value val points to that is a value of 0-100. dir is the direction 0 */
/* indicates left to right and 1 indicates right to left */
Epplet_gadget       Epplet_create_hbar(int x, int y, int w, int h, char dir,
				       int *val);

/* creates a vertical progress bar - dir of 0 is top to bottom, 1 is bottom */
/* to top */
Epplet_gadget       Epplet_create_vbar(int x, int y, int w, int h, char dir,
				       int *val);

/* get the window id of the windopw to draw in in the drawing area */
Window              Epplet_get_drawingarea_window(Epplet_gadget gadget);

/* change the background to either verticla or horizontal for the epplet */
void                Epplet_background_properties(char vertical, Window win);

/* destroy a gadget */
void                Epplet_gadget_destroy(Epplet_gadget gadget);

/* hide a gadget */
void                Epplet_gadget_hide(Epplet_gadget gadget);

/* show a gadget */
void                Epplet_gadget_show(Epplet_gadget gadget);

/* move a gadget */
void                Epplet_gadget_move(Epplet_gadget gadget, int x, int y);

/* get the callback data from a gadget */
void               *Epplet_gadget_get_data(Epplet_gadget gadget);

/* if you chaged the value a gadget is pointing to call this on the gadget */
/* so it can update and redraw */
void                Epplet_gadget_data_changed(Epplet_gadget gadget);

/* Redraw a gadget.  un_only should be set to 1 if you only want to "undraw"
 * the existing gadget.  force should be 1 if you want to draw it even if it
 * is invisible.  Beware that this could be a Bad Thing (tm). */
void                Epplet_gadget_draw(Epplet_gadget gadget, int un_only, int force);

/* Redraw all gadgets. */
void                Epplet_redraw(void);

/* change the popup a popbutton brings up and destroy the popup it currently */
/* brings up */
void                Epplet_change_popbutton_popup(Epplet_gadget gadget,
						  Epplet_gadget popup);

/* change the label string contents of the gadget popbutton */
void                Epplet_change_popbutton_label(Epplet_gadget gadget,
						  char *label);

/* display the popup gadget above or below (dpeending where the window ww */
/* is) the window ww, or if it's 0, deisplay where the pointer is */
void                Epplet_pop_popup(Epplet_gadget gadget, Window ww);

/* change the image file and widht & height of the image gadget */
void                Epplet_change_image(Epplet_gadget gadget, int w, int h,
					char *image);

/* change the x and y coordinates of the image gadget and change the image */
void                Epplet_move_change_image(Epplet_gadget gadget, int x, int y,
					     int w, int h, char *image);

/* change the label string contents of the gadget label */
void                Epplet_change_label(Epplet_gadget gadget, char *label);

/* move the label to a new x/y and change the text too */
void                Epplet_move_change_label(Epplet_gadget gadget, int x, int y, char *label);

/****************************************************************************/
/* Some window creation functions for configs, questions, external displays */
/* or other such things                                                     */
/****************************************************************************/

/* Create a window of width w, height h, with title title, using the themes
 * vertical (1), or horizontal (0) background */
Window              Epplet_create_window(int w, int h, char *title, char vertical);
Window              Epplet_create_window_borderless(int w, int h, char *title, char vertical);

/* Create a config window, of width w, height h, with title title, using the
 * functions listed for ok, apply and cancel. If you specify any of the
 * functions as null, the button won't be displayed. This means you can get
 * just Ok and Cancel by supplying NULL for apply_cb. */
Window              Epplet_create_window_config(int w, int h, char *title,
		   void                (*ok_func) (void *data), void *ok_data,
	     void                (*apply_func) (void *data), void *apply_data,
	  void                (*cancel_func) (void *data), void *cancel_data);

/* This is how you add gadgets to specific windows. When you create a
 * window, its context is automatically pushed for you. This means any
 * gadgets created with this window in context will go onto this window. In
 * fact, until you either push another window onto the context, or pop this
 * one back off, all created gadgets will go on here. A create_window should
 * always be followed by a pop_context (after gadgets have been added). */
void                Epplet_window_push_context(Window newwin);
Window              Epplet_window_pop_context(void);

/* Show and hide an already created Window win. */
void                Epplet_window_show(Window win);
void                Epplet_window_hide(Window win);

/* Destroy Window win. Any gadgets you have added to the window are
 * destroyed also */
void                Epplet_window_destroy(Window win);

/****************************************************************************/
/* basic line, filled rectangle and box outline drawing functions to make   */
/* life easy                                                                */
/****************************************************************************/
/* draw a line from (x1, y1) to (x2, y2) in window win, in color (r, g, b) */
void                Epplet_draw_line(Window win, int x1, int y1, int x2, int y2,
				     int r, int g, int b);

/* draw a box at (x, y) of size (w x h) in window win, in color (r, g, b) */
void                Epplet_draw_box(Window win, int x, int y, int w, int h,
				    int r, int g, int b);

/* draw a box outline at (x, y) of size (w, h) in window win, in color (r, g, b) */
void                Epplet_draw_outline(Window win, int x, int y, int w, int h,
					int r, int g, int b);

/* get the pixel value for the RGB value (r, g, b) and return it */
unsigned long                 Epplet_get_color(int r, int g, int b);

/* pasye the image file image onto window ww, at its original size at (x,y) */
void                Epplet_paste_image(char *image, Window ww, int x, int y);

/* paste the image file image onto window ww at (x,y), at size (w x h) */
void                Epplet_paste_image_size(char *image, Window ww, int x, int y,
					    int w, int h);

/* syncronize all draws (guarantees they are done) */
void                Esync(void);

/****************************************************************************/
/* RGB buffer - for people who want to write raw RGB image data to a drawing */
/* area and want it rendered/dithered etc. for them                         */
/****************************************************************************/
/* create an RGB buffer of size (w x h) */
RGB_buf             Epplet_make_rgb_buf(int w, int h);

/* get a pointer to the RGB data int he RGB buffer */
unsigned char      *Epplet_get_rgb_pointer(RGB_buf buf);

/* render & paste the RGB buffer to a window at x, y */
void                Epplet_paste_buf(RGB_buf buf, Window win, int x, int y);

/* free an RGB buffer */
void                Epplet_free_rgb_buf(RGB_buf buf);

#ifdef HAVE_GLX
/****************************************************************************/
/* OpenGL (GLX) calls. Use these functions to setup and destroy a OpenGL    */
/* context for epplets. All glx contexts are linked to an epplet drawing    */
/* area. You must have one set up first. Double buffer and a single front   */
/* front buffer is available. You must also remember to glXSwapBuffers      */
/* at the end of your Drawing cycle if you use the double buffer.           */
/****************************************************************************/
/* Create an Epplet glx context, pick the default visual, and bind it to a
   window (obtained from drawingarea_window) with a double buffer.
	 You ONLY can request an RGBA space; color indexed is not available.
	 It sucks anyways :P. red, greem, blue are the minimum bit depths you
	 need. aux_buffers are the number of auxiliary buffers you need.
	 Normally this is 0. alpha is the minumum alpha depth you need. Used
	 for blending, ect. depth is the minimum DEPTH buffer you need. You
	 really do not want to ignore this, otherwise you'll end up with
	 really odd results. 8 is a very conservative number. Most modern
	 cards can do at least 16 bit, most can do 32 bits. stencil is the
	 number of stencil bitplanes needed. Defaults to 0. accum_red
	 accum_green and accum_blue are the individual accumultion buffers
	 for each color. Defaults to 0. accum_alpha is the accumulation buffer
	 for the alpha channel. defaults to 0. */
GLXContext          Epplet_bind_double_GL(Epplet_gadget da, int red,
	int blue, int green, int alpha, int aux_buffers, int depth,
	int stencil, int accum_red, int accum_green, int accum_blue,
	int accum_alpha);

/* The same as double buffer, except if this call succedes you are
   returned a single buffered context. */
GLXContext          Epplet_bind_single_GL(Epplet_gadget da, int red,
	int blue, int green, int alpha, int aux_buffers, int depth,
	int stencil, int accum_red, int accum_green, int accum_blue,
	int accum_alpha);

/* This is the "quick" way to define and bind a "good enough" context.
	 The idea here is that this context should give you a basic RGB
	 double buffer with minimal depth buffer, and NO other buffers. Its very
	 basic, but its good enough for simple rendering with light sources
	 and basic texture mapping. */
GLXContext					Epplet_default_bind_GL(Epplet_gadget da);

/* Destroy (unbind) a glx context. */
void                Epplet_unbind_GL(GLXContext cx);

#endif

/* command execution/spawing wrappers to make life easy */

/* This runs the command passed to it and returns its exit code: */
int                 Epplet_run_command(char *cmd);
char               *Epplet_read_run_command(char *cmd);
int                 Epplet_spawn_command(char *cmd);
void                Epplet_pause_spawned_command(int pid);
void                Epplet_unpause_spawned_command(int pid);
void                Epplet_kill_spawned_command(int pid);
void                Epplet_destroy_spawned_command(int pid);
void                Epplet_register_child_handler(void (*func)
		                         (void *data, int pid, int exit_code),
						  void *data);
void                Epplet_change_button_label(Epplet_gadget gadget, char *label);
void                Epplet_change_button_image(Epplet_gadget gadget, char *image);
void                Epplet_clear_window(Window ww);
void                Epplet_show_about(char *name);
void                Epplet_dialog_ok(char *text);
int                 Epplet_get_hslider_clicked(Epplet_gadget gadget);
int                 Epplet_get_vslider_clicked(Epplet_gadget gadget);

/****************************************************************************/
/* Config file handling stuff                                               */
/****************************************************************************/
/* Load the config file (initializes the config data).                      */
void                Epplet_load_config(void);
/* Load config data from a specific file, overriding any previous data.     */
void                Epplet_load_config_file(const char *);
/* This returns the instance of this epplet.                                */
int                 Epplet_get_instance(void);

/* Here you can query a config setting.                                     */
char               *Epplet_query_config(char *key);

/* Same as above, but returns the value of default instead of NULL if no    */
/* match is found.                                                          */
char               *Epplet_query_config_def(char *key, char *def);

/* Use this to change or add a config setting.                              */
void                Epplet_modify_config(char *key, char *value);

/* If you *know* a key doesn't exist, use this to add it.  It takes less    */
/* time than the above function.                                            */
void                Epplet_add_config(char *key, char *value);

/* If you have a set of configurations that belong together and whose       */
/* number may change at runtime, use these:                                 */

/* Here you can define multiple settings. 'num' is the number of strings    */
/* in the 'values' array.                                                   */
void                Epplet_modify_multi_config(char *key, char **values, int num);

/* This lets you query for multiple settings.                               */
/* Note that you have to free the result that is returned here, in contrast */
/* to Epplet_query_config(). The pointer to 'num' returns the number of     */
/* strings that are returned.                                               */
char              **Epplet_query_multi_config(char *key, int *num);

/* This lets you save your current config settings. This is done automati-  */
/* cally when you call Epplet_cleanup(), so you only need to call this when */
/* you want to force the configs to disk for some reason.                   */
void                Epplet_save_config(void);
/* Delete the config information.                                           */
void                Epplet_clear_config(void);

/* Return epplet data directory.                                            */
const char         *Epplet_data_dir(void);

/* Return e16 user configuration directory.                                 */
const char         *Epplet_e16_user_dir(void);
