#include "parser.h"
#include "func.h"
#include <sys/stat.h>
#ifndef __MSDOS__
#include <sys/mman.h>
#endif

#include "hdf5.h"

Var *load_hdf5(hid_t parent);


Var *
make_struct(int ac, Var **av)
{
    Var *o;
    char **names;
    Var **data;
    int i;
    char *zero;

    o = newVar();
    V_TYPE(o) = ID_VSTRUCT;
    V_STRUCT(o).count = ac;
    names = V_STRUCT(o).names = (char **)calloc(ac, sizeof(char *));
    data = V_STRUCT(o).data = (Var **)calloc(ac, sizeof(Var *));

    for (i = 0 ; i < ac ; i++) {
        zero = (char *)calloc(1,1);
        /*
        ** check for duplicate names here
        */
        names[i] = strdup(V_NAME(av[i]));
        data[i] = newVal(BSQ, 1,1,1, BYTE, zero);
        mem_claim(data[i]);
    }
    return(o);
}

Var *
ff_struct(vfuncptr func, Var * arg)
{
    Var *obj = NULL, *compress = NULL, *normalize = NULL;
    int type = 0;
    int x,y,z, n, i, j, dsize, low, high, v;
    int *data;
    char *ptr = NULL;

    int ac;
    Var **av;

    make_args(&ac, &av, func, arg);
    return(make_struct(ac-1, av+1));
}

void
WriteHDF5(hid_t parent, char *name, Var *v)
{
    hid_t dataset, datatype, dataspace, aid2, attr, child;
    hsize_t size[3];
    int org;
    int top = 0;
    int i;
	 hsize_t length;
    Var *e;

    if (V_NAME(v) != NULL) {
        if ((e = eval(v)) != NULL) v = e;
    }
    if (v == NULL || V_TYPE(v) == ID_UNK) {
        parse_error("Can't find variable");
        return;
    }
        

    if (parent < 0) {
        top = 1;
        parent = H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    }

    switch (V_TYPE(v)) {
    case ID_VSTRUCT:

        /*
        ** member is a structure.  We need to create a sub-group
        ** with this name.
        */
        if (top == 0) {
            child = H5Gcreate(parent, name, 0);
        } else {
            child = parent;
        }
        for (i = 0 ; i < V_STRUCT(v).count ; i++) {
            WriteHDF5(child, V_STRUCT(v).names[i], V_STRUCT(v).data[i]);
        }
        if (top == 0) H5Gclose(child);
        break;

    case ID_VAL:

        /*
        ** member is a value.  Create a dataset
        */
        for (i = 0 ; i < 3 ; i++) {
            size[i] = V_SIZE(v)[i];
        }
        org = V_ORG(v);
        dataspace = H5Screate_simple(3, size, NULL);
        switch(V_FORMAT(v)) {
        case BYTE:  datatype = H5Tcopy(H5T_NATIVE_UCHAR); break;
        case SHORT: datatype = H5Tcopy(H5T_NATIVE_SHORT); break;
        case INT:   datatype = H5Tcopy(H5T_NATIVE_INT); break;
        case FLOAT: datatype = H5Tcopy(H5T_NATIVE_FLOAT); break;
        case DOUBLE: datatype = H5Tcopy(H5T_NATIVE_DOUBLE); break;
        }
        dataset = H5Dcreate(parent, name, datatype, dataspace, H5P_DEFAULT);
        H5Dwrite(dataset, datatype, 
                 H5S_ALL, H5S_ALL, H5P_DEFAULT, V_DATA(v));

        aid2 = H5Screate(H5S_SCALAR);
        attr = H5Acreate(dataset, "org", H5T_NATIVE_INT, aid2, H5P_DEFAULT);

        H5Awrite(attr, H5T_NATIVE_INT, &org);
        H5Sclose(aid2);
        H5Aclose(attr);

        H5Tclose(datatype);
        H5Sclose(dataspace);
        H5Dclose(dataset);
        break;


	 case ID_STRING:
		/*
		** Member is a string of characters
		*/
		length=strlen(V_STRING(v))+1;/*String+NULL*/
		dataspace = H5Screate_simple(1,&length,NULL);
		datatype = H5Tcopy(H5T_C_S1);
		H5Tset_size(datatype,strlen(V_STRING(v))+1);
		dataset = H5Dcreate(parent,name, datatype, dataspace, H5P_DEFAULT);
		H5Dwrite(dataset, datatype,H5S_ALL, H5S_ALL, H5P_DEFAULT, V_STRING(v));
		H5Tclose(datatype);
		H5Sclose(dataspace);
	 	H5Dclose(dataset);
		break;

    }
    if (top) H5Fclose(parent);
    return;
}

