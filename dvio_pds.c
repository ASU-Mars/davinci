#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <sys/stat.h>
#include "header.h"
#include "parser.h"
#include "io_lablib3.h"
#include "endian_norm.h"
#include "io_loadmod.h"
#include "dvio.h"
#include "dvio_pds4.h"


#define MAXOBJ 10

#define FIELD_GEN_OBJ_CLASS "GenObjClass"
#define FIELD_OBJ_CLASS     "Object"

#define GEN_OBJ_CLASS_HISTOGRAM "histogram"
#define GEN_OBJ_CLASS_HISTORY   "history"
#define GEN_OBJ_CLASS_IMAGE     "image"
#define GEN_OBJ_CLASS_QUBE      "qube"
#define GEN_OBJ_CLASS_TABLE     "table"
#define GEN_OBJ_CLASS_GROUP     "group"
#define GEN_OBJ_CLASS_PTR       "ptr"
#define GEN_OBJ_CLASS_FIELD     "field"

#define KW_GROUP "GROUP"
#define KW_END_GROUP "END_GROUP"


#define lookupAndWarn(funcName, struc, key, result) \
	if (find_struct((struc), (key), (result)) < 0){ \
		fprintf(stderr, "%s: Required keyword \"%s\" was not found.\n", (funcName), (key)); \
		return 0; \
	}

#define SWAP_INT(x,y) { int tmp = (x); (x) = (y); (y) = tmp; }

static char keyword_prefix[] = "PTR_TO_";
static int keyword_prefix_length = 8;

const char pathSeparator =
#ifdef _WIN32
							'\\';
#else
							'/';
#endif


/*
** If the object is a QUBE AND has suffix planes, we need to run special software to process,
** therefore we need to save the suffix structures without interfering with the object
** information
*/

typedef enum { PDS_QUBE, SUFFIX, PDS_IMAGE, PDS_HISTORY, PDS_TABLE, PDS_HISTOGRAM_IMAGE } PDS_Object;

typedef struct _objectInfo {
  int count;
  int ptr_count;

  PDS_Object *obj_type;       /*What kind of object is it */
  int *obj_ptr;               /*File Byte offset of ^OBJECT = */
  int *obj_size;              /*In RECORD_BYTES */
  void **obj_data;            /*Duh */
  int *obj_dirty;             /*Check this if the object needs to be freed when we're finished */

  Var *sample_suffix;
  int total_sample_count;     /*This is needed for record alignment */
  Var *line_suffix;
  int total_line_count;       /*This is needed for record alignment */
  Var *band_suffix;
  int total_band_count;       /*This is needed for record alignment */
  int Qub_Pad;                /*We can't pad the qub data block, because it doesn't get merged with the suffix
                                planes until later, so we need to save the pad amount, and do it AFTER the qube is
                                written to disk */

} objectInfo;


typedef struct _dataKeys {
  const char *ObjClass;       /*ObjClass Entry in Table; Also Used for searching */
  const char *Name;           /*Name Entry in Table; Used for searching */
  Var *Obj;                   /*Pointer to Parent Var obj to which the data will be assigned */
  char *KeyValue;             /*PTR_TO_<OBJECT> keyword entry's value */
  char *FileName;             /*Name of the file the PDS object is from,
                                could be different than the one originally given */
  int dptr;                   /*offset into file where data begins */
  size_t size;                /* size in bytes */
  OBJDESC *objDesc;

  OBJDESC *pFileDesc;         /* parent FILE object of this object */
  Var *pFileVar;              /* parent FILE object's corresponding davinci Var struct */

} dataKey;

#ifdef LITTLE_ENDIAN
extern char *var_endian(Var * v);
#endif

extern Var * dv_LoadISISFromPDS_New(FILE *fp, char *fn, int dptr, OBJDESC *qube);
extern Var * dv_LoadISISSuffixesFromPDS_New(FILE *fp, char *fname, size_t dptr, OBJDESC *qube);
extern Var * dv_LoadImage_New(FILE *fp, char *fn, int dptr, OBJDESC *image);
extern Var * dv_LoadHistogram_New(FILE *fp, char *fn, int dptr, OBJDESC *hist);

static char *history_parse_buffer(FILE * in);
static char * history_remove_isis_indents(const char *history);

static void Set_Col_Var(Var ** Data, FIELD ** f, LABEL * label, int *size, char **Bufs);
static void ProcessGroupIntoLabel(FILE * fp, int record_bytes, Var * v, char *name);
static void ProcessObjectIntoLabel(FILE * fp, int record_bytes, Var * v, char *name, objectInfo * oi);
static Var *ProcessIntoLabel(FILE * fp, int record_bytes, Var * v, int depth, size_t *label_ptr, objectInfo * oi);
Var *write_PDS_Qube(Var * core, Var * side, Var * bottom, Var * back, FILE * fp);


static Var *do_key(KEYWORD * key);
static int readDataForObjects(Var *st, dataKey objSize[], int nObj, int load_suffix_data, int continueOnError);
static Var *traverseObj(OBJDESC *top, Var *v, dataKey objSizeMap[], int *nObj, OBJDESC *pFileObj, Var *pFileVar);
static char *getGeneralObjClass(const char *objClassName);
static void pickFilename(char *outFname, const char *inFname);
static int rfQube(const dataKey *objSize, Var *vQube, int load_suffix_data);
static void rfBitField(int *j, char **Bufs, char *tmpbuf, FIELD ** f, int ptr, int row, int *size, int num_items);
static int rfTable(dataKey *objSize, Var * ob, LABEL *label);
static int rfImage(dataKey *objSize, Var * ob);
static int rfHistory(dataKey *objSize, Var *ob);
static int rfHistogram(dataKey *objSize, Var *ob);
static Var *do_loadPDS(vfuncptr func, char *filename, int data, int suffix_data);
#ifdef HAVE_LIBXML2
    static Var *do_loadPDS4(vfuncptr func, char *filename, int data, int suffix_data);
    Var *dv_loadPDS4(char *filename);
#endif

/* NOTE: Only add bonafide PDS objects */
static const char *handledObjTypes[] = { 
	GEN_OBJ_CLASS_HISTOGRAM,
	GEN_OBJ_CLASS_HISTORY,
	GEN_OBJ_CLASS_IMAGE,
	GEN_OBJ_CLASS_QUBE,
	GEN_OBJ_CLASS_TABLE
	/* NOTE: Neither groups nor ptrs are not PDS objects, that's why they are not listed here */
};
const int nHandledObjTypes = sizeof(handledObjTypes)/sizeof(char *);

/* GenObjClasses for which name-resloving should not be done */
static const char *dontResolveTypes[] = {
	GEN_OBJ_CLASS_HISTOGRAM,
	GEN_OBJ_CLASS_IMAGE,
	GEN_OBJ_CLASS_QUBE
};
const int nDontResolveTypes = sizeof(dontResolveTypes)/sizeof(char *);

static int
genObjClassCmpr(const void *v1, const void *v2){
	const char *s1 = *(const char **)v1;
	const char *s2 = *(const char **)v2;

	if (s1 == NULL && s2 == NULL)
		return 0;
	if (s1 == NULL && s2 != NULL)
		return -1;
	if (s1 != NULL && s2 == NULL)
		return 1;
	return strcasecmp(s1,s2);
}


static int
make_int(char *number)
{
  char *base;
  char *radix;
  int r_flag = 0;
  int len;
  int i = 0;
  int offset;
  len = strlen(number);

  /*Looking for # which signifies a Base notation integer */

  while (i < len) {
    if (number[i] == '#') {
      r_flag = 1;
      break;
    }
    i++;
  }

  if (!(r_flag))              /*Didn't find it, regular int */
    return (atoi(number));

  /*Gotta convert it! */

  number[i] = '\0';           /*Null it at first # */
  radix = strdup(number);
  i++;
  offset = i;                 /*Start string here now */

  while (i < len) {
    if (number[i] == '#') { /*Other one */
      number[i] = '\0';
      base = strdup((number + offset));
      return ((int) strtol(base, NULL, atoi(radix)));
    }
    i++;
  }

  return (0);                 /*No 2nd #? Then it's junk */
}


static Var *
do_key(KEYWORD * key)
{

  unsigned short kwv;
  Var *o = NULL;
  int *i;
  double *f;

  kwv = OdlGetKwdValueType(key);

  switch (kwv) {

    case ODL_INTEGER:
      i = (int *) calloc(1, sizeof(int));
      *i = make_int(key->value);
      o = newVal(BSQ, 1, 1, 1, INT, i);
      break;
    case ODL_REAL:
      f = (double *) calloc(1, sizeof(double));
      *f = strtod(key->value, NULL);
      o = newVal(BSQ, 1, 1, 1, DOUBLE, f);
      break;
    case ODL_SYMBOL:
    case ODL_DATE:
    case ODL_DATE_TIME:
      o = newVar();
      V_TYPE(o) = ID_STRING;
      V_STRING(o) = strdup(key->value);
      break;
    case ODL_TEXT:

      o = newVar();
      V_TYPE(o) = ID_STRING;
      V_STRING(o) = strdup(key->value);

      break;

    case ODL_SEQUENCE:
    case ODL_SET:
      {
        char **stuff;
        int ii;
        int ptype;
        int num = OdlGetAllKwdValuesArray(key, &stuff);

        if (num) {
          /*
          ** Previous assumption was that all types are the same.
          **
          ** This turns out to be bad if the first value is a real
          ** that just happens to not have a decimal part, and the
          ** rest do.  So, check that either everything is INT or
          ** promote to REAL.  If we get a value that's neither INT
          ** or REAL, abort and use that type (STRING is all that's left)
          */
          ptype = OdlDataType(stuff[0]);
          if (ptype == ODL_INTEGER) {
            int i;
            int ltype;
            for (i = 0 ; i < num ; i++) {
              ltype = OdlDataType(stuff[i]);
              if (ltype == ODL_REAL) ptype = ODL_REAL;
              if (ltype != ODL_INTEGER && ltype != ODL_REAL) {
                ptype = ltype;
                break;
              }
            }
          }

          switch (ptype) {
            case ODL_INTEGER:
              i = (int *) malloc(num * sizeof(int));
              for (ii = 0; ii < num; ii++)
                i[ii] = atoi(stuff[ii]);
              o = newVal(BSQ, num, 1, 1, INT, i);
              break;

            case ODL_REAL:
              f = (double *) malloc(num * sizeof(double));
              for (ii = 0; ii < num; ii++)
                f[ii] = strtod(stuff[ii], NULL);
              o = newVal(BSQ, num, 1, 1, DOUBLE, f);
              break;

            default:
              o = newVar();
              V_TYPE(o) = ID_TEXT;
              V_TEXT(o).Row = num;
              V_TEXT(o).text = (char **) calloc(num, sizeof(char *));
              for (ii = 0; ii < num; ii++) {
                V_TEXT(o).text[ii] = strdup(stuff[ii]);
              }
          }
        } else {
          o = newVar();
          V_TYPE(o) = ID_STRING;
          V_STRING(o) = (char *) calloc(1, sizeof(char));
          V_STRING(o)[0] = '\0';
        }
      }
      break;

    default:
      parse_error("Unknown PDS value type...Setting as string");
      o = newVar();
      V_TYPE(o) = ID_STRING;
      if (key->value != NULL)
        V_STRING(o) = strdup(key->value);
      else {
        V_STRING(o) = (char *) calloc(1, sizeof(char));
        V_STRING(o)[0] = '\0';
      }

  }
  return (o);
}

static char *
mod_name_if_necessary(char *name)
{
  char *new_name;
  if (name[0] != '^')
    return (name);

  new_name = (char *) calloc(strlen(&name[1]) +
                             keyword_prefix_length, sizeof(char));

  strcpy(new_name, keyword_prefix);
  strcat(new_name, &name[1]);
  return (new_name);
}

static char *
gen_next_unused_name_instance(
    char *keyname,
    Var  *s
                              )
{
  char *ser_key_name;
  int   i;
  int   max_ser_no = 1000;
  Var  *v;

  /* alloc a ridiculously large key name buffer */
  ser_key_name = (char *)calloc(strlen(keyname)+64, sizeof(char));

  for(i = 1; i < max_ser_no; i++){
    /* generate a key with the next free serial number */
    sprintf(ser_key_name, "%s_%d", keyname, i);

    if (find_struct(s, ser_key_name, &v) < 0){
      /* if this serial number is unused, return this key */
      return ser_key_name;
    }
  }

  free(ser_key_name);

  return NULL; /* no such instance found */
}

static Var *
decodePtr(KEYWORD *kw){
	char *fname;
	long  start_loc;
	short loc_type;
	Var *v = new_struct(0);

	fname = OdlGetFileName(kw, &start_loc, &loc_type);
	add_struct(v, FIELD_GEN_OBJ_CLASS, newString(strdup(GEN_OBJ_CLASS_PTR)));
	add_struct(v, "file_name", newString(fname));
	add_struct(v, "start_loc", newInt(start_loc));
	add_struct(v, "loc_type", newString(strdup(loc_type == ODL_RECORD_LOCATION? "record": "byte")));
	return v;
}

static KEYWORD *
traverseGroup(KEYWORD *kw, Var *v){
	char *kwName = NULL, *kwVal = NULL;
	Var *tmpVar = NULL;

	add_struct(v, "Object", newString(strdup("GROUP")));
	add_struct(v, FIELD_GEN_OBJ_CLASS, newString(strdup(GEN_OBJ_CLASS_GROUP)));

	for(kw = OdlGetNextKwd(kw); kw != NULL; kw = OdlGetNextKwd(kw)){
		kwName = OdlGetKwdName(kw);

		if (strcasecmp(kwName, KW_END_GROUP) == 0){
			break;
		}
		else if (strcasecmp(kwName, KW_GROUP) == 0){
			char *groupName = fix_name(OdlGetKwdValue(kw));

			Var *sub = new_struct(0);
			kw = traverseGroup(kw, sub);

			if (find_struct(v, groupName, &tmpVar) >= 0){
				char *oldGroupName = groupName;
				groupName = gen_next_unused_name_instance(oldGroupName, v);
				free(oldGroupName);
			}
			add_struct(v, groupName, sub);
		}
		else {
			add_struct(v, fix_name(kwName), do_key(kw));
		}
	}
    return kw;
}

static char *
getGeneralObjClass(const char *objClassName){
	char *p = strrchr(objClassName, '_');
	p = strdup((p != NULL)? p+1: objClassName);
	return lowercase(p);
}

static size_t
getObjSizeQube(Var *obj){
	Var *vCoreItems = NULL;
	Var *vCoreItemBytes = NULL;
	Var *vSuffixItems = NULL;
	Var *vSuffixItemBytes = NULL;
	size_t dsize = 0;
	int size[3]={1,1,1};
	int i, n;

	lookupAndWarn("getObjSizeQube", obj, "core_items", &vCoreItems);
	lookupAndWarn("getObjSizeQube", obj, "core_item_bytes", &vCoreItemBytes);
	if (vCoreItems != NULL && vCoreItemBytes != NULL){
		n = V_SIZE(vCoreItems)[0];
		for(i=0; i<n; i++)
			size[i] = extract_int(vCoreItems, i);
		dsize = size[0] * size[1] * size[2] * extract_int(vCoreItemBytes,0);

		find_struct(obj, "suffix_items", &vSuffixItems);
		find_struct(obj, "suffix_bytes", &vSuffixItemBytes);
		if (vSuffixItems != NULL && vSuffixItemBytes != NULL){
			int *suffixItems = (int *)V_DATA(vSuffixItems);
			int suffixItemBytes = V_INT(vSuffixItemBytes);

			dsize += suffixItems[0] * size[1] * size[2] * suffixItemBytes;
			dsize += suffixItems[1] * size[2] * size[0] * suffixItemBytes;
			dsize += suffixItems[2] * size[0] * size[1] * suffixItemBytes;
			dsize += suffixItems[0] * suffixItems[1] * size[2] * suffixItemBytes;
			dsize += suffixItems[1] * suffixItems[2] * size[0] * suffixItemBytes;
			dsize += suffixItems[2] * suffixItems[0] * size[1] * suffixItemBytes;
			dsize += size[0] * size[1] * size[2] * suffixItemBytes;
		}
	}

	return dsize;
}

