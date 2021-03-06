#ifndef _FF_H_
#define _FF_H_
#include "parser.h"

/**
 ** The list of named functions, their handlers and an optional data value
 **/

struct _vfuncptr vfunclist[] = {
  { "nop",        ff_nop,     NULL , NULL},
  { "exit",       ff_exit,    NULL , NULL},
  { "echo",       ff_echo,    NULL , NULL},

  /*
   * By using the third argument to hold a pointer to an actual library
   * function, we can reuse ff_dfunc() to implement ANY function that
   * expects a single double argument, and returns a single double
   * argument (like all the simple math functions).
   *
   * Presumably, we could extend this idea to handle all integer and
   * string function (ff_ifunc and ff_sfunc), but they are of less use.
   */

  { "cos",        ff_dfunc,   (void *)cos,    NULL},
  { "sin",        ff_dfunc,   (void *)sin,    NULL},
  { "tan",        ff_dfunc,   (void *)tan,    NULL},
  { "acos",       ff_dfunc,   (void *)acos,   NULL},
  { "asin",       ff_dfunc,   (void *)asin,   NULL},
  { "atan",       ff_dfunc,   (void *)atan,   NULL},
  { "cosd",       ff_dfunc,   (void *)cosd,   NULL},
  { "sind",       ff_dfunc,   (void *)sind,   NULL},
  { "tand",       ff_dfunc,   (void *)tand,   NULL},
  { "acosd",      ff_dfunc,   (void *)acosd,  NULL},
  { "asind",      ff_dfunc,   (void *)asind,  NULL},
  { "atand",      ff_dfunc,   (void *)atand,  NULL},
  { "sqrt",       ff_dfunc,   (void *)sqrt,   NULL},
  { "log10",      ff_dfunc,   (void *)log10,  NULL},
  { "log",        ff_dfunc,   (void *)log,    NULL},
  { "ln",         ff_dfunc,   (void *)log,    NULL},
  { "exp",        ff_dfunc,   (void *)exp,    NULL},
  { "floor",      ff_dfunc,   (void *)floor,  NULL},
  { "ceil",       ff_dfunc,   (void *)ceil,   NULL},
  { "abs",        ff_dfunc,   (void *)fabs,   NULL},
  { "round",        ff_dfunc,   (void *)my_round,   NULL},
  { "my_round",        ff_dfunc,   (void *)my_round,   NULL},

  { "pow",        ff_pow,     (void *)NULL,   NULL},

  /* format conversion routines.  All just shorthand for ff_conv() */

  { "char",       ff_conv, (void *)BYTE, NULL},
  { "byte",       ff_conv, (void *)BYTE, NULL},
  { "short",      ff_conv, (void *)SHORT, NULL},
  { "int",        ff_conv, (void *)INT, NULL},
  { "float",      ff_conv, (void *)FLOAT, NULL},
  { "double",     ff_conv, (void *)DOUBLE, NULL},

  /* org conversion.  All just shorthand for ff_org() */

  { "bil",        ff_org, (void *)(BIL+10), NULL},
  { "bsq",        ff_org, (void *)(BSQ+10), NULL},
  { "bip",        ff_org, (void *)(BIP+10), NULL},

  /* some generic information routines */

  { "dim",        ff_dim,         NULL , NULL},
  { "format",     ff_format,      NULL , NULL},
  { "type",       ff_format,      NULL , NULL},
  { "org",        ff_org,         NULL , NULL},
  { "order",      ff_org,         NULL , NULL},

  { "create",     ff_create,      NULL , NULL},
  { "clone",      ff_replicate, NULL , NULL},
  { "cat",        ff_cat,       NULL , NULL},
  { "translate",  ff_translate, NULL , NULL},
  { "struct",         ff_struct,             NULL , NULL},
  { "add_struct",     ff_insert_struct,         NULL , NULL},
  { "get_struct",     ff_get_struct,         NULL , NULL},
  { "remove_struct",     ff_remove_struct,         NULL , NULL},
  { "insert_struct",     ff_insert_struct,         NULL , NULL},
  { "get_struct_key",    ff_get_struct_key,        NULL, NULL},

  /* i/o */

  { "source",     ff_source,      NULL , NULL},
  { "load",       ff_load,        NULL , NULL},
  { "read",       ff_load,        NULL , NULL},        /* an alias */