/*
** Make a VAR out of a HDF5 object
*/
herr_t group_iter(hid_t parent, const char *name, void *data)
{
    H5G_stat_t buf;
    hid_t child, dataset, dataspace, datatype, attr;
    int org, type, size[3],  dsize, i;
	 int var_type;
    hsize_t datasize[3], maxsize[3];
	 H5T_class_t classtype;
    Var *v;
    void *databuf;

    *((Var **)data) = NULL;

    if (H5Gget_objinfo(parent, name, 1, &buf) < 0) return -1;
    type = buf.type;

    switch (type)  {
    case H5G_GROUP:
        child = H5Gopen(parent, name);
        v = load_hdf5(child);
        V_NAME(v) = strdup(name);
        H5Gclose(child);
        break;

    case H5G_DATASET:
     if ((dataset = H5Dopen(parent, name)) < 0) {
          return 0;
     }

     datatype = H5Dget_type(dataset);
	  classtype=(H5Tget_class(datatype));

	  if (classtype==H5T_INTEGER){
        if (H5Tequal(datatype , H5T_NATIVE_UCHAR)) 
			type = BYTE;
        else if (H5Tequal(datatype , H5T_NATIVE_SHORT)) 
			type = SHORT;
        else if (H5Tequal(datatype , H5T_NATIVE_INT))   
			type = INT;
	  }
	  else if (classtype==H5T_FLOAT){
        if (H5Tequal(datatype , H5T_NATIVE_FLOAT)) 
			type = FLOAT;
        else if (H5Tequal(datatype , H5T_NATIVE_DOUBLE)) 
			type = DOUBLE;
     }

     else if (classtype=H5T_STRING){
			type=ID_STRING;
	  }

	  if (type!=ID_STRING){
       attr = H5Aopen_name(dataset, "org");
       H5Aread(attr, H5T_NATIVE_INT, &org);
       H5Aclose(attr);
	  }

       dataspace = H5Dget_space(dataset);
       H5Sget_simple_extent_dims(dataspace, datasize, maxsize);
       for (i = 0 ; i < 3 ; i++) {
        	    size[i] = datasize[i];
       }
       dsize = H5Sget_simple_extent_npoints(dataspace);
       databuf = calloc(dsize, NBYTES(type));
       H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, databuf);

       H5Tclose(datatype);
       H5Sclose(dataspace);
       H5Dclose(dataset);

		 if (type==ID_STRING){
        	v = newVal(0, 0,0,0, BYTE, databuf);
			V_STRING(v)=strdup(databuf);
			V_TYPE(v)=ID_STRING;
        	V_NAME(v) = strdup(name);
		 }
       else {
        	v = newVal(org, size[0], size[1], size[2], type, databuf);
        	V_NAME(v) = strdup(name);
		 }


    }

    *((Var **)data) = v;
    return 1;
}


herr_t count_group(hid_t group, const char *name, void *data)
{
    *((int *)data) += 1;
    return(0);
}

Var *
LoadHDF5(char *filename)
{
    Var *v;
    hid_t group;
    hid_t file;

    if (H5Fis_hdf5(filename) == 0) {
        return(NULL);
    } 

    file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);

    if (file < 0) return(NULL);

    group = H5Gopen(file, "/");

    v = load_hdf5(group);

    H5Gclose(group);
    H5Fclose(file);
    return(v);
}


