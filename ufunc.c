#include "parser.h"

/**
 ** Load function from file.
 ** Find and verify name.
 ** Find split and verify args.
 **/

UFUNC **ufunc_list = NULL;
int nufunc = 0;
int ufsize = 16;
extern int pp_line;

UFUNC *
locate_ufunc(char *name) 
{
    int i;
    for (i = 0 ; i < nufunc ; i++) {
        if (!strcmp(ufunc_list[i]->name, name)) return(ufunc_list[i]);
    }
    return(NULL);
}

int
destroy_ufunc(char *name)
{
    int i;
    for (i = 0 ; i < nufunc ; i++) {
        if (!strcmp(ufunc_list[i]->name, name)) {
            free_ufunc(ufunc_list[i]);
            ufunc_list[i] = ufunc_list[nufunc-1];
            nufunc--;
            return(1);
        }
    }
    return(0);
}

void
store_ufunc(UFUNC *f) 
{
    if (ufunc_list == NULL) {
        ufunc_list = (UFUNC **)calloc(ufsize, sizeof(UFUNC *));
    } else {
        if (nufunc == ufsize) {
            ufsize *= 2;
            ufunc_list = (UFUNC **)my_realloc(ufunc_list, ufsize * sizeof(UFUNC *));
        }
    }
    ufunc_list[nufunc++] = f;
}

void
free_ufunc(UFUNC *f) 
{
    free(f->text);
    free(f->name);
    if (f->argbuf) free(f->argbuf);
    if (f->args) free(f->args);
    if (f->tree) free_tree(f->tree);
    free(f);
}

void
save_function()
{
    UFUNC *f;
    extern char save_file[];

    f = load_function(save_file);
    if (f == NULL) return;
    /**
     ** If a ufunc with this name exists, destroy it
     **/
    if (destroy_ufunc(f->name)) {
        if (VERBOSE) fprintf(stderr, "Replacing function %s\n", f->name);
    } else {
        if (VERBOSE) 
            fprintf(stderr, "Loaded function %s\n", f->name);
    }
    store_ufunc(f);
}