static size_t
getObjSizeTable(Var *obj){
	Var *vRows = NULL;
	Var *vRowBytes = NULL;
	size_t dsize = 0;

	lookupAndWarn("getObjSizeTable", obj, "rows", &vRows);
	lookupAndWarn("getObjSizeTable", obj, "row_bytes", &vRowBytes);
	if (vRows != NULL && vRowBytes != NULL){
		dsize = V_INT(vRows) * V_INT(vRowBytes);
	}

	return dsize;
}

static size_t
getObjSizeHistogram(Var *obj){
	Var *vItems = NULL;
	Var *vItemBytes = NULL;
	Var *vItemBits = NULL;
	size_t dsize = 0;

	lookupAndWarn("getObjSizeHistogram", obj, "items", &vItems);
	if (find_struct(obj, "item_bytes", &vItemBytes) >= 0 ||
		find_struct(obj, "item_bits", &vItemBits) >= 0){
		dsize = V_INT(vItems) * (vItemBytes != NULL? V_INT(vItemBytes): V_INT(vItemBits)/8);
	}
	else {
		fprintf(stderr, "%s: Required keyword \"%s\" or \"%s\" was not found.\n",
			"getObjSizeHistogram", "item_bytes", "item_bits"); \
		return 0;
	}

	return dsize;
}

static size_t
getObjSizeImage(Var *obj){
	Var *vLines = NULL;
	Var *vSamples = NULL;
	Var *vSampleBits = NULL;
	size_t dsize = 0;

	lookupAndWarn("getObjSizeImage", obj, "lines", &vLines);
	lookupAndWarn("getObjSizeImage", obj, "line_samples", &vSamples);
	lookupAndWarn("getObjSizeImage", obj, "sample_bits", &vSampleBits);

	if (vLines != NULL && vSamples != NULL && vSampleBits != NULL){
		dsize = V_INT(vLines) * V_INT(vSamples) * V_INT(vSampleBits)/8;
	}

	return dsize;
}

static size_t
getObjSizeHistory(Var *obj){
	Var *vBytes = NULL;
	size_t dsize = 0;

	lookupAndWarn("getObjSizeHistory", obj, "bytes", &vBytes);
	if (vBytes != NULL){
		dsize = V_INT(vBytes);
	}

	return dsize;
}

static size_t
getObjSize(Var *obj){
	Var *vObjType = NULL;
	size_t dsize = 0;

	find_struct(obj, FIELD_GEN_OBJ_CLASS, &vObjType);

	if (vObjType != NULL){
		char *objType = V_STRING(vObjType);

		if (strcasecmp(objType, GEN_OBJ_CLASS_QUBE) == 0){
			dsize = getObjSizeQube(obj);
		}
		else if (strcasecmp(objType, GEN_OBJ_CLASS_IMAGE) == 0){
			dsize = getObjSizeImage(obj);
		}
		else if (strcasecmp(objType, GEN_OBJ_CLASS_TABLE) == 0){
			dsize = getObjSizeTable(obj);
		}
		else if (strcasecmp(objType, GEN_OBJ_CLASS_HISTOGRAM) == 0){
			dsize = getObjSizeHistogram(obj);
		}
		else if (strcasecmp(objType, GEN_OBJ_CLASS_HISTORY) == 0){
			dsize = getObjSizeHistory(obj);
		}
	}

	return dsize;
}

static char *
find_obj_name(Var *s){
	char *nameKw[] = {
		"core_name",
		"name"
	};
	int nNameKw = sizeof(nameKw)/sizeof(char *);

	int i;
	Var *v;

	for(i=0; i<nNameKw; i++){
		if (find_struct(s, nameKw[i], &v) >= 0){
			return V_STRING(v);
		}
	}
	return NULL;
}

static Var *
traverseObj(OBJDESC *top, Var *v, dataKey objSizeMap[], int *nObj, OBJDESC *pFileDesc, Var *pFileVar){
	OBJDESC *op = NULL;
	KEYWORD *kw = NULL;
	char *kwName, *objName, *groupName, *objClass, *genObjClass;
	unsigned short scope = ODL_CHILDREN_ONLY;
	Var *tmpVar = NULL;

	for(kw = OdlGetFirstKwd(top); kw != NULL; kw = OdlGetNextKwd(kw)){
		kwName = OdlGetKwdName(kw);

		if (kw->is_a_pointer){
			kwName = fix_name(mod_name_if_necessary(kwName));
			add_struct(v, kwName, decodePtr(kw));
		}
		else if (strcasecmp(kwName, KW_GROUP) == 0){
			groupName = fix_name(OdlGetKwdValue(kw));

			Var *sub = new_struct(0);
			kw = traverseGroup(kw, sub);
			if (find_struct(v, groupName, &tmpVar) >= 0){
				char *oldGroupName = groupName;
				groupName = gen_next_unused_name_instance(oldGroupName, v);
				free(oldGroupName);
			}
			add_struct(v, groupName, sub);
		}
		else {
			add_struct(v, fix_name(kwName), do_key(kw));
		}
	}

	for(op = OdlNextObjDesc(top, 0, &scope); op != NULL; op = OdlNextObjDesc(op, 0, &scope)){
		Var *sub = new_struct(0);

		objClass = OdlGetObjDescClassName(op); /* returns "SPECTRAL_QUBE" from "OBJECT = SPECTRAL_QUBE" */
		add_struct(sub, FIELD_OBJ_CLASS, newString(strdup(objClass)));

		genObjClass = getGeneralObjClass(objClass); /* returns "qube" from "SPECTRAL_QUBE" */
		add_struct(sub, FIELD_GEN_OBJ_CLASS, newString(genObjClass));

		if (strcmp(genObjClass, "file") == 0)
			traverseObj(op, sub, objSizeMap, nObj, op, sub);
		else 
			traverseObj(op, sub, objSizeMap, nObj, pFileDesc, pFileVar);

		/* If OBJECT = TABLE has a NAME = FOO then use the name FOO, unless specifically prohibited */
		if (bsearch(&genObjClass, dontResolveTypes, nDontResolveTypes, sizeof(char *), genObjClassCmpr) ||
			(objName = find_obj_name(sub)) == NULL){
			objName = objClass;
		}
		objName = fix_name(objName);

		if (find_struct(v, objName, &tmpVar) >= 0){
			char *oldObjName = objName;
			objName = gen_next_unused_name_instance(oldObjName, v);
			free(oldObjName);
		}
		add_struct(v, objName, sub);

		if (bsearch(&genObjClass, handledObjTypes, nHandledObjTypes, sizeof(char *), genObjClassCmpr)){
			/* obj-class is one that we handle reading data for - add it to the list for data read */
			objSizeMap[*nObj].Name = strdup(fix_name(objName)); /* holds objName - for readData  */
			objSizeMap[*nObj].ObjClass = strdup(fix_name(objClass)); /* holds objClass - for resolvePointers */
			objSizeMap[*nObj].size = getObjSize(sub);
			objSizeMap[*nObj].objDesc = op;
			objSizeMap[*nObj].Obj = v;
			objSizeMap[*nObj].pFileDesc = pFileDesc;
			objSizeMap[*nObj].pFileVar = pFileVar;
			objSizeMap[*nObj].FileName = NULL;
			(*nObj)++;
		}
	}

	return v;
}

static int
resolvePointers(char *fname, OBJDESC *top, Var *topVar, dataKey objSizeMap[], int n){
	char str[1024];
	int i;
	size_t lastOff, lblRecs = 0;
	int recLen;
	Var *v = NULL;

	if (find_struct(topVar, "record_bytes", &v) >= 0)
		recLen = V_INT(v);
	else
		recLen = 0;

	if (find_struct(topVar, "label_records", &v) >= 0){
		lblRecs = V_INT(v);
	}

	lastOff = lblRecs*recLen;

	for(i=0; i<n; i++){
		// If either the object was already processed, or we don't have a
		// matching FILE object for it, skip it
		if (objSizeMap[i].FileName == NULL && topVar == objSizeMap[i].pFileVar){

			sprintf(str, "ptr_to_%s", objSizeMap[i].ObjClass);

			if (find_struct(topVar, str, &v) >= 0){
				Var *vFName = NULL, *vStartLoc = NULL, *vLocType = NULL;
				char *path;

				find_struct(v, "file_name", &vFName);
				find_struct(v, "start_loc", &vStartLoc);
				find_struct(v, "loc_type", &vLocType);

				// Extract a dir prefixed filename
				path = (char *)alloca(strlen(V_STRING(vFName))+strlen(fname)+1);
				strcpy(path, fname);
				strcpy(path, dirname(path));
				strcat(path, "/");
				//strcat(path, V_STRING(vFName));
				strcat(path, basename(V_STRING(vFName)));

				objSizeMap[i].FileName = strdup(path);
				objSizeMap[i].dptr = V_INT(vStartLoc)-1;
				if (strcmp(V_STRING(vLocType), "record") == 0){
					objSizeMap[i].dptr *= recLen;
				}
			}
			else {
				objSizeMap[i].FileName = strdup(fname);
				objSizeMap[i].dptr = lastOff;
				lastOff += objSizeMap[i].size;
			}
		}
	}
}

static char *
correct_name(char *name)
{
  char *newname = strdup(name);
  int i = strlen(newname) - 1;
  while (i >= 0) {
    newname[i] = (char) toupper((int) (0xff & newname[i]));
    i--;
  }

  return (newname);
}

static void
ProcessGroupIntoLabel(FILE * fp, int record_bytes, Var * v, char *name)
{
  Var *tmpvar;
  char *struct_name;
  char *tmpname2;

  tmpname2 = correct_name(name);
  fprintf(fp, "%s = %s\r\n", KW_GROUP, tmpname2);

  get_struct_element(v, 0, &struct_name, &tmpvar);    /*Get the OBJECT Tag and temporarily NULLify it */
  /*      tmpname=strdup(struct_name); */
  /*      *struct_name='\0'; */

  struct_name[0] = '\0';

  ProcessIntoLabel(fp, record_bytes, v, 0, NULL, NULL);

  /*      memcpy(struct_name,tmpname,strlen(tmpname)); */
  /*      free(tmpname); */

  struct_name[0] = 'O';       /* Replace the O in Object */

  fprintf(fp, "%s = %s\r\n", KW_END_GROUP, tmpname2);
  free(tmpname2);

}

static char *
ProcessVarIntoString(Var * element, char *name)
{
  int flag = 0;
  int idx;
  int count;
  char tmp_string[1024];
  char *string = (char *) malloc(1024 * 32 * sizeof(char));

  if (GetSamples(V_SIZE(element), V_ORG(element)) > 1)
    flag += 1;
  if (GetLines(V_SIZE(element), V_ORG(element)) > 1)
    flag += 2;
  if (GetBands(V_SIZE(element), V_ORG(element)) > 1)
    flag += 4;

  if (flag) {
    if (((flag & 1) && (flag & 2)) ||
        ((flag & 1) && (flag & 4)) || ((flag & 2) && (flag & 4))) {

      parse_error
          ("Can't write out a plane or a cube object into the history!");
      sprintf(string, "%s = \"Error\"\r\n", correct_name(name));
      return (string);
    }
  }

  else {

    if (V_FORMAT(element) == INT) {
      sprintf(tmp_string, "%d", V_INT(element));
    }

    else if (V_FORMAT(element) == FLOAT) {
      sprintf(tmp_string, "%f", V_FLOAT(element));
    }

    else if (V_FORMAT(element) == DOUBLE) {
      sprintf(tmp_string, "%.10g", V_DOUBLE(element));
    }

    else {
      parse_error("Found a scalar which isn't covered");
      strcpy(tmp_string, "\"Error\"\r\n");
    }

    sprintf(string, "%s = ", correct_name(name));
    strcat(string, tmp_string);
    strcat(string, "\r\n");
    return (string);
  }



  /*Okay, we're dealing with a vector, NOT a scale so we have to build a list */
  sprintf(string, "%s = (", correct_name(name));
  for (idx = 0; idx < 3; idx++)
    if (flag & (1 << idx))
      break;

  switch (idx) {

    case 0:
      count = GetSamples(V_SIZE(element), V_ORG(element));
      break;

    case 1:
      count = GetLines(V_SIZE(element), V_ORG(element));
      break;

    case 2:
      count = GetBands(V_SIZE(element), V_ORG(element));
      break;
  }


  for (idx = 0; idx < count; idx++) {
    switch (V_FORMAT(element)) {

      case BYTE:

        break;

      case SHORT:

        break;

      case INT:
        sprintf(tmp_string, "%d, ", extract_int(element, idx));
        break;

      case FLOAT:
        sprintf(tmp_string, "%f, ", extract_float(element, idx));
        break;

      case DOUBLE:
        sprintf(tmp_string, "%.10g, ", extract_double(element, idx));
        break;

    }
    strcat(string, tmp_string);

  }

  /*Need to back up two chars and replace them with a ')' */
  strcpy((string + strlen(string) - 2), ")");
  strcat(string, "\r\n");
  return (string);

}


static void
ProcessHistoryIntoString(Var * v, char **theString, int *ptr)
{

  int i;
  int count;
  Var *element;
  char *name;
  char *tmpname;
  char string[1024 * 32];
  static int current_size;

  if (!(*ptr)) {              /*first time through, malloc up some memory */
    current_size = (1024 * 64);
    (*theString) = (char *) malloc(current_size);
  }


  /*Grow theString if needed*/
  if (*ptr >= (current_size / 2)) {
    current_size *= 2;
    (*theString) = realloc(*theString, current_size);
  }

  count = get_struct_count(v);
  for (i = 0; i < count; i++) {
    get_struct_element(v, i, &name, &element);
    if (V_TYPE(element) == ID_STRUCT) {
      tmpname = correct_name(name);
      sprintf(string, "%s = %s\r\n", KW_GROUP, tmpname);
      memcpy((*theString + (*ptr)), string, strlen(string));
      *ptr += strlen(string);

      ProcessHistoryIntoString(element, theString, ptr);

      sprintf(string, "%s = %s\r\n", KW_END_GROUP, tmpname);
      memcpy((*theString + (*ptr)), string, strlen(string));
      *ptr += strlen(string);

      free(tmpname);

    }

    else if (strcasecmp(name, "Object") == 0 || strcasecmp(name, "GenObjClass") == 0)
      continue;

    /*Check for non-string values and convert */
    else if (V_TYPE(element) == ID_VAL) {
      tmpname = ProcessVarIntoString(element, name);
      memcpy((*theString + (*ptr)), tmpname, strlen(tmpname));
      *ptr += strlen(tmpname);
      free(tmpname);
    }

    else if (V_TYPE(element) == ID_STRING) {

      tmpname = correct_name(name);
      sprintf(string, "%s = %s\r\n", tmpname, V_STRING(element));
      memcpy((*theString + (*ptr)), string, strlen(string));
      *ptr += strlen(string);
      free(tmpname);
    }

    else {
      parse_error
          ("History Object contains an unknown object...skipping it!");
    }

  }
}

