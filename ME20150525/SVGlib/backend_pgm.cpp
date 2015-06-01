/* Copyright (C) 2001-2013 Peter Selinger.
   This file is part of Potrace. It is free software and it is covered
   by the GNU General Public License. See the file COPYING for details. */


/* The PGM backend of Potrace. Here we custom-render a set of Bezier
   curves and output the result as a greymap. This is merely a
   convenience, as the same could be achieved by piping the EPS output
   through ghostscript. */

#include <math.h>

#include "backend_pgm.h"
#include "potracelib.h"
#include "lists.h"
#include "greymap.h"
#include "main.h"
#include "auxiliary.h"
#include "trans.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void pgm_path(potrace_curve_t *curve, trans_t t, render_t *rm) {
  dpoint_t *c, c1[3];
  int i;
  int m = curve->n;
  
  c = curve->c[m-1];
  c1[2] = trans(c[2], t);
  render_moveto(rm, c1[2].x, c1[2].y);
  
  for (i=0; i<m; i++) {
    c = curve->c[i];
    switch (curve->tag[i]) {
    case POTRACE_CORNER:
      c1[1] = trans(c[1], t);
      c1[2] = trans(c[2], t);
      render_lineto(rm, c1[1].x, c1[1].y);
      render_lineto(rm, c1[2].x, c1[2].y);
      break;
    case POTRACE_CURVETO:
      c1[0] = trans(c[0], t);
      c1[1] = trans(c[1], t);
      c1[2] = trans(c[2], t);
      render_curveto(rm, c1[0].x, c1[0].y, c1[1].x, c1[1].y, c1[2].x, c1[2].y);
      break;
    }
  }
}


