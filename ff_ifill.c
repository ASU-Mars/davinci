#include "parser.h"

/**
 **/
#define ycorner(c, v) ((c) < 4 ? -(v) : ((c) > 6 ? (v) : 0))
#define xcorner(c, v) ((c) == 1 || (c) == 4 || (c) == 7 ? -(v) : \
                      ((c) == 3 || (c) == 6 || (c) == 9 ? (v) : 0))

void jfill_x(Var *, int , int , int , float *, float , int, int);
void jfill_y(Var *, int , int , int , float *, float , int);
void jfill_merge(float *, float *, float , int);
void jfill_lr(Var *, int , int , int , float *, float , int, int);
void jfill_tb(Var *, int , int , int , float *, float , int);
void jfill_tr(Var *, int , int , int , float *, float , int);
void jfill_tl(Var *, int , int , int , float *, float , int);
void jfill(float *data, int n, float fill, int radius, int wrap);

/**
 ** ff_pause() - Get a line of input from the user
 **/

Var *
ff_skel(vfuncptr func, Var * arg)
{
    Var *v, *s;
    char buf[256];

    if ((v = verify_single_string(func, arg)) != NULL) {
        printf("%s", V_STRING(v));
        fflush(stdout);
    }
    fgets(buf, 256, stdin);
    /**
    ** Create the output object.
    **/
    s = new(Var);
    V_TYPE(s) = ID_STRING;
    V_STRING(s) = strdup(buf);

    return (s);
}


Var *
ff_jfill(vfuncptr func, Var * arg)
{
    char *fname;
    Var *v, *ev, *obj;
	float val[10], dist[10];
    int ecount, xpos[10], ypos[10], width, height, depth;
    int i, j, k, e, x, y, band, pos;
    float *data, d;
	int wrap = 0;			/* left to right wrap around */
    int radius = 3;			/* distance to search */
	int neighbors = 5;		/* minimum number of neighbors */
    float fill = 0.0;		/* fill value */
	int ac;
	Var **av;

	Alist alist[6];
	alist[0] = make_alist( "object",    ID_VAL, NULL, &obj);
	alist[1] = make_alist( "fill",      FLOAT,  NULL, &fill);
	alist[2] = make_alist( "radius",    INT,    NULL, &radius);
	alist[3] = make_alist( "wrap",      INT,    NULL, &wrap);
	alist[4] = make_alist( "neighbors",      INT,    NULL, &neighbors);
	alist[5].name = NULL;

	make_args(&ac, &av, func, arg);
	if (parse_args(ac, av, alist)) return(NULL);

	if (obj == NULL) {
		parse_error("%s: No object specified\n", func->name);
		return(NULL);
	}

    width = GetSamples(V_SIZE(obj), V_ORG(obj));
    height = GetLines(V_SIZE(obj), V_ORG(obj));
    depth = GetBands(V_SIZE(obj), V_ORG(obj));
    data = (float *) calloc(sizeof(float), width * height * depth);

    /** 
    ** Put together return value
    **/

    v = new(Var);
    V_TYPE(v) = ID_VAL;
    V_ORG(v) = V_ORG(obj);
    V_DSIZE(v) = V_DSIZE(obj);
    V_SIZE(v)[0] = V_SIZE(obj)[0];
    V_SIZE(v)[1] = V_SIZE(obj)[1];
    V_SIZE(v)[2] = V_SIZE(obj)[2];
    V_FORMAT(v) = FLOAT;
    V_DATA(v) = data;

    for (band = 0; band < depth; band++) {
        for (j = 0; j < height; j++) {
            for (i = 0; i < width; i++) {
                pos = cpos(i, j, band, obj);
                d = extract_double(obj, pos);
                data[pos] = d;
                if (d != fill) {
                    continue;
                }
                ecount = 0;
                for (k = 1; k < radius; k++) {
                    for (e = 1; e <= 9; e++) {
                        if (k == 1)
                            val[e] = fill;
                        if (val[e] == fill) {
                            x = i + xcorner(e, k);
                            y = j + ycorner(e, k);
                            if (x < 0 || x >= width) {
								if (wrap) {
									x = (x+width) % width;
								} else {
									continue;
								}
							}

							if (y < 0 || y >= height) {
                                continue;
							}

                            val[e] = extract_float(obj, cpos(x, y, band, obj));
                            xpos[e] = x;
                            ypos[e] = y;
                            dist[e] = 1.0 / (k * k);
                            if (val[e] != fill)
                                ecount++;
                        }
                    }
                    if (ecount >= neighbors) {
                        d = 0;
                        for (e = 1; e <= 9; e++) {
                            /**
                            ** compute weighted average
                            **/
                            if (e == 5)
                                continue;
                            if (val[e] != fill) {
                                val[5] += val[e] * dist[e];
                                d += dist[e];
                            }
                        }
                        data[pos] = val[5] / d;
                        /**
                        ** stop everything.
                        **/
                        e = 10; 
                        k = radius;
                    }
                }
            }
        }
    }
    return (v);
}