int set_suffix(Var * suffix, Var ** final)
{
  int v_count = get_struct_count(suffix);
  Var *final_block;

#ifdef LITTLE_ENDIAN

  /*
  ** If we're little endian, we need to step through the suffix structure and var_endian
  ** EACH structure element, relpacing the copy's V_DATA item with the return object
  ** and don't forget to free up the old V_DATA object!
  */
  Var *tmp = V_DUP(suffix);   /*copy the item (it's a structure!) */
  int i = 0;
  Var *element;
  char *innername;
  void *v_data;
  for (i = 0; i < v_count; i++) {     /*Need to skip first element, it's the Object=Name element */
    get_struct_element(tmp, i, &innername, &element);
    v_data = V_DATA(element);
    V_DATA(element) = (void *) var_endian(element);
    free(v_data);
  }
  final_block = tmp;
#else
  final_block = suffix;
#endif

  *final = final_block;

  return (v_count);
}


static void
ProcessObjectIntoLabel(FILE * fp, int record_bytes, Var * v, char *name,
                       objectInfo * oi)
{

  Var *tmpvar;
  int i;
  int count;
  char *struct_name;
  int rem;
  int samples;
  int lines;
  int bands;

  char **name_list;
  int sample_idx = (-1);
  int line_idx = (-1);
  int band_idx = (-1);

  fprintf(fp, "OBJECT = %s\r\n", name);

  if (!(strcasecmp("table", name))) { /*We're processing a table! */
    get_struct_element(v, 0, &struct_name, &tmpvar);        /*Get the OBJECT Tag and temporarily NULLify it */

    /*              tmpname=strdup(struct_name); */
    /*              *struct_name='\0'; */

    struct_name[0] = '\0';

    ProcessIntoLabel(fp, record_bytes, v, 0, NULL, oi);

    /*              memcpy(struct_name,tmpname,strlen(tmpname)); */
    /*              free(tmpname); */

    struct_name[0] = 'O';

    /*Need to make use of the data object here */
    oi->obj_size[oi->count] = 0;
    oi->obj_data[oi->count] = (void *) NULL;
    oi->obj_dirty[oi->count] = 0;
    oi->obj_type[oi->count] = PDS_TABLE;
    oi->count++;
  }

  else if ((!(strcasecmp("history", name)))) {
    char end[] = "END\n";
    int ptr = 0;
    int size;

    count = get_struct_count(v);
    for (i = 0; i < count; i++) {   /*We could assume the DATA object is last, but that could lead to trouble */
      get_struct_element(v, i, &struct_name, &tmpvar);
      if (!(strcasecmp(struct_name, "data"))) {   /*Found it! */
        ProcessHistoryIntoString(tmpvar,
                                 (char **) &oi->obj_data[oi->
                                                         count],
                                 &ptr);
        memcpy(((char *) oi->obj_data[oi->count] + ptr), end,
               strlen(end));
        ptr += strlen(end);

        size = (ptr / record_bytes) + 1;
        oi->obj_data[oi->count] =
            (void *) realloc(oi->obj_data[oi->count],
                             size * record_bytes);
        if (ptr % record_bytes) {
          int rem = size * record_bytes - ptr;
          memset((char *) oi->obj_data[oi->count] + ptr, 0x20,
                 rem);
          ptr += rem;
        }
        oi->obj_size[oi->count] = (ptr / record_bytes);
        oi->obj_dirty[oi->count] = 1;
        oi->obj_type[oi->count] = PDS_HISTORY;
        oi->count++;
        break;
      }


    }

    get_struct_element(v, 0, &struct_name, &tmpvar);        /*Get the OBJECT Tag and temporarily NULLify it */

    /*              tmpname=strdup(struct_name); */
    /*              *struct_name='\0'; */

    struct_name[0] = '\0';

    ProcessIntoLabel(fp, record_bytes, v, 1, NULL, oi);

    /*              memcpy(struct_name,tmpname,strlen(tmpname)); */
    /*              free(tmpname); */


    struct_name[0] = 'O';

  }

  /*
  ** The QUBE (SPECTRAL_QUBE):
  ** The Qube requires that we not only calculate it's size but that we figure out
  ** if there are additional pieces of data, ie SUFFIX_PLANES.  We must traverse
  ** the entire spectral_qube structure looking for 4 things:
  **                      1) data
  **                      2) sample_suffix
  **                      3) line_suffix
  **                      4) band_suffix
  **
  **      The first one (data) is of course mandatory.  The suffix objects however,
  ** are not and can appear altogether, in any partial combination, or not
  ** at all.  Therefore, we MUST determine if any are present and adjust the qube's
  ** size accordingly.
  */


  else if ((!(strcasecmp("spectral_qube", name))) || (!(strcasecmp("qube", name))) || (!(strcasecmp("image", name)))) {       /*We're processing a qube */

    count = get_struct_names(v, &name_list, NULL);

    for (i = 0; i < count && name_list[i] != NULL; i++) {
      if (!(strcasecmp("sample_suffix", name_list[i])))
        sample_idx = i;
      else if (!(strcasecmp("line_suffix", name_list[i])))
        line_idx = i;
      else if (!(strcasecmp("band_suffix", name_list[i])))
        band_idx = i;

      free(name_list[i]);
    }

    free(name_list);

    if (sample_idx >= 0) {
      get_struct_element(v, sample_idx, &struct_name, &tmpvar);
      oi->total_sample_count =
          set_suffix(tmpvar, &oi->sample_suffix);
    }

    if (line_idx >= 0) {
      get_struct_element(v, line_idx, &struct_name, &tmpvar);
      oi->total_line_count = set_suffix(tmpvar, &oi->line_suffix);
    }

    if (band_idx >= 0) {
      get_struct_element(v, band_idx, &struct_name, &tmpvar);
      oi->total_band_count = set_suffix(tmpvar, &oi->band_suffix);
    }


    count = get_struct_count(v);
    for (i = 0; i < count; i++) {   /*We could assume the DATA object is last, but that could lead to trouble */
      get_struct_element(v, i, &struct_name, &tmpvar);

      if (!(strcasecmp(struct_name, "data"))) {   /*Found it! */

#ifdef LITTLE_ENDIAN
        Var *data = V_DUP(tmpvar);
        char *tmp = V_DATA(data);
        V_DATA(data) = var_endian(data);
        free(tmp);

        oi->obj_data[oi->count] = data;
        oi->obj_dirty[oi->count] = 1;
#else
        oi->obj_data[oi->count] = tmpvar;
        /*Now we should have a pointer to the data */
        oi->obj_dirty[oi->count] = 0;
#endif

        oi->obj_size[oi->count] =
            (V_SIZE(tmpvar)[0] * V_SIZE(tmpvar)[1] *
             V_SIZE(tmpvar)[2] * NBYTES(V_FORMAT(tmpvar)));

        /*
        ** We've calculated the size of the CORE but we need to check for SUFFIX
        ** information which will have already been processed above.
        */

        samples = GetX(tmpvar);
        lines = GetY(tmpvar);
        bands = GetZ(tmpvar);

        oi->obj_size[oi->count] += bands * lines * oi->total_sample_count * 4 + bands * samples * oi->total_line_count * 4 + samples * lines * oi->total_band_count * 4 + bands * oi->total_sample_count * oi->total_line_count * 4;    /*Need to add 2nd corner info! */




        /* Check for alignment and pad as needed */

        if (oi->obj_size[oi->count] % record_bytes) {
          /* We're not aligned to record bytes..copy and pad */

          rem = (oi->obj_size[oi->count] / record_bytes);
          rem++;
          rem *= record_bytes;
          oi->Qub_Pad = rem - oi->obj_size[oi->count];        /*#-Bytes we'll need to pad */

          oi->obj_size[oi->count] = rem / record_bytes;
        } else
          oi->obj_size[oi->count] /= record_bytes;

        if (!(strcasecmp("image", name)))
          oi->obj_type[oi->count] = PDS_IMAGE;
        else
          oi->obj_type[oi->count] = PDS_QUBE;

        oi->count++;
        break;
      }
    }

    /*finish processing the object label */
    /*We send in nulls because at this point....we SHOULD NOT find any labels or objects that we want! */

    get_struct_element(v, 0, &struct_name, &tmpvar);        /*Get the OBJECT Tag and temporarily NULLify it */

    /*              tmpname=strdup(struct_name); */
    /*              *struct_name='\0'; */

    struct_name[0] = '\0';

    ProcessIntoLabel(fp, record_bytes, v, 0, NULL, oi);

    /*              memcpy(struct_name,tmpname,strlen(tmpname)); */
    /*              free(tmpname); */

    struct_name[0] = 'O';

  }

  else {
    ProcessIntoLabel(fp, record_bytes, v, 0, NULL, oi);
  }

  fprintf(fp, "END_OBJECT = %s\r\n", name);
}

/*Any Var which has more than a single value must be "iterated" */
void output_big_var(FILE * out, Var * data, char *inset, char *name)
{
  int numelements = V_SIZE(data)[0] * V_SIZE(data)[1] * V_SIZE(data)[2];
  int i;
  char *cp;
  short *sp;
  int *ip;
  float *fp;
  double *dp;
  char dmrk[10];



  fprintf(out, "%s%s = (", inset, name);
  strcpy(dmrk, ", ");
  for (i = 0; i < numelements; i++) {
    if ((i + 1) == numelements)
      strcpy(dmrk, ")\r\n");

    switch (V_FORMAT(data)) {

      case BYTE:
        cp = ((char *) V_DATA(data));
        fprintf(out, "%d%s", cp[i], dmrk);
        break;

      case SHORT:
        sp = ((short *) V_DATA(data));
        fprintf(out, "%d%s", sp[i], dmrk);
        break;


      case INT:
        ip = ((int *) V_DATA(data));
        fprintf(out, "%d%s", ip[i], dmrk);
        break;


      case FLOAT:
        fp = ((float *) V_DATA(data));
        fprintf(out, "%f%s", fp[i], dmrk);
        break;


      case DOUBLE:
        dp = ((double *) V_DATA(data));
        fprintf(out, "%.10g%s", dp[i], dmrk);
        break;
    }
  }
}

Var *
ProcessIntoLabel(FILE * fp, int record_bytes, Var * v, int depth,
                      size_t *label_ptrs, objectInfo * oi)
{
  int i;
  /* int label_ptrs[3]; 0=FILE_RECORDS; 1=LABEL_RECORDS */

  int count;
  Var *data = newVar();
  Var *tmp_var = newVar();
  char *name;
  char pad[26] = { 0 };
  char inset[1024] = { '\0' };
  char *tmpname;

  depth++;

  if (depth > 0) {
    memset(inset, 0x0, depth + 1);  /* if depth is > 1023 I'll eat my hat! */
    memset(inset, '\t', depth);
  } else
    inset[0] = '\0';


  memset(pad, 0x20, 25);

  count = get_struct_count(v);
  i = 0;

  while (count > 0) {
    get_struct_element(v, i++, &name, &data);

    if (name == NULL) {
      parse_error("Found a NULL element...skipping");
      count--;
      continue;
    } else if (*name == '\0') {
      count--;
      continue;
    }
    if (!(strcmp((name), FIELD_GEN_OBJ_CLASS))) {
      	count--;
		continue;
	}

    /* There are a set of FIXED label items we look for */

    /*Don't want the first label indented...screws up OUR readers */
    if (!(strcasecmp((name), "PDS_VERSION_ID"))) {
      fprintf(fp, "PDS_VERSION_ID = \"PDS3\"\r\n");
    }


    else if (!(strcasecmp((name), "FILE_RECORDS"))) {
      label_ptrs[0] = ftell(fp) + strlen("FILE_RECORDS = ");
      fprintf(fp, "%sFILE_RECORDS = %s\r\n", inset, pad);
    }

    else if (!(strcasecmp(name, "LABEL_RECORDS"))) {
      label_ptrs[1] = ftell(fp) + strlen("LABEL_RECORDS = ");
      fprintf(fp, "%sLABEL_RECORDS = %s\r\n", inset, pad);
    }


    else if (!(strncasecmp(name, "PTR_TO_", 7))) {
      oi->obj_ptr[oi->ptr_count++] = ftell(fp) + strlen((name + 7)) + 4;  /* +4 for the space and ='s */
      tmpname = correct_name((name + 7));
      fprintf(fp, "%s^%s = %s\r\n", inset, tmpname, pad);
      free(tmpname);
    }

    else if (!(strncasecmp(name, "END", 3))) {
      return NULL;             /*This function is used recusively from the group/object functions */
    }

    else if (!(strcasecmp(name, "DATA"))) {
      count--;
      continue;           /*Skip over the data object */
    }

    else if (V_TYPE(data) == ID_STRUCT) {
      char *newname;
      get_struct_element(data, 0, &newname, &tmp_var);

      if ((!(strcasecmp("sample_suffix", name))) ||
          (!(strcasecmp("line_suffix", name))) ||
          (!(strcasecmp("band_suffix", name))) ||
          (!(strcasecmp("suffix_data", name)))) {
        count--;
        continue;
      }
      else {
    	  if (!(strcmp(newname, FIELD_GEN_OBJ_CLASS))) {
    		  get_struct_element(data, 1, &newname, &tmp_var);
    	  }

    	  if (strcmp("Object", (newname))) {
    		  parse_error("Parsing unknown structure");
    		  ProcessIntoLabel(fp, record_bytes, data, depth, label_ptrs,
    				  oi);
    	  }

    	  else {
    		  //strcmp or strcase? I think strcase will help in some rare situations
    		  if (!(strcasecmp(KW_GROUP, (V_STRING(tmp_var)))))
    			  ProcessGroupIntoLabel(fp, record_bytes, data, name);

    		  else
    			  ProcessObjectIntoLabel(fp, record_bytes, data,
    					  V_STRING(tmp_var), oi);
    	  }
      }


    }

    else if (V_TYPE(data) == ID_TEXT) {
      int ti;
      tmpname = correct_name(name);
      fprintf(fp, "%s%s = (", inset, tmpname);
      free(tmpname);
      for (ti = 0; ti < V_TEXT(data).Row - 1; ti++)
        fprintf(fp, "%s, ", V_TEXT(data).text[ti]);
      fprintf(fp, "%s)\r\n", V_TEXT(data).text[ti]);
    }




    else if (V_TYPE(data) == ID_STRING) {
      tmpname = correct_name(name);
      fprintf(fp, "%s%s = %s\r\n", inset, tmpname, V_STRING(data));
      free(tmpname);

    }

    else if (V_TYPE(data) == ID_VAL) {

      if (V_SIZE(data)[0] > 1 ||
          V_SIZE(data)[1] > 1 || V_SIZE(data)[2] > 1) {
        int axis_count = 0;
        if (V_SIZE(data)[0] > 1)
          axis_count++;
        if (V_SIZE(data)[1] > 1)
          axis_count++;
        if (V_SIZE(data)[2] > 1)
          axis_count++;
        if (axis_count > 1) {
          parse_error
              ("I'm not writing out anything larger than a vector (ie no planes or qubes OTHER than data)");
          count--;
          continue;
        }
        output_big_var(fp, data, inset, correct_name(name));
      }

      else {

        tmpname = correct_name(name);
        switch (V_FORMAT(data)) {

          case BYTE:
            fprintf(fp, "%s%s = %d\r\n", inset, tmpname,
                    (0xff & V_INT(data)));
            break;

          case SHORT:
            fprintf(fp, "%s%s = %d\r\n", inset, tmpname,
                    (0xffff & V_INT(data)));
            break;

          case INT:
            fprintf(fp, "%s%s = %d\r\n", inset, tmpname,
                    V_INT(data));
            break;

          case FLOAT:
            fprintf(fp, "%s%s = %12.6f\r\n", inset, tmpname,
                    V_FLOAT(data));
            break;

          case DOUBLE:
            fprintf(fp, "%s%s = %.10g\r\n", inset, tmpname,
                    (*((double *) V_DATA(data))));
            break;
        }               /*Switch */

        free(tmpname);
      }                   /*else */

    }
    /*else if */
    else {
      parse_error("What the hell is this: %d", V_TYPE(data));
    }

    count--;
    fflush(fp);
  }
}




