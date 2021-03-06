/* GNUPLOT - help.c */
/*
 * Copyright (C) 1986 - 1993   Thomas Williams, Colin Kelley
 *
 * Permission to use, copy, and distribute this software and its
 * documentation for any purpose with or without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and 
 * that both that copyright notice and this permission notice appear 
 * in supporting documentation.
 *
 * Permission to modify the software is granted, but not the right to
 * distribute the modified code.  Modifications are to be distributed 
 * as patches to released version.
 *  
 * This software is provided "as is" without express or implied warranty.
 * 
 *
 * AUTHORS
 * 
 *   Original Software:
 *     Thomas Williams,  Colin Kelley.
 * 
 *   Gnuplot 2.0 additions:
 *       Russell Lang, Dave Kotz, John Campbell.
 *
 *   Gnuplot 3.0 additions:
 *       Gershon Elber and many others.
 * 
 * There is a mailing list for gnuplot users. Note, however, that the
 * newsgroup 
 *       comp.graphics.gnuplot 
 * is identical to the mailing list (they
 * both carry the same set of messages). We prefer that you read the
 * messages through that newsgroup, to subscribing to the mailing list.
 * (If you can read that newsgroup, and are already on the mailing list,
 * please send a message info-gnuplot-request@dartmouth.edu, asking to be
 * removed from the mailing list.)
 *
 * The address for mailing to list members is
 *         info-gnuplot@dartmouth.edu
 * and for mailing administrative requests is 
 *         info-gnuplot-request@dartmouth.edu
 * The mailing list for bug reports is 
 *         bug-gnuplot@dartmouth.edu
 * The list of those interested in beta-test versions is
 *         info-gnuplot-beta@dartmouth.edu
 */
#include "parser.h"

#ifndef DVHELP
#if !(defined(__CYGWIN__) || defined(__MINGW32__))
#define DVHELP "/usr/local/share/davinci/docs/dv.gih"
#else
#define DVHELP "docs\\dv.gih"
#endif
#endif

#include "func.h"
#include <errno.h>
extern int errno;

#define SAME    0       /* for strcmp() */

#include "help.h"       /* values passed back */

/* help -- help subsystem that understands defined keywords
   **
   ** Looks for the desired keyword in the help file at runtime, so you
   ** can give extra help or supply local customizations by merely editing
   ** the help file.
   **
   ** The original (single-file) idea and algorithm is by John D. Johnson,
   ** Hewlett-Packard Company.  Thanx and a tip of the Hatlo hat!
   **
   ** Much extension by David Kotz for use in gnutex, and then in gnuplot.
   ** Added output paging support, both unix and builtin. Rewrote completely
   ** to read helpfile into memory, avoiding reread of help file. 12/89.
   **
   ** Modified by Russell Lang to avoid reading completely into memory
   ** if __CYGWIN__ defined.  This uses much less memory.  6/91
   **
   ** The help file looks like this (the question marks are really in column 1):
   **
   **   ?topic
   **   This line is printed when the user wants help on "topic".
   **   ?keyword
   **   ?Keyword
   **   ?KEYWORD
   **   These lines will be printed on the screen if the user wanted
   **   help on "keyword", "Keyword", or "KEYWORD".  No casefolding is
   **   done on the keywords.
   **   ?subject
   **   ?alias
   **   This line is printed for help on "subject" and "alias".
   **   ?
   **   ??
   **   Since there is a null keyword for this line, this section
   **   is printed when the user wants general help (when a help
   **   keyword isn't given).  A command summary is usually here.
   **   Notice that the null keyword is equivalent to a "?" keyword
   **   here, because of the '?' and '??' topic lines above.
   **   If multiple keywords are given, the first is considered the 
   **   'primary' keyword. This affects a listing of available topics.
   **   ?last-subject
   **   Note that help sections are terminated by the start of the next
   **   '?' entry or by EOF.  So you can't have a leading '?' on a line
   **   of any help section.  You can re-define the magic character to
   **   recognize in column 1, though, if '?' is too useful.  (Try ^A.)
 */

