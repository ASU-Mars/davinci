/*
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define __P(a) a

#include "parser.h"

#include <sys/types.h>

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PF(out, f, func) { \
	char *b = NULL; \
	if (fieldwidth) \
		if (precision) \
			(void)asprintf(&b, f, fieldwidth, precision, func); \
		else \
			(void)asprintf(&b, f, fieldwidth, func); \
	else if (precision) \
		(void)asprintf(&b, f, precision, func); \
	else \
		(void)asprintf(&b, f, func); \
	if (b) { \
                out = str_append(out, b); \
		free(b); \
	} \
}

static int	 asciicode __P((void));
static void	 escape __P((char *));
static int	 getchr __P((char *));
static int	 getdouble __P((double *));
static int	 getint __P((int *));
static int	 getstr __P((char **));
static char	*mklong __P((char *, int));
static void	 usage __P((void));
static int asprintf(char **, char *, ...);

static Var *gv;
char *do_sprintf(vfuncptr func, Var *arg);


Var *
ff_printf(vfuncptr func, Var *arg)
{
    char *p;
    p = do_sprintf(func,arg);
    if (p) fputs(p, stdout);
    return(NULL);
}

Var *
ff_sprintf(vfuncptr func, Var *arg)
{
    char *p;
	Var *s = NULL;

    p = do_sprintf(func,arg);
    if (p) {
		s = new(Var);
		V_TYPE(s) = ID_STRING;
		V_STRING(s) = p;
	}
    return(s);
}


char *
str_append(char *s1, char *s2)
{
    s1 = (char *)realloc(s1, strlen(s1)+strlen(s2)+1);
    strcat(s1, s2);
    return(s1);
}


char *
do_sprintf(vfuncptr func, Var *arg)
{
    static char *skip1, *skip2;
    int ch, end, fieldwidth, precision;
    char convch, nextch, *format, *fmt, *start;
    Var *v, *e;
    char *out;

    /*
     * Basic algorithm is to scan the format string for conversion
     * specifications -- once one is found, find out if the field
     * width or precision is a '*'; if it is, gather up value.  Note,
     * format strings are reused as necessary to use up the provided
     * arguments, arguments of zero/null string are provided to use
     * up the format string.
     */
    skip1 = "#-+ 0";
    skip2 = "0123456789";

    if (arg == NULL) {
        parse_error("Format string must be first paramter");
        return(NULL);
    }
    v = arg;
    if ((e = eval(v)) != NULL) v = e;
    if (V_TYPE(v) != ID_STRING) {
        parse_error("Format string not specified");
        return(NULL);
    }

    out = (char *)calloc(1,1);
    escape(fmt = format = V_STRING(v));		/* backslash interpretation */
    gv = arg->next;

    for (;;) {
        end = 0;
        /* find next format specification */
    next:
        for (start = fmt;; ++fmt) {
            if (!*fmt) {
                /* avoid infinite loop */
                if (end == 1) {
                    fprintf(stderr,"missing format character", NULL, NULL);
                    free(out);
                    return (NULL);
                }
                end = 1;
                if (fmt > start)
                    out = str_append(out, start);
                if (gv == NULL) {
                    return(out);
                }
                fmt = format;
                goto next;
            }
            /* %% prints a % */
            if (*fmt == '%') {
                if (*++fmt != '%')
                    break;
                *fmt++ = '\0';
                out = str_append(out, start);
                goto next;
            }
        }

        /* skip to field width */
        for (; strchr(skip1, *fmt); ++fmt);
        if (*fmt == '*') {
            if (getint(&fieldwidth)) {
                free(out);
                return (NULL);
            }
            ++fmt;
        } else {
            fieldwidth = 0;

            /* skip to possible '.', get following precision */
            for (; strchr(skip2, *fmt); ++fmt);
        }
        if (*fmt == '.') {
            /* precision present? */
            ++fmt;
            if (*fmt == '*') {
                if (getint(&precision)) {
                    free(out);
                    return (NULL);
                }
                ++fmt;
            } else {
                precision = 0;

                /* skip to conversion char */
                for (; strchr(skip2, *fmt); ++fmt);
            }
        } else
            precision = 0;
        if (!*fmt) {
            fprintf(stderr,"missing format character", NULL, NULL);
            free(out);
            return (NULL);
        }

        convch = *fmt;
        nextch = *++fmt;
        *fmt = '\0';
        switch(convch) {
        case 'c': {
            char p;

            if (getchr(&p)) {
                free(out);
                return(NULL);
            }

            PF(out, start, p);
            break;
        }
        case 's': {
            char *p;

            if (getstr(&p)) {
                free(out);
                return(NULL);
            }       
            PF(out, start, p);
            break;
        }
        case 'd': case 'i': case 'o': case 'u': case 'x': case 'X': {
            int p;
            char *f;

            if ((f = mklong(start, convch)) == NULL || getint(&p)) {
                free(out);
                return (NULL);
            }
            PF(out, f, p);
            break;
        }
        case 'e': case 'E': case 'f': case 'g': case 'G': {
            double p;

            if (getdouble(&p)) {
                free(out);
                return(NULL);
            }
            PF(out, start, p);
            break;
        }
        default:
            fprintf(stderr,"illegal format character %c",  convch, NULL);
            free(out);
            return (NULL);
        }
        *fmt = nextch;
    }
    /* NOTREACHED */
}

static char *
mklong(char *str, int ch)
{
    static char copy[64];
    int len;

    len = strlen(str) + 2;
    memmove(copy, str, len - 3);
    copy[len - 3] = 'l';
    copy[len - 2] = ch;
    copy[len - 1] = '\0';
    return (copy);
}