  { "load_ir",        ff_Frame_Grabber_Read,  NULL, NULL},
  { "load_frame",     ff_Frame_Grabber_Read,  NULL, NULL},
  { "load_vis",       ff_GSE_VIS_Read,        NULL, NULL},
  { "load_paci",      ff_PACI_Read,           NULL, NULL},
  { "load_specpr",    ff_loadspecpr,          NULL, NULL},
  { "load_vanilla",   ff_loadvan,             NULL, NULL},
  { "load_PDS",       ReadPDS,                NULL, NULL},
  { "load_pds",       ReadPDS,                NULL, NULL},
  { "load_isis3",     ReadISIS3,              NULL, NULL},
  { "load_ISIS3",     ReadISIS3,              NULL, NULL},
  { "write_isis3",    WriteISIS3,             NULL, NULL},
  { "write_ISIS3",    WriteISIS3,             NULL, NULL},
#ifdef HAVE_LIBXML2
  { "load_PDS4",       ReadPDS4,                NULL, NULL},
  { "load_pds4",       ReadPDS4,                NULL, NULL},
#endif
#ifdef HAVE_LIBCFITSIO
  { "load_fits",      ReadFITS,               NULL, NULL},
  { "write_fits",      WriteFITS,               NULL, NULL},
#endif
  { "isis",           ff_read_suffix_plane,   NULL, NULL},
  { "load_raw",   ff_raw,         NULL, NULL},
  { "ascii",      ff_ascii,       NULL , NULL},
  { "read_ascii", ff_ascii,       NULL , NULL},
  { "read_text",  ff_read_text,   NULL , NULL},
  { "read_lines", ff_read_lines,  NULL, NULL},
  { "unpack",     ff_unpack,      NULL, NULL},
  { "pack",       ff_pack,        NULL, NULL},

  { "save",       ff_write,       NULL , NULL},
  { "write",      ff_write,       NULL , NULL},        /* an alias */
  { "export",     ff_write,       NULL , NULL},        /* an alias */
  { "filetype",   ff_filetype,    NULL , NULL},
  { "ls",         ff_list,        NULL , NULL},
  { "list",       ff_list,        NULL , NULL},
  { "save_pds",   WritePDS,       NULL , NULL},
  { "write_pds",  WritePDS,       NULL , NULL},
  { "load_csv",   ff_loadcsv,     NULL , NULL},

  /* string functions */

  { "string",     ff_string,      NULL , NULL},

  { "atoi",       ff_atoi,        NULL , NULL},
  { "atof",       ff_atof,        NULL , NULL},
  { "atod",       ff_atof,        NULL , NULL},
  { "delim",      ff_delim,       NULL , NULL},
  { "issubstring", ff_issubstring, NULL, NULL},
  { "strlen",     ff_strlen,      NULL, NULL},
  { "strstr",     ff_strstr,      NULL, NULL},
  { "grep",       ff_grep,        NULL, NULL},
  { "basename",   ff_filename,    (void *)1, NULL},
  { "dirname",    ff_filename,    (void *)2, NULL},
  { "strsub",     ff_stringsubst, NULL, NULL},
  { "rtrim",      ff_rtrim,       NULL, NULL},

  { "dump",       ff_dump,        NULL, NULL},
  { "global",     ff_global,      NULL, NULL},
  { "delete",     ff_delete,      NULL, NULL},
  { "equals",     ff_equals,      NULL, NULL},
  { "length",     ff_length,      NULL, NULL},
  { "hasvalue",   HasValue,       NULL , NULL},
  { "HasValue",   HasValue,       NULL , NULL},

  { "printf",     ff_printf,      NULL , NULL},
  { "sprintf",    ff_sprintf,     NULL , NULL},
  { "fprintf",    ff_fprintf,     NULL , NULL},

  { "syscall",    ff_syscall,     NULL, NULL},
  { "shell",      ff_syscall,     NULL, NULL},
  { "system",     ff_system,      NULL , NULL},
  { "fsize",      ff_fsize,       NULL , NULL},
  { "pause",      ff_pause,       NULL , NULL},    /* get input from user */
  { "eval",       ff_eval,        NULL , NULL},
  { "putenv",     ff_putenv,      NULL, NULL},

  { "edit",       ufunc_edit,     NULL , NULL},
  { "history",    ff_history,     NULL , NULL},
  { "h",          ff_history,     NULL , NULL},

  { "image_resize",      ff_image_resize,       NULL , NULL},
  { "resize",     ff_resize,      NULL , NULL},
  { "fork",       ff_fork,        NULL , NULL},
  { "audit",      ff_audit,       NULL , NULL},
  { "header",     ff_header,      NULL , NULL},
  { "killall",    ff_killchild,   NULL, NULL},
  { "version",    ff_version,     NULL , NULL},

  { "rand",       ff_random,      (void *)"rand" , NULL},
  { "random",     ff_random,      NULL , NULL},
  { "rnoise",     ff_random,      (void *)"rnoise" , NULL},
  { "gnoise",     ff_gnoise,      NULL , NULL},
  { "cluster",    ff_cluster,     NULL , NULL},
  { "ccount",     ff_ccount,      NULL , NULL},