#define KEYFLAG '?'     /* leading char in help file topic lines */

/*
   ** Calling sequence:
   **   int result;             # 0 == success
   **   char *keyword;          # topic to give help on
   **   char *pathname;         # path of help file
   **      int subtopics;               # set to TRUE if only subtopics to be listed
   **                           # returns TRUE if subtopics were found
   **   result = help(keyword, pathname, &subtopics);
   ** Sample:
   **   cmd = "search\n";
   **   helpfile = "/usr/local/lib/program/program.help";
   **   subtopics = FALSE;
   **   if (help(cmd, helpfile, &subtopics) != H_FOUND)
   **           printf("Sorry, no help for %s", cmd);
   **
   **
   ** Speed this up by replacing the stdio calls with open/close/read/write.
 */

#define PATHSIZE    BUFSIZ

#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif

typedef struct line_s LINEBUF;
struct line_s {
    char *line;     /* the text of this line */
    LINEBUF *next;      /* the next line */
};

typedef struct linkey_s LINKEY;
struct linkey_s {
    char *key;      /* the name of this key */
    long pos;       /* ftell position */
    LINEBUF *text;      /* the text for this key */
    boolean primary;    /* TRUE -> is a primary name for a text block */
    LINKEY *next;       /* the next key in linked list */
};

typedef struct key_s KEY;
struct key_s {
    char *key;      /* the name of this key */
    long pos;       /* ftell position */
    LINEBUF *text;      /* the text for this key */
    boolean primary;    /* TRUE -> is a primary name for a text block */
};
static LINKEY *keylist = NULL;  /* linked list of keys */
static KEY *keys = NULL;    /* array of keys */
static int keycount = 0;    /* number of keys */
static FILE *helpfp = NULL;

static FILE *outfile;       /* for unix pager, if any */
static int pagelines;       /* count for builtin pager */

static void OutLine( char *line);
static void EndOutput();
char * do_help(char *input, char *path);
char * cleanup_input(char *s);
static int LoadHelp(char *path);
static LINEBUF * storeline( char *text);
static LINKEY * storekey( char *key);
static int keycomp( const void *a, const void *b);
static void sortkeys();
void FreeHelp();
static KEY * FindHelp( char *keyword);
static boolean Ambiguous( KEY *key, int len);
static void PrintHelp( KEY *key, boolean *subtopics);
static void ShowSubtopics( KEY *key, boolean *subtopics);
static void StartOutput();
int instring(char *str, char c);
void squash_spaces(char *s);

#define SCREENSIZE 24       /* lines on screen (most have at least 24) */

/* help:
 * print a help message 
 * also print available subtopics, if subtopics is TRUE
 */
int
help(
     char *keyword,     /* on this topic */
     char *path,        /* from this file */
     boolean *subtopics     /* (in) - subtopics only? */
                            /* (out) - are there subtopics? */
)
{
    static char oldpath[PATHSIZE] = ""; /* previous help file */
    int status;     /* result of LoadHelp */
    KEY *key;       /* key that matches keyword */

    /*
       ** Load the help file if necessary (say, first time we enter this routine,
       ** or if the help file changes from the last time we were called).
       ** Also may occur if in-memory copy was freed.
       ** Calling routine may access errno to determine cause of H_ERROR.
     */
    errno = 0;
    if (strncmp(oldpath, path, PATHSIZE) != SAME)
    	FreeHelp();
    
    if (keys == NULL) {
        status = LoadHelp(path);
        if (status == H_ERROR)
            return (status);

        /* save the new path in oldpath */
        if (strlen(path) < PATHSIZE)
            (void) strcpy(oldpath, path);
        else {      /* not enough room in oldpath, sigh */
            (void) strncpy(oldpath, path, PATHSIZE - 1);
            oldpath[PATHSIZE - 1] = '\0';
        }
    }
    /* look for the keyword in the help file */
    key = FindHelp(keyword);
    if (key != NULL) {
        /* found the keyword: print help and return */
        PrintHelp(key, subtopics);
        status = H_FOUND;
    } else {
        status = H_NOTFOUND;
    }

    return (status);
}