static void
escape(register char *fmt)
{
     char *store;
     int value, c;

    for (store = fmt; (c = *fmt); ++fmt, ++store) {
        if (c != '\\') {
            *store = c;
            continue;
        }
        switch (*++fmt) {
        case '\0':		/* EOS, user error */
            *store = '\\';
            *++store = '\0';
            return;
        case '\\':		/* backslash */
        case '\'':		/* single quote */
            *store = *fmt;
            break;
        case 'a':		/* bell/alert */
            *store = '\7';
            break;
        case 'b':		/* backspace */
            *store = '\b';
            break;
        case 'f':		/* form-feed */
            *store = '\f';
            break;
        case 'n':		/* newline */
            *store = '\n';
            break;
        case 'r':		/* carriage-return */
            *store = '\r';
            break;
        case 't':		/* horizontal tab */
            *store = '\t';
            break;
        case 'v':		/* vertical tab */
            *store = '\13';
            break;
            /* octal constant */
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            for (c = 3, value = 0;
                 c-- && *fmt >= '0' && *fmt <= '7'; ++fmt) {
                value <<= 3;
                value += *fmt - '0';
            }
            --fmt;
            *store = value;
            break;
        default:
            *store = *fmt;
            break;
        }
    }
    *store = '\0';
}

static int
getchr(char *p)
{
    Var *v = gv, *e;

    if (v == NULL) return(1);
    if ((e = eval(v)) == NULL) {
        sprintf(error_buf, "Variable not found: %s", V_NAME(v));
        parse_error(NULL);
        return(1);
    }
    v = e;

    gv = gv->next;

    if (V_TYPE(v) == ID_VAL) {
        *p = extract_int(v, 0);
    } else if (V_TYPE(v) == ID_STRING) {
        *p = (V_STRING(v)[0]);
    }
    return(0);
}

static int
getstr(char **ip)
{
    Var *v = gv, *e;
 
    if (v == NULL) return(1);
    if ((e = eval(v)) == NULL) {
        sprintf(error_buf, "Variable not found: %s", V_NAME(v));
        parse_error(NULL);
        return(1);
    }
    v = e;
    gv = gv->next;
    
    if (V_TYPE(v) != ID_STRING) {
        parse_error("sprintf: Expected a string");
        return(1);
    } 
    *ip = V_STRING(v);
    return(0);
}

static int
getint(int *ip)
{
    Var *v = gv, *e;
 
    if (v == NULL) return(1);
    if ((e = eval(v)) == NULL) {
        sprintf(error_buf, "Variable not found: %s", V_NAME(v));
        parse_error(NULL);
        return(1);
    }
    v = e;
    gv = gv->next;

    if (V_TYPE(v) != ID_VAL) {
        sprintf(error_buf, "sprintf: Expected a value");
        parse_error(NULL);
        return(1);
    }
    *ip = extract_int(v, 0);
    return (0);
}

static int
getdouble(double *ip)
{
    Var *v = gv, *e;
 
    if (v == NULL) return(0);
    if ((e = eval(v)) == NULL) {
        sprintf(error_buf, "Variable not found: %s", V_NAME(v));
        parse_error(NULL);
        return(1);
    }
    v = e;
    gv = gv->next;

    if (V_TYPE(v) != ID_VAL) {
        sprintf(error_buf, "sprintf: Expected a value");
        parse_error(NULL);
        return(1);
    }
    *ip = extract_double(v, 0);
	return(0);
}

/* Like vsprintf but provides a pointer to malloc'd storage, which must
   be freed by the caller.
   Copyright (C) 1994 Free Software Foundation, Inc.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


unsigned long strtoul ();
/* char *malloc (); */

static int
int_vasprintf (char **result, char *format, va_list args)
{
    char *p = format;
    /* Add one to make sure that it is never zero, which might cause malloc
       to return NULL.  */
    int total_width = strlen (format) + 1;
    va_list ap = args;

    while (*p != '\0')
    {
        if (*p++ == '%')
	{
            while (strchr ("-+ #0", *p))
                ++p;
            if (*p == '*')
	    {
                ++p;
                total_width += abs (va_arg (args, int));
	    }
            else
                total_width += strtoul (p, &p, 10);
            if (*p == '.')
	    {
                ++p;
                if (*p == '*')
		{
                    ++p;
                    total_width += abs (va_arg (args, int));
		}
                else
                    total_width += strtoul (p, &p, 10);
	    }
            while (strchr ("hlL", *p))
                ++p;
            /* Should be big enough for any format specifier except %s.  */
            total_width += 30;
            switch (*p)
	    {
	    case 'd':
	    case 'i':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
	    case 'c':
                (void) va_arg (args, int);
                break;
	    case 'f':
	    case 'e':
	    case 'E':
	    case 'g':
	    case 'G':
                (void) va_arg (args, double);
                break;
	    case 's':
                total_width += strlen (va_arg (args, char *));
                break;
	    case 'p':
	    case 'n':
                (void) va_arg (args, char *);
                break;
	    }
	}
    }

    *result = (char *)malloc (total_width);
    if (*result != NULL)
        return vsprintf (*result, format, ap);
    else
        return 0;
}

int
vasprintf (char **result, char *format, va_list args)
{
    return int_vasprintf (result, format, args);
}

int
asprintf(char **str, char *fmt, ...)
{
    int ret;
    va_list ap;
 
    va_start(ap, fmt);
    ret = vasprintf(str, fmt, ap);
    va_end(ap);
    return (ret);
}