Var *
ff_ifill(vfuncptr func, Var * arg)
{
    char *fname;
    Var *v, *ev, *obj;
    int radius = 3;
    float fill = 0.0;
    float *data1, *data2, *data3, d;
	int state=0;
	int width,height,depth, dsize;
	char *pass = "1234";
	int wrap = 0;

	int ac;
	Var **av;
	Alist alist[6];
	alist[0] = make_alist( "object",    ID_VAL, NULL, &obj);
	alist[1] = make_alist( "fill",      FLOAT,  NULL, &fill);
	alist[2] = make_alist( "radius",    INT,    NULL, &radius);
	alist[3] = make_alist( "pass",      ID_STRING,    NULL, &pass);
	alist[4] = make_alist( "wrap",      INT,    NULL, &wrap);
	alist[5].name = NULL;

	make_args(&ac, &av, func, arg);
	if (parse_args(ac, av, alist)) return(NULL);

	if (obj == NULL) {
		parse_error("%s: No object specified\n", func->name);
		return(NULL);
	}

    width = GetSamples(V_SIZE(obj), V_ORG(obj));
    height = GetLines(V_SIZE(obj), V_ORG(obj));
    depth = GetBands(V_SIZE(obj), V_ORG(obj));
	dsize = width*height*depth;
    data1 = (float *) calloc(sizeof(float), dsize);
    data2 = (float *) calloc(sizeof(float), dsize);
    data3 = (float *) calloc(sizeof(float), dsize);

    /** 
    ** Put together return value
    **/

	if (strchr(pass, '3') && strchr(pass, '4')) {
		jfill_tl(obj, width, height, depth, data1, fill, radius);
		jfill_tr(obj, width, height, depth, data2, fill, radius);
		jfill_merge(data1, data2, fill, dsize);
	} else if (strchr(pass, '4')) {
		jfill_tl(obj, width, height, depth, data1, fill, radius);
	} else if (strchr(pass, '3')) {
		jfill_tr(obj, width, height, depth, data1, fill, radius);
	}

	if (strchr(pass, '1') && strchr(pass, '2')) {
		jfill_tb(obj, width, height, depth, data2, fill, radius);
		jfill_lr(obj, width, height, depth, data3, fill, radius, wrap);
		jfill_merge(data2, data3, fill, dsize);
	} else if (strchr(pass, '1')) {
		jfill_tb(obj, width, height, depth, data2, fill, radius);
	} else if (strchr(pass, '2')) {
		jfill_lr(obj, width, height, depth, data2, fill, radius, wrap);
	}

	if ((strchr(pass, '3') || strchr(pass, '4')) &&
	    (strchr(pass, '1') || strchr(pass, '2'))) {
			jfill_merge(data1, data2, fill, dsize);
	} else if (strchr(pass, '1') || strchr(pass, '2')) {
		void *t = data2;
		data2 = data1;
		data1 = t;
	}

    v = new(Var);
    V_TYPE(v) = ID_VAL;
    V_ORG(v) = V_ORG(obj);
    V_DSIZE(v) = V_DSIZE(obj);
    V_SIZE(v)[0] = V_SIZE(obj)[0];
    V_SIZE(v)[1] = V_SIZE(obj)[1];
    V_SIZE(v)[2] = V_SIZE(obj)[2];
    V_FORMAT(v) = FLOAT;
    V_DATA(v) = data1;

	free(data2);
	free(data3);
    return (v);
}