  { "moment",     ff_moment,      NULL , NULL},
  { "moments",     ff_moments,      NULL , NULL},
  { "avg",     ff_avg2,           NULL , NULL},
  { "stddev",  ff_avg2,           NULL , NULL},
  { "sum",     ff_avg2,            NULL , NULL},
  { "min",     ff_min,            NULL , NULL},
  { "max",     ff_min,            NULL , NULL},
  { "sort",     ff_sort,          NULL , NULL},
  { "unique",     ff_unique,          NULL , NULL},
  { "minchan",  ff_findmin,       NULL , NULL},
  { "maxchan",  ff_findmin,       NULL , NULL},
  { "maxpos",  ff_maxpos,       NULL , NULL},
  { "minpos",  ff_minpos,       NULL , NULL},
  { "valpos",  ff_valpos,       NULL , NULL},


  { "interp",     ff_interp,      NULL , NULL},
  { "resample",   ff_resample,    NULL , NULL},
  { "interp2d",   ff_interp2d,    NULL , NULL},
  { "flood_fill", ff_flood_fill,    NULL , NULL},
  { "gplot",      ff_gplot,       NULL , NULL},
  { "plot",       ff_vplot,        NULL , NULL}, //former ff_plot
  { "splot",      ff_splot,       NULL , NULL},
  { "xplot",      ff_xplot,       NULL, NULL},
  { "vplot",      ff_vplot,       NULL, NULL},
  { "display",    ff_display,     NULL , NULL},
  { "fit",        ff_fit,         NULL , NULL},
  { "ipi",        ff_ipi,         NULL , NULL},

  { "bbr",        ff_bop,      (void *)bbr   , NULL},
  { "btemp",      ff_bop,      (void *)btemp , NULL},
  { "atan2",      ff_bop,      (void *)atan2 , NULL},

  { "vignette",   ff_vignette, NULL , NULL},
  { "ifill",      ff_ifill,    NULL , NULL},    /* interpolated fill */
  { "jfjll",      ff_jfill,    NULL , NULL},    /* jnterpolated fjll */
  { "kfill",      ff_kfill,    NULL , NULL},
  { "contour",    ff_contour,   NULL , NULL},
  { "rotate",    ff_rotation,   NULL , NULL},


  { "basis",      ff_basis,     NULL , NULL},
  { "mxm",        ff_mxm,       NULL , NULL},
  { "identity",   ff_identity,  NULL , NULL},
  { "minvert",    ff_minvert,   NULL , NULL},
  { "eigen",      ff_eigen,     NULL , NULL},
  { "covar",      ff_covar,     NULL , NULL},
  { "corr",       ff_covar,     NULL , NULL},
  { "scp",        ff_covar,     NULL , NULL},
  { "pcs",        ff_pcs,       NULL , NULL},

  { "histogram",  ff_histogram, NULL , NULL},
  { "hstats",     ff_hstats,    NULL , NULL},
  { "rgb2hsv",    ff_rgb2hsv,   NULL , NULL},
  { "hsv2rgb",    ff_hsv2rgb,   NULL , NULL},
  { "rgb",        ff_rgb,       NULL , NULL},
  { "ramp",       ff_ramp,      NULL , NULL},
  { "sawtooth",   ff_sawtooth,  NULL , NULL},
  { "grassfile",  ff_grassfire, NULL, NULL},

  { "entropy",    ff_entropy,   NULL, NULL},

  { "dct",        ff_dct,     NULL , NULL},
  { "fft",        ff_fft,     (void *)1 , NULL},
  { "ifft",       ff_fft,     (void *)0 , NULL},
  { "rfft",       ff_realfft,     (void *)1 , NULL},
  { "irfft",      ff_realfft,     (void *)0 , NULL},
  { "rfft2",      ff_realfft2,     (void *)1 , NULL},
  { "irfft2",     ff_realfft2,     (void *)0 , NULL},
  { "rfft3",      ff_realfft3,     (void *)1 , NULL},
  { "irfft3",     ff_realfft3,     (void *)0 , NULL},

  { "self_convolve", ff_self_convolve,        NULL , NULL},
  { "convolve",      ff_convolve,             NULL , NULL},
  { "convolve2",     ff_convolve2,            NULL , NULL},
  { "convolve3",     ff_convolve3,            NULL , NULL},
  { "fncc",       ff_fncc,                      NULL, NULL},