Var *
load_hdf5(hid_t parent)
{
    Var *o, *e;
    int count = 0;
    int idx = 0;
    herr_t ret;

    H5Giterate(parent, ".", NULL, count_group, &count);

    if (count <= 0) {
        parse_error("Group count < 0");
        return(NULL);
    }

    o = newVar();
    V_TYPE(o) = ID_VSTRUCT;
    V_STRUCT(o).count = count;
    V_STRUCT(o).names = (char **)calloc(count, sizeof(char *));
    V_STRUCT(o).data = (Var **)calloc(count, sizeof(Var *));
    
    while ((ret = H5Giterate(parent, ".", &idx, group_iter, &e)) > 0)  {
        V_STRUCT(o).names[idx] = V_NAME(e);
        V_STRUCT(o).data[idx] = e;
        V_NAME(e) = NULL;
        idx++;
    }
    return(o);
}


Var *
LoadVanilla(char *filename)
{
#ifdef __MSDOS__
	extern unsigned char *mmap(void *, size_t , int, int , int , size_t );
	extern void munmap(unsigned char *, int);
	typedef	void*	caddr_t;
#endif
 
	int fd;
    struct stat sbuf;
    int rows;
    int cols;
    int len;
    int i, j, k;
    int state;
    char *buf, *p, **data, c;
    int *type;
    char **names;
    Var *o, *v;

    rows = cols = 0;
    
    if (stat(filename, &sbuf) != 0) {
        parse_error("Unable to open file: %s\n", filename);
        return(NULL);
    }
    if ((len = sbuf.st_size) == 0) return(NULL);

    if ((fd= open(filename, O_RDONLY)) < 0) {
        parse_error("Unable to open file: %s", filename);
        return(NULL);
    }
    buf =  mmap(NULL, len+1, (PROT_READ | PROT_WRITE), MAP_PRIVATE, fd, 0);
    close(fd);

    /*
    ** Remove trailing spaces and newlines
    */
    for (i = len - 1; i > 0 ; i--) {
        if (isspace(buf[i])) {
            buf[i] = ' ';
            continue;
        }
        break;
    }
    if (i == 0) return(NULL);			/* empty file? */

    /*
    ** How many rows?
    */
    rows = 1;
    for (i = 0 ; i < len ; i++) {
        if (buf[i] == '\n') rows++;
    }

    /*
    ** If we wanted to be nice to the user, we could allow them
    ** to create an empty structure with one line of header, no data.
    */
    if (rows == 1) {
        parse_error("No data in file: %s\n", filename);
        munmap(buf, len);
        return(NULL);
    }

    /*
    ** How many columns?
    */
    p = buf;
    while (*p != '\n') {
        while (*p == ' ' || *p == '\t') p++;
        while (!isspace(*p)) p++;
        cols++;
        while (*p == ' ' || *p == '\t') p++;
    }


    type = (int *)calloc(cols, sizeof(int));
    data = (char **)calloc(rows*cols, sizeof(char *));
	names = (char **)calloc(cols, sizeof(char *));
	names[0] = strndup(buf, p-buf+1);

	i = 0;
	for (p = strtok(names[0], " \t\n") ; p && *p ; p = strtok(NULL, " \t\n")) {
		names[i++] = p;
	}

    /*
    ** Find each column and verify the format of each column
    */

    i = j = 0;
    state = 0;
    while (i < len) {
        c = buf[i];
        if (state == 0) {		/* reading spaces */
            if (!isspace(c)) {
                if (j == rows*cols) {
                    parse_error("Too many values in file");
                    break;
                }
                data[j++] = buf+i;
                state = 1;
            }
        } else {                        /* reading !spaces */
            if (isspace(c)) {
                buf[i] = '\0';
                state = 0;
            }
        }

        /*
        ** Update column type
        */
        if (j-1 > cols) {
            k = (j-1) % cols;
            if (isdigit(c) || c == '+' || c == '-')  type[k] |= 1;
            else if (strchr("Ee.", c))               type[k] |= 2;
            else if (!isspace(c))                    type[k] |= 4;
        }

        /*
        ** verify we got enough columns per row
        */
        if (c == '\n' && (j % cols) != 0) {
            parse_error("Ragged row in file: %s row %d\n", filename, j / cols);
            break;
        }
        i++;
    }

    /* error condition */
    if (i != len || j != rows*cols) {
		fprintf(stderr, "Error condition\n");
        munmap(buf, len);
        free(data);
        return(NULL);
    }


    o = newVar();
    V_TYPE(o) = ID_VSTRUCT;
    V_STRUCT(o).count = cols;
    V_STRUCT(o).names = (char **)calloc(cols, sizeof(char *));
    V_STRUCT(o).data = (Var **)calloc(cols, sizeof(Var *));

    /*
    ** Ok, we have each column in text.  Create the Var and convert the data
    */
    for (i = 0 ; i < cols ; i++) {
        if (type[i] & 4) {			/* string */
            char **out = (char **)calloc(rows, sizeof(char *));
			char *zero;
            for (j = 0 ; j < rows ; j++)  {
                out[i] = strdup(data[(j*cols)+i]);
            }
            /* v = newString(1, cols, 1, out); */
			zero = (char *)calloc(1,1);
			v = newVal(BSQ, 1,1,1, BYTE, zero);
        } else if (type[i] & 2) {	/* float */
            float *out = (float *)calloc(rows, sizeof(float));
            for (j = 0 ; j < rows ; j++)  {
                out[i] = atof(data[(j*cols)+i]);
            }
            v = newVal(BSQ, 1, rows, 1, FLOAT, out);
        } else if (type[i] & 1) {	/* int */
            int *out = (int *)calloc(rows, sizeof(float));
            for (j = 0 ; j < rows ; j++) {
                out[i] = atoi(data[(j*cols)+i]);
            }
            v = newVal(BSQ, 1, rows, 1, INT, out);
        }
        V_STRUCT(o).names[i] = strdup(names[i]);
        V_STRUCT(o).data[i] = v;
    }

    return(o);
}