void
Fix_Label(FILE * fp, int record_bytes, size_t *label_ptr, objectInfo * oi)
{
  int rem;
  char *pad;
  size_t size;
  size_t size_in_records;
  int i;
  size_t total;

  rewind(fp);
  /*
  ** label_ptr[0] is the #-bytes into fp where the FILE_RECORDS=
  ** label_ptr[1] is the #-bytes into fp where the LABEL_RECORDS=
  ** label_ptr[2] is the #-bytes into fp where the label ends.
  */

  size = label_ptr[2];
  fseek(fp, size, SEEK_SET);  /*back to end! */
  size_in_records = size / record_bytes;
  rem = (size % record_bytes);
  if (rem) {                  /*there is always the possibility that we're on the money! */
    size_in_records++;
    rem = ((size_in_records) * record_bytes) - size;
    pad = (char *) malloc(rem);
    memset(pad, 0x20 /*space */ , rem - 1);
    memset(pad + rem - 1, 0x0a /* \n */ , 1);
    fwrite(pad, sizeof(char), rem, fp);     /*Okay! now our label is padded appropriately */
    fflush(fp);
  }



  total = size_in_records;

  /*Now to go back and fix the  label spots*/

  rewind(fp);
  fseek(fp, label_ptr[1] + 1, SEEK_SET);
  fprintf(fp, "%ld", size_in_records);
  rewind(fp);

  size_in_records++;

  for (i = 0; i < oi->count; i++) {
    rewind(fp);
    fseek(fp, oi->obj_ptr[i] + 1, SEEK_SET);
    fprintf(fp, "%ld", size_in_records);
    size_in_records += oi->obj_size[i];
    total += oi->obj_size[i];
  }

  rewind(fp);
  fseek(fp, label_ptr[0] + 1, SEEK_SET);
  fprintf(fp, "%ld", total);

}

Var *WritePDS(vfuncptr func, Var * arg)
{
  Var *v = NULL;
  FILE *fp;
  int force = 0;
  char *name;
  Var *data = newVar();
  int count;
  int i;
  char *filename = NULL, *fname = NULL;
  int record_bytes;           /*            Gotta keep track of this baby! */
  size_t label_ptr[4];
  objectInfo oi;

  Alist alist[4];
  alist[0] = make_alist("object", ID_STRUCT, NULL, &v);
  alist[1] = make_alist("filename", ID_STRING, NULL, &filename);
  alist[2] = make_alist("force", INT, NULL, &force);
  alist[3].name = NULL;

  if (parse_args(func, arg, alist) == 0)
    return (NULL);

  if (v == NULL) {
    parse_error("%s: No object specfiied.", func->name);
    return (NULL);
  }

  if (filename == NULL) {
    parse_error("%s: No filename specfiied.", func->name);
    return (NULL);
  }

  fname = dv_locate_file(filename);

  if (!file_exists(fname)) {
    parse_error("%s: Unable to expand filename %s\n", func->name,
                filename);
    return (NULL);
  }

  if (!force && access(fname, F_OK) == 0) {
    parse_error("%s: File %s already exists.", func->name, filename);
    return (NULL);
  }

  if ((fp = fopen(fname, "wb")) == NULL) {
    fprintf(stderr, "%s: Unable to open file: %s\n", func->name,
            filename);
    return (NULL);
  }

  oi.obj_ptr = (int *) malloc(sizeof(int) * MAXOBJ);
  oi.obj_size = (int *) malloc(sizeof(int) * MAXOBJ);
  oi.obj_dirty = (int *) malloc(sizeof(int) * MAXOBJ);
  oi.obj_type = (PDS_Object *) malloc(sizeof(PDS_Object) * MAXOBJ);
  oi.obj_data = (void **) malloc(sizeof(void *) * MAXOBJ);
  oi.count = 0;
  oi.ptr_count = 0;
  oi.sample_suffix = NULL;
  oi.total_sample_count = 0;
  oi.line_suffix = NULL;
  oi.total_line_count = 0;
  oi.band_suffix = NULL;
  oi.total_band_count = 0;
  oi.Qub_Pad = 0;

  count = get_struct_count(v);
  i = 0;
  while (count >= 0) {
    get_struct_element(v, i++, &name, &data);       /* check and see if this is what it should be */
    if (!(strcasecmp("record_bytes", name))) {
      record_bytes = V_INT(data); /*Contains the integer value for the size of a record */
      break;
    }
    count--;
  }
  if (count < 0) {
    parse_error
        ("Your object doesn't contain the necessary elements for a PDS label");
    fclose(fp);
    return (NULL);
  }

  memset(label_ptr, 0, sizeof(label_ptr));
  ProcessIntoLabel(fp, record_bytes, v, -1, label_ptr, &oi);
  fprintf(fp, "END\r\n");
  label_ptr[2] = ftell(fp);
  Fix_Label(fp, record_bytes, label_ptr, &oi);
  fseek(fp, 0, SEEK_END);
  /*
  ** At this point, the data object should have themselves
  **  been padded as needed so that we can we can just dump
  ** 'em
  */

  for (i = 0; i < oi.count; i++) {
    if (oi.obj_type[i] != PDS_QUBE) {
      fwrite(oi.obj_data[i], sizeof(char),
             (oi.obj_size[i] * record_bytes), fp);
      if (oi.obj_dirty[i]) {
        free(oi.obj_data[i]);
      }
    } else {
      if (write_PDS_Qube
          ((Var *) oi.obj_data[i], oi.sample_suffix, oi.line_suffix,
           oi.band_suffix, fp) == NULL) {
        parse_error("Error Writing Qube!");
        fclose(fp);
        return (NULL);
      }
      if (oi.Qub_Pad) {
        char *pad = (char *) malloc(oi.Qub_Pad * sizeof(char));
        memset(pad, 0x20, oi.Qub_Pad);
        fwrite(pad, sizeof(char), oi.Qub_Pad, fp);
      }
    }
  }
  fclose(fp);
  return (NULL);
}


#ifdef HAVE_LIBISIS
Var *
ff_write_isis_cub(vfuncptr func, Var *args)
{
  Var   *obj = NULL;
  char  *fname = NULL;
  int    force = 0;
  Alist  alist[4];

  alist[0] = make_alist("obj",      ID_STRUCT, NULL, &obj);
  alist[1] = make_alist("filename", ID_STRING, NULL, &fname);
  alist[2] = make_alist("force",    INT,       NULL, &force);
  alist[3].name = NULL;

  if (parse_args(func, args, alist) == 0) return NULL;

  if (obj == NULL){
    parse_error("%s(): Missing \"%s\".", func->name, alist[0].name);
    parse_error("Usage: %s(obj=STRUCT, filename=STRING [, force=1])", func->name);
    parse_error("Example: a=load_pds(\"foo.cub\"); %s(a, \"bar.cub\")", func->name);
    return NULL;
  }

  if (fname == NULL){
    parse_error("%s(): Missing \"%s\".\n", func->name, alist[1].name);
    return NULL;
  }

  return newInt(dv_WriteISISStruct(obj, fname, force));
}
#endif /* HAVE_LIBISIS */


/**
 ** Historically ReadPDS did not load suffix data. Thus loading
 ** suffix data by default may break davinci scripts left and right.
 ** Also, integrating this conditional load of suffix data does not
 ** fit very well in the current call structure of rf_XXXX functions.
 ** In order for a proper fit to happen, we may want an element by
 ** the name of "function_data", which holds the special data needed
 ** by one particular reader function.
 **
 ** For now, I am controlling loading of suffix data using the
 ** following global variable.
 **/

Var *ReadPDS(vfuncptr func, Var * arg)
{
  Var *fn = NULL;
  char *filename = NULL;
  int data = 1;
  int suffix_data = 0;
  int i;

  Alist alist[4];
  alist[0] = make_alist("filename", ID_UNK, NULL, &fn);
  alist[1] = make_alist("data", INT, NULL, &data);
  alist[2] = make_alist("suffix_data", INT, NULL, &suffix_data);
  alist[3].name = NULL;

  if (parse_args(func, arg, alist) == 0)
    return (NULL);

  /* Handle loading many filenames */
  if (fn != NULL && V_TYPE(fn) == ID_TEXT) {
    Var *s = new_struct(V_TEXT(fn).Row);
    for (i = 0; i < V_TEXT(fn).Row; i++) {
      filename = strdup(V_TEXT(fn).text[i]);
      Var *t = do_loadPDS(func, filename, data, suffix_data);
      if (t) {
        add_struct(s, filename, t);
      }
    }
    if (get_struct_count(s)) {
      return (s);
    } else {
      free_struct(s);
      return (NULL);
    }
  } else if (fn != NULL && V_TYPE(fn) == ID_STRING) {
    filename = V_STRING(fn);
    return(do_loadPDS(func, filename, data, suffix_data));
  } else {
    parse_error("Illegal argument to function %s(%s), expected STRING",
                func->name, "filename");
    return (NULL);
  }
}

static Var *
do_loadPDS(vfuncptr func, char *filename, int data, int suffix_data)
{
  OBJDESC *ob, *obFile;
  KEYWORD *key;
  char *err_file = NULL;
  char *fname;
  dataKey objSize[100];
  int nObj = 0;
  FILE *fp;
  Var *v = NULL, *vFile;
  char fileObjName[128];
  int i;


  if (filename == NULL) {
    parse_error("%s: No filename specified\n", func->name);
    return (NULL);
  }
  if ((fname = dv_locate_file(filename)) == (char*)NULL) {
    parse_error("%s: Unable to expand filename %s\n", func->name,
                filename);
    return (NULL);
  }

  if (access(fname, R_OK) != 0) {
    parse_error("%s: Unable to find file %s.", func->name, filename);
    return (NULL);
  }

  /**
  *** What about compression?
  **/
  if ((fp = fopen(fname, "rb")) != NULL) {
    if (iom_is_compressed(fp)) {
      /* fprintf(stderr, "is compressed\n");    FIX: remove */
      fclose(fp);
      fname = iom_uncompress_with_name(fname);
      fp = fopen(fname, "rb");
    }
  }

  /* In load_pds case, we will allow IO modules to take a crack at it first,
     chiefly because our ODL parser is a very spammy parser and doesn't detect
     that it may be weird until it does a lot of screen output.  If we can
     sidetrack it with a more elegant, purpose-built IO module, let's do so.

     Of course, there's no guarantee that an I/O module won't be spammy either.
  */

#ifdef BUILD_MODULE_SUPPORT
  if ((v = load_pds_from_io_module(fp, fname, data, suffix_data)) != NULL) {
    fclose(fp);
    return v;
  }
#endif
  ob = (OBJDESC *) OdlParseLabelFile(fname, err_file, ODL_EXPAND_STRUCTURE, VERBOSE == 0);
  fclose(fp);

  if (ob == NULL){
    parse_error("%s: Unable to parse file %s\n", func->name, fname);
	return NULL;
  }

  v = new_struct(0);
  traverseObj(ob, v, objSize, &nObj, ob, v);

  // Resolve ^XXXX pointers at the top level (implicit) FILE object
  //resolvePointers(fname, ob, v, objSize, nObj);
  // Traverse the ^XXXX pointers in the explicit FILE objects
  vFile = v;
  obFile = ob;
  for (i = 0; i<nObj; i++){
	resolvePointers(fname, objSize[i].pFileDesc, objSize[i].pFileVar, objSize, nObj);
  }

  if (data) {
     // TODO Read data within explicit FILE objects
	 readDataForObjects(v, objSize, nObj, suffix_data, 1);
  }

  // TODO Free space consumed by FileNames allocated within objSize array
  OdlFreeTree(ob);

  if (get_struct_count(v) == 0){
  	mem_claim(v);
  	free_struct(v);
  	return NULL;
  }
  return (v);
}

static int
readDataForObjects(Var *st, dataKey objSize[], int nObj, int load_suffix_data, int continueOnError){
	int i;
	int rc = 1;
	char *fileName;
	Var *sub = NULL, *vObjClass = NULL;
	LABEL *label = NULL;

	for(i=0; i<nObj && (rc || continueOnError); i++){
		if (find_struct(objSize[i].Obj, objSize[i].Name, &sub) >= 0){
		//if (find_struct(st, objSize[i].Name, &sub) >= 0){
			if (find_struct(sub, FIELD_GEN_OBJ_CLASS, &vObjClass) >= 0){
				char *objClass = V_STRING(vObjClass);
				if (strcasecmp(objClass, GEN_OBJ_CLASS_QUBE) == 0){
					rc &= rfQube(&objSize[i], sub, load_suffix_data);
				}
				else if (strcasecmp(objClass, GEN_OBJ_CLASS_IMAGE) == 0){
					rc &= rfImage(&objSize[i], sub);
				}
				else if (strcasecmp(objClass, GEN_OBJ_CLASS_HISTORY) == 0){
					rc &= rfHistory(&objSize[i], sub);
				}
				else if (strcasecmp(objClass, GEN_OBJ_CLASS_TABLE) == 0){
					if (objSize[i].FileName == NULL){
						fprintf(stderr, "Null filename while reading TABLE\n");
						return 0;
					}

					fileName = (char *)alloca(strlen(objSize[i].FileName)+1);
					pickFilename(fileName, objSize[i].FileName);

					label = LoadLabelFromObjDesc(objSize[i].objDesc, fileName);
					if (label == NULL) {
						fprintf(stderr, "Unable to load label from \"%s\".\n", fileName);
						return 0;
					}

					rc &= rfTable(&objSize[i], sub, label);
				}
				else if (strcasecmp(objClass, GEN_OBJ_CLASS_HISTOGRAM) == 0){
					rc &= rfHistogram(&objSize[i], sub);
				}
			}
		}
	}

	return rc;
}

static void
pickFilename(char *outFname, const char *inFname){
	struct stat sbuf;

	strcpy(outFname, inFname);
	if (stat(outFname, &sbuf) == 0)
		return;

	strcpy(outFname, inFname);
	lowercase(basename(outFname));
	if (stat(outFname, &sbuf) == 0)
		return;

	strcpy(outFname, inFname);
	uppercase(basename(outFname));
	if (stat(outFname, &sbuf) == 0)
		return;

	strcpy(outFname, inFname);
}


