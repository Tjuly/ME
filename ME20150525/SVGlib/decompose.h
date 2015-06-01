#ifndef DECOMPOSE_H
#define DECOMPOSE_H

#include "potracelib.h"
#include "progress.h"
#include "curve.h"

int bm_to_pathlist(const potrace_bitmap_t *bm, path_t **plistp, const potrace_param_t *param, progress_t *progress);

//added by xudi
void pathlist_to_tree(path_t *plist, potrace_bitmap_t *bm);

#endif /* DECOMPOSE_H */