/* we only read the file once, into memory
 * except for MSDOS when we don't read all the file -
 * just the keys and location of the text
 */
static int
LoadHelp(char *path)
{
    LINKEY *key;        /* this key */
    long pos;       /* ftell location within help file */
    char buf[BUFSIZ];   /* line from help file */
    LINEBUF *head;      /* head of text list  */
    LINEBUF *firsthead = NULL;
    boolean primary;    /* first ? line of a set is primary */
    boolean flag;

    if ((helpfp = fopen(path, "r")) == NULL) {
        /* can't open help file, so error exit */
        return (H_ERROR);
    }
    /*
       ** The help file is open.  Look in there for the keyword.
     */
    (void) fgets(buf, BUFSIZ - 1, helpfp);
    while (!feof(helpfp)) {
        /*
         ** Make an entry for each synonym keyword
         */
        primary = TRUE;
        while (buf[0] == KEYFLAG) {
            key = storekey(buf + 1);    /* store this key */
            key->primary = primary;
            key->text = NULL;   /* fill in with real value later */
            key->pos = 0;   /* fill in with real value later */
            primary = FALSE;
            pos = ftell(helpfp);
            if (fgets(buf, BUFSIZ - 1, helpfp) == (char *) NULL)
                break;
        }
        /*
         ** Now store the text for this entry.
         ** buf already contains the first line of text.
         */
        firsthead = storeline(buf);
        head = firsthead;
        while ((fgets(buf, BUFSIZ - 1, helpfp) != (char *) NULL)
               && (buf[0] != KEYFLAG)) {
            /* save text line */
            head->next = storeline(buf);
            head = head->next;
        }
        /* make each synonym key point to the same text */
        do {
            key->pos = pos;
            key->text = firsthead;
            flag = key->primary;
            key = key->next;
        } while (flag != TRUE && key != NULL);
    }
    (void) fclose(helpfp);

    /* we sort the keys so we can use binary search later */
    sortkeys();
    return (H_FOUND);   /* ok */
}

/* make a new line buffer and save this string there */
static LINEBUF *
storeline(char *text) 
{
    LINEBUF *l;

    l = (LINEBUF *) malloc(sizeof(LINEBUF));
    if (l == NULL)
        parse_error("not enough memory to store help file");
    if (text != NULL) {
        l->line = (char *) malloc((unsigned int) (strlen(text) + 1));
        if (l->line == NULL)
            parse_error("not enough memory to store help file");
        (void) strcpy(l->line, text);
    } else
        l->line = NULL;

    l->next = NULL;

    return (l);
}

/* Add this keyword to the keys list, with the given text */
static LINKEY *
storekey( char *key)
{
    LINKEY *l;

    key[strlen(key) - 1] = '\0';    /* cut off \n  */

    l = (LINKEY *) malloc(sizeof(LINKEY));
    if (l == NULL)
        parse_error("not enough memory to store help file");
    l->key = (char *) malloc((unsigned int) (strlen(key) + 1));

    if (l->key == NULL)
        parse_error("not enough memory to store help file");
    (void) strcpy(l->key, key);

    /* add to front of list */
    l->next = keylist;
    keylist = l;
    keycount++;
    return (l);
}

static int
keycomp(const void *a, const void *b)
{
    return (strcmp(((KEY *)a)->key, ((KEY *)b)->key));
}

/* we sort the keys so we can use binary search later */
/* We have a linked list of keys and the number.
 * to sort them we need an array, so we reform them into an array,
 * and then throw away the list.
 */
static void
sortkeys()
{
    LINKEY *p, *n;      /* pointers to linked list */
    int i;          /* index into key array */

    /* allocate the array */
    keys = (KEY *) malloc((unsigned int) ((keycount + 1) * sizeof(KEY)));
    if (keys == NULL)
        parse_error("not enough memory to store help file");
    /* copy info from list to array, freeing list */
    for (p = keylist, i = 0; p != NULL; p = n, i++) {
 	keys[i].key = p->key;
        keys[i].pos = p->pos;
        keys[i].text = p->text;
        keys[i].primary = p->primary;
        n = p->next;
        free((char *) p);
    }

    /* a null entry to terminate subtopic searches */
    keys[keycount].key = NULL;
    keys[keycount].pos = 0;
    keys[keycount].text = NULL;

    /* sort the array */
    /* note that it only moves objects of size (two pointers + long + int) */
    /* it moves no strings */
    qsort((char *) keys, keycount, sizeof(KEY), keycomp);
}