static int
rfQube(const dataKey *objSize, Var *vQube, int load_suffix_data){
	FILE *fp;
	Var *data = NULL, *suffix_data = NULL;
	char *fileName;

	if (objSize->FileName == NULL){
		fprintf(stderr, "Null filename while reading QUBE\n");
		return 0;
	}
	
	fileName = (char *)alloca(strlen(objSize->FileName)+1);
	pickFilename(fileName, objSize->FileName);
	parse_error("Reading %s qube from %s...\n", objSize->Name? objSize->Name: "unnamed", fileName);
	if ((fp = fopen(fileName, "rb")) == NULL){
		fprintf(stderr, "Unable to open file for reading: \"%s\". Reason: %s\n", fileName, strerror(errno));
		return 0;
	}

	data = dv_LoadISISFromPDS_New(fp, fileName, objSize->dptr, objSize->objDesc);
	if (data == NULL) {
		fclose(fp); 
		return 0;
	}

	add_struct(vQube, fix_name("DATA"), data);

	if (load_suffix_data){
		suffix_data = dv_LoadISISSuffixesFromPDS_New(fp, fileName, objSize->dptr, objSize->objDesc);
		if (suffix_data == NULL){
			fclose(fp);
			return 0;
		}

		/* create a suffix part only when there are suffixes, i.e.
		   avoid blank suffix structure */
		if (get_struct_count(suffix_data) > 0){
			add_struct(vQube, fix_name("SUFFIX_DATA"), suffix_data);
		}
		else {
			mem_claim(suffix_data);
			free_struct(suffix_data);
			/* NOTE: if one does not do mem_claim and free_struct
			   the garbage collector will take care of it */
		}
	}

	fclose(fp);
	return 1;
}

static void
rfBitField(int *j, char **Bufs, char *tmpbuf, FIELD ** f, int ptr, int row, int *size, int num_items)
{
  int i = (*j);
  /*
  ** We've already loaded the entire contents of the all the bit collumns into a single value,
  ** now we're going to iterate through each bit-collumn and place it (but first we step one forward
  ** so that we are now on the first bit collumn
  */
  i++;
  while (i < num_items && f[i]->bitfield != NULL) {
    memcpy(Bufs[i] + row * size[i], (tmpbuf + ptr), size[i]);       /*Copy the whole thing in we'll fix it later */
    i++;
  }
  *j = (i - 1);
}


static int
rfTable(dataKey *objSize, Var * ob, LABEL *label){
	Var *data;
	char **bufs;
	char *tmpbuf;
	int i, j, k;
	int fd;
	int Offset;
	FIELD **f;
	int *size;
	int err;
	int num_items = 0;
	int rc;
	char *fileName;
	
	if (objSize->FileName == NULL){
		fprintf(stderr, "Null filename while reading TABLE\n");
		return 0;
	}

	fileName = (char *)alloca(strlen(objSize->FileName)+1);
	pickFilename(fileName, objSize->FileName);
	parse_error("Reading %s table from %s...\n", objSize->Name? objSize->Name: "unnamed", fileName);

	f = (FIELD **) label->fields->ptr;
	num_items = label->fields->number;  /*This is a count of BOTH columns AND bit-columns */

	/*Add new structure to parent ob*/
	data = new_struct(0);

	/*Initialize a set of buffers to read in the data */
	bufs = (char **) calloc(num_items, sizeof(char *));
	tmpbuf = (char *) calloc(label->reclen, sizeof(char));
	size = (int *) calloc(num_items, sizeof(int));
	for (j = 0; j < num_items; j++) {
		size[j] = f[j]->dimension? f[j]->size * f[j]->dimension: f[j]->size;
		bufs[j] = (char *) calloc((label->nrows * size[j]), sizeof(char));
	}

	Offset = objSize->dptr;

	#if defined(__CYGWIN__) || defined(__MINGW32__)
	fd = open(fileName, O_RDONLY | O_BINARY, 0);
	#else
	fd = open(fileName, O_RDONLY, 0);
	#endif                          /* __CYGWIN__ */

	rc = 1;
	if (fd < 0){
		fprintf(stderr, "Unable to open file for reading \"%s\". Reason: %s.\n",
			fileName, strerror(errno));
		rc = 0;
	}
	else {
		lseek(fd, Offset, SEEK_SET);
		char* tmp = (char*)calloc(1, sizeof(char));
		for (i = 0; i < label->nrows && rc; i++) {
			/*Read each row as a block */
			if ((err = read(fd, tmpbuf, label->reclen)) != label->reclen) {
				if ( (i == (label->nrows-1)) && (err == (label->reclen-1))) { /* Case where last line has no '\n' */	
					for (j = 0; j < num_items; j++) {
						/*Place in the approiate buffer */
						memcpy((bufs[j] + i * size[j]), (tmpbuf + f[j]->start), size[j]);
						if (f[j]->eformat == MSB_BIT_FIELD)
							rfBitField(&j, bufs, tmpbuf, f, f[j]->start, i, size, num_items);
					}
				}
				else {
					fprintf(stderr, "Short file: %s. Read %d bytes, should be %d\n",
						fileName, err, label->reclen);
					rc=0;
				}
			}
			else {
				if (tmpbuf[label->reclen-1] == '\r') {
					int err2 = read(fd, tmp, 1);
					if(*tmp != '\n') lseek(fd, -1, SEEK_CUR);
				}
				else if (tmpbuf[label->reclen-1] == '\n') {
					int err2 = read(fd, tmp, 1);
					if(*tmp != '\r') lseek(fd, -1, SEEK_CUR);
				}
				for (j = 0; j < num_items; j++) {
					/*Place in the approiate buffer */
					memcpy((bufs[j] + i * size[j]), (tmpbuf + f[j]->start), size[j]);
					if (f[j]->eformat == MSB_BIT_FIELD)
						rfBitField(&j, bufs, tmpbuf, f, f[j]->start, i, size, num_items);
				}
			}
		}
		free(tmp);
		close(fd);
	}

	if (rc){
		/*Set each field Var's data and parameter information */
		Set_Col_Var(&data, f, label, size, bufs);
		add_struct(ob, fix_name("DATA"), data);
	}

	free(tmpbuf);
	free(size);
	for (j = 0; j < num_items; j++)
		free(bufs[j]);
	free(bufs);

	return rc;
}


static double
Scale(int size, void *ptr, FIELD * f)
{
  unsigned char *ucp;
  unsigned int *uip;
  unsigned short *usp;
  char *cp;
  int *ip;
  short *sp;
  float *fp;
  double *dp;
  char num[256];              /* this was WAY too small at [9] */

  /*Set up pointer casts for our data type*/

  ucp = ptr;
  uip = ptr;
  usp = ptr;
  cp = ptr;
  ip = ptr;
  sp = ptr;
  fp = ptr;
  dp = ptr;

  switch (f->eformat) {
    case LSB_UNSIGNED_INTEGER:
    case MSB_UNSIGNED_INTEGER:
      switch (f->size) {
        case 4:
          return (((double) uip[0]) * f->scale + f->offset);
        case 2:
          return (((double) usp[0]) * f->scale + f->offset);
        case 1:
          return (((double) ucp[0]) * f->scale + f->offset);
      }
	break;

    case MSB_INTEGER:
    case LSB_INTEGER:
      switch (f->size) {
        case 4:
          return (((double) ip[0]) * f->scale + f->offset);
        case 2:
          return (((double) sp[0]) * f->scale + f->offset);
        case 1:
          return (((double) cp[0]) * f->scale + f->offset);
      }

      break;

    case IEEE_REAL:
    case PC_REAL:
      switch (f->size) {
        case 8:
          return (dp[0] * f->scale + f->offset);
        case 4:
          return ((double) fp[0] * f->scale + f->offset);
      }

      break;
    case ASCII_INTEGER:
      memcpy(num, cp, f->size);
      num[f->size] = '\0';
      return ((double) (atoi(num)) * f->scale + f->offset);

      break;
    case ASCII_REAL:
      memcpy(num, cp, f->size);
      num[f->size] = '\0';
      return ((double) (atof(num)) * f->scale + f->offset);
    default:
      // This should never happen
      return 0;
  }

  return (0);
}


static char *
DoScale(int rows, FIELD * f, char *in)
{
  double *out;
  void *ptr;
  int count = 0;
  int i, j;
  int dim = (f->dimension ? f->dimension : 1);
  int size = f->size;

  out = (double *) calloc(rows * dim, sizeof(double));
  ptr = in;
  for (i = 0; i < rows; i++) {
    for (j = 0; j < dim; j++) {
      out[count++] = Scale(size, ptr, f);
      ptr = (unsigned char *) ptr + size;
    }
  }

  free(in);
  return ((char *) out);
}

static void
Set_Col_Var(Var ** Data, FIELD ** f, LABEL * label, int *size, char **Bufs)
{
  int j, i, k;
  void *data;
  char **text;
  Var *v;
  char num[128];
  int inum;
  double fnum;
  int step;
  int dim;
  int num_fields;
  int nitems;

  num_fields = label->fields->number;

  /**
  *** Note: the calloc in all these routines is stupid, as it
  ***       allocates (nbytes*nitems, 1)
  **/

  for (j = 0; j < num_fields; j++) {
  	v = NULL;
    dim = (f[j]->dimension ? f[j]->dimension : 1);
	nitems = label->nrows * dim;
    step = 0;

    /* Do appropriate endian conversion */
    switch (f[j]->eformat) {
      case LSB_INTEGER:
      case LSB_UNSIGNED_INTEGER:
      case LSB_BIT_FIELD:
        LSB(Bufs[j], label->nrows * dim, f[j]->size);
        break;
      case MSB_INTEGER:
      case MSB_UNSIGNED_INTEGER:
      case MSB_BIT_FIELD:
        MSB(Bufs[j], label->nrows * dim, f[j]->size);
        break;
      case IEEE_REAL:
        MSB(Bufs[j], label->nrows * dim, f[j]->size);
        break;
      case PC_REAL:
        LSB(Bufs[j], label->nrows * dim, f[j]->size);
        break;
    }

    if (f[j]->scale != 1.0) {
      /*
      ** special case: result is always a double
      ** regardless of input size
      */
      Bufs[j] = DoScale(label->nrows, f[j], Bufs[j]);
      data = calloc(dim * sizeof(double) * label->nrows, sizeof(char));
      memcpy(data, Bufs[j], dim * sizeof(double) * label->nrows);
      v = newVal(BSQ, dim, label->nrows, 1, DOUBLE, data);
    } else {
      switch (f[j]->eformat) {
        case CHARACTER:
          text = (char **) calloc(label->nrows, sizeof(char *));
          for (i = 0; i < label->nrows; i++) {
            text[i] = (char *) calloc(size[j] + 1, sizeof(char));
            memcpy(text[i], (Bufs[j] + i * size[j]), size[j]);
            text[i][size[j]] = '\0';
          }
          v = newText(label->nrows, text);
          break;

        case MSB_INTEGER:
        case LSB_INTEGER:
          switch (f[j]->size) {
            case 4:
              data = calloc(size[j] * label->nrows, sizeof(char));
              memcpy(data, Bufs[j], size[j] * label->nrows);
              v = newVal(BSQ, dim, label->nrows, 1, INT, data);
              break;
            case 2:
              data = calloc(size[j] * label->nrows, sizeof(char));
              memcpy(data, Bufs[j], size[j] * label->nrows);
              v = newVal(BSQ, dim, label->nrows, 1, SHORT, data);
              break;
            case 1:
              // davinci BYTE type is unsigned char
              data = calloc(size[j] * 2 * label->nrows, sizeof(char)); // upscale the data type
              for(k=0; k<nitems; k++){
                *(short *)(data + k*sizeof(short)) = (short)*(unsigned char *)(Bufs[j] + k * sizeof(char));
              }
              v = newVal(BSQ, dim, label->nrows, 1, SHORT, data);
          }
          break;

        case MSB_UNSIGNED_INTEGER:
        case LSB_UNSIGNED_INTEGER:
          switch (f[j]->size) {
            case 4:
              data = calloc(size[j] * 2 * label->nrows, sizeof(char)); // upscale the data type
              for(k=0; k<nitems; k++){
                *(double *)(data + k*sizeof(double)) = (double)*(unsigned int * )(Bufs[j] + k * sizeof(int));
              }
              v = newVal(BSQ, dim, label->nrows, 1, DOUBLE, data);
              break;
            case 2:
              data = calloc(size[j] * 2 * label->nrows, sizeof(char)); // upscale the data type
              for(k=0; k<nitems; k++){
                *(int *)(data + k*sizeof(int)) = (int)*(unsigned short *)(Bufs[j] + k * sizeof(short));
              }
              v = newVal(BSQ, dim, label->nrows, 1, INT, data);
              break;
            case 1:
              // davinci BYTE type is unsigned char
              data = calloc(size[j] * label->nrows, sizeof(char));
              memcpy(data, Bufs[j], size[j] * label->nrows);
              v = newVal(BSQ, dim, label->nrows, 1, BYTE, data);
          }
          break;

        case IEEE_REAL:
        case PC_REAL:
          data = calloc(size[j] * label->nrows, sizeof(char));
          memcpy(data, Bufs[j], size[j] * label->nrows);

          switch (f[j]->size) {
            case 8:
              v = newVal(BSQ, dim, label->nrows, 1, DOUBLE, data);
              break;
            case 4:
              v = newVal(BSQ, dim, label->nrows, 1, FLOAT, data);
              break;
          }
          break;

        case ASCII_INTEGER:
          data = calloc(sizeof(int) * label->nrows, sizeof(char));
          for (i = 0; i < (label->nrows * dim); i++) {
            memcpy(num, Bufs[j] + f[j]->size * i, f[j]->size);
            num[f[j]->size] = '\0';
            inum = atoi(num);
            memcpy((char *) data + step, &inum, sizeof(int));
            step += sizeof(int);
          }

          v = newVal(BSQ, dim, label->nrows, 1, INT, data);
          break;

        case ASCII_REAL:
          data = calloc(sizeof(double) * label->nrows, sizeof(char));
          for (i = 0; i < (label->nrows * dim); i++) {
            memcpy(num, Bufs[j] + f[j]->size * i, f[j]->size);
            num[f[j]->size] = '\0';
            fnum = atof(num);
            memcpy((char *) data + step, &fnum, sizeof(double));
            step += sizeof(double);
          }

          v = newVal(BSQ, dim, label->nrows, 1, DOUBLE, data);
          break;

        case BYTE_OFFSET:
          data = calloc(size[j] * label->nrows, sizeof(char));
          memcpy(data, Bufs[j], size[j] * label->nrows);
          v = newVal(BSQ, dim, label->nrows, 1, INT, data);
          break;

        case MSB_BIT_FIELD:
        case LSB_BIT_FIELD:
          data = calloc(size[j] * label->nrows, sizeof(char));
          memcpy(data, Bufs[j], size[j] * label->nrows);
          v = newVal(BSQ, dim, label->nrows, 1, INT, data);
          break;

      }
    }
	if (v == NULL){
		parse_error("Column data conversion failed for %s.", f[j]->name);
	}
	else {
		add_struct(*Data, fix_name(f[j]->name), v);
	}
  }
}

static int
rfImage(dataKey *objSize, Var * ob){
	FILE *fp;
	Var *data = NULL;
	int rc = 0;
	char *fileName;
	
	if (objSize->FileName == NULL){
		fprintf(stderr, "Null filename while reading IMAGE\n");
		return 0;
	}

	fileName = (char *)alloca(strlen(objSize->FileName)+1);
	pickFilename(fileName, objSize->FileName);
	parse_error("Reading %s image from %s...\n", objSize->Name? objSize->Name: "unnamed", fileName);
	if ((fp = fopen(fileName, "rb")) == NULL){
		fprintf(stderr, "Unable to open file for reading: \"%s\". Reason: %s\n", fileName, strerror(errno));
		return 0;
	}

	data = dv_LoadImage_New(fp, fileName, objSize->dptr, objSize->objDesc);
	if (data != NULL) {
		add_struct(ob, fix_name("DATA"), data);
		rc=1;
	}

	fclose(fp);
	return rc;
}


