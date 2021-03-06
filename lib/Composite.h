typedef struct _composite {
	Display *display;
	Window parent;
	Window window;
	XFontStruct *font;
	int x;
	int y;
	int width;
	int height;
	int font_height;
	short hilite;
	Button Edit;
	Button Popdown;
	List list;
	CallBack load_proc;
	char **items;
	int nitems;
	char *current_text;
	int ext;
} *Composite;