void
jfill_x(Var *obj, int width, int height, int depth, float *data, 
		float fill, int radius, int wrap)
{
    int i, j, k, band, pos, x1, x2, y1, y2;
    float m, d;
	int state = 0;
	int w = width;

	if (wrap) w = width+radius;

    for (band = 0; band < depth; band++) {
        for (k = 0; k < w ; k++) {
			i = (wrap ? k % width : k);
			state = 0;
            for (j = 0; j < height; j++) {
                pos = cpos(i, j, band, obj);
                d = extract_float(obj, pos);
                data[pos] = d;
				/*
				** Step through values to find first hole
				*/
                if (state == 0 && d != fill) {
						/* found a value */
                        y1 = d;
                        x1 = j;
                        state = 1;
                } else if (state == 1) {
                    if (d == fill) {
						/* found a hole.  Get ready to fill it */
                        state = 2;
                    } else {
						/* found another value, move to it. */
                        y1 = d;
                        x1 = j;
                    }
                } else if (state == 2 && d != fill) {
					/* found a value on the other side of the hole */
                    y2 = d;
                    x2 = j;
					if (x2-x1 > radius) {
						state = 0;
						continue;
					}
                    m = (y2 - y1)/(x2-x1);
                    for (j = x1+1 ; j < x2 ; j++) {
                        pos = cpos(i, j, band, obj);
                        data[pos] = y1+m*(j-x1);
                    }
                    x1 = x2;
                    y1 = y2;
                    state = 1;
                }
            }
        }
    }
}

void
jfill_y(Var *obj, int width, int height, int depth, float *data, 
		float fill, int radius)
{
    int i, j, band, pos, x1, x2, y1, y2;
    float m, d;
	int state = 0;

    for (band = 0; band < depth; band++) {
		for (i = 0; i < height; i++) {
			state = 0;
			for (j = 0; j < width; j++) {
                pos = cpos(j, i, band, obj);
                d = extract_float(obj, pos);
                data[pos] = d;
                if (state == 0 && d != fill) {
                        y1 = d;
                        x1 = j;
                        state = 1;
                } else if (state == 1) {
                    if (d == fill) {
                        state = 2;
                    } else {
                        y1 = d;
                        x1 = j;
                    }
                } else if (state == 2 && d != fill) {
                    y2 = d;
                    x2 = j;
					if (x2-x1 > radius) {
						state = 0;
						continue;
					}
                    m = (y2 - y1)/(x2-x1);
                    for (j = x1+1 ; j < x2 ; j++) {
                        pos = cpos(j, i, band, obj);
                        data[pos] = y1+m*(j-x1);
                    }
                    x1 = x2;
                    y1 = y2;
                    state = 1;
                }
            }
        }
    }
}

void
jfill_lr(Var *obj, int x, int y, int z, float *data, float fill, int radius, int wrap)
{
    int i, j, k, *p;
    float m, *d;

	/**
	 ** do across
	 **/
	d = calloc(x, sizeof(float));
	p = calloc(x, sizeof(int));

    for (k = 0; k < z; k++) {
		for (j = 0; j < y; j++) {
			for (i = 0; i < x; i++) {
				p[i] = cpos(i, j, k, obj);
				d[i] = extract_float(obj, p[i]);
            }
			jfill(d, x, fill, radius, wrap);
			for (i = 0; i < x; i++) {
				data[p[i]] = d[i];
            }
        }
    }
	free(d);
	free(p);
}

void
jfill_tb(Var *obj, int x, int y, int z, float *data, float fill, int radius)
{
    int i, j, k, *p;
    float m, *d;

	d = calloc(y, sizeof(float));
	p = calloc(y, sizeof(int));

    for (k = 0; k < z; k++) {
		for (i = 0; i < x; i++) {
			for (j = 0; j < y; j++) {
				p[j] = cpos(i, j, k, obj);
				d[j] = extract_float(obj, p[j]);
            }
			jfill(d, y, fill, radius, 0);
			for (j = 0; j < y; j++) {
				data[p[j]] = d[j];
            }
        }
    }
	free(d);
	free(p);
}