static int
rfHistogram(dataKey *objSize, Var * ob){
	FILE *fp;
	Var *data = NULL;
	int rc = 0;
	char *fileName;
	
	if (objSize->FileName == NULL){
		fprintf(stderr, "NULL filename while reading HISTOGRAM\n");
		return 0;
	}

	fileName = (char *)alloca(strlen(objSize->FileName)+1);
	pickFilename(fileName, objSize->FileName);
  parse_error("Reading %s histogram from %s...\n", objSize->Name? objSize->Name: "unnamed", fileName);
	if ((fp = fopen(fileName, "rb")) == NULL){
		fprintf(stderr, "Unable to open file for reading: \"%s\". Reason: %s\n", fileName, strerror(errno));
		return 0;
	}

	data = dv_LoadHistogram_New(fp, fileName, objSize->dptr, objSize->objDesc);
	if (data != NULL) {
		add_struct(ob, fix_name("DATA"), data);
		rc=1;
	}

	fclose(fp);
	return rc;
}

static char *history_parse_buffer(FILE * in)
{
  char buf[1024];
  int max = 2048;
  int ptr = 0;
  char *TheString;

  TheString = (char *) malloc(max);


  while (1) {
    fgets(buf, 1023, in);   /*Suck in the next line */

    if ((ptr + strlen(buf)) >= max) {
      max *= 2;
      TheString = (char *) realloc((TheString), max);
    }

    strcpy((TheString + ptr), buf);
    ptr += strlen(buf);

    if (!(strncasecmp("END", buf, 3))) {
      if (!(strncasecmp("END_", buf, 4))) {
        continue;
      } else {
        break;
      }
    }
  }

  TheString = (char *) realloc(TheString, strlen(TheString));

  return (TheString);
}

/*
** history_remove_isis_indents - removes the history-indent
** marks from ISIS history objects. It removes these indents
** blindly.
** Returned history is new memory block - it is the
** caller's responsibility to free() it.
*/
static char *
history_remove_isis_indents(const char *history)
{
  char *src_hist = strdup(history);
  int   src_hist_len = strlen(history);
  char *tgt_hist = NULL;
  char *line, **lines;
  char *p;
  LIST *lines_list;
  regex_t indent_regex;
  regmatch_t matches[1];
  int i, n;
  int rc;

  rc = regcomp(&indent_regex, "^(([|]{1,5})|([|][0-9]{3}[|]))(----)?", REG_EXTENDED);
  if (rc != 0){
    parse_error("regcomp() failed for indents pattern.");
    return NULL;
  }

  lines_list = new_list();
  for (p = src_hist; line = strtok(p, "\n"); p = NULL) {
    list_add(lines_list, line);
  }


  n = list_count(lines_list);
  lines = (char **)list_data(lines_list);

  for(i = 0; i < n; i++){
    /* remove ISIS style vertical-bar indent-end marker */
    rc = regexec(&indent_regex, lines[i], sizeof(matches)/sizeof(regmatch_t), matches, 0);
    if (rc == 0){
      /* a match was found: get rid of indent */
      memmove(lines[i], &lines[i][matches[0].rm_eo], strlen(&lines[i][matches[0].rm_eo])+1);
    }
  }

  regfree(&indent_regex);

  tgt_hist = (char *)calloc(sizeof(char), src_hist_len+n+10);
  strcpy(tgt_hist, "");
  for(i = 0; i < n; i++){
    if (i > 0){ strcat(tgt_hist, "\n"); }
    strcat(tgt_hist, lines[i]);
  }

  list_free(lines_list);
  free(src_hist);

  return tgt_hist;
}

static int
rfHistory(dataKey *objSize, Var *ob) {
	FILE *fp;
	Var *data = new_struct(0);
	OBJDESC *top;
	char *history, *p;
	int rc = 0;
	char *fileName;

	if (objSize->FileName == NULL){
		fprintf(stderr, "Null filename while reading HISTORY\n");
		return 0;
	}
	
	fileName = (char *)alloca(strlen(objSize->FileName)+1);
	pickFilename(fileName, objSize->FileName);
	parse_error("Reading %s history from %s...\n", objSize->Name? objSize->Name: "unnamed", fileName);
	if ((fp = fopen(fileName, "rb")) == NULL){
		fprintf(stderr, "Unable to open file for reading: \"%s\". Reason: %s\n", fileName, strerror(errno));
		return 0;
	}

	/*Read in the history object as a giant string*/
	fseek(fp, objSize->dptr, SEEK_SET);
	history = history_parse_buffer(fp);
	fclose(fp);

	/* Assuming ISIS history object: remove any indent marks */
	p = history_remove_isis_indents(history);
	if (p != NULL){
		/* replace history text with history-text-without-indent-marks */
		free(history);
		history = p;
	}

	/*Call the OdlParseLabelString fucntion and make it an ODL object*/
	top = OdlParseLabelString(history, NULL, ODL_EXPAND_STRUCTURE, VERBOSE == 0);
	traverseObj(top, data, NULL, 0, top, data);

	if (get_struct_count(data))
		add_struct(ob, fix_name("DATA"), data);

	return 1;
}


static int
GetInt(OBJDESC *ob, char *key, int *val)
{
	KEYWORD *kw;

    if ((kw = OdlFindKwd(ob, key, NULL,0, ODL_THIS_OBJECT)) != NULL) {
		*val = atoi(OdlGetKwdValue(kw));
		return(1);
	}
	return(0);
}

/*
remove the pds header from a file 
*/
Var *
ff_pdshead(vfuncptr func, Var *arg)
{
	OBJDESC *ob;
	int size, reclen, lrec, i, err;
	char buf[8192];
	char *fname = NULL, *outfname = NULL;
	int fd = -1, ofd = -1;
	int convert_short=0;
	int data = 0;
	int fdata = 0;
	char t;

	Alist alist[6];
	alist[0] = make_alist("fname", ID_STRING, NULL, &fname);
	alist[1] = make_alist("outfname", ID_STRING, NULL, &outfname);
	alist[2] = make_alist("convert_short", INT, NULL, &convert_short);
	alist[3] = make_alist("data", INT, NULL, &data);
	alist[4] = make_alist("fdata", INT, NULL, &fdata);
	alist[5].name = NULL;

	if (parse_args(func, arg, alist) == 0)
		return (NULL);

	if (fname == NULL){
		parse_error("%s(... fname= ...) missing\n", func->name);
		return NULL;
	}

	if (outfname == NULL){
		parse_error("%s(... outfname= ...) missing\n", func->name);
		return NULL;
	}

	if (fdata == 1)
		data = 1;

	ob = OdlParseLabelFile(fname, NULL, ODL_EXPAND_STRUCTURE, 1);

    if (ob == NULL) {
		parse_error("%s: Unable to read file: %s\n", func->name, fname);
		return NULL;
	}

	if (!GetInt(ob, "RECORD_BYTES", &reclen)) {
		parse_error("%s: Unable to find keyword: RECORD_BYTES\n", func->name);
		return NULL;
	}
	if (!GetInt(ob, "^SPECTRAL_QUBE", &lrec)) {
		if (!GetInt(ob, "^QUBE", &lrec)) {
			if(!GetInt(ob, "^IMAGE", &lrec)) {
				parse_error("%s: Unable to find keywords: QUBE or IMAGE\n", func->name);
				return NULL;
			}
		}
	}

	size = reclen*(lrec-1);

#ifdef _WIN32
	fd = open(fname, O_RDONLY|O_BINARY);
#else
	fd = open(fname, O_RDONLY);
#endif
	if (fd < 0){
		parse_error("%s: Unable to open file %s for reading: %s\n", func->name, fname, strerror(errno));
		return NULL;
	}

#ifdef _WIN32
	ofd = open(outfname, O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 00666);
#else
	ofd = open(outfname, O_WRONLY|O_TRUNC|O_CREAT, 00666);
#endif
	if (ofd < 0){
		parse_error("%s: Unable to open file %s for writing: %s\n", func->name, outfname, strerror(errno));
		close(fd);
		return NULL;
	}

	if (data == 0) {
		for (i = 0 ; i < size ; i+= 8192) {
			err = read(fd, buf, min(8192, size-i));
			write(ofd, buf, err);
		}
	} else {
		lseek(fd, size, 0);
		while ((err = read(fd, buf, 8192)) > 0) {
			if (fdata == 1) {
				for (i = 0 ; i < err ; i+=4) {
					t = buf[i];
					buf[i] = buf[i+3];
					buf[i+3] = t;
					t = buf[i+1];
					buf[i+1] = buf[i+2];
					buf[i+2] = t;
				}
			}
			write(ofd, buf, err);
		}
	}

	close(fd);
	if (ofd >= 0)
		close(ofd);

	return newInt(1);
}

#ifdef HAVE_LIBXML2

/**
 * This function reads a PDS4 file into davinci memory
 *
 * libxml2 linked in at compile time and available on the system
 * at run time.
 *
 * Should be thread safe IF the libxml2 conditions at:
 * http://xmlsoft.org/threads.html
 * are met.
 */
Var *ReadPDS4(vfuncptr func, Var * arg)
{
    Var *fn = NULL;
    char *filename = NULL;
    int get_data = 1;
    int use_names = 1;
    int i;

    Alist alist[4];
    alist[0] = make_alist("filename", ID_UNK, NULL, &fn);
    alist[1] = make_alist("use_names", INT, NULL, &use_names);
    alist[2] = make_alist("data", INT, NULL, &get_data);
    alist[3].name = NULL;

    if (parse_args(func, arg, alist) == 0)
        return (NULL);

    if (fn == NULL)
    {
        parse_error("No filename specified to load_pds4()");
        return (Var *) NULL;
    }

    /* Handle loading many filenames */
    if (V_TYPE(fn) == ID_TEXT)
    {
        Var *s = new_struct(V_TEXT(fn).Row);
        for (i = 0; i < V_TEXT(fn).Row; i++)
        {
            filename = strdup(V_TEXT(fn).text[i]);
            Var *t = do_loadPDS4(func, filename, use_names, get_data);
            if (t)
            {
                add_struct(s, filename, t);
            }
        }
        if (get_struct_count(s))
        {
            return (s);
        }
        else
        {
            free_struct(s);
            return (NULL);
        }
    }
    else if (V_TYPE(fn) == ID_STRING)
    {
        filename = V_STRING(fn);
         return (do_loadPDS4(func, filename, use_names, get_data));
    }
    else
    {
        parse_error("Illegal argument to function %s(%s), expected STRING",
                func->name, "filename");
        return (NULL);
    }
}

static void print_pds4_structs(LABEL *label, dataKey *data_key)
{
    int i, field_count;
    FIELD **fields = NULL;
    FIELD *f = NULL;

    if (label != NULL)
    {
        printf("label->reclen: %d\n", label->reclen);
        printf("label->name: %s\n", label->name);
        printf("label->nfields: %d\n", label->nfields);
        printf("label->nrows: %d\n", label->nrows);
        if (label->fields != NULL)
        {
            field_count = list_count(label->fields);
            printf("label->fields->count: %d\n", field_count);
            fields = (FIELD**) list_data(label->fields);

            for (i = 0; i < field_count; i++)
            {
                f = fields[i];
                printf("\nfield->name: %s\n", f->name);
                printf("field->eformat: %d\n", f->eformat);
                printf("field->iformat: %d\n", f->iformat);
                printf("field->size: %d\n", f->size);
                printf("field->start: %d\n", f->start);
                printf("field->scale: %f\n", f->scale);
                printf("field->offset: %f\n", f->offset);
                printf("field->dimension: %d\n", f->dimension);

            }
        }
    }
    else
    {
        printf("label is NULL\n");
    }

    if (data_key != NULL)
    {
        printf("data_key->Name: %s\n", data_key->Name);
        printf("data_key->FileName: %s\n", data_key->FileName);
        printf("data_key->dptr: %d\n", data_key->dptr);
        printf("data_key->size: %d\n", data_key->size);

    }
    else
    {
        printf("data_key is NULL\n");
    }
}

static iom_edf 
pds4TypeToIomEformat(const char *data_type) {
    iom_edf eformat = iom_EDF_INVALID;
    int bytes = -1;

    if (!strcmp(data_type, UNSIGNED_BYTE)){
        eformat = iom_MSB_INT_1;
    }
    else if (!strcmp(data_type, UNSIGNED_MSB2)){
        eformat = iom_MSB_INT_2;
    }
    else if (!strcmp(data_type, UNSIGNED_MSB4)) {
        eformat = iom_MSB_INT_4;
    }
    else if (!strcmp(data_type, UNSIGNED_LSB2)) {
        eformat = iom_LSB_INT_2;
    }
    else if (!strcmp(data_type, UNSIGNED_LSB4)) {
        eformat = iom_LSB_INT_4;
    }
    else if (!strcmp(data_type, SIGNED_LSB2)) {
        eformat = iom_LSB_INT_2;
    }
    else if (!strcmp(data_type, SIGNED_LSB4)) {
        eformat = iom_LSB_INT_4;
    }
    else if (!strcmp(data_type, SIGNED_MSB2)) {
        eformat = iom_MSB_INT_2;
    }
    else if (!strcmp(data_type, SIGNED_MSB4)) {
        eformat = iom_MSB_INT_4;
    }
    else if (!strcmp(data_type, IEEE_LSB_DOUBLE)) {
        eformat = iom_LSB_IEEE_REAL_8;
    }
    else if (!strcmp(data_type, IEEE_LSB_SINGLE)) {
        eformat = iom_LSB_IEEE_REAL_4;
    }
    else if (!strcmp(data_type, IEEE_MSB_DOUBLE)) {
        eformat = iom_MSB_IEEE_REAL_8;
    }
    else if (!strcmp(data_type, IEEE_MSB_SINGLE)) {
        eformat = iom_MSB_IEEE_REAL_4;
    }
    
    return eformat;
}

