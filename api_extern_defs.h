extern pl_pladv(int page);
extern pl_plaxes(float x0,float y0,char *xopt,float xtick,int nxsub,char *yopt,float ytick,int nysub);
extern pl_plbin(int nbin,float *x,float *y,int center);
extern pl_plbop();
extern pl_plbox(char *xopt,float xtick,int nxsub,char *yopt,float ytick,int nysub);
extern pl_plbox3(char *xopt,char *xlabel,float xtick,int nsubx,char *yopt,char *ylabel,float ytick,int nsuby ,char *zopt,char *zlabel,float ztick,int nsubz);
extern pl_plcol0(int icol0);
extern pl_plcol1(float col1);
extern pl_plcpstrm(int iplsr,int flags);
extern pl_plend();
extern pl_plend1();
extern pl_plenv(float xmin,float xmax,float ymin,float ymax,int just,int axis);
extern pl_pleop();
extern pl_plerrx(int n,float *xmin,float *xmax,float *y);
extern pl_plerry(int n,float *x,float *ymin,float *ymax);
extern pl_plfamadv();
extern pl_plfill(int n,float *x,float *y);
extern pl_plflush();
extern pl_plfont(int ifont);
extern pl_plfontld(int fnt);
extern pl_plgchr(float *p_def,float *p_ht);
extern pl_plgcol0(int icol0,int *r,int *g,int *b);
extern pl_plgcolbg(int *r,int *g,int *b);
extern pl_plgdev(char *p_dev);
extern pl_plgdidev(float *p_mar,float *p_aspect,float *p_jx,float *p_jy);
extern pl_plgdiori(float *p_rot);
extern pl_plgdiplt(float *p_xmin,float *p_ymin,float *p_xmax,float *p_ymax);
extern pl_plgfam(int *p_fam,int *p_num,int *p_bmax);
extern pl_plgfnam(char *fnam);
extern pl_plglevel(int *p_level);
extern pl_plgpage(float *p_xp,float *p_yp,int *p_xleng,int *p_yleng,int *p_xoff,int *p_yoff);
extern pl_plgra();
extern pl_plgspa(float *xmin,float *xmax,float *ymin,float *ymax);
extern pl_plgstrm(int *p_strm);
extern pl_plgver(char *p_ver);
extern pl_plgwid(int *width);
extern pl_plgxax(int *p_digmax,int *p_digits);
extern pl_plgyax(int *p_digmax,int *p_digits);
extern pl_plgzax(int *p_digmax,int *p_digits);
extern pl_plhist(int n,float *data,float datmin,float datmax,int nbin,int oldwin);
extern pl_plhls(float h,float l,float s);
extern pl_plinit();
extern pl_pljoin(float x1,float y1,float x2,float y2);
extern pl_pllab(char *xlabel,char *ylabel,char *tlabel);
extern pl_pllabelbox(char *xopt, float xtick1, char *yopt, float ytick1);
extern pl_plline(int n,float *x,float *y);
extern pl_plline3(int n,float *x,float *y,float *z);
extern pl_pllsty(int lin);
extern pl_plmkstrm(int *p_strm);
extern pl_plmtex(char *side,float disp,float pos,float just,char *text);
extern pl_plpat(int nlin,int *inc,int *del);
extern pl_plpoin(int n,float *x,float *y,int code);
extern pl_plpoin3(int n,float *x,float *y,float *z,int code);
extern pl_plpoly3(int n,float *x,float *y,float *z,int *draw);
extern pl_plprec(int setp,int prec);
extern pl_plpsty(int patt);
extern pl_plptex(float x,float y,float dx,float dy,float just,char *text);
extern pl_plptexd(float x,float y,float dx,float dy,float just,char *text);
extern pl_plreplot();
extern pl_plrgb(float r,float g,float b);
extern pl_plrgb1(int r,int g,int b);
extern pl_plschr(float def,float scale);
extern pl_plscmap0(int *r,int *g,int *b,int ncol0);
extern pl_plscmap0n(int ncol0);
extern pl_plscmap1(int *r,int *g,int *b,int ncol1);
extern pl_plscmap1l(int itype,int npts,float *intensity,float *coord1,float *coord2,float *coord3,int *rev);
extern pl_plscmap1n(int ncol1);
extern pl_plscol0(int icol0,int r,int g,int b);
extern pl_plscolbg(int r,int g,int b);
extern pl_plscolor(int color);
extern pl_plsdev(char *devname);
extern pl_plsdidev(float mar,float aspect,float jx,float jy);
extern pl_plsdimap(int dimxmin,int dimxmax,int dimymin,int dimymax,float dimxpmm,float dimypmm);
extern pl_plsdiori(float rot);
extern pl_plsdiplt(float xmin,float ymin,float xmax,float ymax);
extern pl_plsdiplz(float xmin,float ymin,float xmax,float ymax);
extern pl_plsesc(char esc);
extern pl_plsfam(int fam,int num,int bmax);
extern pl_plsfnam(char *fnam);
extern pl_plsmaj(float def,float scale);
extern pl_plsmin(float def,float scale);
extern pl_plsori(int ori);
extern pl_plspage(float xp,float yp,int xleng,int yleng,int xoff,int yoff);
extern pl_plspause(int pause);
extern pl_plsplw(char *title);
extern pl_plsstrm(int strm);
extern pl_plssub(int nx,int ny);
extern pl_plssym(float def,float scale);
extern pl_plstar(int nx,int ny);
extern pl_plstart(char *devname,int nx,int ny);
extern pl_plstyl(int nms,int *mark,int *space);
extern pl_plsvpa(float xmin,float xmax,float ymin,float ymax);
extern pl_plsxax(int digmax,int digits);
extern pl_plsyax(int digmax,int digits);
extern pl_plsym(int n,float *x,float *y,int code);
extern pl_plszax(int digmax,int digits);
extern pl_pltext();
extern pl_plvasp(float aspect);
extern pl_plvpas(float xmin,float xmax,float ymin,float ymax,float aspect);
extern pl_plvpor(float xmin,float xmax,float ymin,float ymax);
extern pl_plvsta();
extern pl_plw3d(float basex,float basey,float height,float xmin0,float xmax0,float ymin0,float ymax0,float zmin0,float zmax0,float alt,float az);
extern pl_plwid(int width);
extern pl_plwind(float xmin,float xmax,float ymin,float ymax);
extern Zoom(int flag,float x,float y, int bufnum);
extern Reset_Zoom(int bufnum);