void
jfill_tr(Var *obj, int x, int y, int z, float *data, float fill, int radius)
{
    int n, i, j, k, l, *p;
    float m, *d;

	/**
	 ** do top to right
	 **/
	n = max(x,y);
	d = calloc(n, sizeof(float));
	p = calloc(n, sizeof(int));

    for (k = 0; k < z; k++) {
		for (i = 0 ; i < x ; i++) {
			for (j = 0 ; j < y ; j++) {
				if (i+j >= x) break;
				p[j] = cpos(i+j, j, k, obj);
				d[j] = extract_float(obj, p[j]);
			}
			jfill(d, j, fill, radius, 0);
			for (l = 0 ; l < j ; l++) {
				data[p[l]] = d[l];
			}
		}
		/* we've already done the zero stripe, start at 1 */
		for (j = 1 ; j < y ; j++) {
			for (i = 0 ; i < x ; i++) {
				if (i+j >= y) break;
				p[i] = cpos(i, j+i, k, obj);
				d[i] = extract_float(obj, p[i]);
			}
			jfill(d, i, fill, radius, 0);
			for (l = 0 ; l < i ; l++) {
				data[p[l]] = d[l];
			}
		}
	}
	free(d);
	free(p);
}


void
jfill_tl(Var *obj, int x, int y, int z, float *data, float fill, int radius)
{
    int n, i, j, k, l, *p;
    float m, *d;

	/**
	 ** do top to right
	 **/
	n = max(x,y);
	d = calloc(n, sizeof(float));
	p = calloc(n, sizeof(int));

    for (k = 0; k < z; k++) {
		for (i = 0 ; i < x ; i++) {
			for (j = 0 ; j < y ; j++) {
				if (i-j < 0) break;
				p[j] = cpos(i-j, j, k, obj);
				d[j] = extract_float(obj, p[j]);
			}
			jfill(d, j, fill, radius, 0);
			for (l = 0 ; l < j ; l++) {
				data[p[l]] = d[l];
			}
		}
		/* we've already done the zero stripe, start at 1 */

		for (j = 1 ; j < y ; j++) {
			for (i = 0 ; i < x ; i++) {
				if (x-i-1 < 0 || j+i >= y) break;
				p[i] = cpos(x-i-1, j+i, k, obj);
				d[i] = extract_float(obj, p[i]);
			}
			jfill(d, i, fill, radius, 0);
			for (l = 0 ; l < i ; l++) {
				data[p[l]] = d[l];
			}
		}
	}
	free(d);
	free(p);
}

void
jfill_merge(float *d1, float *d2, float fill, int dsize)
{
	int i;

	for (i = 0 ; i < dsize ; i++) {
		if (d1[i] == fill)  {
			if (d2[i] != fill) {
				d1[i] = d2[i];
			}
		} else {
			if (d2[i] != fill) {
				d1[i] += d2[i];
				d1[i] /= 2.0;
			}
		}
	}
}

void
jfill(float *data, int n, float fill, int radius, int wrap)
{
	int i, j, k, x1, x2, state = 0;
	float y1, y2, m, d;
	int N = n;

	if (wrap) N += radius;

	for (i = 0 ; i < N ; i++) {
		d = data[i % n];
		if (state == 0 && d != fill) {
				y1 = d;
				x1 = i;
				state = 1;
		} else if (state == 1) {
			if (d == fill) {
				state = 2;
			} else {
				y1 = d;
				x1 = i;
			}
		} else if (state == 2 && d != fill) {
			y2 = d;
			x2 = i;
			if (x2-x1 > radius) {
				x1 = x2;
				y1 = y2;
				state = 1;
				continue;
			}
			m = (y2 - y1)/(x2-x1);
			for (j = x1+1 ; j < x2 ; j++) {
				data[j % n] = y1+m*(j-x1);
			}
			x1 = x2;
			y1 = y2;
			state = 1;
		}
	}
}