/* Free the help file from memory. */
/* May be called externally if space is needed */
void
FreeHelp()
{
    int i;          /* index into keys[] */
    LINEBUF *t, *next;

    if (keys == NULL)
        return;

    for (i = 0; i < keycount; i++) {
        free((char *) keys[i].key);
        if (keys[i].primary)    /* only try to release text once! */
            for (t = keys[i].text; t != NULL; t = next) {
                free((char *) t->line);
                next = t->next;
                free((char *) t);
            }
    }
    free((char *) keys);
    keys = NULL;

    free(keylist);
    keylist = NULL;

    keycount = 0;
}

/* FindHelp:
 *  Find the key that matches the keyword.
 *  The keys[] array is sorted by key.
 *  We could use a binary search, but a linear search will aid our
 *  attempt to allow abbreviations. We search for the first thing that
 *  matches all the text we're given. If not an exact match, then
 *  it is an abbreviated match, and there must be no other abbreviated
 *  matches -- for if there are, the abbreviation is ambiguous. 
 *  We print the ambiguous matches in that case, and return not found.
 */
static KEY *            /* NULL if not found */
FindHelp(
	char *keyword      /* string we look for */
)
{
    KEY *key;
    int len = strlen(keyword);
    int compare;

    for (key = keys, compare = 1; key->key != NULL && compare > 0; key++) {
        compare = strncmp(keyword, key->key, len);
        if (compare == 0)   /* we have a match! */
            if (!Ambiguous(key, len)) {
                /* non-ambiguous abbreviation */
                (void) strcpy(keyword, key->key);   /* give back the full spelling */
                return (key);   /* found!! */
            }
    }

    /* not found, or ambiguous */
    return (NULL);
}

/* Ambiguous:
 * Check the key for ambiguity up to the given length.
 * It is ambiguous if it is not a complete string and there are other
 * keys following it with the same leading substring.
 */
static boolean
Ambiguous(
     KEY *key,
     int len
)
{
    char *first;
    char *prev;
    boolean status = FALSE; /* assume not ambiguous */
    int compare;
    int sublen;

    if (key->key[len] == '\0')
        return (FALSE);

    for (prev = first = key->key, compare = 0, key++;
         key->key != NULL && compare == 0; key++) {
        compare = strncmp(first, key->key, len);
        if (compare == 0) {
            /* So this key matches the first one, up to len.
             * But is it different enough from the previous one
             * to bother printing it as a separate choice?
             */
            sublen = instring(prev + len, ' ');
            if (strncmp(key->key, prev, len + sublen) != 0) {
                /* yup, this is different up to the next space */
                if (!status) {
                    /* first one we have printed is special */
                    fprintf(stderr,
                        "Ambiguous request '%.*s'; possible matches:\n",
                        len, first);
                    fprintf(stderr, "\t%s\n", prev);
                    status = TRUE;
                }
                fprintf(stderr, "\t%s\n", key->key);
                prev = key->key;
            }
        }
    }

    return (status);
}

/* PrintHelp:
 * print the text for key
 */
static void
PrintHelp(
     KEY *key,
     boolean *subtopics     /* (in) - subtopics only? */
                            /* (out) - are there subtopics? */
)
{
    LINEBUF *t;
    StartOutput();

    if (subtopics == NULL || !*subtopics) {
        for (t = key->text; t != NULL; t = t->next)
            OutLine(t->line);   /* print text line */
    }
    ShowSubtopics(key, subtopics);
    OutLine("\n");

    EndOutput();
}

/* ShowSubtopics:
 *  Print a list of subtopic names
 */
