#ifndef BACKEND_SVG_H
#define BACKEND_SVG_H

#include "potracelib.h"
#include "main.h"

//vector<potrace_path_s*>& ,int added by xudi
int page_svg(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo, potrace_path_s **arrayAppend, int arrayLen);
int page_gimp(FILE *fout, potrace_path_t *plist, imginfo_t *imginfo, potrace_path_s **arrayAppend, int arrayLen);

#endif /* BACKEND_SVG_H */

