/*
 * Callback.h
 *
 * Typedef and structure def for callback routines and lists of same.
 *
 * $Header$
 *
 * $Log$
 * Revision 1.1  1999/06/16 03:24:23  gorelick
 * Initial revision
 *
 * Revision 1.1  1999/06/16 01:40:34  gorelick
 * Initial install
 *
 * Revision 0.2  91/09/23  17:55:03  17:55:03  ngorelic (Noel S. Gorelick)
 * *** empty log message ***
 * 
 * Revision 0.1  91/07/24  18:04:07  18:04:07  rray (Randy Ray)
 * *** empty log message ***
 * 
 *
 */

/* A function ptr to allow dynamic lists of functions called by tools */
typedef void (*CallBack)();

/* Maintain a linked-list of callback routines */
struct CallBackList
{
  CallBack proc;                /* address of callback */
  struct CallBackList* next;    /* next callback, if any */
};

/* Some significant, pre-provided callbacks */

/* The default assigned to expose- and update- callbacks for buttons */
/* void defaultButtonCallback(Button, XEvent *); */

/* The defaults assigned to the default slots of sliders */
void defaultSliderCallback();
void defaultSliderUpdateCallback();

/* The defaults for joysticks */
void defaultJoystickCallback();
void defaultJoystickUpdateCallback();

/* Default for AMap's */
void defaultAMapCallback();
void defaultAMapUpdateCallback();