UFUNC *
load_function(char *fname)
{
    /**
     ** locate and verify important portions of function definition
     **/
    int i,j;
    struct stat sbuf;
    char *buf, *str, *p;
    int fd;
    int nlen = 0;
    char name[256];
    UFUNC *f = NULL;
    FILE *fp;
    Scope *scope;

    /**
     ** Get text from file
     **/
    if (stat(fname, &sbuf) != 0) {
        fprintf(stderr, "Internal error: load_function, no file\n");
        return(NULL);
    }
    buf = (char *)calloc(1, sbuf.st_size+1);
    if ((fd = open(fname, O_RDONLY)) < 0) {
        fprintf(stderr, "Internal error: load_function, no data in file\n");
        return(NULL);
    }
    read(fd, buf, sbuf.st_size);
    buf[sbuf.st_size] = '\0';
    close(fd);
    unlink(fname);

    str = buf;

    while(isspace(*str)) str++;
    if (strncasecmp(str, "define", 6)) {
        free(buf);
        fprintf(stderr, "Internal error: load_function, no 'define' in file\n");
        return(NULL);
    }
    str += 6;
    while(*str && isspace(*str)) str++;
    while(*str && (isalnum(*str) || *str == '_')) name[nlen++] = *str++;
    if (nlen == 0) {
        /**
         ** If we want to be nice to the user, we could spit out all
         ** the ufunc's here.
         **/
        free(buf);
        parse_error("empty DEFINE statement.\n");
        return(NULL);
    }
    name[nlen] = '\0';
    if (!(isalpha(*name) || *name == '_')) {
        sprintf(error_buf,"Function name must begin with a letter: %s", name);
        parse_error(NULL);
        free(buf);
        return(NULL);
    }

    f = (UFUNC *)calloc(1, sizeof(UFUNC));
    f->name = strdup(name);
    f->text = buf;
    f->ready = 0;

    /**
     ** should now find '( args )'.
     ** args should be limited to ids, space and numbers
     **/
    while(*str && isspace(*str)) str++;

    if (*str && *str == '(') {
        str++;
        p = str;
        while(*str && (isalnum(*str) || isspace(*str) || strchr(",_", *str))) 
            str++;
        if (*str && *str == ')') {
            /**
             ** Believe we found a complete args section.  Parse 'em up.
             ** Duplicate the string for cutting on.
             **/
            if (p != str) {
                f->argbuf = strndup(p, str-p);
                split_string(f->argbuf, &f->nargs, &f->args, ",");
            }
            /**
             ** verify arg limits
             **/
            f->min_args = -1;
            f->max_args = -1;
            if (f->nargs) {
                if (strspn(f->args[f->nargs-1], "0123456789")) {
                    f->min_args = atoi(f->args[f->nargs-1]);
                    f->nargs--;
                    if (f->nargs && strspn(f->args[f->nargs-1], "0123456789")) {
                        f->max_args = f->min_args;
                        f->min_args = atoi(f->args[f->nargs-1]);
                        f->nargs--;
                    }
                }
                if (f->max_args != -1 && f->min_args > f->max_args) {
                    parse_error("min_args > max_args.\n");
                    return(f);
                }
            }
            /**
             ** Verify arg names.
             **/
            for (i = 0 ; i < f->nargs ; i++) {
                if (!(isalpha(f->args[i][0]))) {
                    sprintf(error_buf, "Illegal argument name: %s", f->args[i]);
                    parse_error(NULL);
                    return(f);
                }
                for (j = 0 ; j < strlen(f->args[i]) ; j++) {
                    if (!(isalnum(f->args[i][j]) || f->args[i][j] == '_')) {
                        sprintf(error_buf, "Illegal argument name: %s", f->args[i]);
                        parse_error(NULL);
                        return(f);
                    }
                }
            }
        } else {
            /**
             ** Next character is not ')'.  Panic.
             **/
            sprintf(error_buf, "function %s, bad args.", f->name);
            parse_error(NULL);
            return(f);
        }
    } else {
        /**
         ** If we wanted to be nice to the user, we could spit out the
         ** contents of the function named in f->name here.
         **/
        sprintf(error_buf, "loading function %s, no args.", f->name);
        parse_error(NULL);
        return(f);
    }
    str++;

    /**
     ** Presumably, everything else is body.
     **/
    if (*str) f->body = str;

    /**
     ** Shove the function into the stream for evaluation.
     **/
    fname = tempnam(NULL, NULL);
    fp = fopen(fname, "w");
    fputs(f->body, fp);
    fclose(fp);
    fp = fopen(fname, "r");

	unlink(fname);
	xfree(fname);

    push_input_stream(fp);
    scope = new_scope();
    scope->ufunc = f;
    scope_push(scope);

    pp_line = 0;

    f->ready = 1;

    return(f);
}


/**
 ** dispatch_ufunc - dispatch a ufunc.
 **
 ** Start a new scope.
 ** Initialize all the functions named args to NOT_PRESENT.
 ** Scan args for keyword pairs.  Pull em off and store 'em in scope
 ** Init $N variables.
 ** Send function text to parser.
 **
 **
 ** Scope should include a symtab pointer to hold memory allocated
 ** while in this scope.  To be deallocated on exit.
 **/