  { "pnmcut",         ff_cut,                 NULL, NULL},
  { "pnmcrop",        ff_crop,                NULL, NULL},

#ifdef INCLUDE_API
  { "popen",   ff_popen,   NULL, NULL},
  { "pprint",  ff_pprint,  NULL, NULL},
  { "pplot",   ff_pplot,   NULL, NULL},
  { "ptext",   ff_ptext,   NULL, NULL},
  { "pline",   ff_pline,   NULL, NULL},
  { "pbox",    ff_pbox,    NULL, NULL},
  { "pzoom",   ff_pzoom,   NULL, NULL},
#endif

#if 0
  { "projection", ff_projection,   NULL , NULL},
#endif

#ifdef HAVE_XM_XR3DT_H
  { "xrt3d",          ff_xrt3d,              NULL , NULL},
#endif

#ifdef BUILD_MODULE_SUPPORT
  { "load_module",    ff_load_dv_module,     NULL, NULL},
  { "unload_module",  ff_unload_dv_module,   NULL, NULL},
  { "list_modules",   ff_list_dv_modules,    NULL, NULL},
  { "insmod", ff_insmod, NULL, NULL},
  { "rmmod", ff_rmmod, NULL, NULL},
  { "lsmod", ff_lsmod, NULL, NULL},
#endif /* BUILD_MODULE_SUPPORT */

  /* add additional functions here.  Don't forget the trailing comma. */

  { "vis_downshift", ff_GSE_VIS_downshift,  NULL,NULL},
  { "vis_upshift",   ff_GSE_VIS_upshift,    NULL,NULL},

  { "write_isis",    write_isis_planes,     NULL,NULL},

  /* */

  { "shade",         ff_shade,              NULL,NULL },
  { "shade2",        ff_shade2,             NULL,NULL },
  { "fexists",       ff_exists,             NULL,NULL },
  { "fremove",       ff_unlink,             NULL,NULL },
  { "contains",      ff_contains,           NULL,NULL },


#if defined(HAVE_LIBQMV) && defined(HAVE_QMV_HVECTOR_H)
  { "geom_ghost",    ff_deghost,            NULL,NULL },
#endif
  { "deleted",       ff_deleted,            NULL,NULL },
  { "set_deleted",   ff_set_deleted,        NULL,NULL },

  { "bindct",        ff_bindct,             NULL, NULL },
  { "binidct",       ff_bindct,             NULL, NULL },
  { "rice",          ff_rice,               NULL, NULL },
  { "unrice",        ff_unrice,             NULL, NULL },

  { "distance_map",  ff_distance_map,       NULL, NULL },
  { "slant",         ff_slant,              NULL, NULL },
  { "unslant",       ff_unslant,            NULL, NULL },
  { "unslant_shear", ff_unslant_shear,      NULL, NULL },
  { "chdir",         ff_chdir,              NULL, NULL },
  { "copy",         	ff_copy,            	NULL, NULL },

  { "fncc_fft2d",         ff_fncc_fft2d,    NULL,NULL },
  { "fncc_ifft2d",        ff_fncc_ifft2d,   NULL,NULL },
  { "fncc_cmplx_mul",     ff_fncc_cmplx_mul, NULL,NULL },
  { "fncc_fft_conv_real", ff_fncc_fft_conv_real, NULL, NULL },
  { "fncc_write_mat",     ff_fncc_write_mat, NULL, NULL },
  { "boxfilter",          ff_boxfilter,     NULL, NULL },

  { "hstretch",          ff_hstretch,       NULL, NULL },
  { "sstretch",          ff_sstretch2,      NULL, NULL },
  { "coreg",             ff_coreg,          NULL, NULL },
  { "coreg2",            ff_coreg2,         NULL, NULL },
  { "gconvolve",         ff_gconvolve,      NULL, NULL },
  { "warp",              ff_warp,           NULL, NULL },
  { "window",            ff_window,         NULL, NULL },
  { "extract",           ff_extract,        NULL, NULL },
  { "radial_symmetry",   ff_radial_symmetry, NULL, NULL },
  { "radial_symmetry2",  ff_radial_symmetry2, NULL, NULL },
  { "radial_symmetry3",  ff_radial_symmetry3, NULL, NULL },
  { "local_max",         ff_local_maximum,  NULL, NULL },
  { "drawshape",         ff_drawshape,      NULL, NULL },

#ifdef HAVE_LIBISIS
  { "write_isis_cub",    ff_write_isis_cub, NULL, NULL },
#endif /* HAVE_LIBISIS */

  { "load_bin5",         ff_load_bin5,      NULL, NULL },
  { "load_tdb",          ff_load_tdb,       NULL, NULL },
  { "cinterp",           ff_cinterp,        NULL, NULL },
  /*  { "blend",             ff_blend,          NULL, NULL }, */
  { "text",           ff_create_text,        NULL, NULL },

  { "pdshead",           ff_pdshead,        NULL, NULL },

  { NULL,                NULL,              NULL, NULL }

};

#endif