#define PER_LINE 4

static void
ShowSubtopics(
     KEY *key,          /* the topic */
     boolean *subtopics     /* (out) are there any subtopics */
)
{
    int subt = 0;       /* printed any subtopics yet? */
    KEY *subkey;        /* subtopic key */
    int len;        /* length of key name */
    char line[BUFSIZ];  /* subtopic output line */
    char *start;        /* position of subname in key name */
    int sublen;     /* length of subname */
    int pos;
    char *prev = NULL;  /* the last thing we put on the list */

    *line = '\0';
    len = strlen(key->key);

    for (subkey = key + 1; subkey->key != NULL; subkey++) {
        if (strncmp(subkey->key, key->key, len) == 0) {
            /* find this subtopic name */
            start = subkey->key + len;
            if (len > 0)
                if (*start == ' ')
                    start++;    /* skip space */
                else
                    break;  /* not the same topic after all  */
            else
                /* here we are looking for main topics */ if (!subkey->primary)
                continue;   /* not a main topic */
            sublen = instring(start, ' ');
            if (prev == NULL || strncmp(start, prev, sublen) != 0) {
                if (subt == 0) {
                    subt++;
                    if (len)
                        (void) sprintf(line, "\nSubtopics available for %s:\n",
                                   key->key);
                    else
                        (void) sprintf(line, "\nHelp topics available:\n");
                    OutLine(line);
                    *line = '\0';
                    pos = 0;
                }
                if (pos == PER_LINE) {
                    (void) strcat(line, "\n");
                    OutLine(line);
                    *line = '\0';
                    pos = 0;
                }
                /* adapted by DvdSchaaf */
                {
#define FIRSTCOL    6
#define COLLENGTH   15
                    int spacelen, ispacelen;

                    if (pos == 0)
                        spacelen = FIRSTCOL;
                    for (ispacelen = 0;
                      ispacelen < spacelen; ispacelen++)
                        (void) strcat(line, " ");
                    /* commented out *
                       (void) strcat(line, "\t");
                     */
                    (void) strncat(line, start, sublen);
                    spacelen = COLLENGTH - sublen;
                    if (spacelen <= 0)
                        spacelen = 1;
                }
                pos++;
                prev = start;
            }
        } else {
            /* new topic */
            break;
        }
    }

    /* put out the last line */
    if (subt > 0 && pos > 0) {
        (void) strcat(line, "\n");
        OutLine(line);
    }
/*
   if (subt == 0) {
   OutLine("\n");
   OutLine("No subtopics available\n");
   }
 */

    if (subtopics)
        *subtopics = (subt != 0);
}


/* StartOutput:
 * Open a file pointer to a pipe to user's $PAGER, if there is one,
 * otherwise use our own pager.
 */
static void
StartOutput()
{
    char *pager_name = getenv("PAGER");

    if (pager_name != NULL && *pager_name != '\0')
        if ((outfile = popen(pager_name, "w")) != (FILE *) NULL)
            return; /* success */
    outfile = stderr;
    /* fall through to built-in pager */

    /* built-in pager */
    pagelines = 0;
}

/* write a line of help output  */
/* line should contain only one \n, at the end */
static void
OutLine( char *line)
{
    int c;          /* dummy input char */
    if (outfile != stderr) {
        fputs(line, outfile);
        return;
    }
    /* built-in dumb pager */
    /* leave room for prompt line */
    if (pagelines >= SCREENSIZE - 2) {
        fprintf(stderr, "Press return for more: ");
        do
            c = getchar();
        while (c != EOF && c != '\n');
        pagelines = 0;
    }
    fputs(line, stderr);
    pagelines++;
}

static void
EndOutput()
{
    if (outfile != stderr)
        (void) pclose(outfile);
}

/*
 * do_help: (not VMS, although it would work) Give help to the user. It
 * parses the command line into helpbuf and supplies help for that string.
 * Then, if there are subtopics available for that key, it prompts the user
 * with this string. If more input is given, do_help is called recursively,
 * with the argument the index of null character in the string. Thus a more
 * specific help can be supplied. This can be done repeatedly. If null input
 * is given, the function returns, effecting a backward climb up the tree.
 * David Kotz (David.Kotz@Dartmouth.edu) 10/89
 */