Var *
dispatch_ufunc(UFUNC *f, Var *arg)
{
    Scope *scope = new_scope();
    int i, argc;
    Var *v, *p, *e;
    int insert = 0;

    /**
     ** Create identifiers for all the named arguments.  These dont
     ** yet have pointers to data, indicating they're NOT_PRESENT.
     **/
    for (i = 0 ; i < f->nargs ; i++) {
        dd_put(scope, f->args[i], NULL);
    }
    /**
     ** Parse through args looking for keyword pairs, and storing their
     ** values.  While we are at it, if we encounter a value without a 
     ** keyword, store it in ARGV
     **
     ** Total number of args is stored in $0
     **/
    for ( p = arg ; p != NULL ; p=p->next) {
        if (V_TYPE(p) == ID_KEYWORD) {
            if (dd_find(scope, V_NAME(p)) == 0) {
                sprintf(error_buf, "Unknown keyword to ufunc: %s(... %s= ...)",
                        f->name, V_NAME(p));
                parse_error(NULL);
                free_scope(scope);
                return(NULL);
            } else {
                v = V_KEYVAL(p);
                if ((e = eval(v)) != NULL) v = e;
                dd_put(scope, V_NAME(p), v);
            }
        } else {
            v = p;
            if ((e = eval(v)) != NULL) v = e;
            argc = dd_put_argv(scope, v);

            if (f->max_args >= 0 && argc > f->max_args) {
                sprintf(error_buf, 
                        "Too many arguments to ufunc: %s().  Only expecting %d",
                        f->name, f->max_args);
                parse_error(NULL);
				dd_unput_argv(scope);
                free_scope(scope);
                return(NULL);
            }
        }
    }
    if (f->min_args && (argc = dd_argc(scope)) < f->min_args) {
        sprintf(error_buf, 
                "Not enough arguments to ufunc: %s().  Expecting %d.", 
                f->name, f->min_args);
        parse_error(NULL);
        free_scope(scope);
		dd_unput_argv(scope);
        return(NULL);
    }
    /**
     ** Okay, now we have dealt with all the args.
     ** Push this scope into the scope stack, and run the function.
     **/
    scope->ufunc = f;
    scope_push(scope);
    evaluate(f->tree);

    /**
     **  Additionally, we need to transfer this value OUT of this scope's
     **  symtab, since that memory will go away when the scope is cleaned.
     **/
    dd_unput_argv(scope);
        
    /**
     ** options the user may exercise with the RETURN statement:
     **
     ** return($1)  - scope doesn't own this memory.
     ** return(arg) - scope doesn't own this memory either.
     ** return(sym) - must get this value out of the local symtab, to keep
     **                   it from being free'd when the scope exits.  Stuff
     **                   it into the parent's tmptab to be claimed or free'd
     ** return(tmp) - must get this value out of the local tmptab, to keep
     **                   it from being free'd when the scope exits.  Stuff
     **                   it into the parent't tmptab to be claimed or free'd
	 **
	 ** when we promote a value up to the parent's scope, if the value had
	 ** a name, we need to get rid of it.
     **/
    if ((v = scope->rval) != NULL) {
        if (mem_claim(v) != NULL || rm_symtab(v) != NULL) {
            insert = 1;
        }
    }
    clean_scope(scope_pop());
        
    if (insert) {
        Symtable *sym;
        Scope *scope = scope_tos();

		if (V_NAME(v) != NULL) {
			free(V_NAME(v));
			V_NAME(v) = NULL;
		}

        sym = (Symtable *)calloc(1, sizeof(Symtable));
        sym->value = v;
        sym->next = scope->tmp;
        scope->tmp = sym;
    }
    return(v);
}

/**
 ** find ufunc.  
 ** Spit to file.  
 ** Call editor.
 ** Check file time, and reload if newer.
 **/

Var *
ufunc_edit(vfuncptr func , Var *arg)
{
    UFUNC *ufunc;
    char *name;
    struct stat sbuf;
    time_t time = 0; 
    FILE *fp;
    char buf[256];
    char *fname, *filename, *editor;
    int temp = 0;

    if (arg == NULL) return(NULL);
    if (V_TYPE(arg) == ID_STRING) {
        filename = V_STRING(arg);
        if ((fname = locate_file(filename)) == NULL) {
            fname = filename;
        }
    } else if (V_TYPE(arg) == ID_VAL) {
		/**
		 ** Numeric arg, call hedit to do history editing()
		 **/
		 return(ff_hedit(func,arg));
	} else {
        if ((name  = V_NAME(arg)) == NULL) {
            return(NULL);
        }

        if ((ufunc = locate_ufunc(name)) == NULL) return(NULL);

        fname = tempnam(NULL,NULL);
        fp = fopen(fname, "w");
        fputs(ufunc->text, fp);
        fclose(fp);
        temp=1;
    }

    if (stat(fname, &sbuf) == 0)  {
        time = sbuf.st_mtime;
    }

    if ((editor = getenv("EDITOR")) == NULL) 
        editor = "/bin/vi";

    sprintf(buf, "%s %s", editor, fname);
    system(buf);

    if (stat(fname, &sbuf) == 0) {
        if (time != sbuf.st_mtime) {
            fp = fopen(fname, "r");
            push_input_stream(fp);
        } else {
            fprintf(stderr, "File not changed.\n");
        }
    }
    if (temp) {
		unlink(fname);
		xfree(fname);
	} else if (filename && fname != filename) free(fname);

    return(NULL);
}