static int loadFieldBinary(Var *v, LABEL *label, dataKey *data_key,
        int use_names, int grp_count, int grp_start, char *parent_name)
{
    int i, j;
    int err = 0, warn = 0;
    int count = 0;
    int elem_cnt = 0;
    int num_groups = 0;
    int num_fields = 0;
    int grp_loc = 0;
    int grp_len = 0;
    char *elem_type = NULL;
    char *name = NULL;
    char *elem_name = NULL;
    char *data_type = NULL;
    FIELD *field = NULL;
    Var *s = NULL;
    Var *t = NULL;

    count = get_struct_count(v);

    if (count > 0 && label->fields == NULL)
    {
        label->fields = new_list();
        if (label->fields == NULL)
        {
            parse_error("Error allocating memory for a FIELD list\n");
            return 0;
        }
    }

    field = (FIELD *) calloc(1, sizeof(FIELD));
    if (field == NULL)
    {
        parse_error("Error allocating memory for a FIELD_BINARY\n");
        return 0;
    }

    field->label = label;
    field->alias = NULL;
    field->name = NULL;
    field->start = 0;
    field->scale = 1.0;
    field->offset = 0.0;
    field->dimension = grp_count;
    field->size = 0;

    for (i = 0; i < count; i++)
    {
        get_struct_element(v, i, &name, &s);
        if (name == NULL || s == NULL)
        {
            parse_error(
                    "Encountered a NULL davinci child node while loading a field binary record\n");
            err = 1;
            goto FINISH;
        }
        if (!strcmp(name, FIELD_LOCATION))
        {
            // if grp_start is already initialized we want to use it
            if (grp_start > -1)
            {
                field->start = grp_start;
            }
            else
            {
                elem_cnt = get_struct_count(s);
                for (j = 0; j < elem_cnt; j++)
                {
                    get_struct_element(s, j, &elem_name, &t);
                    if (elem_name == NULL || t == NULL)
                    {
                        parse_error(
                                "Encountered a NULL davinci child node while loading a field binary location record\n");
                        err = 1;
                        goto FINISH;
                    }
                    else if (!strcmp(elem_name, VALUE))
                    {
                        errno = 0;
                        field->start = V_INT(t) - 1; // need to convert from 1 index to zero index
                        if (errno == ERANGE)
                        {
                            parse_error(
                                    "Math error processing the field location value\n");
                            err = 1;
                            goto FINISH;
                        }
                        if (field->start < 0)
                        {
                            parse_error(
                                    "Warning!: The starting value of a field binary cannot be < 0\n");
                            warn = 1;
                            goto FINISH;
                        }
                    }
                }
            }
            if (field->start > label->reclen)
            {
                parse_error(
                        "Warning!: a Field cannot be located beyond the length of the record\n");
                warn = 1;
                goto FINISH;
            }
        }
        else if (!strcmp(name, DATA_TYPE))
        {
            data_type = V_STRING(s);
            if (data_type == NULL || !strlen(data_type))
            {
                parse_error("Warning! Missing Field Binary data type\n");
                warn = 1;
                goto FINISH;
            }
            if (!strcmp(data_type, UNSIGNED_BYTE) || !strcmp(data_type,
                    UNSIGNED_MSB2) || !strcmp(data_type, UNSIGNED_MSB4))
            {
                field->eformat = MSB_UNSIGNED_INTEGER;
                field->iformat = eformat_to_iformat(MSB_UNSIGNED_INTEGER);
            }
            else if (!strcmp(data_type, UNSIGNED_LSB2) || !strcmp(data_type,
                    UNSIGNED_LSB4))
            {
                field->eformat = LSB_UNSIGNED_INTEGER;
                field->iformat = eformat_to_iformat(LSB_UNSIGNED_INTEGER);
            }
            else if (!strcmp(data_type, SIGNED_LSB2) || !strcmp(data_type,
                    SIGNED_LSB4))
            {
                field->eformat = LSB_INTEGER;
                field->iformat = eformat_to_iformat(LSB_INTEGER);
            }
            else if (!strcmp(data_type, SIGNED_MSB2) || !strcmp(data_type,
                    SIGNED_MSB4))
            {
                field->eformat = MSB_INTEGER;
                field->iformat = eformat_to_iformat(MSB_INTEGER);
            }
            else if (!strcmp(data_type, IEEE_LSB_DOUBLE) || !strcmp(data_type,
                    IEEE_LSB_SINGLE))
            {
                field->eformat = PC_REAL;
                field->iformat = eformat_to_iformat(PC_REAL);
            }
            else if (!strcmp(data_type, IEEE_MSB_SINGLE) || !strcmp(data_type,
                    IEEE_MSB_DOUBLE))
            {
                field->eformat = IEEE_REAL;
                field->iformat = eformat_to_iformat(IEEE_REAL);
            }
            else if (!strcmp(data_type, ASCII_STRING))
            {
                field->eformat = CHARACTER;
                field->iformat = eformat_to_iformat(CHARACTER);
            }
        }
        else if (!strcmp(name, FIELD_LENGTH))
        {
            elem_cnt = get_struct_count(s);
            for (j = 0; j < elem_cnt; j++)
            {
                get_struct_element(s, j, &elem_name, &t);
                if (elem_name == NULL || t == NULL)
                {
                    parse_error(
                            "Encountered a NULL davinci child node while loading a field binary length record\n");
                    err = 1;
                    goto FINISH;
                }
                else if (!strcmp(elem_name, VALUE))
                {
                    errno = 0;
                    if (V_FORMAT(t) != INT)
                    {
                        parse_error(
                                "Warning!: Field length should be an Integer value\n");
                        warn = 1;
                        goto FINISH;
                    }

                    field->size = V_INT(t);
                    if (errno == ERANGE)
                    {
                        parse_error(
                                "Math error processing the field length value\n");
                        err = 1;
                        goto FINISH;
                    }
                    if (field->start + field->size > label->reclen)
                    {
                        parse_error(
                                "Warning!: a Field cannot extend beyond the length of the record\n");
                        warn = 1;
                        goto FINISH;
                    }
                }
            }
        }
        else if (!strcmp(name, SCALING_FACTOR))
        {
            if (V_FORMAT(s) == INT)
            {
                field->scale = V_INT(s);
            }
            else if (V_FORMAT(s) == FLOAT)
            {
                field->scale = V_FLOAT(s);
            }
            else
            {
                parse_error(
                        "Warning: scaling factor should be a integer or a float value\n");
                warn = 1;
                goto FINISH;
            }
        }
        else if (!strcmp(name, VALUE_OFFSET))
        {
            if (V_FORMAT(s) == INT)
            {
                field->offset = V_INT(s);
            }
            else if (V_FORMAT(s) == FLOAT)
            {
                field->offset = V_FLOAT(s);
            }
            else
            {
                parse_error(
                        "Warning: offset should be a integer or a float value\n");
                warn = 1;
                goto FINISH;
            }
        }
        else if (field->name == NULL && !use_names && !strcmp(name,
                NAME_ATTRIBUTE))
        {
            field->name = strdup(V_STRING(s));
        }
        else if (field->name == NULL && use_names && parent_name != NULL)
        {
            // need the name of the parent node
            field->name = strdup(parent_name);
        }
    }

    FINISH: if (warn || err)
    {
        if (field != NULL)
        {
            if (field->name != NULL)
            {
                parse_error("Field %s was not read from the table\n",
                        field->name);
                free(field->name);
            }
            free(field);
        }
        if (warn)
        {
            return -1;
        }
        else if (err)
        {
            return 0;
        }
    }
    if (field != NULL)
    {
        list_add(label->fields, field);
        label->nfields += 1;
    }
    return 1;
}

static int loadGroupFieldBinary(Var *v, LABEL *label, dataKey *data_key,
        int use_names)
{
    int i, j, warn = 0, err = 0;
    int count = 0;
    int elem_cnt = 0;
    int num_groups = 0;
    int num_fields = 0;
    int grp_field_cnt = 0;
    int field_found = 0;
    int grp_loc = -1;
    int grp_len = 0;
    int grp_rep = 0;
    char *elem_type = NULL;
    char *name = NULL;
    char *elem_name = NULL;
    Var *s = NULL;
    Var *t = NULL;
    Var *u = NULL;

    count = get_struct_count(v);

    for (i = 0; i < count; i++)
    {
        get_struct_element(v, i, &name, &s);
        if (name == NULL || s == NULL)
        {
            parse_error(
                    "Encountered a NULL davinci child node while loading a group field binary record\n");
            err = 1;
            goto FINISH;
        }
        if (!strcmp(name, GROUP_FIELD_BINARY))
        {
            parse_error(
                    "Warning!: Davinci does not allow a group field binary element to contain another group field binary element!\n");
            warn = 1;
            goto FINISH;
        }
        else if (!strcmp(name, FIELDS))
        {
            errno = 0;
            grp_field_cnt = V_INT(s);
            if (errno == ERANGE)
            {
                parse_error("Math error processing the FIELDS value\n");
                err = 1;
                goto FINISH;
            }
            if (grp_field_cnt != 1)
            {
                parse_error(
                        "Warning!: Davinci only allows one field per each group field binary element!\n");
                warn = 1;
                goto FINISH;
            }
            // we would normally increment label->nfields here but do it in repetitions instead
        }
        else if (!strcmp(name, REPETITIONS))
        {
            errno = 0;
            num_fields = V_INT(s);
            if (errno == ERANGE)
            {
                parse_error("Math error processing the REPETITIONS value\n");
                err = 1;
                goto FINISH;
            }
            label->nfields += num_fields;
        }
        else if (!strcmp(name, GROUPS))
        {
            errno = 0;
            num_groups += V_INT(s);
            if (errno == ERANGE)
            {
                parse_error("Math error processing the GROUPS value\n");
                err = 1;
                goto FINISH;
            }

            if (num_groups != 0)
            {
                parse_error(
                        "ERROR: davinci does not currently allow groups within groups in a PDS4 label\n");
                err = 1;
                goto FINISH;
            }
        }
        else if (!strcmp(name, GROUP_LOCATION))
        {
            elem_cnt = get_struct_count(s);

            for (j = 0; j < elem_cnt; j++)
            {
                get_struct_element(s, j, &elem_name, &t);
                if (elem_name == NULL || t == NULL)
                {
                    parse_error(
                            "Encountered a NULL davinci child node while loading a group field binary location\n");
                    err = 1;
                    goto FINISH;
                }
                if (!strcmp(elem_name, VALUE))
                {
                    errno = 0;
                    grp_loc = V_INT(t) - 1; // need to convert from 1 index to zero index
                    if (errno == ERANGE)
                    {
                        parse_error(
                                "Math error processing the GROUPS location value\n");
                        err = 1;
                        goto FINISH;
                    }
                    if (label != NULL && grp_loc > label->reclen)
                    {
                        parse_error(
                                "Warning!: a Group cannot be located beyond the length of the table\n");
                        warn = 1;
                        goto FINISH;
                    }
                }
            }
        }
        else if (!strcmp(name, GROUP_LENGTH))
        {
            elem_cnt = get_struct_count(s);

            for (j = 0; j < elem_cnt; j++)
            {
                get_struct_element(s, j, &elem_name, &t);
                if (elem_name == NULL || t == NULL)
                {
                    parse_error(
                            "Encountered a NULL davinci child node while loading a group length record\n");
                    err = 1;
                    goto FINISH;
                }
                if (!strcmp(elem_name, VALUE))
                {
                    errno = 0;
                    grp_len = V_INT(t);
                    if (errno == ERANGE)
                    {
                        parse_error(
                                "Math error processing the GROUPS length value\n");
                        err = 1;
                        goto FINISH;
                    }
                    if (label != NULL && (grp_len + grp_loc > label->reclen))
                    {
                        parse_error(
                                "Warning!: a Group cannot extend beyond the length of the record\n");
                        warn = 1;
                        goto FINISH;
                    }
                }
            }
        }
        else if (strstr(name, FIELD_BINARY) != NULL && label != NULL
                && data_key != NULL)
        {
            if (field_found)
            {
                parse_error(
                        "Warning!: Davinci only allows one field per each group field binary element!\n");
                warn = 1;
                goto FINISH;
            }
            if (!loadFieldBinary(s, label, data_key, use_names, num_fields,
                    grp_loc, (char *) NULL))
            {
                return 0;
            }
            field_found++;
        }
        else if (find_struct(s, ELEMENT_TYPE, &u) >= 0)
        {
            elem_type = strdup(V_STRING(u));
            if (elem_type != NULL && !strcmp(elem_type, FIELD_BINARY))
            {
                if (field_found)
                {
                    parse_error(
                            "Warning!: Davinci only allows one field per each group field binary element!\n");
                    warn = 1;
                    goto FINISH;
                }
                if (!loadFieldBinary(s, label, data_key, use_names, num_fields,
                        grp_loc, name))
                {
                    return 0;
                }
                field_found++;
            }
            if (elem_type != NULL)
            {
                free(elem_type);
            }
        }
    }
    FINISH: if (warn)
    {
        parse_error("A Group Field was not read from the table\n");
        return -1;
    }
    else if (err)
    {
        return 0;
    }

    return 1;
}

static int loadRecordBinary(Var *v, LABEL *label, dataKey *data_key,
        int use_names)
{
    int i, j;
    int count = 0;
    int elem_cnt = 0;
    int num_fields = 0;
    char *elem_type = NULL;
    char *name = NULL;
    Var *s = NULL;
    Var *t = NULL;
    Var *u = NULL;

    count = get_struct_count(v);

    for (i = 0; i < count; i++)
    {
        get_struct_element(v, i, &name, &s);
        if (name == NULL || s == NULL)
        {
            // error msg
            parse_error(
                    "Encountered a NULL davinci child node while loading a binary record\n");
            return 0;
        }
        if (!strcmp(name, FIELDS))
        {
            errno = 0;
            num_fields += V_INT(s);
            if (errno == ERANGE)
            {
                // error msg couldn't read the file name
                parse_error("Math error processing the FIELDS value\n");
                return 0;
            }
        }
        else if (!strcmp(name, GROUPS))
        {
            errno = 0;
            num_fields += V_INT(s);
            if (errno == ERANGE)
            {
                parse_error("Math error processing the GROUPS value\n");
                return 0;
            }
        }
        else if (!strcmp(name, RECORD_LENGTH))
        {
            elem_cnt = get_struct_count(s);
            for (j = 0; j < elem_cnt; j++)
            {
                get_struct_element(s, j, &name, &t);
                if (name == NULL || t == NULL)
                {
                    // error msg
                    parse_error(
                            "Encountered a NULL davinci child node while loading a binary record length\n");
                    return 0;
                }

                if (!strcmp(name, VALUE))
                {
                    errno = 0;
                    label->reclen = V_INT(t);
                    if (errno == ERANGE)
                    {
                        // error msg couldn't read the file name
                        parse_error(
                                "Math error processing the record length value\n");
                        return 0;
                    }
                }
            }
        }
        else if (strstr(name, GROUP_FIELD_BINARY) != NULL && label != NULL
                && data_key != NULL)
        {
            if (!loadGroupFieldBinary(s, label, data_key, use_names))
            {
                return 0;
            }
        }
        else if (strstr(name, FIELD_BINARY) != NULL && label != NULL
                && data_key != NULL)
        {
            if (!loadFieldBinary(s, label, data_key, use_names, 1, -1,
                    (char *) NULL))
            {
                return 0;
            }
        }
        else if (find_struct(s, ELEMENT_TYPE, &u) >= 0)
        {
            elem_type = strdup(V_STRING(u));
            if (elem_type != NULL && !strcmp(elem_type, FIELD_BINARY))
            {
                if (!loadFieldBinary(s, label, data_key, use_names, 1, -1, name))
                {
                    return 0;
                }
            }
            if (elem_type != NULL)
            {
                free(elem_type);
            }
        }
    }
    label->nfields = num_fields;

    return 1;

}

static int loadBinaryTable(Var *v, LABEL *label, dataKey *data_key,
        int use_names, int get_data)
{
    int i, j;
    int count = 0;
    int elem_cnt = 0;
    int ret_val = 0;
    char *name = NULL;
    Var *s = NULL;
    Var *t = NULL;

    count = get_struct_count(v);

    for (i = 0; i < count; i++)
    {
        get_struct_element(v, i, &name, &s);
        if (name == NULL || s == NULL)
        {
            // error msg
            parse_error(
                    "Encountered a NULL davinci child node while loading a binary table\n");
            return 0;
        }
        if (!strcmp(name, OFFSET))
        {
            elem_cnt = get_struct_count(s);
            for (j = 0; j < elem_cnt; j++)
            {
                get_struct_element(s, j, &name, &t);
                if (name == NULL || t == NULL)
                {
                    // error msg
                    parse_error(
                            "Encountered a NULL davinci child node while fetching table offset\n");
                    return 0;
                }

                if (!strcmp(name, VALUE))
                {
                    errno = 0;
                    data_key->dptr = V_INT(t);
                    if (errno == ERANGE)
                    {
                        // error msg couldn't read the file name
                        parse_error(
                                "Error encountered while accessing table offset value\n");
                        return 0;
                    }
                }
            }

        }
        else if (!strcmp(name, RECORDS))
        {
            label->nrows = V_INT(s);
        }
        else if (!strcmp(name, DESCRIPTION))
        {
            if ((label->name = strdup(V_STRING(s))) == NULL)
            {
                parse_error(
                        "Error encountered while accessing table description\n");
                return 0;
            }
        }
        else if (!strcmp(name, RECORD_BINARY))
        {
            if (!loadRecordBinary(s, label, data_key, use_names))
            {
                return 0;
            }
        }
    }

//    print_pds4_structs(label, data_key);

    // call rfTable() to read in the table
    if (get_data)
    {
        ret_val = rfTable(data_key, v, label);
        if (!ret_val)
        {
            // fail slow so the user can at least have the label
            parse_error("Unable to read all or part of the binary table\n");
            ret_val = -1;
        }
    }
    return ret_val;
}