char *
do_help(char *input, char *path)
{
	static char *helpbuf = NULL;
	static char *prompt = NULL;
	int base;		/* index of first char AFTER help string */
	int len;		/* length of current help string */
	boolean more_help;
	boolean only;		/* TRUE if only printing subtopics */
	int subtopics;		/* 0 if no subtopics for this topic */
	char *help_ptr;		/* name of help file */
	char *input_line;

	if (path == NULL) {
		if ((help_ptr = getenv("DVHELP")) == (char *) NULL) {
			/* if can't find environment variable then just use DVHELP */
			help_ptr = DVHELP;
		}
	} else {
		help_ptr = path;
	}

	if (helpbuf == NULL) {
		helpbuf = (char *)malloc((unsigned long) PATHSIZE);
		prompt = (char *)malloc((unsigned long) PATHSIZE);
		helpbuf[0] = prompt[0] = 0;
	}
	len = base = strlen(helpbuf);

	if (input && strlen(input)) {
		if (len > 0)
			strcat(helpbuf, " ");
		strcat(helpbuf, input);
	}
	len = strlen(helpbuf);
	squash_spaces(helpbuf + base);  /* only bother with new stuff */


	/* now, a lone ? will print subtopics only */
	if (strcmp(helpbuf + (base ? base + 1 : 0), "?") == 0) {
		/* subtopics only */
		subtopics = 1;
		only = 1;
		helpbuf[base] = '\0';	/* cut off question mark */
	} else {
		/* normal help request */
		subtopics = 0;
		only = 0;
	}

	switch (help(helpbuf, help_ptr, &subtopics)) {
	    case H_FOUND:
		    {
			    /* already printed the help info */
			    /* subtopics now is true if there were any subtopics */

			    do {
				    if (subtopics && !only) {
					    /* prompt for subtopic with current help string */
					    if (len > 0)
						    (void) sprintf(prompt, "Subtopic of %s: ", helpbuf);
					    else
						    (void) strcpy(prompt, "Help topic: ");
						input_line = cleanup_input(readline(prompt));
					    more_help = (input_line && strlen(input_line));
					    if (more_help)
						    /* base for next level is all of current helpbuf */
						    do_help(input_line, help_ptr);
				    } else
					    more_help = FALSE;
			    } while (more_help);

			    break;
		    }
	    case H_NOTFOUND:{
			    printf("Sorry, no help for '%s'\n", helpbuf);
			    break;
		    }
	    case H_ERROR:{
			    perror(help_ptr);
			    break;
		    }
	    default:{		/* defensive programming */
			    parse_error("Impossible case in switch");
			    /* NOTREACHED */
		    }
	}

	helpbuf[base] = '\0';	/* cut it off where we started */
	return(NULL);
}

char *
cleanup_input(char *s)
{
	char *p;
	if (!s || *s == 0) return(NULL);
	while(s && *s && isspace(*s)) s++;
	for (p = s+strlen(s)-1 ; p >= s && isspace(*p) ; *p-- = 0);
	return(s);
	if (s && *s == 0) s = NULL;
	return(s);
}
int
instring(char *str, char c)
{
    int pos = 0;

    while (str != NULL && *str != '\0' && c != *str) {
       str++;
       pos++;
    }
    return (pos);
}

void
squash_spaces(char *s)
{
  register char *r = s;     /* reading point */
  register char *w = s;     /* writing point */
  int space = 0;       /* TRUE if we've already copied a space */

  for (w = r = s; *r != '\0'; r++) {
     if (isspace(*r)) {
        /* white space; only copy if we haven't just copied a space */
        if (!space) {
            space = 1;
            *w++ = ' ';
        }               /* else ignore multiple spaces */
     } else {
        /* non-space character; copy it and clear flag */
        *w++ = *r;
        space = 0;
     }
  }
  *w = '\0';                /* null terminate string */
}