Var *
ff_add_struct(vfuncptr func, Var * arg)
{
	Var *a = NULL, b, *v = NULL, *e;
	char *name = NULL;

	int ac;
	Var **av;
	Alist alist[4];
	alist[0] = make_alist( "object",    ID_VSTRUCT,    NULL,     &a);
	alist[1] = make_alist( "name",      ID_STRING,     NULL,     &name);
	alist[2] = make_alist( "value",     ID_UNK,     NULL,     &v);
	alist[3].name = NULL;

	make_args(&ac, &av, func, arg);
	if (parse_args(ac, av, alist)) return(NULL);

	if (a == NULL) {
		parse_error("Object is null");
		return(NULL);
	}
	
	if (name == NULL && (v == NULL || (v != NULL && V_NAME(v) == NULL))) {
		parse_error("name is null");
		return(NULL);
	}

	V_TYPE(&b) = ID_UNK;
	if (name != NULL) {
		V_NAME(&b) = name;
	}  else if (v != NULL && V_NAME(v) != NULL) {
		V_NAME(&b) = strdup(V_NAME(v));
	}

	if (v == NULL) {
		v = newVal(BSQ, 1, 1, 1, BYTE, calloc(1,1));
	} else {
		e = eval(v);
		if (e == NULL) {
			parse_error("Unable to find variable: %s\n", V_NAME(v));
			return(NULL);
		}
		v = e;
	}

	return(pp_set_struct(a, &b, v));
}

Var *
ff_get_struct(vfuncptr func, Var * arg)
{
	Var *a = NULL, b, **v;
	char *name = NULL;

	int ac;
	Var **av;
	Alist alist[3];
	alist[0] = make_alist( "object",    ID_VSTRUCT,    NULL,     &a);
	alist[1] = make_alist( "name",      ID_STRING,     NULL,     &name);
	alist[2].name = NULL;

	make_args(&ac, &av, func, arg);
	if (parse_args(ac, av, alist)) return(NULL);

	if (a == NULL) {
		parse_error("Object is null");
		return(NULL);
	}
	
	if (name == NULL) {
		parse_error("name is null");
		return(NULL);
	}

	V_TYPE(&b) = ID_UNK;
	V_NAME(&b) = name;

	v = find_struct(a, &b);

	return(*v);
}