static int load2dImage(Var *v, LABEL *label, dataKey *data_key,
        int use_names, int get_data)
{
    int i, j;
    int count = 0;
    int elem_cnt = 0;
    int ret_val = 0;
    char *name = NULL;
    Var *s = NULL;
    Var *t = NULL;
    int ndims = 0;
    int dims[3];
    size_t size = 0;
    int org = BSQ;
    int axisSerial = 0;
    int dim[3] = {0,0,0};
    int axes[3] = {0,1,2};
    Var *ss = NULL;
    iom_edf eformat = iom_EDF_INVALID;
    int eformat_len = -1;

    count = get_struct_count(v);

    for (i = 0; i < count; i++)
    {
        get_struct_element(v, i, &name, &s);
        if (name == NULL || s == NULL)
        {
            // TODO include the name of the object being loaded here
            // error msg
            parse_error(
                    "Encountered a NULL davinci child node in load2dImage()\n");
            return 0;
        }
        if (!strcmp(name, OFFSET))
        {
            elem_cnt = get_struct_count(s);
            for (j = 0; j < elem_cnt; j++)
            {
                get_struct_element(s, j, &name, &t);
                if (name == NULL || t == NULL)
                {
                    // error msg
                    parse_error("Encountered a NULL davinci child node while fetching offset, assuming zero\n");
                    return 0;
                }

                if (!strcmp(name, VALUE))
                {
                    errno = 0;
                    data_key->dptr = V_INT(t);
                    if (errno == ERANGE)
                    {
                        // error msg couldn't read the file name
                        parse_error("Error encountered while accessing offset value\n");
                        return 0;
                    }
                }
            }

        }
        else if (!strcmp(name, ARRAY_2D_IMAGE)){
        }
        else if (!strcmp(name, AXES))
        {
            ndims = V_INT(s);
        }
        else if (!strcmp(name, AXIS_INDEX_ORDER))
        {
            char *orgStr = NULL;
            if ((orgStr = V_STRING(s)) != NULL){
                if (!strcmp(orgStr, LAST_INDEX_FASTEST)){
                    // TODO use below in determing org
                }
            }
        }
        else if (!strcmp(name, OFFSET)){
            data_key->dptr = V_INT(s); // TODO handle other offset units, assuming bytes for now
        }
        else if (!strcmp(name, DESCRIPTION))
        {
            if ((label->name = strdup(V_STRING(s))) == NULL)
            {
                parse_error("Error encountered while accessing object description\n");
                return 0;
            }
        }
        else if (!strcmp(name, ELEMENT_ARRAY)){
            if (find_struct(s, DATA_TYPE, &ss) >= 0){
                char *typeStr = V_STRING(ss);
                eformat = pds4TypeToIomEformat(typeStr);
            }
        }
        else if (!strncmp(name, AXIS_ARRAY, strlen(AXIS_ARRAY))){ // there could be more than one, with suffix _1, _2, ...
            char *axisName = NULL;
            int dimSize = 0;
            int axisSeq = axisSerial++;
            if (find_struct(s, AXIS_NAME, &ss) >= 0){
                axisName = V_STRING(ss);
            }
            if (find_struct(s, ELEMENTS, &ss) >= 0){
                dimSize = V_INT(ss);
            }
            if (find_struct(s, SEQUENCE_NUMBER, &ss) >= 0){
                axisSeq = V_INT(ss)-1;
            }

            dim[axisSeq] = dimSize;
            axes[axisSeq] = (!strcmp(axisName,"Sample"))? 0: (!strcmp(axisName,"Line"))? 1: 2;
        }
        // TODO start here
    }

    // PDS4 default axis_index_order = "Last Index Fastest"; davinci has first index fastest, so we flip axes & dims
    for(i=0; i<ndims/2; i++){
        SWAP_INT(axes[i], axes[ndims-1-i]);
        SWAP_INT(dim[i], dim[ndims-1-i]);
    }

    // axes order (0=x,1=y,2=z): bsq=0,1,2; bip=2,0,1; bil=0,2,1
    org = (axes[0] == 0)? ((axes[1] == 1)? BSQ: BIL): BIP;

    // calculate size
    size = ndims > 0? 1: 0;
    for(i=0; i<ndims; i++){
        size *= dim[i];
    }
    data_key->size = size * eformat_to_iformat(eformat);

//    print_pds4_structs(label, data_key);

    // read image data
    if (get_data)
    {
        int fd;
        struct iom_iheader h;
        char *datafile = data_key->FileName;
        char *data = NULL;

#if defined(__CYGWIN__) || defined(__MINGW32__)
        if ((fd = open(datafile, O_RDONLY|O_BINARY)) < 0) {
            fprintf(stderr, "Unable to open data file: %s\n", datafile);
            ret_val = -1;
        }
#else
        if ((fd = open(datafile, O_RDONLY)) < 0) {
            fprintf(stderr, "Unable to open data file: %s\n", datafile);
            ret_val = -1;
        }
#endif /* __CYGWIN__ */

        iom_init_iheader(&h);
        h.dptr = data_key->dptr;
        memcpy(h.size, dim, sizeof(h.size));
        h.eformat = eformat;
        h.org = org;

        data = iom_read_qube_data(fd, &h);
        close(fd);

        if (data != NULL){
            Var *datav = iom_iheader2var(&h);
            V_DATA(datav) = data;
            add_struct(v, strdup("data"), datav);
            ret_val = 1;
        }
        else {
            parse_error("Unable to read all or part of the image from: %s\n", datafile);
            ret_val = -1;
        }
    }
    return ret_val;
}

static int loadFileInfo(Var *v, LABEL *label, dataKey *data_key)
{
    int i, j;
    int count = 0;
    int fCount = 0;
    int m_count = 0;
    char *name = NULL;
    char *finfo = NULL;
    char *fname = NULL;
    char *tmp = NULL;
    Var *s = NULL;
    Var *t = NULL;

    count = get_struct_count(v);
    for (i = 0; i < count; i++)
    {
        get_struct_element(v, i, &name, &s);
        if (name == NULL || s == NULL)
        {
            parse_error(
                    "Encountered a NULL davinci child node while searching for a PDS4 table file name\n");
            return 0;
        }
        if (!strcmp(name, FILE_NAME))
        {
            if ((fname = strdup(V_STRING(s))) == NULL)
            {
                // error msg couldn't read the file name
                parse_error("Error extracting table file name\n");
                return 0;
            }
            if (data_key->FileName != NULL)
            {
                if ((tmp = (char *) malloc(strlen(fname) + strlen(
                        data_key->FileName) + sizeof(char))) == NULL)
                {
                    parse_error(
                            "Unable to allocate memory for table file name expansion\n");
                    if (fname != NULL)
                    {
                        free(fname);
                    }
                    return 0;
                }
                else
                {
                    tmp = strcpy(tmp, data_key->FileName);
                    tmp = strcat(tmp, fname);
                    if (data_key->FileName != NULL)
                    {
                        free(data_key->FileName);
                    }
                    data_key->FileName = tmp;
                    free(fname);
                }
            }
            else
            {
                data_key->FileName = fname;
            }

        }
        else if (!strcmp(name, FILE_SIZE))
        {
            fCount = get_struct_count(s);
            for (j = 0; j < fCount; j++)
            {
                get_struct_element(s, j, &finfo, &t);
                if (finfo == NULL || t == NULL || !strcmp(finfo, VALUE))
                {
                    data_key->size = V_INT(t);
                    if (errno == ERANGE)
                    {
                        parse_error("Could not read the PDS4 table file size\n");
                        return 0;
                    }
                }
            }
        }
    }

    return 1;
}

static Var * xmlParseLabelFiles(Var * v, LABEL *label, dataKey *data_key,
        int use_names, int get_data, char *file_path)
{
    char *elem_type = NULL;
    char *node_name = NULL;
    char *name = NULL;
    char *table_name = NULL;
    int count = 0, err = 0, i;
    Var *s = NULL;
    Var *t = NULL;
    Var *u = NULL;

    count = get_struct_count(v);
    for (i = 0; i < count; i++)
    {
        get_struct_element(v, i, &name, &s);
        if (name != NULL)
        {
            node_name = strdup(name);
            if (node_name == NULL)
            {
                /* do something sensible here */
                parse_error(
                        "Failure trying to parse the PDS4 xml label files\n");
                err = 1;
                break;
            }
            // if this node is a file
            if (strcmp(node_name, EXTERNAL_FILE) == 0)
            {
                if (data_key == NULL)
                {
                    // create a new LABEL and pass it in
                    label = malloc(sizeof(LABEL));
                    if (label == NULL)
                    {
                        parse_error(
                                "Memory allocation error while trying to parse the PDS4 xml label files\n");
                        err = 1;
                        break;
                    }
                }
                label->reclen = 0;
                label->name = strdup(node_name);
                label->nfields = 0;
                label->nrows = 0;
                label->fields = NULL;
                label->keys = NULL;
                label->table = NULL;

                // create a new dataKey and pass it in as well
                data_key = calloc(1,sizeof(dataKey));
                if (data_key == NULL)
                {
                    parse_error(
                            "Memory allocation error while trying to parse the PDS4 xml label files\n");
                    err = 1;
                    break;
                }
                data_key->ObjClass = NULL;
                data_key->Obj = NULL;
                data_key->KeyValue = NULL;
                if (file_path != NULL)
                {
                    data_key->FileName = strdup(file_path);
 //                   free(file_path);
                }
                else
                {
                    data_key->FileName = NULL;
                }
                data_key->dptr = 0;
                data_key->size = 0;
                data_key->objDesc = NULL;
                data_key->pFileDesc = NULL;
                data_key->pFileVar = NULL;

                if (!loadFileInfo(s, label, data_key))
                {
                    err = 1;
                    break;
                }
            }
            else if (strstr(node_name, ARRAY_2D_IMAGE) != NULL && label != NULL
                    && data_key != NULL)
            {
                if (find_struct(s, NAME_ATTRIBUTE, &t) >= 0)
                {
                    char *image_name = strdup(V_STRING(t));
                    if (image_name != NULL)
                    {
                        data_key->Name = image_name;
                    }
                }

                if (!load2dImage(s, label, data_key, use_names, get_data))
                {
                    err = 1;
                    break;
                }

            }
            else if (strstr(node_name, TABLE_BINARY) != NULL && label != NULL
                    && data_key != NULL)
            {
                if (find_struct(s, NAME_ATTRIBUTE, &t) >= 0)
                {
                    table_name = strdup(V_STRING(t));
                    if (table_name != NULL)
                    {
                        data_key->Name = table_name;
                    }
                }

                if (!loadBinaryTable(s, label, data_key, use_names, get_data))
                {
                    err = 1;
                    break;
                }

            }
            else if (strstr(node_name, ELEMENT_TYPE) != NULL && label != NULL
                    && data_key != NULL)
            {
                elem_type = strdup(V_STRING(s));
                if (elem_type != NULL && strstr(elem_type, TABLE_BINARY)
                        != NULL)
                {
                    free(elem_type);
                    if (!loadBinaryTable(v, label, data_key, use_names,
                            get_data))
                    {
                        err = 1;
                        break;
                    }
                }
            }
            else if (use_names) // looking for the name of the potential binary table
            {
                if (V_TYPE(s) == ID_STRUCT)
                {
                    if (find_struct(s, ELEMENT_TYPE, &u) >= 0)
                    {
                        elem_type = strdup(V_STRING(u));
                        if (elem_type != NULL && !strcmp(elem_type,
                                TABLE_BINARY))
                        {
                            if (data_key == NULL)
                            {
                                // create a new LABEL and pass it in
                                label = malloc(sizeof(LABEL));
                                if (label == NULL)
                                {
                                    parse_error(
                                            "Memory allocation error while trying to parse the PDS4 xml label files\n");
                                    err = 1;
                                    break;
                                }
                            }
                            data_key->Name = strdup(node_name);
                        }
                    }
                }
            }
        }
        if (s != NULL)
        {
            if (V_TYPE(s) == ID_STRUCT)
            {
                if (!xmlParseLabelFiles(s, label, data_key, use_names,
                        get_data, file_path))
                {
                    // error message;
                    err = 1;
                    break;
                }
            }
        }
        else
        {
            parse_error(
                    "Encountered a NULL davinci child node while parsing an PDS4 XML label\n");
            err = 1;
        }
    }
    if (err)
    {
        if (node_name != NULL)
        {
            free(node_name);
        }
        return NULL;
    }
    return v;
}

/**
 * This function loads a PDS4 label file and any associated tables into
 * davinci struct(s) if successful.
 *
 * Dependent on davinci being built with libxml2 and the libxml2
 * libraries being available on the system.
 *
 * Should be thread safe IF the libxml2 conditions at:
 * http://xmlsoft.org/threads.html
 * are met.
 */
Var *
do_loadPDS4(vfuncptr func, char *filename, int use_names, int get_data)
{
    char *fname;
    char *file_path = NULL, *path_end = NULL;
    FILE *fp;
    Var *labelFile = NULL;
    int path_size = 0;

    if (filename == NULL)
    {
        if (func == NULL)
        {
            parse_error("No filename specified\n");
        }
        else
            parse_error("%s: No filename specified\n", func->name);
        return (NULL);
    }
    if ((fname = dv_locate_file(filename)) == (char*) NULL)
    {
        if (func == NULL)
        {
            parse_error("Unable to expand filename %s\n", filename);
        }
        else
            parse_error("%s: Unable to expand filename %s\n", func->name, filename);
        return (NULL);
    }

    if (access(fname, R_OK) != 0)
    {
        if (func == NULL)
        {
            parse_error("Unable to find file %s.", filename);
        }
        else
            parse_error("%s: Unable to find file %s.", func->name, filename);
        return (NULL);
    }

    /**
     *** TODO What about compression? We'll deal with this at a later date
     *
     if ((fp = fopen(fname, "rb")) != NULL) {
     if (iom_is_compressed(fp)) {
     // fprintf(stderr, "is compressed\n");    FIX: remove
     fclose(fp);
     fname = iom_uncompress_with_name(fname);
     fp = fopen(fname, "rb");
     }
     }
     */

    labelFile = dv_LoadXML(fname, use_names);

    if (labelFile != NULL)
    {
        // parse the directory from the file name and pass it in

        path_end = strrchr(fname, pathSeparator);

        if (path_end) // the file isn't in the current directory
        {
            path_size = path_end - fname + 2;
            file_path = (char*) malloc(path_size);
            if (file_path)
            {
                file_path = strncpy(file_path, fname, path_size);
                file_path[path_size - 1] = '\0';
            }
        }

        labelFile = xmlParseLabelFiles(labelFile, (LABEL *) NULL,
                (dataKey *) NULL, use_names, get_data, file_path);
    }

    return labelFile;
}

/**
 * The same functionality and dependencies as do_loadPDS4() elsewhere in this file.
 *
 */
Var *dv_loadPDS4(char *filename)
{
    return do_loadPDS4((vfuncptr) NULL, filename, 1, 1);
}

#endif
