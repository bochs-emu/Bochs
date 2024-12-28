/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 *  Portion of this software comes with the following license
 */

/***************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

Bit32u voodoo_last_msg = 255;


#define poly_wait(x,y)
#define cpu_eat_cycles(x,y)

#define DEBUG_DEPTH     (0)
#define DEBUG_LOD     (0)

#define LOG_VBLANK_SWAP   (0)
#define LOG_FIFO      (0)
#define LOG_FIFO_VERBOSE  (0)
#define LOG_REGISTERS   (0)
#define LOG_WAITS     (0)
#define LOG_LFB       (0)
#define LOG_TEXTURE_RAM   (0)
#define LOG_RASTERIZERS   (0)
#define LOG_CMDFIFO     (0)
#define LOG_CMDFIFO_VERBOSE (0)

#define MODIFY_PIXEL(VV)

/* fifo thread variable */
BX_THREAD_VAR(fifo_thread_var);
/* CMDFIFO thread mutex (Voodoo2) */
BX_MUTEX(cmdfifo_mutex);
/* render mutex (Banshee) */
BX_MUTEX(render_mutex);
/* FIFO event stuff */
BX_MUTEX(fifo_mutex);
bx_thread_sem_t fifo_wakeup;
bx_thread_sem_t fifo_not_full;
static bx_thread_sem_t vertical_sem;

/* fast dither lookup */
static Bit8u dither4_lookup[256*16*2];
static Bit8u dither2_lookup[256*16*2];

/* fast log2 lookup */
Bit8u voodoo_log[1 << LOG_LOOKUP_BITS];


void raster_function(int tmus, void *destbase, Bit32s y, const poly_extent *extent, const void *extradata, int threadid) {
	const poly_extra_data *extra = (const poly_extra_data *) extradata;
	voodoo_state *v = extra->state;
	stats_block *stats = &v->thread_stats[threadid];
	DECLARE_DITHER_POINTERS;
	Bit32s startx = extent->startx;
	Bit32s stopx = extent->stopx;
	Bit32s iterr, iterg, iterb, itera;
	Bit32s iterz;
	Bit64s iterw, iterw0 = 0, iterw1 = 0;
	Bit64s iters0 = 0, iters1 = 0;
	Bit64s itert0 = 0, itert1 = 0;
	Bit16u *depth;
	Bit16u *dest;
	Bit32s dx, dy;
	Bit32s scry;
	Bit32s x;

	Bit32u fbzcolorpath= v->reg[fbzColorPath].u;
	Bit32u fbzmode= v->reg[fbzMode].u;
	Bit32u alphamode= v->reg[alphaMode].u;
	Bit32u fogmode= v->reg[fogMode].u;
	Bit32u texmode0= (tmus==0? 0 : v->tmu[0].reg[textureMode].u);
	Bit32u texmode1= (tmus<=1? 0 : v->tmu[1].reg[textureMode].u);

	/* determine the screen Y */
	scry = y;
	if (FBZMODE_Y_ORIGIN(fbzmode))
		scry = (v->fbi.yorigin - y) & v->fbi.clip_mask;

	/* compute dithering */
	COMPUTE_DITHER_POINTERS(fbzmode, y);

	/* apply clipping */
	if (FBZMODE_ENABLE_CLIPPING(fbzmode)) {
		Bit32s tempclip;

		/* Y clipping buys us the whole scanline */
		if (scry < (Bit32s) ((v->reg[clipLowYHighY].u >> 16) & v->fbi.clip_mask)
				|| scry >= (Bit32s) (v->reg[clipLowYHighY].u & v->fbi.clip_mask)) {
			stats->pixels_in += stopx - startx;
			stats->clip_fail += stopx - startx;
			return;
		}

		/* X clipping */
		tempclip = (v->reg[clipLeftRight].u >> 16) & v->fbi.clip_mask;
		if (startx < tempclip) {
			stats->pixels_in += tempclip - startx;
			startx = tempclip;
		}
		tempclip = v->reg[clipLeftRight].u & v->fbi.clip_mask;
		if (stopx >= tempclip) {
			stats->pixels_in += stopx - tempclip;
			stopx = tempclip - 1;
		}
	}

	/* get pointers to the target buffer and depth buffer */
	dest = (Bit16u *) destbase + scry * v->fbi.rowpixels;
	depth =
			(v->fbi.auxoffs != (Bit32u) ~0) ?
					((Bit16u *) (v->fbi.ram + v->fbi.auxoffs)
							+ scry * v->fbi.rowpixels) :
					NULL;

	/* compute the starting parameters */
	dx = startx - (extra->ax >> 4);
	dy = y - (extra->ay >> 4);
	iterr = extra->startr + dy * extra->drdy + dx * extra->drdx;
	iterg = extra->startg + dy * extra->dgdy + dx * extra->dgdx;
	iterb = extra->startb + dy * extra->dbdy + dx * extra->dbdx;
	itera = extra->starta + dy * extra->dady + dx * extra->dadx;
	iterz = extra->startz + dy * extra->dzdy + dx * extra->dzdx;
	iterw = extra->startw + dy * extra->dwdy + dx * extra->dwdx;
	if (tmus >= 1) {
		iterw0 = extra->startw0 + dy * extra->dw0dy + dx * extra->dw0dx;
		iters0 = extra->starts0 + dy * extra->ds0dy + dx * extra->ds0dx;
		itert0 = extra->startt0 + dy * extra->dt0dy + dx * extra->dt0dx;
	}
	if (tmus >= 2) {
		iterw1 = extra->startw1 + dy * extra->dw1dy + dx * extra->dw1dx;
		iters1 = extra->starts1 + dy * extra->ds1dy + dx * extra->ds1dx;
		itert1 = extra->startt1 + dy * extra->dt1dy + dx * extra->dt1dx;
	}

	/* loop in X */
	for (x = startx; x < stopx; x++) {
		rgb_union iterargb = { 0 };
		rgb_union texel = { 0 };

		/* pixel pipeline part 1 handles depth testing and stippling */
		PIXEL_PIPELINE_BEGIN(v, stats, x, y, fbzcolorpath, fbzmode,
				iterz, iterw)
			;

			/* run the texture pipeline on TMU1 to produce a value in texel */
			/* note that they set LOD min to 8 to "disable" a TMU */
			if (tmus >= 2 && v->tmu[1].lodmin < (8 << 8))
				TEXTURE_PIPELINE(&v->tmu[1], x, dither4, texmode1, texel,
						v->tmu[1].lookup, extra->lodbase1, iters1, itert1,
						iterw1, texel);

			/* run the texture pipeline on TMU0 to produce a final */
			/* result in texel */
			/* note that they set LOD min to 8 to "disable" a TMU */
			if (tmus >= 1 && v->tmu[0].lodmin < (8 << 8)) {
				if (v->send_config == 0)
					TEXTURE_PIPELINE(&v->tmu[0], x, dither4, texmode0, texel,
							v->tmu[0].lookup, extra->lodbase0, iters0, itert0,
							iterw0, texel);
				/* send config data to the frame buffer */
				else
					texel.u = v->tmu_config;
			}
			/* colorpath pipeline selects source colors and does blending */
			CLAMPED_ARGB(iterr, iterg, iterb, itera, fbzcolorpath, iterargb);
			COLORPATH_PIPELINE(v, stats, fbzcolorpath, fbzmode, alphamode,
					texel, iterz, iterw, iterargb);

			/* pixel pipeline part 2 handles fog, alpha, and final output */
			PIXEL_PIPELINE_END(v, stats, dither, dither4, dither_lookup, x,
					dest, depth, fbzmode, fbzcolorpath, alphamode, fogmode,
					iterz, iterw, iterargb);

		/* update the iterated parameters */
		iterr += extra->drdx;
		iterg += extra->dgdx;
		iterb += extra->dbdx;
		itera += extra->dadx;
		iterz += extra->dzdx;
		iterw += extra->dwdx;
		if (tmus >= 1) {
			iterw0 += extra->dw0dx;
			iters0 += extra->ds0dx;
			itert0 += extra->dt0dx;
		}
		if (tmus >= 2) {
			iterw1 += extra->dw1dx;
			iters1 += extra->ds1dx;
			itert1 += extra->dt1dx;
		}
	}
}

/*************************************
 *
 *  NCC table management
 *
 *************************************/

void ncc_table_write(ncc_table *n, offs_t regnum, Bit32u data)
{
  /* I/Q entries reference the plaette if the high bit is set */
  if (regnum >= 4 && (data & 0x80000000) && n->palette)
  {
    int index = ((data >> 23) & 0xfe) | (regnum & 1);

    /* set the ARGB for this palette index */
    n->palette[index] = 0xff000000 | data;

    /* if we have an ARGB palette as well, compute its value */
    if (n->palettea)
    {
      int a = ((data >> 16) & 0xfc) | ((data >> 22) & 0x03);
      int r = ((data >> 10) & 0xfc) | ((data >> 16) & 0x03);
      int g = ((data >>  4) & 0xfc) | ((data >> 10) & 0x03);
      int b = ((data <<  2) & 0xfc) | ((data >>  4) & 0x03);
      n->palettea[index] = MAKE_ARGB(a, r, g, b);
    }

    /* this doesn't dirty the table or go to the registers, so bail */
    return;
  }

  /* if the register matches, don't update */
  if (data == n->reg[regnum].u)
    return;
  n->reg[regnum].u = data;

  /* first four entries are packed Y values */
  if (regnum < 4)
  {
    regnum *= 4;
    n->y[regnum+0] = (data >>  0) & 0xff;
    n->y[regnum+1] = (data >>  8) & 0xff;
    n->y[regnum+2] = (data >> 16) & 0xff;
    n->y[regnum+3] = (data >> 24) & 0xff;
  }

  /* the second four entries are the I RGB values */
  else if (regnum < 8)
  {
    regnum &= 3;
    n->ir[regnum] = (Bit32s)(data <<  5) >> 23;
    n->ig[regnum] = (Bit32s)(data << 14) >> 23;
    n->ib[regnum] = (Bit32s)(data << 23) >> 23;
  }

  /* the final four entries are the Q RGB values */
  else
  {
    regnum &= 3;
    n->qr[regnum] = (Bit32s)(data <<  5) >> 23;
    n->qg[regnum] = (Bit32s)(data << 14) >> 23;
    n->qb[regnum] = (Bit32s)(data << 23) >> 23;
  }

  /* mark the table dirty */
  n->dirty = 1;
}


void ncc_table_update(ncc_table *n)
{
  int r, g, b, i;

  /* generte all 256 possibilities */
  for (i = 0; i < 256; i++)
  {
    int vi = (i >> 2) & 0x03;
    int vq = (i >> 0) & 0x03;

    /* start with the intensity */
    r = g = b = n->y[(i >> 4) & 0x0f];

    /* add the coloring */
    r += n->ir[vi] + n->qr[vq];
    g += n->ig[vi] + n->qg[vq];
    b += n->ib[vi] + n->qb[vq];

    /* clamp */
    CLAMP(r, 0, 255);
    CLAMP(g, 0, 255);
    CLAMP(b, 0, 255);

    /* fill in the table */
    n->texel[i] = MAKE_ARGB(0xff, r, g, b);
  }

  /* no longer dirty */
  n->dirty = 0;
}

void recompute_texture_params(tmu_state *t)
{
  int bppscale;
  Bit32u base;
  int lod;
  static Bit32u count = 0;

  /* Unimplemented switch */
  if (TEXLOD_LOD_ZEROFRAC(t->reg[tLOD].u)) {
    if (count < 50) BX_ERROR(("TEXLOD_LOD_ZEROFRAC not implemented yet"));
    count++;
  }
  /* Banshee: unimplemented switches */
  if (TEXLOD_TMIRROR_S(t->reg[tLOD].u)) {
    BX_ERROR(("TEXLOD_TMIRROR_S not implemented yet"));
  }
  if (TEXLOD_TMIRROR_T(t->reg[tLOD].u)) {
    BX_ERROR(("TEXLOD_TMIRROR_T not implemented yet"));
  }
  /* extract LOD parameters */
  t->lodmin = TEXLOD_LODMIN(t->reg[tLOD].u) << 6;
  t->lodmax = TEXLOD_LODMAX(t->reg[tLOD].u) << 6;
  t->lodbias = (Bit8s)(TEXLOD_LODBIAS(t->reg[tLOD].u) << 2) << 4;

  /* determine which LODs are present */
  t->lodmask = 0x1ff;
  if (TEXLOD_LOD_TSPLIT(t->reg[tLOD].u))
  {
    if (!TEXLOD_LOD_ODD(t->reg[tLOD].u))
      t->lodmask = 0x155;
    else
      t->lodmask = 0x0aa;
  }

  /* determine base texture width/height */
  t->wmask = t->hmask = 0xff;
  if (TEXLOD_LOD_S_IS_WIDER(t->reg[tLOD].u))
    t->hmask >>= TEXLOD_LOD_ASPECT(t->reg[tLOD].u);
  else
    t->wmask >>= TEXLOD_LOD_ASPECT(t->reg[tLOD].u);

  /* determine the bpp of the texture */
  bppscale = TEXMODE_FORMAT(t->reg[textureMode].u) >> 3;

  /* start with the base of LOD 0 */
  if (t->texaddr_shift == 0 && (t->reg[texBaseAddr].u & 1))
    BX_DEBUG(("Tiled texture"));
  base = (t->reg[texBaseAddr].u & t->texaddr_mask) << t->texaddr_shift;
  t->lodoffset[0] = base & t->mask;

  /* LODs 1-3 are different depending on whether we are in multitex mode */
  /* Several Voodoo 2 games leave the upper bits of TLOD == 0xff, meaning we think */
  /* they want multitex mode when they really don't -- disable for now */
  if (TEXLOD_TMULTIBASEADDR(t->reg[tLOD].u)) {
    BX_ERROR(("TEXLOD_TMULTIBASEADDR disabled for now"));
  }
  if (0)//TEXLOD_TMULTIBASEADDR(t->reg[tLOD].u))
  {
    base = (t->reg[texBaseAddr_1].u & t->texaddr_mask) << t->texaddr_shift;
    t->lodoffset[1] = base & t->mask;
    base = (t->reg[texBaseAddr_2].u & t->texaddr_mask) << t->texaddr_shift;
    t->lodoffset[2] = base & t->mask;
    base = (t->reg[texBaseAddr_3_8].u & t->texaddr_mask) << t->texaddr_shift;
    t->lodoffset[3] = base & t->mask;
  }
  else
  {
    if (t->lodmask & (1 << 0))
      base += (((t->wmask >> 0) + 1) * ((t->hmask >> 0) + 1)) << bppscale;
    t->lodoffset[1] = base & t->mask;
    if (t->lodmask & (1 << 1))
      base += (((t->wmask >> 1) + 1) * ((t->hmask >> 1) + 1)) << bppscale;
    t->lodoffset[2] = base & t->mask;
    if (t->lodmask & (1 << 2))
      base += (((t->wmask >> 2) + 1) * ((t->hmask >> 2) + 1)) << bppscale;
    t->lodoffset[3] = base & t->mask;
  }

  /* remaining LODs make sense */
  for (lod = 4; lod <= 8; lod++)
  {
    if (t->lodmask & (1 << (lod - 1)))
    {
      Bit32u size = ((t->wmask >> (lod - 1)) + 1) * ((t->hmask >> (lod - 1)) + 1);
      if (size < 4) size = 4;
      base += size << bppscale;
    }
    t->lodoffset[lod] = base & t->mask;
  }

  /* set the NCC lookup appropriately */
  t->texel[1] = t->texel[9] = t->ncc[TEXMODE_NCC_TABLE_SELECT(t->reg[textureMode].u)].texel;

  /* pick the lookup table */
  t->lookup = t->texel[TEXMODE_FORMAT(t->reg[textureMode].u)];

  /* compute the detail parameters */
  t->detailmax = TEXDETAIL_DETAIL_MAX(t->reg[tDetail].u);
  t->detailbias = (Bit8s)(TEXDETAIL_DETAIL_BIAS(t->reg[tDetail].u) << 2) << 6;
  t->detailscale = TEXDETAIL_DETAIL_SCALE(t->reg[tDetail].u);

  /* no longer dirty */
  t->regdirty = 0;

  /* check for separate RGBA filtering */
  if (TEXDETAIL_SEPARATE_RGBA_FILTER(t->reg[tDetail].u))
    BX_PANIC(("Separate RGBA filters!"));
}

BX_CPP_INLINE Bit32s prepare_tmu(tmu_state *t)
{
  /* if the texture parameters are dirty, update them */
  if (t->regdirty) {
    recompute_texture_params(t);

    /* ensure that the NCC tables are up to date */
    if ((TEXMODE_FORMAT(t->reg[textureMode].u) & 7) == 1)
    {
      ncc_table *n = &t->ncc[TEXMODE_NCC_TABLE_SELECT(t->reg[textureMode].u)];
      t->texel[1] = t->texel[9] = n->texel;
      if (n->dirty)
        ncc_table_update(n);
    }
  }

  // compute (ds^2 + dt^2) in both X and Y; note that these values are
  // each .32, so the square is a .64 fixed point value
  double fdsdx = (double)t->dsdx;
  double fdsdy = (double)t->dsdy;
  double fdtdx = (double)t->dtdx;
  double fdtdy = (double)t->dtdy;
  double texdx = fdsdx * fdsdx + fdtdx * fdtdx;
  double texdy = fdsdy * fdsdy + fdtdy * fdtdy;

  // pick whichever is larger
  double maxval = BX_MAX(texdx, texdy);

  // use our fast reciprocal/log on this value; 64 to indicate how many
  // bits of fractional resolution in the source, and divide by 2 because
  // we really want the log of the square root
  return fast_log2(maxval, 64) / 2;
}


BX_CPP_INLINE Bit32s round_coordinate(float value)
{
  Bit32s result = (Bit32s)floor(value);
  return result + (value - (float)result > 0.5f);
}

Bit32u poly_render_triangle(void *dest, const rectangle *cliprect, int texcount, int paramcount, const poly_vertex *v1, const poly_vertex *v2, const poly_vertex *v3, poly_extra_data *extra)
{
  float dxdy_v1v2, dxdy_v1v3, dxdy_v2v3;
  const poly_vertex *tv;
  Bit32s curscan, scaninc=1;

  Bit32s v1yclip, v3yclip;
  Bit32s v1y, v3y;
  Bit32s pixels = 0;

  /* first sort by Y */
  if (v2->y < v1->y)
  {
    tv = v1;
    v1 = v2;
    v2 = tv;
  }
  if (v3->y < v2->y)
  {
    tv = v2;
    v2 = v3;
    v3 = tv;
    if (v2->y < v1->y)
    {
      tv = v1;
      v1 = v2;
      v2 = tv;
    }
  }

  /* compute some integral X/Y vertex values */
  v1y = round_coordinate(v1->y);
  v3y = round_coordinate(v3->y);

  /* clip coordinates */
  v1yclip = v1y;
  v3yclip = v3y;
  if (cliprect != NULL)
  {
    v1yclip = MAX(v1yclip, cliprect->min_y);
    v3yclip = MIN(v3yclip, cliprect->max_y + 1);
  }
  if (v3yclip - v1yclip <= 0)
    return 0;

  /* compute the slopes for each portion of the triangle */
  dxdy_v1v2 = (v2->y == v1->y) ? 0.0f : (v2->x - v1->x) / (v2->y - v1->y);
  dxdy_v1v3 = (v3->y == v1->y) ? 0.0f : (v3->x - v1->x) / (v3->y - v1->y);
  dxdy_v2v3 = (v3->y == v2->y) ? 0.0f : (v3->x - v2->x) / (v3->y - v2->y);

  /* compute the X extents for each scanline */
  poly_extent extent;
  int extnum=0;
  for (curscan = v1yclip; curscan < v3yclip; curscan += scaninc)
  {
    {
      float fully = (float)(curscan + extnum) + 0.5f;
      float startx = v1->x + (fully - v1->y) * dxdy_v1v3;
      float stopx;
      Bit32s istartx, istopx;

      /* compute the ending X based on which part of the triangle we're in */
      if (fully < v2->y)
        stopx = v1->x + (fully - v1->y) * dxdy_v1v2;
      else
        stopx = v2->x + (fully - v2->y) * dxdy_v2v3;

      /* clamp to full pixels */
      istartx = round_coordinate(startx);
      istopx = round_coordinate(stopx);

      /* force start < stop */
      if (istartx > istopx)
      {
        Bit32s temp = istartx;
        istartx = istopx;
        istopx = temp;
      }

      /* apply left/right clipping */
      if (cliprect != NULL)
      {
        if (istartx < cliprect->min_x)
          istartx = cliprect->min_x;
        if (istopx > cliprect->max_x)
          istopx = cliprect->max_x + 1;
      }

      /* set the extent and update the total pixel count */
      if (istartx >= istopx)
        istartx = istopx = 0;
      extent.startx = istartx;
      extent.stopx = istopx;
      raster_function(texcount,dest,curscan,&extent,extra,0);

      pixels += istopx - istartx;
    }
  }

  return pixels;
}

Bit32s triangle_create_work_item(Bit16u *drawbuf, int texcount)
{
  poly_extra_data extra;
  poly_vertex vert[3];
  Bit32u retval;

  /* fill in the vertex data */
  vert[0].x = (float)v->fbi.ax * (1.0f / 16.0f);
  vert[0].y = (float)v->fbi.ay * (1.0f / 16.0f);
  vert[1].x = (float)v->fbi.bx * (1.0f / 16.0f);
  vert[1].y = (float)v->fbi.by * (1.0f / 16.0f);
  vert[2].x = (float)v->fbi.cx * (1.0f / 16.0f);
  vert[2].y = (float)v->fbi.cy * (1.0f / 16.0f);

  /* fill in the extra data */
  extra.state = v;

  /* fill in triangle parameters */
  extra.ax = v->fbi.ax;
  extra.ay = v->fbi.ay;
  extra.startr = v->fbi.startr;
  extra.startg = v->fbi.startg;
  extra.startb = v->fbi.startb;
  extra.starta = v->fbi.starta;
  extra.startz = v->fbi.startz;
  extra.startw = v->fbi.startw;
  extra.drdx = v->fbi.drdx;
  extra.dgdx = v->fbi.dgdx;
  extra.dbdx = v->fbi.dbdx;
  extra.dadx = v->fbi.dadx;
  extra.dzdx = v->fbi.dzdx;
  extra.dwdx = v->fbi.dwdx;
  extra.drdy = v->fbi.drdy;
  extra.dgdy = v->fbi.dgdy;
  extra.dbdy = v->fbi.dbdy;
  extra.dady = v->fbi.dady;
  extra.dzdy = v->fbi.dzdy;
  extra.dwdy = v->fbi.dwdy;

  /* fill in texture 0 parameters */
  if (texcount > 0)
  {
    extra.starts0 = v->tmu[0].starts;
    extra.startt0 = v->tmu[0].startt;
    extra.startw0 = v->tmu[0].startw;
    extra.ds0dx = v->tmu[0].dsdx;
    extra.dt0dx = v->tmu[0].dtdx;
    extra.dw0dx = v->tmu[0].dwdx;
    extra.ds0dy = v->tmu[0].dsdy;
    extra.dt0dy = v->tmu[0].dtdy;
    extra.dw0dy = v->tmu[0].dwdy;
    extra.lodbase0 = prepare_tmu(&v->tmu[0]);

    /* fill in texture 1 parameters */
    if (texcount > 1)
    {
      extra.starts1 = v->tmu[1].starts;
      extra.startt1 = v->tmu[1].startt;
      extra.startw1 = v->tmu[1].startw;
      extra.ds1dx = v->tmu[1].dsdx;
      extra.dt1dx = v->tmu[1].dtdx;
      extra.dw1dx = v->tmu[1].dwdx;
      extra.ds1dy = v->tmu[1].dsdy;
      extra.dt1dy = v->tmu[1].dtdy;
      extra.dw1dy = v->tmu[1].dwdy;
      extra.lodbase1 = prepare_tmu(&v->tmu[1]);
    }
  }

  /* farm the rasterization out to other threads */
  retval = poly_render_triangle(drawbuf, NULL, texcount, 0, &vert[0], &vert[1], &vert[2], &extra);

  return retval;
}


Bit32s triangle()
{
  int texcount = 0;
  Bit16u *drawbuf;
  int destbuf;
  int pixels;

  /* determine the number of TMUs involved */
  texcount = 0;
  if (!FBIINIT3_DISABLE_TMUS(v->reg[fbiInit3].u) && FBZCP_TEXTURE_ENABLE(v->reg[fbzColorPath].u))
  {
    texcount = 1;
    if (v->chipmask & 0x04)
      texcount = 2;
  }

  /* perform subpixel adjustments */
  if (FBZCP_CCA_SUBPIXEL_ADJUST(v->reg[fbzColorPath].u))
  {
    Bit32s dx = 8 - (v->fbi.ax & 15);
    Bit32s dy = 8 - (v->fbi.ay & 15);

    /* adjust iterated R,G,B,A and W/Z */
    v->fbi.startr += (dy * v->fbi.drdy + dx * v->fbi.drdx) >> 4;
    v->fbi.startg += (dy * v->fbi.dgdy + dx * v->fbi.dgdx) >> 4;
    v->fbi.startb += (dy * v->fbi.dbdy + dx * v->fbi.dbdx) >> 4;
    v->fbi.starta += (dy * v->fbi.dady + dx * v->fbi.dadx) >> 4;
    v->fbi.startw += (dy * v->fbi.dwdy + dx * v->fbi.dwdx) >> 4;
    v->fbi.startz += mul_32x32_shift(dy, v->fbi.dzdy, 4) + mul_32x32_shift(dx, v->fbi.dzdx, 4);

    /* adjust iterated W/S/T for TMU 0 */
    if (texcount >= 1)
    {
      v->tmu[0].startw += (dy * v->tmu[0].dwdy + dx * v->tmu[0].dwdx) >> 4;
      v->tmu[0].starts += (dy * v->tmu[0].dsdy + dx * v->tmu[0].dsdx) >> 4;
      v->tmu[0].startt += (dy * v->tmu[0].dtdy + dx * v->tmu[0].dtdx) >> 4;

      /* adjust iterated W/S/T for TMU 1 */
      if (texcount >= 2)
      {
        v->tmu[1].startw += (dy * v->tmu[1].dwdy + dx * v->tmu[1].dwdx) >> 4;
        v->tmu[1].starts += (dy * v->tmu[1].dsdy + dx * v->tmu[1].dsdx) >> 4;
        v->tmu[1].startt += (dy * v->tmu[1].dtdy + dx * v->tmu[1].dtdx) >> 4;
      }
    }
  }

  /* determine the draw buffer */
  destbuf = (v->type >= VOODOO_BANSHEE) ? 1 : FBZMODE_DRAW_BUFFER(v->reg[fbzMode].u);
  switch (destbuf)
  {
    case 0:   /* front buffer */
      drawbuf = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.frontbuf]);
      v->fbi.video_changed = 1;
      break;

    case 1:   /* back buffer */
      drawbuf = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.backbuf]);
      if (v->fbi.rgboffs[v->fbi.frontbuf] == v->fbi.rgboffs[v->fbi.backbuf])
        v->fbi.video_changed = 1;
      break;

    default:  /* reserved */
      return TRIANGLE_SETUP_CLOCKS;
  }

  /* find a rasterizer that matches our current state */
  pixels = triangle_create_work_item(/*v, */drawbuf, texcount);

  /* update stats */
  v->reg[fbiTrianglesOut].u++;

  /* 1 pixel per clock, plus some setup time */
  if (LOG_REGISTERS) BX_DEBUG(("cycles = %d", TRIANGLE_SETUP_CLOCKS + pixels));
  return TRIANGLE_SETUP_CLOCKS + pixels;
}


static Bit32s setup_and_draw_triangle()
{
  float dx1, dy1, dx2, dy2;
  float divisor, tdiv;

  /* grab the X/Ys at least */
  v->fbi.ax = (Bit16s)(v->fbi.svert[0].x * 16.0);
  v->fbi.ay = (Bit16s)(v->fbi.svert[0].y * 16.0);
  v->fbi.bx = (Bit16s)(v->fbi.svert[1].x * 16.0);
  v->fbi.by = (Bit16s)(v->fbi.svert[1].y * 16.0);
  v->fbi.cx = (Bit16s)(v->fbi.svert[2].x * 16.0);
  v->fbi.cy = (Bit16s)(v->fbi.svert[2].y * 16.0);

  /* compute the divisor */
  divisor = 1.0f / ((v->fbi.svert[0].x - v->fbi.svert[1].x) * (v->fbi.svert[0].y - v->fbi.svert[2].y) -
            (v->fbi.svert[0].x - v->fbi.svert[2].x) * (v->fbi.svert[0].y - v->fbi.svert[1].y));

  /* backface culling */
  if (v->reg[sSetupMode].u & 0x20000)
  {
    int culling_sign = (v->reg[sSetupMode].u >> 18) & 1;
    int divisor_sign = (divisor < 0);

    /* if doing strips and ping pong is enabled, apply the ping pong */
    if ((v->reg[sSetupMode].u & 0x90000) == 0x00000)
      culling_sign ^= (v->fbi.sverts - 3) & 1;

    /* if our sign matches the culling sign, we're done for */
    if (divisor_sign == culling_sign)
      return TRIANGLE_SETUP_CLOCKS;
  }

  /* compute the dx/dy values */
  dx1 = v->fbi.svert[0].y - v->fbi.svert[2].y;
  dx2 = v->fbi.svert[0].y - v->fbi.svert[1].y;
  dy1 = v->fbi.svert[0].x - v->fbi.svert[1].x;
  dy2 = v->fbi.svert[0].x - v->fbi.svert[2].x;

  /* set up R,G,B */
  tdiv = divisor * 4096.0f;
  if (v->reg[sSetupMode].u & (1 << 0))
  {
    v->fbi.startr = (Bit32s)(v->fbi.svert[0].r * 4096.0f);
    v->fbi.drdx = (Bit32s)(((v->fbi.svert[0].r - v->fbi.svert[1].r) * dx1 - (v->fbi.svert[0].r - v->fbi.svert[2].r) * dx2) * tdiv);
    v->fbi.drdy = (Bit32s)(((v->fbi.svert[0].r - v->fbi.svert[2].r) * dy1 - (v->fbi.svert[0].r - v->fbi.svert[1].r) * dy2) * tdiv);
    v->fbi.startg = (Bit32s)(v->fbi.svert[0].g * 4096.0f);
    v->fbi.dgdx = (Bit32s)(((v->fbi.svert[0].g - v->fbi.svert[1].g) * dx1 - (v->fbi.svert[0].g - v->fbi.svert[2].g) * dx2) * tdiv);
    v->fbi.dgdy = (Bit32s)(((v->fbi.svert[0].g - v->fbi.svert[2].g) * dy1 - (v->fbi.svert[0].g - v->fbi.svert[1].g) * dy2) * tdiv);
    v->fbi.startb = (Bit32s)(v->fbi.svert[0].b * 4096.0f);
    v->fbi.dbdx = (Bit32s)(((v->fbi.svert[0].b - v->fbi.svert[1].b) * dx1 - (v->fbi.svert[0].b - v->fbi.svert[2].b) * dx2) * tdiv);
    v->fbi.dbdy = (Bit32s)(((v->fbi.svert[0].b - v->fbi.svert[2].b) * dy1 - (v->fbi.svert[0].b - v->fbi.svert[1].b) * dy2) * tdiv);
  }

  /* set up alpha */
  if (v->reg[sSetupMode].u & (1 << 1))
  {
    v->fbi.starta = (Bit32s)(v->fbi.svert[0].a * 4096.0);
    v->fbi.dadx = (Bit32s)(((v->fbi.svert[0].a - v->fbi.svert[1].a) * dx1 - (v->fbi.svert[0].a - v->fbi.svert[2].a) * dx2) * tdiv);
    v->fbi.dady = (Bit32s)(((v->fbi.svert[0].a - v->fbi.svert[2].a) * dy1 - (v->fbi.svert[0].a - v->fbi.svert[1].a) * dy2) * tdiv);
  }

  /* set up Z */
  if (v->reg[sSetupMode].u & (1 << 2))
  {
    v->fbi.startz = (Bit32s)(v->fbi.svert[0].z * 4096.0);
    v->fbi.dzdx = (Bit32s)(((v->fbi.svert[0].z - v->fbi.svert[1].z) * dx1 - (v->fbi.svert[0].z - v->fbi.svert[2].z) * dx2) * tdiv);
    v->fbi.dzdy = (Bit32s)(((v->fbi.svert[0].z - v->fbi.svert[2].z) * dy1 - (v->fbi.svert[0].z - v->fbi.svert[1].z) * dy2) * tdiv);
  }

  /* set up Wb */
  tdiv = divisor * 65536.0f * 65536.0f;
  if (v->reg[sSetupMode].u & (1 << 3))
  {
    v->fbi.startw = v->tmu[0].startw = v->tmu[1].startw = (Bit64s)(v->fbi.svert[0].wb * 65536.0f * 65536.0f);
    v->fbi.dwdx = v->tmu[0].dwdx = v->tmu[1].dwdx = (Bit64s)(((v->fbi.svert[0].wb - v->fbi.svert[1].wb) * dx1 - (v->fbi.svert[0].wb - v->fbi.svert[2].wb) * dx2) * tdiv);
    v->fbi.dwdy = v->tmu[0].dwdy = v->tmu[1].dwdy = (Bit64s)(((v->fbi.svert[0].wb - v->fbi.svert[2].wb) * dy1 - (v->fbi.svert[0].wb - v->fbi.svert[1].wb) * dy2) * tdiv);
  }

  /* set up W0 */
  if (v->reg[sSetupMode].u & (1 << 4))
  {
    v->tmu[0].startw = v->tmu[1].startw = (Bit64s)(v->fbi.svert[0].w0 * 65536.0f * 65536.0f);
    v->tmu[0].dwdx = v->tmu[1].dwdx = (Bit64s)(((v->fbi.svert[0].w0 - v->fbi.svert[1].w0) * dx1 - (v->fbi.svert[0].w0 - v->fbi.svert[2].w0) * dx2) * tdiv);
    v->tmu[0].dwdy = v->tmu[1].dwdy = (Bit64s)(((v->fbi.svert[0].w0 - v->fbi.svert[2].w0) * dy1 - (v->fbi.svert[0].w0 - v->fbi.svert[1].w0) * dy2) * tdiv);
  }

  /* set up S0,T0 */
  if (v->reg[sSetupMode].u & (1 << 5))
  {
    v->tmu[0].starts = v->tmu[1].starts = (Bit64s)(v->fbi.svert[0].s0 * 65536.0f * 65536.0f);
    v->tmu[0].dsdx = v->tmu[1].dsdx = (Bit64s)(((v->fbi.svert[0].s0 - v->fbi.svert[1].s0) * dx1 - (v->fbi.svert[0].s0 - v->fbi.svert[2].s0) * dx2) * tdiv);
    v->tmu[0].dsdy = v->tmu[1].dsdy = (Bit64s)(((v->fbi.svert[0].s0 - v->fbi.svert[2].s0) * dy1 - (v->fbi.svert[0].s0 - v->fbi.svert[1].s0) * dy2) * tdiv);
    v->tmu[0].startt = v->tmu[1].startt = (Bit64s)(v->fbi.svert[0].t0 * 65536.0f * 65536.0f);
    v->tmu[0].dtdx = v->tmu[1].dtdx = (Bit64s)(((v->fbi.svert[0].t0 - v->fbi.svert[1].t0) * dx1 - (v->fbi.svert[0].t0 - v->fbi.svert[2].t0) * dx2) * tdiv);
    v->tmu[0].dtdy = v->tmu[1].dtdy = (Bit64s)(((v->fbi.svert[0].t0 - v->fbi.svert[2].t0) * dy1 - (v->fbi.svert[0].t0 - v->fbi.svert[1].t0) * dy2) * tdiv);
  }

  /* set up W1 */
  if (v->reg[sSetupMode].u & (1 << 6))
  {
    v->tmu[1].startw = (Bit64s)(v->fbi.svert[0].w1 * 65536.0f * 65536.0f);
    v->tmu[1].dwdx = (Bit64s)(((v->fbi.svert[0].w1 - v->fbi.svert[1].w1) * dx1 - (v->fbi.svert[0].w1 - v->fbi.svert[2].w1) * dx2) * tdiv);
    v->tmu[1].dwdy = (Bit64s)(((v->fbi.svert[0].w1 - v->fbi.svert[2].w1) * dy1 - (v->fbi.svert[0].w1 - v->fbi.svert[1].w1) * dy2) * tdiv);
  }

  /* set up S1,T1 */
  if (v->reg[sSetupMode].u & (1 << 7))
  {
    v->tmu[1].starts = (Bit64s)(v->fbi.svert[0].s1 * 65536.0f * 65536.0f);
    v->tmu[1].dsdx = (Bit64s)(((v->fbi.svert[0].s1 - v->fbi.svert[1].s1) * dx1 - (v->fbi.svert[0].s1 - v->fbi.svert[2].s1) * dx2) * tdiv);
    v->tmu[1].dsdy = (Bit64s)(((v->fbi.svert[0].s1 - v->fbi.svert[2].s1) * dy1 - (v->fbi.svert[0].s1 - v->fbi.svert[1].s1) * dy2) * tdiv);
    v->tmu[1].startt = (Bit64s)(v->fbi.svert[0].t1 * 65536.0f * 65536.0f);
    v->tmu[1].dtdx = (Bit64s)(((v->fbi.svert[0].t1 - v->fbi.svert[1].t1) * dx1 - (v->fbi.svert[0].t1 - v->fbi.svert[2].t1) * dx2) * tdiv);
    v->tmu[1].dtdy = (Bit64s)(((v->fbi.svert[0].t1 - v->fbi.svert[2].t1) * dy1 - (v->fbi.svert[0].t1 - v->fbi.svert[1].t1) * dy2) * tdiv);
  }

  /* draw the triangle */
  v->fbi.cheating_allowed = 1;
  return triangle();
}


static Bit32s begin_triangle()
{
  setup_vertex *sv = &v->fbi.svert[2];

  /* extract all the data from registers */
  sv->x = v->reg[sVx].f;
  sv->y = v->reg[sVy].f;
  sv->wb = v->reg[sWb].f;
  sv->w0 = v->reg[sWtmu0].f;
  sv->s0 = v->reg[sS_W0].f;
  sv->t0 = v->reg[sT_W0].f;
  sv->w1 = v->reg[sWtmu1].f;
  sv->s1 = v->reg[sS_Wtmu1].f;
  sv->t1 = v->reg[sT_Wtmu1].f;
  sv->a = v->reg[sAlpha].f;
  sv->r = v->reg[sRed].f;
  sv->g = v->reg[sGreen].f;
  sv->b = v->reg[sBlue].f;

  /* spread it across all three verts and reset the count */
  v->fbi.svert[0] = v->fbi.svert[1] = v->fbi.svert[2];
  v->fbi.sverts = 1;

  return 0;
}


static Bit32s draw_triangle()
{
  setup_vertex *sv = &v->fbi.svert[2];
  int cycles = 0;

  /* for strip mode, shuffle vertex 1 down to 0 */
  if (!(v->reg[sSetupMode].u & (1 << 16)))
    v->fbi.svert[0] = v->fbi.svert[1];

  /* copy 2 down to 1 regardless */
  v->fbi.svert[1] = v->fbi.svert[2];

  /* extract all the data from registers */
  sv->x = v->reg[sVx].f;
  sv->y = v->reg[sVy].f;
  sv->wb = v->reg[sWb].f;
  sv->w0 = v->reg[sWtmu0].f;
  sv->s0 = v->reg[sS_W0].f;
  sv->t0 = v->reg[sT_W0].f;
  sv->w1 = v->reg[sWtmu1].f;
  sv->s1 = v->reg[sS_Wtmu1].f;
  sv->t1 = v->reg[sT_Wtmu1].f;
  sv->a = v->reg[sAlpha].f;
  sv->r = v->reg[sRed].f;
  sv->g = v->reg[sGreen].f;
  sv->b = v->reg[sBlue].f;

  /* if we have enough verts, go ahead and draw */
  if (++v->fbi.sverts >= 3)
    cycles = setup_and_draw_triangle();

  return cycles;
}


static void raster_fastfill(void *destbase, Bit32s y, const poly_extent *extent, const void *extradata, int threadid)
{
  const poly_extra_data *extra = (const poly_extra_data *)extradata;
  voodoo_state *v = extra->state;
  stats_block *stats = &v->thread_stats[threadid];
  Bit32s startx = extent->startx;
  Bit32s stopx = extent->stopx;
  int scry, x;

  /* determine the screen Y */
  scry = y;
  if (FBZMODE_Y_ORIGIN(v->reg[fbzMode].u))
    scry = (v->fbi.yorigin - y) & v->fbi.clip_mask;

  /* fill this RGB row */
  if (FBZMODE_RGB_BUFFER_MASK(v->reg[fbzMode].u))
  {
    const Bit16u *ditherow = &extra->dither[(y & 3) * 4];
    Bit64u expanded = *(Bit64u *)ditherow;
    Bit16u *dest = (Bit16u *)destbase + scry * v->fbi.rowpixels;

    for (x = startx; x < stopx && (x & 3) != 0; x++)
      dest[x] = ditherow[x & 3];
    for ( ; x < (stopx & ~3); x += 4)
      *(Bit64u *)&dest[x] = expanded;
    for ( ; x < stopx; x++)
      dest[x] = ditherow[x & 3];
    stats->pixels_out += stopx - startx;
  }

  /* fill this dest buffer row */
  if (FBZMODE_AUX_BUFFER_MASK(v->reg[fbzMode].u) && v->fbi.auxoffs != (Bit32u)~0)
  {
    Bit16u color = v->reg[zaColor].u;
    Bit64u expanded = ((Bit64u)color << 48) | ((Bit64u)color << 32) | (color << 16) | color;
    Bit16u *dest = (Bit16u *)(v->fbi.ram + v->fbi.auxoffs) + scry * v->fbi.rowpixels;

    for (x = startx; x < stopx && (x & 3) != 0; x++)
      dest[x] = color;
    for ( ; x < (stopx & ~3); x += 4)
      *(Bit64u *)&dest[x] = expanded;
    for ( ; x < stopx; x++)
      dest[x] = color;
  }
}


Bit32u poly_render_triangle_custom(void *dest, const rectangle *cliprect, int startscanline, int numscanlines, const poly_extent *extents, poly_extra_data *extra)
{
  Bit32s curscan, scaninc;
  Bit32s v1yclip, v3yclip;
  Bit32s pixels = 0;

  /* clip coordinates */
  if (cliprect != NULL)
  {
    v1yclip = MAX(startscanline, cliprect->min_y);
    v3yclip = MIN(startscanline + numscanlines, cliprect->max_y + 1);
  }
  else
  {
    v1yclip = startscanline;
    v3yclip = startscanline + numscanlines;
  }
  if (v3yclip - v1yclip <= 0)
    return 0;

  /* compute the X extents for each scanline */
  for (curscan = v1yclip; curscan < v3yclip; curscan += scaninc)
  {
    int extnum=0;

    /* determine how much to advance to hit the next bucket */
    scaninc = 1;

    /* iterate over extents */
    {
      const poly_extent *extent = &extents[(curscan + extnum) - startscanline];
      Bit32s istartx = extent->startx, istopx = extent->stopx;

      /* force start < stop */
      if (istartx > istopx)
      {
        Bit32s temp = istartx;
        istartx = istopx;
        istopx = temp;
      }

      /* apply left/right clipping */
      if (cliprect != NULL)
      {
        if (istartx < cliprect->min_x)
          istartx = cliprect->min_x;
        if (istopx > cliprect->max_x)
          istopx = cliprect->max_x + 1;
      }

      /* set the extent and update the total pixel count */
      raster_fastfill(dest,curscan,extent,extra,0);
      if (istartx < istopx)
        pixels += istopx - istartx;
    }
  }
#if KEEP_STATISTICS
  poly->unit_max = MAX(poly->unit_max, poly->unit_next);
#endif

  return pixels;
}

Bit32s fastfill(voodoo_state *v)
{
  int sx = (v->reg[clipLeftRight].u >> 16) & v->fbi.clip_mask;
  int ex = (v->reg[clipLeftRight].u >> 0) & v->fbi.clip_mask;
  int sy = (v->reg[clipLowYHighY].u >> 16) & v->fbi.clip_mask;
  int ey = (v->reg[clipLowYHighY].u >> 0) & v->fbi.clip_mask;
  poly_extent extents[64];
  Bit16u dithermatrix[16];
  Bit16u *drawbuf = NULL;
  Bit32u pixels = 0;
  int extnum, x, y;

  /* if we're not clearing either, take no time */
  if (!FBZMODE_RGB_BUFFER_MASK(v->reg[fbzMode].u) && !FBZMODE_AUX_BUFFER_MASK(v->reg[fbzMode].u))
    return 0;

  /* are we clearing the RGB buffer? */
  if (FBZMODE_RGB_BUFFER_MASK(v->reg[fbzMode].u))
  {
    /* determine the draw buffer */
    int destbuf = (v->type >= VOODOO_BANSHEE) ? 1 : FBZMODE_DRAW_BUFFER(v->reg[fbzMode].u);
    switch (destbuf)
    {
      case 0:   /* front buffer */
        drawbuf = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.frontbuf]);
        break;

      case 1:   /* back buffer */
        drawbuf = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.backbuf]);
        break;

      default:  /* reserved */
        break;
    }

    /* determine the dither pattern */
    for (y = 0; y < 4; y++)
    {
      DECLARE_DITHER_POINTERS;
      UNUSED(dither);
      COMPUTE_DITHER_POINTERS(v->reg[fbzMode].u, y);
      for (x = 0; x < 4; x++)
      {
        int r = v->reg[color1].rgb.r;
        int g = v->reg[color1].rgb.g;
        int b = v->reg[color1].rgb.b;

        APPLY_DITHER(v->reg[fbzMode].u, x, dither_lookup, r, g, b);
        dithermatrix[y*4 + x] = (r << 11) | (g << 5) | b;
      }
    }
  }

  /* fill in a block of extents */
  extents[0].startx = sx;
  extents[0].stopx = ex;
  for (extnum = 1; extnum < (int)ARRAY_LENGTH(extents); extnum++)
    extents[extnum] = extents[0];

  poly_extra_data extra;
  /* iterate over blocks of extents */
  for (y = sy; y < ey; y += ARRAY_LENGTH(extents))
  {
    int count = MIN(ey - y, (int) ARRAY_LENGTH(extents));

    extra.state = v;
    memcpy(extra.dither, dithermatrix, sizeof(extra.dither));

    pixels += poly_render_triangle_custom(drawbuf, NULL, y, count, extents, &extra);
  }

  /* 2 pixels per clock */
  return pixels / 2;
}

void swap_buffers(voodoo_state *v)
{
  int count;

  /* force a partial update */
  v->fbi.video_changed = 1;

  /* keep a history of swap intervals */
  count = v->fbi.vblank_count;
  if (count > 15)
    count = 15;
  v->reg[fbiSwapHistory].u = (v->reg[fbiSwapHistory].u << 4) | count;

  /* rotate the buffers */
  if (v->type <= VOODOO_2)
  {
    if (v->type < VOODOO_2 || !v->fbi.vblank_dont_swap)
    {
      if (v->fbi.rgboffs[2] == (Bit32u)~0)
      {
        v->fbi.frontbuf = 1 - v->fbi.frontbuf;
        v->fbi.backbuf = 1 - v->fbi.frontbuf;
      }
      else
      {
        v->fbi.frontbuf = (v->fbi.frontbuf + 1) % 3;
        v->fbi.backbuf = (v->fbi.frontbuf + 1) % 3;
      }
    }
  }
  else {
    BX_LOCK(render_mutex);
    v->fbi.rgboffs[0] = v->reg[leftOverlayBuf].u & v->fbi.mask & ~0x0f;
    BX_UNLOCK(render_mutex);
  }

  /* decrement the pending count and reset our state */
  if (v->fbi.swaps_pending)
    v->fbi.swaps_pending--;
  v->fbi.vblank_count = 0;
  v->fbi.vblank_swap_pending = 0;
}

/*-------------------------------------------------
    swapbuffer - execute the 'swapbuffer'
    command
-------------------------------------------------*/
Bit32s swapbuffer(voodoo_state *v, Bit32u data)
{
  /* set the don't swap value for Voodoo 2 */
  v->fbi.vblank_swap_pending = 1;
  v->fbi.vblank_swap = (data >> 1) & 0xff;
  v->fbi.vblank_dont_swap = (data >> 9) & 1;

  /* if we're not syncing to the retrace, process the command immediately */
  if (!(data & 1))
  {
    BX_LOCK(fifo_mutex);
    swap_buffers(v);
    BX_UNLOCK(fifo_mutex);
    return 0;
  } else {
    if (v->vtimer_running) {
      bx_wait_sem(&vertical_sem);
    }
  }

  /* determine how many cycles to wait; we deliberately overshoot here because */
  /* the final count gets updated on the VBLANK */
  return (v->fbi.vblank_swap + 1) * v->freq / 30;
}


/*************************************
 *
 *  Statistics management
 *
 *************************************/

static void accumulate_statistics(voodoo_state *v, const stats_block *stats)
{
  /* apply internal voodoo statistics */
  v->reg[fbiPixelsIn].u += stats->pixels_in;
  v->reg[fbiPixelsOut].u += stats->pixels_out;
  v->reg[fbiChromaFail].u += stats->chroma_fail;
  v->reg[fbiZfuncFail].u += stats->zfunc_fail;
  v->reg[fbiAfuncFail].u += stats->afunc_fail;
}

static void update_statistics(voodoo_state *v, int accumulate)
{
  int threadnum;

  /* accumulate/reset statistics from all units */
  for (threadnum = 0; threadnum < WORK_MAX_THREADS; threadnum++)
  {
    if (accumulate)
      accumulate_statistics(v, &v->thread_stats[threadnum]);
    memset(&v->thread_stats[threadnum], 0, sizeof(v->thread_stats[threadnum]));
  }

  /* accumulate/reset statistics from the LFB */
  if (accumulate)
    accumulate_statistics(v, &v->fbi.lfb_stats);
  memset(&v->fbi.lfb_stats, 0, sizeof(v->fbi.lfb_stats));
}

void reset_counters(voodoo_state *v)
{
  update_statistics(v, FALSE);
  v->reg[fbiPixelsIn].u = 0;
  v->reg[fbiChromaFail].u = 0;
  v->reg[fbiZfuncFail].u = 0;
  v->reg[fbiAfuncFail].u = 0;
  v->reg[fbiPixelsOut].u = 0;
}


void soft_reset(voodoo_state *v)
{
  reset_counters(v);
  v->reg[fbiTrianglesOut].u = 0;
  fifo_reset(&v->fbi.fifo);
  fifo_reset(&v->pci.fifo);
  v->pci.op_pending = 0;
}


void recompute_video_memory(voodoo_state *v)
{
  Bit32u buffer_pages = FBIINIT2_VIDEO_BUFFER_OFFSET(v->reg[fbiInit2].u);
  Bit32u fifo_start_page = FBIINIT4_MEMORY_FIFO_START_ROW(v->reg[fbiInit4].u);
  Bit32u fifo_last_page = FBIINIT4_MEMORY_FIFO_STOP_ROW(v->reg[fbiInit4].u);
  Bit32u memory_config;
  int buf;

  BX_DEBUG(("buffer_pages 0x%x", buffer_pages));
  /* memory config is determined differently between V1 and V2 */
  memory_config = FBIINIT2_ENABLE_TRIPLE_BUF(v->reg[fbiInit2].u);
  if (v->type == VOODOO_2 && memory_config == 0)
    memory_config = FBIINIT5_BUFFER_ALLOCATION(v->reg[fbiInit5].u);

  /* tiles are 64x16/32; x_tiles specifies how many half-tiles */
  v->fbi.tile_width = (v->type == VOODOO_1) ? 64 : 32;
  v->fbi.tile_height = (v->type == VOODOO_1) ? 16 : 32;
  v->fbi.x_tiles = FBIINIT1_X_VIDEO_TILES(v->reg[fbiInit1].u);
  if (v->type == VOODOO_2)
  {
    v->fbi.x_tiles = (v->fbi.x_tiles << 1) |
            (FBIINIT1_X_VIDEO_TILES_BIT5(v->reg[fbiInit1].u) << 5) |
            (FBIINIT6_X_VIDEO_TILES_BIT0(v->reg[fbiInit6].u));
  }
  v->fbi.rowpixels = v->fbi.tile_width * v->fbi.x_tiles;

  /* first RGB buffer always starts at 0 */
  v->fbi.rgboffs[0] = 0;

  if (buffer_pages>0) {
    /* second RGB buffer starts immediately afterwards */
    v->fbi.rgboffs[1] = buffer_pages * 0x1000;

    /* remaining buffers are based on the config */
    switch (memory_config) {
      case 3: /* reserved */
        BX_ERROR(("Unexpected memory configuration in recompute_video_memory!"));
        break;

      case 0: /* 2 color buffers, 1 aux buffer */
        v->fbi.rgboffs[2] = ~0;
        v->fbi.auxoffs = 2 * buffer_pages * 0x1000;
        break;

      case 1: /* 3 color buffers, 0 aux buffers */
        v->fbi.rgboffs[2] = 2 * buffer_pages * 0x1000;
        v->fbi.auxoffs = 3 * buffer_pages * 0x1000;
        break;

      case 2: /* 3 color buffers, 1 aux buffers */
        v->fbi.rgboffs[2] = 2 * buffer_pages * 0x1000;
        v->fbi.auxoffs = 3 * buffer_pages * 0x1000;
        break;
    }
  }

  /* clamp the RGB buffers to video memory */
  for (buf = 0; buf < 3; buf++)
    if (v->fbi.rgboffs[buf] != (Bit32u)~0 && v->fbi.rgboffs[buf] > v->fbi.mask)
      v->fbi.rgboffs[buf] = v->fbi.mask;

  /* clamp the aux buffer to video memory */
  if (v->fbi.auxoffs != (Bit32u)~0 && v->fbi.auxoffs > v->fbi.mask)
    v->fbi.auxoffs = v->fbi.mask;

  /* compute the memory FIFO location and size */
  if (fifo_last_page > v->fbi.mask / 0x1000)
    fifo_last_page = v->fbi.mask / 0x1000;

  /* is it valid and enabled? */
  if ((fifo_start_page <= fifo_last_page) && v->fbi.fifo.enabled)
  {
    v->fbi.fifo.base = (Bit32u *)(v->fbi.ram + fifo_start_page * 0x1000);
    v->fbi.fifo.size = (fifo_last_page + 1 - fifo_start_page) * 0x1000 / 4;
    if (v->fbi.fifo.size > 65536*2)
      v->fbi.fifo.size = 65536*2;
  }

  /* if not, disable the FIFO */
  else
  {
    v->fbi.fifo.base = NULL;
    v->fbi.fifo.size = 0;
  }

  /* reset the FIFO */
  //fifo_reset(&v->fbi.fifo);
  if (fifo_empty_locked(&v->pci.fifo)) v->pci.op_pending = 0;

  /* reset our front/back buffers if they are out of range */
  if (v->fbi.rgboffs[2] == (Bit32u)~0)
  {
    if (v->fbi.frontbuf == 2)
      v->fbi.frontbuf = 0;
    if (v->fbi.backbuf == 2)
      v->fbi.backbuf = 0;
  }
}


void voodoo2_bitblt_mux(Bit8u rop, Bit8u *dst_ptr, Bit8u *src_ptr, int dpxsize)
{
  Bit8u mask, inbits, outbits;

  for (int i = 0; i < dpxsize; i++) {
    mask = 0x80;
    outbits = 0;
    for (int b = 7; b >= 0; b--) {
      inbits = (*dst_ptr & mask) > 0;
      inbits |= ((*src_ptr & mask) > 0) << 1;
      outbits |= ((rop & (1 << inbits)) > 0) << b;
      mask >>= 1;
    }
    *dst_ptr++ = outbits;
    src_ptr++;
  }
}

#define BLT v->blt

bool clip_check(Bit16u x, Bit16u y)
{
  if (!BLT.clip_en)
    return 1;
  if ((x >= BLT.clipx0) && (x < BLT.clipx1) &&
      (y >= BLT.clipy0) && (y < BLT.clipy1)) {
    return 1;
  }
  return 0;
}


Bit8u chroma_check(Bit8u *ptr, Bit16u min, Bit16u max, bool dst)
{
  Bit8u pass = 0;
  Bit32u color;
  Bit8u r, g, b, rmin, rmax, gmin, gmax, bmin, bmax;

  color = *ptr;
  color |= *(ptr + 1) << 8;
  r = (color >> 11);
  g = (color >> 5) & 0x3f;
  b = color & 0x1f;
  rmin = (min >> 11) & 0x1f;
  rmax = (max >> 11) & 0x1f;
  gmin = (min >> 5) & 0x3f;
  gmax = (max >> 5) & 0x3f;
  bmin = min & 0x1f;
  bmax = max & 0x1f;
  pass = ((r >= rmin) && (r <= rmax) && (g >= gmin) && (g <= gmax) &&
          (b >= bmin) && (b <= bmax));
  if (!dst) pass <<= 1;
  return pass;
}

void voodoo2_bitblt(void)
{
  Bit8u cmd, rop = 0, *dst_ptr, *src_ptr;
  Bit16u c, cols, src_x, src_y, r, rows, size, x;
  Bit32u src_base, doffset, soffset, dstride, sstride;
  bool src_tiled, dst_tiled, x_dir, y_dir;
  int tmpval;

  cmd = (Bit8u)(v->reg[bltCommand].u & 0x07);
  BLT.src_fmt = (Bit8u)((v->reg[bltCommand].u >> 3) & 0x1f);
  BLT.src_swizzle = (Bit8u)((v->reg[bltCommand].u >> 8) & 0x03);
  BLT.chroma_en = (Bit8u)((v->reg[bltCommand].u >> 10) & 0x01);
  BLT.chroma_en |= (Bit8u)((v->reg[bltCommand].u >> 11) & 0x02);
  src_tiled = ((v->reg[bltCommand].u >> 14) & 0x01);
  dst_tiled = ((v->reg[bltCommand].u >> 15) & 0x01);
  BLT.clip_en = ((v->reg[bltCommand].u >> 16) & 0x01);
  BLT.transp = ((v->reg[bltCommand].u >> 17) & 0x01);
  BLT.dst_w = (v->reg[bltSize].u & 0x7ff) + 1;
  x_dir = (v->reg[bltSize].u >> 11) & 1;
  tmpval = (v->reg[bltSize].u & 0xfff);
  if (x_dir && ((cmd == 0) || (cmd == 2))) {
    tmpval |= 0xfffff000;
  }
  BLT.dst_w = abs(tmpval) + 1;
  y_dir = (v->reg[bltSize].u >> 27) & 1;
  tmpval = ((v->reg[bltSize].u >> 16) & 0xfff);
  if (y_dir && ((cmd == 0) || (cmd == 2))) {
    tmpval |= 0xfffff000;
  }
  BLT.dst_h = abs(tmpval) + 1;
  BLT.dst_x = (Bit16u)(v->reg[bltDstXY].u & 0x7ff);
  BLT.dst_y = (Bit16u)((v->reg[bltDstXY].u >> 16) & 0x7ff);
  if (src_tiled) {
    src_base = (v->reg[bltSrcBaseAddr].u & 0x3ff) << 12;
    sstride = (v->reg[bltXYStrides].u & 0x3f) << 6;
  } else {
    src_base = v->reg[bltSrcBaseAddr].u & 0x3ffff8;
    sstride = v->reg[bltXYStrides].u & 0xff8;
  }
  if (dst_tiled) {
    BLT.dst_base = (v->reg[bltDstBaseAddr].u & 0x3ff) << 12;
    BLT.dst_pitch = (v->reg[bltXYStrides].u >> 10) & 0xfc0;
  } else {
    BLT.dst_base = v->reg[bltDstBaseAddr].u & 0x3ffff8;
    BLT.dst_pitch = (v->reg[bltXYStrides].u >> 16) & 0xff8;
  }
  BLT.h2s_mode = 0;
  switch (cmd) {
    case 0:
      BX_DEBUG(("Screen-to-Screen bitBLT: w = %d, h = %d, rop0 = %d",
                BLT.dst_w, BLT.dst_h, BLT.rop[0]));
      src_x = (Bit16u)(v->reg[bltSrcXY].u & 0x7ff);
      src_y = (Bit16u)((v->reg[bltSrcXY].u >> 16) & 0x7ff);
      cols = BLT.dst_w;
      rows = BLT.dst_h;
      dstride = BLT.dst_pitch;
      doffset = BLT.dst_base + BLT.dst_y * dstride + BLT.dst_x * 2;
      soffset = src_base + src_y * sstride + src_x * 2;
      for (r = 0; r <= rows; r++) {
        dst_ptr = &v->fbi.ram[doffset & v->fbi.mask];
        src_ptr = &v->fbi.ram[soffset & v->fbi.mask];
        x = BLT.dst_x;
        for (c = 0; c < cols; c++) {
          if (clip_check(x, BLT.dst_y)) {
            if (BLT.chroma_en & 1) {
              rop = chroma_check(src_ptr, BLT.src_col_min, BLT.src_col_max, 0);
            }
            if (BLT.chroma_en & 2) {
              rop |= chroma_check(dst_ptr, BLT.dst_col_min, BLT.dst_col_max, 1);
            }
            voodoo2_bitblt_mux(BLT.rop[rop], dst_ptr, src_ptr, 2);
          }
          if (x_dir) {
            dst_ptr -= 2;
            src_ptr -= 2;
            x--;
          } else {
            dst_ptr += 2;
            src_ptr += 2;
            x++;
          }
        }
        if (y_dir) {
          doffset -= dstride;
          soffset -= sstride;
          BLT.dst_y--;
        } else {
          doffset += dstride;
          soffset += sstride;
          BLT.dst_y++;
        }
      }
      break;
    case 1:
      BX_DEBUG(("CPU-to-Screen bitBLT: w = %d, h = %d, rop0 = %d",
                BLT.dst_w, BLT.dst_h, BLT.rop[0]));
      BLT.h2s_mode = 1;
      BLT.cur_x = BLT.dst_x;
      break;
    case 2:
      BX_DEBUG(("Rectangle fill: w = %d, h = %d, rop0 = %d",
                BLT.dst_w, BLT.dst_h, BLT.rop[0]));
      cols = BLT.dst_w;
      rows = BLT.dst_h;
      dstride = BLT.dst_pitch;
      doffset = BLT.dst_base + BLT.dst_y * dstride + BLT.dst_x * 2;
      src_ptr = BLT.fgcolor;
      for (r = 0; r <= rows; r++) {
        dst_ptr = &v->fbi.ram[doffset & v->fbi.mask];
        x = BLT.dst_x;
        for (c = 0; c < cols; c++) {
          if (clip_check(x, BLT.dst_y)) {
            if (BLT.chroma_en & 2) {
              rop = chroma_check(dst_ptr, BLT.dst_col_min, BLT.dst_col_max, 1);
            }
            voodoo2_bitblt_mux(BLT.rop[rop], dst_ptr, src_ptr, 2);
          }
          if (x_dir) {
            dst_ptr -= 2;
            x--;
          } else {
            dst_ptr += 2;
            x++;
          }
        }
        if (y_dir) {
          doffset -= dstride;
          BLT.dst_y--;
        } else {
          doffset += dstride;
          BLT.dst_y++;
        }
      }
      break;
    case 3:
      BLT.dst_x = (Bit16u)(v->reg[bltDstXY].u & 0x1ff);
      BLT.dst_y = (Bit16u)((v->reg[bltDstXY].u >> 16) & 0x3ff);
      cols = (Bit16u)(v->reg[bltSize].u & 0x1ff);
      rows = (Bit16u)((v->reg[bltSize].u >> 16) & 0x3ff);
      BX_DEBUG(("SGRAM fill: x = %d y = %d w = %d h = %d color = 0x%02x%02x",
                BLT.dst_x, BLT.dst_y, cols, rows, BLT.fgcolor[1], BLT.fgcolor[0]));
      dstride = (1 << 12);
      doffset = BLT.dst_y * dstride;
      for (r = 0; r <= rows; r++) {
        if (r == 0) {
          dst_ptr = &v->fbi.ram[(doffset + BLT.dst_x * 8) & v->fbi.mask];
          size = dstride / 2 - (BLT.dst_x * 4);
        } else {
          dst_ptr = &v->fbi.ram[doffset & v->fbi.mask];
          if (r == rows) {
            size = cols * 4;
          } else {
            size = dstride / 2;
          }
        }
        for (c = 0; c < size; c++) {
          *dst_ptr = BLT.fgcolor[0];
          *(dst_ptr + 1) = BLT.fgcolor[1];
          dst_ptr += 2;
        }
        doffset += dstride;
      }
      break;
    default:
      BX_ERROR(("Voodoo bitBLT: unknown command %d)", cmd));
  }
  v->fbi.video_changed = 1;
}

void voodoo2_bitblt_cpu_to_screen(Bit32u data)
{
  Bit8u rop = 0, *dst_ptr, *dst_ptr1, *src_ptr, color[2];
  Bit8u b, c, g, i, j, r;
  bool set;
  Bit8u colfmt = BLT.src_fmt & 7, rgbfmt = BLT.src_fmt >> 3;
  Bit16u count = BLT.dst_x + BLT.dst_w - BLT.cur_x;
  Bit32u doffset = BLT.dst_base + BLT.dst_y * BLT.dst_pitch + BLT.cur_x * 2;
  dst_ptr = &v->fbi.ram[doffset & v->fbi.mask];

  if (BLT.src_swizzle & 1) {
    data = bx_bswap32(data);
  }
  if (BLT.src_swizzle & 2) {
    data = (data >> 16) | (data << 16);
  }
  if ((colfmt == 0) || (colfmt == 1)) {
    if (colfmt == 0) {
      c = (count > 32) ? 32 : count;
      r = 1;
    } else {
      c = (count > 8) ? 8 : count;
      r = (BLT.dst_h > 4) ? 4 : BLT.dst_h;
    }
    for (j = 0; j < r; j++) {
      dst_ptr1 = dst_ptr;
      for (i = 0; i < c; i++) {
        b = (i & 0x18) + (7 - (i & 7));
        set = (data & (1U << b)) > 0;
        if (set) {
          src_ptr = BLT.fgcolor;
        } else {
          src_ptr = BLT.bgcolor;
        }
        if (set || !BLT.transp) {
          if (clip_check(BLT.cur_x + i, BLT.dst_y + j)) {
            if (BLT.chroma_en & 2) {
              rop = chroma_check(dst_ptr1, BLT.dst_col_min, BLT.dst_col_max, 1);
            }
            voodoo2_bitblt_mux(BLT.rop[rop], dst_ptr1, src_ptr, 2);
          }
        }
        dst_ptr1 += 2;
      }
      if (colfmt == 0) {
        if (c < count) {
          BLT.cur_x += c;
        } else {
          BLT.cur_x = BLT.dst_x;
          if (BLT.dst_h > 1) {
            BLT.dst_y++;
            BLT.dst_h--;
          } else {
            BLT.h2s_mode = 0;
          }
        }
      } else {
        data >>= 8;
        dst_ptr += BLT.dst_pitch;
      }
    }
    if (colfmt == 1) {
      if (c < count) {
        BLT.cur_x += c;
      } else {
        BLT.cur_x = BLT.dst_x;
        if (BLT.dst_h > 4) {
          BLT.dst_y += 4;
          BLT.dst_h -= 4;
        } else {
          BLT.h2s_mode = 0;
        }
      }
    }
  } else if (colfmt == 2) {
    if (rgbfmt & 1) {
      BX_ERROR(("Voodoo bitBLT: color order other than RGB not supported yet"));
    }
#ifdef BX_BIG_ENDIAN
    data = bx_bswap32(data);
#endif
    src_ptr = (Bit8u*)&data;
    c = (count > 2) ? 2 : count;
    for (i = 0; i < c; i++) {
      if (clip_check(BLT.cur_x, BLT.dst_y)) {
        if (BLT.chroma_en & 1) {
          rop = chroma_check(src_ptr, BLT.src_col_min, BLT.src_col_max, 0);
        }
        if (BLT.chroma_en & 2) {
          rop |= chroma_check(dst_ptr, BLT.dst_col_min, BLT.dst_col_max, 1);
        }
        voodoo2_bitblt_mux(BLT.rop[rop], dst_ptr, src_ptr, 2);
      }
      dst_ptr += 2;
      src_ptr += 2;
      BLT.cur_x++;
      if (--count == 0) {
        BLT.cur_x = BLT.dst_x;
        BLT.dst_y++;
        if (--BLT.dst_h == 0) {
          BLT.h2s_mode = 0;
        }
      }
    }
  } else if ((colfmt >= 3) && (colfmt <= 5)) {
    if (colfmt > 3) {
      BX_ERROR(("Voodoo bitBLT: 24 bpp source dithering not supported yet"));
      colfmt = 3;
    }
    switch (rgbfmt) {
      case 1:
        r = (Bit8u)((data >> 3) & 0x1f);
        g = (Bit8u)((data >> 10) & 0x3f);
        b = (Bit8u)((data >> 19) & 0x1f);
        break;
      case 2:
        r = (Bit8u)((data >> 27) & 0x1f);
        g = (Bit8u)((data >> 18) & 0x3f);
        b = (Bit8u)((data >> 11) & 0x1f);
        break;
      case 3:
        r = (Bit8u)((data >> 11) & 0x1f);
        g = (Bit8u)((data >> 18) & 0x3f);
        b = (Bit8u)((data >> 27) & 0x1f);
        break;
      default:
        r = (Bit8u)((data >> 19) & 0x1f);
        g = (Bit8u)((data >> 10) & 0x3f);
        b = (Bit8u)((data >> 3) & 0x1f);
    }
    color[0] = (Bit8u)((g << 5) | b);
    color[1] = (r << 3) | (g >> 3);
    src_ptr = color;
    if (clip_check(BLT.cur_x, BLT.dst_y)) {
      if (BLT.chroma_en & 1) {
        rop = chroma_check(src_ptr, BLT.src_col_min, BLT.src_col_max, 0);
      }
      if (BLT.chroma_en & 2) {
        rop |= chroma_check(dst_ptr, BLT.dst_col_min, BLT.dst_col_max, 1);
      }
      voodoo2_bitblt_mux(BLT.rop[rop], dst_ptr, src_ptr, 2);
    }
    BLT.cur_x++;
    if (--count == 0) {
      BLT.cur_x = BLT.dst_x;
      BLT.dst_y++;
      if (--BLT.dst_h == 0) {
        BLT.h2s_mode = 0;
      }
    }
  } else {
    BX_ERROR(("CPU-to-Screen bitBLT: unknown color format 0x%02x", colfmt));
  }
  v->fbi.video_changed = 1;
}


void dacdata_w(dac_state *d, Bit8u regnum, Bit8u data)
{
  d->reg[regnum] = data;

  /* switch off the DAC register requested */
  switch (regnum) {
    case 4: // PLLWMA
    case 7: // PLLRMA
      if (data == 0x0e) {
        d->data_size = 1;
      } else {
        d->data_size = 2;
      }
      break;
    case 5: // PLLDATA
      switch (d->reg[4]) { // PLLWMA
        case 0x00:
          if (d->data_size == 2) {
            d->clk0_m = data;
          } else if (d->data_size == 1) {
            d->clk0_n = data & 0x1f;
            d->clk0_p = data >> 5;
          }
          break;
        case 0x0e:
          if ((d->data_size == 1) && (data == 0xf8)) {
            v->vidclk = 14318184.0f * ((float)(d->clk0_m + 2) / (float)(d->clk0_n + 2)) / (float)(1 << d->clk0_p);
            Bit8u dacr6 = d->reg[6] & 0xf0;
            if ((dacr6 == 0x20) || (dacr6 == 0x60) || (dacr6 == 0x70)) {
              v->vidclk /= 2.0f;
            }
            Voodoo_update_timing();
          }
          break;
      }
      d->data_size--;
      break;
  }
}


void dacdata_r(dac_state *d, Bit8u regnum)
{
  Bit8u result = 0xff;

  /* switch off the DAC register requested */
  switch (regnum) {
    case 5: // PLLDATA
      switch (d->reg[7]) { // PLLRMA
        case 0x00:
          if (d->data_size == 2) {
            result = d->clk0_m;
          } else if (d->data_size == 1) {
            result = d->clk0_n | (d->clk0_p << 5);
          }
          break;
        /* this is just to make startup happy */
        case 0x01:  result = 0x55; break;
        case 0x07:  result = 0x71; break;
        case 0x0b:  result = 0x79; break;
      }
      d->data_size--;
      break;

    default:
      result = d->reg[regnum];
      break;
  }

  /* remember the read result; it is fetched elsewhere */
  d->read_result = result;
}

void register_w(Bit32u offset, Bit32u data, bool log)
{
  Bit32u regnum  = (offset) & 0xff;
  Bit32u chips   = (offset>>8) & 0xf;
  Bit64s data64;
  static Bit32u count = 0;

  if (chips == 0)
    chips = 0xf;

  /* the first 64 registers can be aliased differently */
  if ((offset & 0x800c0) == 0x80000 && v->alt_regmap)
    regnum = register_alias_map[offset & 0x3f];
  else
    regnum = offset & 0xff;

  if (log)
    BX_DEBUG(("write chip 0x%x reg 0x%x value 0x%08x(%s)", chips, regnum<<2, data, v->regnames[regnum]));

  switch (regnum) {
    /* Vertex data is 12.4 formatted fixed point */
    case fvertexAx:
      data = float_to_Bit32s(data, 4);
    case vertexAx:
      if (chips & 1) v->fbi.ax = (Bit16s)data;
      break;

    case fvertexAy:
      data = float_to_Bit32s(data, 4);
    case vertexAy:
      if (chips & 1) v->fbi.ay = (Bit16s)data;
      break;

    case fvertexBx:
      data = float_to_Bit32s(data, 4);
    case vertexBx:
      if (chips & 1) v->fbi.bx = (Bit16s)data;
      break;

    case fvertexBy:
      data = float_to_Bit32s(data, 4);
    case vertexBy:
      if (chips & 1) v->fbi.by = (Bit16s)data;
      break;

    case fvertexCx:
      data = float_to_Bit32s(data, 4);
    case vertexCx:
      if (chips & 1) v->fbi.cx = (Bit16s)data;
      break;

    case fvertexCy:
      data = float_to_Bit32s(data, 4);
    case vertexCy:
      if (chips & 1) v->fbi.cy = (Bit16s)data;
      break;

    /* RGB data is 12.12 formatted fixed point */
    case fstartR:
      data = float_to_Bit32s(data, 12);
    case startR:
      if (chips & 1) v->fbi.startr = (Bit32s)(data << 8) >> 8;
      break;

    case fstartG:
      data = float_to_Bit32s(data, 12);
    case startG:
      if (chips & 1) v->fbi.startg = (Bit32s)(data << 8) >> 8;
      break;

    case fstartB:
      data = float_to_Bit32s(data, 12);
    case startB:
      if (chips & 1) v->fbi.startb = (Bit32s)(data << 8) >> 8;
      break;

    case fstartA:
      data = float_to_Bit32s(data, 12);
    case startA:
      if (chips & 1) v->fbi.starta = (Bit32s)(data << 8) >> 8;
      break;

    case fdRdX:
      data = float_to_Bit32s(data, 12);
    case dRdX:
      if (chips & 1) v->fbi.drdx = (Bit32s)(data << 8) >> 8;
      break;

    case fdGdX:
      data = float_to_Bit32s(data, 12);
    case dGdX:
      if (chips & 1) v->fbi.dgdx = (Bit32s)(data << 8) >> 8;
      break;

    case fdBdX:
      data = float_to_Bit32s(data, 12);
    case dBdX:
      if (chips & 1) v->fbi.dbdx = (Bit32s)(data << 8) >> 8;
      break;

    case fdAdX:
      data = float_to_Bit32s(data, 12);
    case dAdX:
      if (chips & 1) v->fbi.dadx = (Bit32s)(data << 8) >> 8;
      break;

    case fdRdY:
      data = float_to_Bit32s(data, 12);
    case dRdY:
      if (chips & 1) v->fbi.drdy = (Bit32s)(data << 8) >> 8;
      break;

    case fdGdY:
      data = float_to_Bit32s(data, 12);
    case dGdY:
      if (chips & 1) v->fbi.dgdy = (Bit32s)(data << 8) >> 8;
      break;

    case fdBdY:
      data = float_to_Bit32s(data, 12);
    case dBdY:
      if (chips & 1) v->fbi.dbdy = (Bit32s)(data << 8) >> 8;
      break;

    case fdAdY:
      data = float_to_Bit32s(data, 12);
    case dAdY:
      if (chips & 1) v->fbi.dady = (Bit32s)(data << 8) >> 8;
      break;

    /* Z data is 20.12 formatted fixed point */
    case fstartZ:
      data = float_to_Bit32s(data, 12);
    case startZ:
      if (chips & 1) v->fbi.startz = (Bit32s)data;
      break;

    case fdZdX:
      data = float_to_Bit32s(data, 12);
    case dZdX:
      if (chips & 1) v->fbi.dzdx = (Bit32s)data;
      break;

    case fdZdY:
      data = float_to_Bit32s(data, 12);
    case dZdY:
      if (chips & 1) v->fbi.dzdy = (Bit32s)data;
      break;

    /* S,T data is 14.18 formatted fixed point, converted to 16.32 internally */
    case fstartS:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 2) v->tmu[0].starts = data64;
      if (chips & 4) v->tmu[1].starts = data64;
      break;
    case startS:
      if (chips & 2) v->tmu[0].starts = (Bit64s)(Bit32s)data << 14;
      if (chips & 4) v->tmu[1].starts = (Bit64s)(Bit32s)data << 14;
      break;

    case fstartT:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 2) v->tmu[0].startt = data64;
      if (chips & 4) v->tmu[1].startt = data64;
      break;
    case startT:
      if (chips & 2) v->tmu[0].startt = (Bit64s)(Bit32s)data << 14;
      if (chips & 4) v->tmu[1].startt = (Bit64s)(Bit32s)data << 14;
      break;

    case fdSdX:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 2) v->tmu[0].dsdx = data64;
      if (chips & 4) v->tmu[1].dsdx = data64;
      break;
    case dSdX:
      if (chips & 2) v->tmu[0].dsdx = (Bit64s)(Bit32s)data << 14;
      if (chips & 4) v->tmu[1].dsdx = (Bit64s)(Bit32s)data << 14;
      break;

    case fdTdX:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 2) v->tmu[0].dtdx = data64;
      if (chips & 4) v->tmu[1].dtdx = data64;
      break;
    case dTdX:
      if (chips & 2) v->tmu[0].dtdx = (Bit64s)(Bit32s)data << 14;
      if (chips & 4) v->tmu[1].dtdx = (Bit64s)(Bit32s)data << 14;
      break;

    case fdSdY:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 2) v->tmu[0].dsdy = data64;
      if (chips & 4) v->tmu[1].dsdy = data64;
      break;
    case dSdY:
      if (chips & 2) v->tmu[0].dsdy = (Bit64s)(Bit32s)data << 14;
      if (chips & 4) v->tmu[1].dsdy = (Bit64s)(Bit32s)data << 14;
      break;

    case fdTdY:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 2) v->tmu[0].dtdy = data64;
      if (chips & 4) v->tmu[1].dtdy = data64;
      break;
    case dTdY:
      if (chips & 2) v->tmu[0].dtdy = (Bit64s)(Bit32s)data << 14;
      if (chips & 4) v->tmu[1].dtdy = (Bit64s)(Bit32s)data << 14;
      break;

    /* W data is 2.30 formatted fixed point, converted to 16.32 internally */
    case fstartW:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 1) v->fbi.startw = data64;
      if (chips & 2) v->tmu[0].startw = data64;
      if (chips & 4) v->tmu[1].startw = data64;
      break;
    case startW:
      if (chips & 1) v->fbi.startw = (Bit64s)(Bit32s)data << 2;
      if (chips & 2) v->tmu[0].startw = (Bit64s)(Bit32s)data << 2;
      if (chips & 4) v->tmu[1].startw = (Bit64s)(Bit32s)data << 2;
      break;

    case fdWdX:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 1) v->fbi.dwdx = data64;
      if (chips & 2) v->tmu[0].dwdx = data64;
      if (chips & 4) v->tmu[1].dwdx = data64;
      break;
    case dWdX:
      if (chips & 1) v->fbi.dwdx = (Bit64s)(Bit32s)data << 2;
      if (chips & 2) v->tmu[0].dwdx = (Bit64s)(Bit32s)data << 2;
      if (chips & 4) v->tmu[1].dwdx = (Bit64s)(Bit32s)data << 2;
      break;

    case fdWdY:
      data64 = float_to_Bit64s(data, 32);
      if (chips & 1) v->fbi.dwdy = data64;
      if (chips & 2) v->tmu[0].dwdy = data64;
      if (chips & 4) v->tmu[1].dwdy = data64;
      break;
    case dWdY:
      if (chips & 1) v->fbi.dwdy = (Bit64s)(Bit32s)data << 2;
      if (chips & 2) v->tmu[0].dwdy = (Bit64s)(Bit32s)data << 2;
      if (chips & 4) v->tmu[1].dwdy = (Bit64s)(Bit32s)data << 2;
      break;
    /* setup bits */
    case sARGB:
      if (chips & 1)
      {
        v->reg[sAlpha].f = (float)RGB_ALPHA(data);
        v->reg[sRed].f = (float)RGB_RED(data);
        v->reg[sGreen].f = (float)RGB_GREEN(data);
        v->reg[sBlue].f = (float)RGB_BLUE(data);
      }
      break;

    /* mask off invalid bits for different cards */
    case fbzColorPath:
      poly_wait(v->poly, v->regnames[regnum]);
      if (v->type < VOODOO_2)
        data &= 0x0fffffff;
      if (chips & 1) v->reg[fbzColorPath].u = data;
      break;

    case fbzMode:
      poly_wait(v->poly, v->regnames[regnum]);
      if (v->type < VOODOO_2)
        data &= 0x001fffff;
      if (chips & 1) v->reg[fbzMode].u = data;
      break;

    case fogMode:
      poly_wait(v->poly, v->regnames[regnum]);
      if (v->type < VOODOO_2)
        data &= 0x0000003f;
      if (chips & 1) v->reg[fogMode].u = data;
      break;

    /* triangle drawing */
    case triangleCMD:
      v->fbi.cheating_allowed = (v->fbi.ax != 0 || v->fbi.ay != 0 || v->fbi.bx > 50 || v->fbi.by != 0 || v->fbi.cx != 0 || v->fbi.cy > 50);
      v->fbi.sign = data;
      triangle();
      break;

    case ftriangleCMD:
      v->fbi.cheating_allowed = 1;
      v->fbi.sign = data;
      triangle();
      break;

    case sBeginTriCMD:
      begin_triangle();
      break;

    case sDrawTriCMD:
      draw_triangle();
      break;

    /* other commands */
    case nopCMD:
      poly_wait(v->poly, v->regnames[regnum]);
      if (data & 1)
        reset_counters(v);
      if (data & 2)
        v->reg[fbiTrianglesOut].u = 0;
      break;

    case fastfillCMD:
      fastfill(v);
      break;

    case swapbufferCMD:
      poly_wait(v->poly, v->regnames[regnum]);
      swapbuffer(v, data);
      break;
    /* gamma table access -- Voodoo/Voodoo2 only */
    case clutData:
      if (v->type <= VOODOO_2 && (chips & 1))
      {
        poly_wait(v->poly, v->regnames[regnum]);
        if (!FBIINIT1_VIDEO_TIMING_RESET(v->reg[fbiInit1].u))
        {
          int index = data >> 24;
          if (index <= 32)
          {
            BX_LOCK(fifo_mutex);
            v->fbi.clut[index] = data;
            v->fbi.clut_dirty = 1;
            BX_UNLOCK(fifo_mutex);
          }
        }
        else
          BX_DEBUG(("clutData ignored because video timing reset = 1"));
      }
      break;
    /* nccTable entries are processed and expanded immediately */
    case nccTable+0:
    case nccTable+1:
    case nccTable+2:
    case nccTable+3:
    case nccTable+4:
    case nccTable+5:
    case nccTable+6:
    case nccTable+7:
    case nccTable+8:
    case nccTable+9:
    case nccTable+10:
    case nccTable+11:
      poly_wait(v->poly, v->regnames[regnum]);
      if (chips & 2) ncc_table_write(&v->tmu[0].ncc[0], regnum - nccTable, data);
      if (chips & 4) ncc_table_write(&v->tmu[1].ncc[0], regnum - nccTable, data);
      break;

    case nccTable+12:
    case nccTable+13:
    case nccTable+14:
    case nccTable+15:
    case nccTable+16:
    case nccTable+17:
    case nccTable+18:
    case nccTable+19:
    case nccTable+20:
    case nccTable+21:
    case nccTable+22:
    case nccTable+23:
      poly_wait(v->poly, v->regnames[regnum]);
      if (chips & 2) ncc_table_write(&v->tmu[0].ncc[1], regnum - (nccTable+12), data);
      if (chips & 4) ncc_table_write(&v->tmu[1].ncc[1], regnum - (nccTable+12), data);
      break;

    /* fogTable entries are processed and expanded immediately */
    case fogTable+0:
    case fogTable+1:
    case fogTable+2:
    case fogTable+3:
    case fogTable+4:
    case fogTable+5:
    case fogTable+6:
    case fogTable+7:
    case fogTable+8:
    case fogTable+9:
    case fogTable+10:
    case fogTable+11:
    case fogTable+12:
    case fogTable+13:
    case fogTable+14:
    case fogTable+15:
    case fogTable+16:
    case fogTable+17:
    case fogTable+18:
    case fogTable+19:
    case fogTable+20:
    case fogTable+21:
    case fogTable+22:
    case fogTable+23:
    case fogTable+24:
    case fogTable+25:
    case fogTable+26:
    case fogTable+27:
    case fogTable+28:
    case fogTable+29:
    case fogTable+30:
    case fogTable+31:
      poly_wait(v->poly, v->regnames[regnum]);
      if (chips & 1)
      {
        int base = 2 * (regnum - fogTable);
        v->fbi.fogdelta[base + 0] = (data >> 0) & 0xff;
        v->fbi.fogblend[base + 0] = (data >> 8) & 0xff;
        v->fbi.fogdelta[base + 1] = (data >> 16) & 0xff;
        v->fbi.fogblend[base + 1] = (data >> 24) & 0xff;
      }
      break;

    /* texture modifications cause us to recompute everything */
    case textureMode:
      if (((chips & 6) > 0) && TEXMODE_TRILINEAR(data)) {
        if (count < 50) BX_INFO(("Trilinear textures not implemented yet"));
        count++;
      }
    case tLOD:
    case tDetail:
    case texBaseAddr:
    case texBaseAddr_1:
    case texBaseAddr_2:
    case texBaseAddr_3_8:
      poly_wait(v->poly, v->regnames[regnum]);
      if (chips & 2)
      {
        v->tmu[0].reg[regnum].u = data;
        v->tmu[0].regdirty = 1;
      }
      if (chips & 4)
      {
        v->tmu[1].reg[regnum].u = data;
        v->tmu[1].regdirty = 1;
      }
      break;

    case trexInit1:
      /* send tmu config data to the frame buffer */
      v->send_config = TREXINIT_SEND_TMU_CONFIG(data);
      goto default_case;
      break;

    case userIntrCMD:
      BX_ERROR(("Writing to register %s not supported yet", v->regnames[regnum]));
      v->reg[regnum].u = data;
      break;

    case bltData:
      v->reg[regnum].u = data;
      if (BLT.h2s_mode) {
        voodoo2_bitblt_cpu_to_screen(data);
      } else {
        BX_ERROR(("Write to register %s ignored", v->regnames[regnum]));
      }
      break;

    case bltSrcChromaRange:
      v->reg[regnum].u = data;
      BLT.src_col_min = (Bit16u)data;
      BLT.src_col_max = (Bit16u)(data >> 16);
      break;

    case bltDstChromaRange:
      v->reg[regnum].u = data;
      BLT.dst_col_min = (Bit16u)data;
      BLT.dst_col_max = (Bit16u)(data >> 16);
      break;

    case bltClipX:
      v->reg[regnum].u = data;
      BLT.clipx0 = (Bit16u)(data >> 16);
      BLT.clipx1 = (Bit16u)(data & 0x0fff);
      break;

    case bltClipY:
      v->reg[regnum].u = data;
      BLT.clipy0 = (Bit16u)(data >> 16);
      BLT.clipy1 = (Bit16u)(data & 0x0fff);
      break;

    case bltRop:
      v->reg[regnum].u = data;
      BLT.rop[0] = (Bit8u)(data & 0x0f);
      BLT.rop[1] = (Bit8u)((data >> 4) & 0x0f);
      BLT.rop[2] = (Bit8u)((data >> 8) & 0x0f);
      BLT.rop[3] = (Bit8u)((data >> 12) & 0x0f);
      break;

    case bltColor:
      v->reg[regnum].u = data;
      BLT.fgcolor[0] = (Bit8u)data;
      BLT.fgcolor[1] = (Bit8u)(data >> 8);
      BLT.bgcolor[0] = (Bit8u)(data >> 16);
      BLT.bgcolor[1] = (Bit8u)(data >> 24);
      break;

    case bltDstXY:
    case bltSize:
    case bltCommand:
      v->reg[regnum].u = data;
      if ((data >> 31) & 1) {
        voodoo2_bitblt();
      }
      break;

    case colBufferAddr: /* Banshee */
      if (v->type >= VOODOO_BANSHEE && (chips & 1)) {
        v->fbi.rgboffs[1] = data & v->fbi.mask & ~0x0f;
      }
      break;

    case colBufferStride: /* Banshee */
      if (v->type >= VOODOO_BANSHEE && (chips & 1)) {
        if (data & 0x8000)
          v->fbi.rowpixels = (data & 0x7f) << 6;
        else
          v->fbi.rowpixels = (data & 0x3fff) >> 1;
      }
      break;

    case auxBufferAddr: /* Banshee */
      if (v->type >= VOODOO_BANSHEE && (chips & 1)) {
        v->fbi.auxoffs = data & v->fbi.mask & ~0x0f;
      }
      break;

    case auxBufferStride: /* Banshee */
      if (v->type >= VOODOO_BANSHEE && (chips & 1)) {
        Bit32u rowpixels;

        if (data & 0x8000)
          rowpixels = (data & 0x7f) << 6;
        else
          rowpixels = (data & 0x3fff) >> 1;
        if (v->fbi.rowpixels != rowpixels)
          BX_ERROR(("aux buffer stride differs from color buffer stride"));
      }
      break;

    /* these registers are referenced in the renderer; we must wait for pending work before changing */
    case chromaRange:
    case chromaKey:
    case alphaMode:
    case fogColor:
    case stipple:
    case zaColor:
    case color1:
    case color0:
    case clipLowYHighY:
    case clipLeftRight:
      poly_wait(v->poly, v->regnames[regnum]);
      /* fall through to default implementation */

    /* by default, just feed the data to the chips */
    default:
default_case:
      if (chips & 1) v->reg[0x000 + regnum].u = data;
      if (chips & 2) v->reg[0x100 + regnum].u = data;
      if (chips & 4) v->reg[0x200 + regnum].u = data;
      if (chips & 8) v->reg[0x300 + regnum].u = data;
      break;
  }
}

Bit32s texture_w(Bit32u offset, Bit32u data)
{
  int tmunum = (offset >> 19) & 0x03;
  BX_DEBUG(("write TMU%d offset 0x%x value 0x%x", tmunum, offset, data));

  tmu_state *t;

  /* point to the right TMU */
  if (!(v->chipmask & (2 << tmunum)) || (tmunum >= MAX_TMU))
    return 0;
  t = &v->tmu[tmunum];

  if (TEXLOD_TDIRECT_WRITE(t->reg[tLOD].u))
    BX_PANIC(("Texture direct write!"));

  /* wait for any outstanding work to finish */
  poly_wait(v->poly, "Texture write");

  /* update texture info if dirty */
  if (t->regdirty)
    recompute_texture_params(t);

  /* swizzle the data */
  if (TEXLOD_TDATA_SWIZZLE(t->reg[tLOD].u))
    data = bx_bswap32(data);
  if (TEXLOD_TDATA_SWAP(t->reg[tLOD].u))
    data = (data >> 16) | (data << 16);

  /* 8-bit texture case */
  if (TEXMODE_FORMAT(t->reg[textureMode].u) < 8)
  {
    int lod, tt, ts;
    Bit32u tbaseaddr;
    Bit8u *dest;

    /* extract info */
    if (v->type <= VOODOO_2)
    {
      lod = (offset >> 15) & 0x0f;
      tt = (offset >> 7) & 0xff;

      /* old code has a bit about how this is broken in gauntleg unless we always look at TMU0 */
      if (TEXMODE_SEQ_8_DOWNLD(v->tmu[0].reg[textureMode].u))
        ts = (offset << 2) & 0xfc;
      else
        ts = (offset << 1) & 0xfc;

      /* validate parameters */
      if (lod > 8)
        return 0;

      /* compute the base address */
      tbaseaddr = t->lodoffset[lod];
      tbaseaddr += tt * ((t->wmask >> lod) + 1) + ts;

      if (LOG_TEXTURE_RAM) BX_DEBUG(("Texture 8-bit w: lod=%d s=%d t=%d data=0x%08x", lod, ts, tt, data));
    }
    else
    {
      tbaseaddr = t->lodoffset[0] + offset*4;

      if (LOG_TEXTURE_RAM) BX_DEBUG(("Texture 16-bit w: offset=0x%x data=0x%08x", offset*4, data));
    }

    /* write the four bytes in little-endian order */
    dest = t->ram;
    tbaseaddr &= t->mask;
    dest[BYTE4_XOR_LE(tbaseaddr + 0)] = (data >> 0) & 0xff;
    dest[BYTE4_XOR_LE(tbaseaddr + 1)] = (data >> 8) & 0xff;
    dest[BYTE4_XOR_LE(tbaseaddr + 2)] = (data >> 16) & 0xff;
    dest[BYTE4_XOR_LE(tbaseaddr + 3)] = (data >> 24) & 0xff;
  }

  /* 16-bit texture case */
  else
  {
    int lod, tt, ts;
    Bit32u tbaseaddr;
    Bit16u *dest;

    /* extract info */
    if (v->type <= VOODOO_2)
    {
      tmunum = (offset >> 19) & 0x03;
      lod = (offset >> 15) & 0x0f;
      tt = (offset >> 7) & 0xff;
      ts = (offset << 1) & 0xfe;

      /* validate parameters */
      if (lod > 8)
        return 0;

      /* compute the base address */
      tbaseaddr = t->lodoffset[lod];
      tbaseaddr += 2 * (tt * ((t->wmask >> lod) + 1) + ts);

      if (LOG_TEXTURE_RAM) BX_DEBUG(("Texture 16-bit w: lod=%d s=%d t=%d data=%08X", lod, ts, tt, data));
    }
    else
    {
      tbaseaddr = t->lodoffset[0] + offset*4;

      if (LOG_TEXTURE_RAM) BX_DEBUG(("Texture 16-bit w: offset=0x%x data=0x%08x", offset*4, data));
    }

    /* write the two words in little-endian order */
    dest = (Bit16u *)t->ram;
    tbaseaddr &= t->mask;
    tbaseaddr >>= 1;
    dest[BYTE_XOR_LE(tbaseaddr + 0)] = (data >> 0) & 0xffff;
    dest[BYTE_XOR_LE(tbaseaddr + 1)] = (data >> 16) & 0xffff;
  }

  return 0;
}

Bit32u lfb_w(Bit32u offset, Bit32u data, Bit32u mem_mask)
{
  Bit16u *dest, *depth;
  Bit32u destmax, depthmax;

  int sr[2], sg[2], sb[2], sa[2], sw[2];
  int x, y, scry, mask;
  int pix, destbuf;

  BX_DEBUG(("write LFB offset 0x%x value 0x%08x", offset, data));

  /* byte swizzling */
  if (LFBMODE_BYTE_SWIZZLE_WRITES(v->reg[lfbMode].u))
  {
    data = bx_bswap32(data);
    mem_mask = bx_bswap32(mem_mask);
  }

  /* word swapping */
  if (LFBMODE_WORD_SWAP_WRITES(v->reg[lfbMode].u))
  {
    data = (data << 16) | (data >> 16);
    mem_mask = (mem_mask << 16) | (mem_mask >> 16);
  }

  /* extract default depth and alpha values */
  sw[0] = sw[1] = v->reg[zaColor].u & 0xffff;
  sa[0] = sa[1] = v->reg[zaColor].u >> 24;

  /* first extract A,R,G,B from the data */
  switch (LFBMODE_WRITE_FORMAT(v->reg[lfbMode].u) + 16 * LFBMODE_RGBA_LANES(v->reg[lfbMode].u))
  {
    case 16*0 + 0:    /* ARGB, 16-bit RGB 5-6-5 */
    case 16*2 + 0:    /* RGBA, 16-bit RGB 5-6-5 */
      EXTRACT_565_TO_888(data, sr[0], sg[0], sb[0]);
      EXTRACT_565_TO_888(data >> 16, sr[1], sg[1], sb[1]);
      mask = LFB_RGB_PRESENT | (LFB_RGB_PRESENT << 4);
      offset <<= 1;
      break;
    case 16*1 + 0:    /* ABGR, 16-bit RGB 5-6-5 */
    case 16*3 + 0:    /* BGRA, 16-bit RGB 5-6-5 */
      EXTRACT_565_TO_888(data, sb[0], sg[0], sr[0]);
      EXTRACT_565_TO_888(data >> 16, sb[1], sg[1], sr[1]);
      mask = LFB_RGB_PRESENT | (LFB_RGB_PRESENT << 4);
      offset <<= 1;
      break;

    case 16*0 + 1:    /* ARGB, 16-bit RGB x-5-5-5 */
      EXTRACT_x555_TO_888(data, sr[0], sg[0], sb[0]);
      EXTRACT_x555_TO_888(data >> 16, sr[1], sg[1], sb[1]);
      mask = LFB_RGB_PRESENT | (LFB_RGB_PRESENT << 4);
      offset <<= 1;
      break;
    case 16*1 + 1:    /* ABGR, 16-bit RGB x-5-5-5 */
      EXTRACT_x555_TO_888(data, sb[0], sg[0], sr[0]);
      EXTRACT_x555_TO_888(data >> 16, sb[1], sg[1], sr[1]);
      mask = LFB_RGB_PRESENT | (LFB_RGB_PRESENT << 4);
      offset <<= 1;
      break;
    case 16*2 + 1:    /* RGBA, 16-bit RGB x-5-5-5 */
      EXTRACT_555x_TO_888(data, sr[0], sg[0], sb[0]);
      EXTRACT_555x_TO_888(data >> 16, sr[1], sg[1], sb[1]);
      mask = LFB_RGB_PRESENT | (LFB_RGB_PRESENT << 4);
      offset <<= 1;
      break;
    case 16*3 + 1:    /* BGRA, 16-bit RGB x-5-5-5 */
      EXTRACT_555x_TO_888(data, sb[0], sg[0], sr[0]);
      EXTRACT_555x_TO_888(data >> 16, sb[1], sg[1], sr[1]);
      mask = LFB_RGB_PRESENT | (LFB_RGB_PRESENT << 4);
      offset <<= 1;
      break;

    case 16*0 + 2:    /* ARGB, 16-bit ARGB 1-5-5-5 */
      EXTRACT_1555_TO_8888(data, sa[0], sr[0], sg[0], sb[0]);
      EXTRACT_1555_TO_8888(data >> 16, sa[1], sr[1], sg[1], sb[1]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | ((LFB_RGB_PRESENT | LFB_ALPHA_PRESENT) << 4);
      offset <<= 1;
      break;
    case 16*1 + 2:    /* ABGR, 16-bit ARGB 1-5-5-5 */
      EXTRACT_1555_TO_8888(data, sa[0], sb[0], sg[0], sr[0]);
      EXTRACT_1555_TO_8888(data >> 16, sa[1], sb[1], sg[1], sr[1]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | ((LFB_RGB_PRESENT | LFB_ALPHA_PRESENT) << 4);
      offset <<= 1;
      break;
    case 16*2 + 2:    /* RGBA, 16-bit ARGB 1-5-5-5 */
      EXTRACT_5551_TO_8888(data, sr[0], sg[0], sb[0], sa[0]);
      EXTRACT_5551_TO_8888(data >> 16, sr[1], sg[1], sb[1], sa[1]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | ((LFB_RGB_PRESENT | LFB_ALPHA_PRESENT) << 4);
      offset <<= 1;
      break;
    case 16*3 + 2:    /* BGRA, 16-bit ARGB 1-5-5-5 */
      EXTRACT_5551_TO_8888(data, sb[0], sg[0], sr[0], sa[0]);
      EXTRACT_5551_TO_8888(data >> 16, sb[1], sg[1], sr[1], sa[1]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | ((LFB_RGB_PRESENT | LFB_ALPHA_PRESENT) << 4);
      offset <<= 1;
      break;

    case 16*0 + 4:    /* ARGB, 32-bit RGB x-8-8-8 */
      EXTRACT_x888_TO_888(data, sr[0], sg[0], sb[0]);
      mask = LFB_RGB_PRESENT;
      break;
    case 16*1 + 4:    /* ABGR, 32-bit RGB x-8-8-8 */
      EXTRACT_x888_TO_888(data, sb[0], sg[0], sr[0]);
      mask = LFB_RGB_PRESENT;
      break;
    case 16*2 + 4:    /* RGBA, 32-bit RGB x-8-8-8 */
      EXTRACT_888x_TO_888(data, sr[0], sg[0], sb[0]);
      mask = LFB_RGB_PRESENT;
      break;
    case 16*3 + 4:    /* BGRA, 32-bit RGB x-8-8-8 */
      EXTRACT_888x_TO_888(data, sb[0], sg[0], sr[0]);
      mask = LFB_RGB_PRESENT;
      break;

    case 16*0 + 5:    /* ARGB, 32-bit ARGB 8-8-8-8 */
      EXTRACT_8888_TO_8888(data, sa[0], sr[0], sg[0], sb[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT;
      break;
    case 16*1 + 5:    /* ABGR, 32-bit ARGB 8-8-8-8 */
      EXTRACT_8888_TO_8888(data, sa[0], sb[0], sg[0], sr[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT;
      break;
    case 16*2 + 5:    /* RGBA, 32-bit ARGB 8-8-8-8 */
      EXTRACT_8888_TO_8888(data, sr[0], sg[0], sb[0], sa[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT;
      break;
    case 16*3 + 5:    /* BGRA, 32-bit ARGB 8-8-8-8 */
      EXTRACT_8888_TO_8888(data, sb[0], sg[0], sr[0], sa[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT;
      break;

    case 16*0 + 12:   /* ARGB, 32-bit depth+RGB 5-6-5 */
    case 16*2 + 12:   /* RGBA, 32-bit depth+RGB 5-6-5 */
      sw[0] = data >> 16;
      EXTRACT_565_TO_888(data, sr[0], sg[0], sb[0]);
      mask = LFB_RGB_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;
    case 16*1 + 12:   /* ABGR, 32-bit depth+RGB 5-6-5 */
    case 16*3 + 12:   /* BGRA, 32-bit depth+RGB 5-6-5 */
      sw[0] = data >> 16;
      EXTRACT_565_TO_888(data, sb[0], sg[0], sr[0]);
      mask = LFB_RGB_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;

    case 16*0 + 13:   /* ARGB, 32-bit depth+RGB x-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_x555_TO_888(data, sr[0], sg[0], sb[0]);
      mask = LFB_RGB_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;
    case 16*1 + 13:   /* ABGR, 32-bit depth+RGB x-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_x555_TO_888(data, sb[0], sg[0], sr[0]);
      mask = LFB_RGB_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;
    case 16*2 + 13:   /* RGBA, 32-bit depth+RGB x-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_555x_TO_888(data, sr[0], sg[0], sb[0]);
      mask = LFB_RGB_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;
    case 16*3 + 13:   /* BGRA, 32-bit depth+RGB x-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_555x_TO_888(data, sb[0], sg[0], sr[0]);
      mask = LFB_RGB_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;

    case 16*0 + 14:   /* ARGB, 32-bit depth+ARGB 1-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_1555_TO_8888(data, sa[0], sr[0], sg[0], sb[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;
    case 16*1 + 14:   /* ABGR, 32-bit depth+ARGB 1-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_1555_TO_8888(data, sa[0], sb[0], sg[0], sr[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;
    case 16*2 + 14:   /* RGBA, 32-bit depth+ARGB 1-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_5551_TO_8888(data, sr[0], sg[0], sb[0], sa[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;
    case 16*3 + 14:   /* BGRA, 32-bit depth+ARGB 1-5-5-5 */
      sw[0] = data >> 16;
      EXTRACT_5551_TO_8888(data, sb[0], sg[0], sr[0], sa[0]);
      mask = LFB_RGB_PRESENT | LFB_ALPHA_PRESENT | LFB_DEPTH_PRESENT_MSW;
      break;

    case 16*0 + 15:   /* ARGB, 16-bit depth */
    case 16*1 + 15:   /* ARGB, 16-bit depth */
    case 16*2 + 15:   /* ARGB, 16-bit depth */
    case 16*3 + 15:   /* ARGB, 16-bit depth */
      sw[0] = data & 0xffff;
      sw[1] = data >> 16;
      mask = LFB_DEPTH_PRESENT | (LFB_DEPTH_PRESENT << 4);
      offset <<= 1;
      break;

    default:      /* reserved */
      return 0;
  }

  /* compute X,Y */
  x = (offset << 0) & ((1 << v->fbi.lfb_stride) - 1);
  y = (offset >> v->fbi.lfb_stride) & 0x7ff;

  /* adjust the mask based on which half of the data is written */
  if (!ACCESSING_BITS_0_15)
    mask &= ~(0x0f - LFB_DEPTH_PRESENT_MSW);
  if (!ACCESSING_BITS_16_31)
    mask &= ~(0xf0 + LFB_DEPTH_PRESENT_MSW);

  /* select the target buffer */
  destbuf = (v->type >= VOODOO_BANSHEE) ? 1 : LFBMODE_WRITE_BUFFER_SELECT(v->reg[lfbMode].u);
  switch (destbuf)
  {
    case 0:     /* front buffer */
      dest = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.frontbuf]);
      destmax = (v->fbi.mask + 1 - v->fbi.rgboffs[v->fbi.frontbuf]) / 2;
      v->fbi.video_changed = 1;
      break;

    case 1:     /* back buffer */
      dest = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.backbuf]);
      destmax = (v->fbi.mask + 1 - v->fbi.rgboffs[v->fbi.backbuf]) / 2;
      if (v->fbi.rgboffs[v->fbi.frontbuf] == v->fbi.rgboffs[v->fbi.backbuf])
        v->fbi.video_changed = 1;
      break;

    default:    /* reserved */
      return 0;
  }
  depth = (Bit16u *)(v->fbi.ram + v->fbi.auxoffs);
  depthmax = (v->fbi.mask + 1 - v->fbi.auxoffs) / 2;

  /* wait for any outstanding work to finish */
  poly_wait(v->poly, "LFB Write");

  /* simple case: no pipeline */
  if (!LFBMODE_ENABLE_PIXEL_PIPELINE(v->reg[lfbMode].u))
  {
    DECLARE_DITHER_POINTERS;
    UNUSED(dither);
    Bit32u bufoffs;

    if (LOG_LFB) BX_DEBUG(("VOODOO.%d.LFB:write raw mode %X (%d,%d) = %08X & %08X", v->index, LFBMODE_WRITE_FORMAT(v->reg[lfbMode].u), x, y, data, mem_mask));

    /* determine the screen Y */
    scry = y;
    if (LFBMODE_Y_ORIGIN(v->reg[lfbMode].u))
      scry = (v->fbi.yorigin - y) & v->fbi.clip_mask;

    /* advance pointers to the proper row */
    bufoffs = scry * v->fbi.rowpixels + x;

    /* compute dithering */
    COMPUTE_DITHER_POINTERS(v->reg[fbzMode].u, y);

    /* loop over up to two pixels */
    for (pix = 0; mask; pix++)
    {
      /* make sure we care about this pixel */
      if (mask & 0x0f)
      {
        /* write to the RGB buffer */
        if ((mask & LFB_RGB_PRESENT) && bufoffs < destmax)
        {
          /* apply dithering and write to the screen */
          APPLY_DITHER(v->reg[fbzMode].u, x, dither_lookup, sr[pix], sg[pix], sb[pix]);
          dest[bufoffs] = (sr[pix] << 11) | (sg[pix] << 5) | sb[pix];
        }

        /* make sure we have an aux buffer to write to */
        if (depth && bufoffs < depthmax)
        {
          /* write to the alpha buffer */
          if ((mask & LFB_ALPHA_PRESENT) && FBZMODE_ENABLE_ALPHA_PLANES(v->reg[fbzMode].u))
            depth[bufoffs] = sa[pix];

          /* write to the depth buffer */
          if ((mask & (LFB_DEPTH_PRESENT | LFB_DEPTH_PRESENT_MSW)) && !FBZMODE_ENABLE_ALPHA_PLANES(v->reg[fbzMode].u))
            depth[bufoffs] = sw[pix];
        }

        /* track pixel writes to the frame buffer regardless of mask */
        v->reg[fbiPixelsOut].u++;
      }

      /* advance our pointers */
      bufoffs++;
      x++;
      mask >>= 4;
    }
  }
  /* tricky case: run the full pixel pipeline on the pixel */
  else
  {
    DECLARE_DITHER_POINTERS;

    if (LOG_LFB) BX_DEBUG(("VOODOO.%d.LFB:write pipelined mode %X (%d,%d) = %08X & %08X", v->index, LFBMODE_WRITE_FORMAT(v->reg[lfbMode].u), x, y, data, mem_mask));

    /* determine the screen Y */
    scry = y;
    if (FBZMODE_Y_ORIGIN(v->reg[fbzMode].u))
      scry = (v->fbi.yorigin - y) & v->fbi.clip_mask;

    /* advance pointers to the proper row */
    dest += scry * v->fbi.rowpixels;
    if (depth)
      depth += scry * v->fbi.rowpixels;

    /* compute dithering */
    COMPUTE_DITHER_POINTERS(v->reg[fbzMode].u, y);

    /* loop over up to two pixels */
    for (pix = 0; mask; pix++)
    {
      /* make sure we care about this pixel */
      if (mask & 0x0f)
      {
        stats_block *stats = &v->fbi.lfb_stats;
        Bit64s iterw = sw[pix] << (30-16);
        Bit32s iterz = sw[pix] << 12;
        rgb_union color;

        /* apply clipping */
        if (FBZMODE_ENABLE_CLIPPING(v->reg[fbzMode].u))
        {
          if (x < (int)((v->reg[clipLeftRight].u >> 16) & v->fbi.clip_mask) ||
            x >= (int)(v->reg[clipLeftRight].u & v->fbi.clip_mask) ||
            scry < (int)((v->reg[clipLowYHighY].u >> 16) & v->fbi.clip_mask) ||
            scry >= (int)(v->reg[clipLowYHighY].u & v->fbi.clip_mask))
          {
            stats->pixels_in++;
            stats->clip_fail++;
            goto nextpixel;
          }
        }

        /* pixel pipeline part 1 handles depth testing and stippling */
        PIXEL_PIPELINE_BEGIN(v, stats, x, y, v->reg[fbzColorPath].u, v->reg[fbzMode].u, iterz, iterw);

        /* use the RGBA we stashed above */
        color.rgb.r = r = sr[pix];
        color.rgb.g = g = sg[pix];
        color.rgb.b = b = sb[pix];
        color.rgb.a = a = sa[pix];

        /* apply chroma key, alpha mask, and alpha testing */
        APPLY_CHROMAKEY(v, stats, v->reg[fbzMode].u, color);
        APPLY_ALPHAMASK(v, stats, v->reg[fbzMode].u, color.rgb.a);
        APPLY_ALPHATEST(v, stats, v->reg[alphaMode].u, color.rgb.a);

        /* pixel pipeline part 2 handles color combine, fog, alpha, and final output */
        PIXEL_PIPELINE_END(v, stats, dither, dither4, dither_lookup, x, dest, depth, v->reg[fbzMode].u, v->reg[fbzColorPath].u, v->reg[alphaMode].u, v->reg[fogMode].u, iterz, iterw, v->reg[zaColor]);
      }
nextpixel:
      /* advance our pointers */
      x++;
      mask >>= 4;
    }
  }

  return 0;
}

Bit32u cmdfifo_calc_depth_needed(cmdfifo_info *f)
{
  Bit32u command, needed = BX_MAX_BIT32U;
  Bit8u type;
  int i, count = 0;

  if (f->depth == 0)
    return needed;
  command = *(Bit32u*)(&v->fbi.ram[f->rdptr & v->fbi.mask]);
  type = (Bit8u)(command & 0x07);
  switch (type) {
    case 0:
      if (((command >> 3) & 7) == 4) {
        needed = 2;
      } else {
        needed = 1;
      }
      break;
    case 1:
      needed = 1 + (command >> 16);
      break;
    case 2:
      for (i = 3; i <= 31; i++)
        if (command & (1 << i)) count++;
      needed = 1 + count;
      break;
    case 3:
      count = 2;                             /* X/Y */
      if (command & (1 << 28)) {
        if (command & (3 << 10)) count++;    /* ARGB */
      } else {
        if (command & (1 << 10)) count += 3; /* RGB */
        if (command & (1 << 11)) count++;    /* A */
      }
      if (command & (1 << 12)) count++;      /* Z */
      if (command & (1 << 13)) count++;      /* Wb */
      if (command & (1 << 14)) count++;      /* W0 */
      if (command & (1 << 15)) count += 2;   /* S0/T0 */
      if (command & (1 << 16)) count++;      /* W1 */
      if (command & (1 << 17)) count += 2;   /* S1/T1 */
      count *= (command >> 6) & 15;          /* numverts */
      needed = 1 + count + (command >> 29);
      break;
    case 4:
      for (i = 15; i <= 28; i++)
        if (command & (1 << i)) count++;
      needed = 1 + count + (command >> 29);
      break;
    case 5:
      needed = 2 + ((command >> 3) & 0x7ffff);
      break;
    default:
      BX_ERROR(("CMDFIFO: unsupported packet type %d", type));
  }
  return needed;
}

void cmdfifo_w(cmdfifo_info *f, Bit32u fbi_offset, Bit32u data)
{
  BX_LOCK(cmdfifo_mutex);
  *(Bit32u*)(&v->fbi.ram[fbi_offset]) = data;
  /* count holes? */
  if (f->count_holes) {
    if ((f->holes == 0) && (fbi_offset == (f->amin + 4))) {
      /* in-order, no holes */
      f->amin = f->amax = fbi_offset;
      f->depth++;
    } else if (fbi_offset < f->amin) {
      /* out-of-order, below the minimum */
      if (f->holes != 0) {
        BX_ERROR(("Unexpected CMDFIFO: AMin=0x%08x AMax=0x%08x Holes=%d WroteTo:0x%08x RdPtr:0x%08x",
                  f->amin, f->amax, f->holes, fbi_offset, f->rdptr));
      }
      f->amin = f->amax = fbi_offset;
      f->depth++;
    } else if (fbi_offset < f->amax) {
      /* out-of-order, but within the min-max range */
      f->holes--;
      if (f->holes == 0) {
        f->depth += (f->amax - f->amin) / 4;
        f->amin = f->amax;
      }
    } else {
      /* out-of-order, bumping max */
      f->holes += (fbi_offset - f->amax) / 4 - 1;
      f->amax = fbi_offset;
    }
  }
  else {
    f->amax = fbi_offset;
    f->depth++;
  }
  if (f->depth_needed == BX_MAX_BIT32U) {
    f->depth_needed = cmdfifo_calc_depth_needed(f);
  }
  if (f->depth >= f->depth_needed) {
    f->cmd_ready = 1;
    if (!v->vtimer_running) {
      bx_set_sem(&fifo_wakeup);
    }
  }
  BX_UNLOCK(cmdfifo_mutex);
}

Bit32u cmdfifo_r(cmdfifo_info *f)
{
  Bit32u data;

  data = *(Bit32u*)(&v->fbi.ram[f->rdptr & v->fbi.mask]);
  f->rdptr += 4;
  if (!f->jsr) {
    if (f->rdptr >= f->end) {
      BX_INFO(("CMDFIFO RdPtr rollover"));
      f->rdptr = f->base;
    }
    f->depth--;
  }
  return data;
}

void cmdfifo_process(cmdfifo_info *f)
{
  Bit32u command, data, mask, nwords, regaddr;
  Bit8u type, code, nvertex, smode, disbytes, datalen = 0;
  bool inc, pcolor;
  voodoo_reg reg;
  int i, w0, wn;
  setup_vertex svert = {0};

  command = cmdfifo_r(f);
  type = (Bit8u)(command & 0x07);
  switch (type) {
    case 0:
      code = (Bit8u)((command >> 3) & 0x07);
      switch (code) {
        case 0: // NOP
          break;
        case 1: // JSR
          if (f->jsr) {
            BX_ERROR(("cmdfifo_process(): JSR: already inside of subroutine"));
          } else {
            f->jsr = true;
            f->retAddr = f->rdptr;
            f->rdptr = (command >> 4) & 0xfffffc;
          }
          break;
        case 2: // RET
          if (!f->jsr) {
            BX_ERROR(("cmdfifo_process(): RET: not inside of subroutine"));
          } else {
            f->rdptr = f->retAddr;
            f->retAddr = 0;
            f->jsr = false;
          }
          break;
        case 3: // JMP
          f->rdptr = (command >> 4) & 0xfffffc;
          if (f->count_holes) {
            BX_DEBUG(("cmdfifo_process(): JMP 0x%08x", f->rdptr));
          }
          break;
        case 4: // TODO: JMP AGP
          data = cmdfifo_r(f);
        default:
          BX_ERROR(("CMDFIFO packet type 0: unsupported code %d", code));
      }
      break;
    case 1:
      nwords = (command >> 16);
      regaddr = (command & 0x7ff8) >> 3;
      inc = (command >> 15) & 1;
      for (i = 0; i < (int)nwords; i++) {
        data = cmdfifo_r(f);
        BX_UNLOCK(cmdfifo_mutex);
        Voodoo_reg_write(regaddr, data);
        BX_LOCK(cmdfifo_mutex);
        if (inc) regaddr++;
      }
      break;
    case 2:
      mask = (command >> 3);
      if (v->type < VOODOO_BANSHEE) {
        regaddr = bltSrcBaseAddr;
      } else {
        regaddr = blt_clip0Min;
      }
      while (mask) {
        if (mask & 1) {
          data = cmdfifo_r(f);
          BX_UNLOCK(cmdfifo_mutex);
          if (v->type < VOODOO_BANSHEE) {
            register_w(regaddr, data, 1);
          } else {
            Banshee_2D_write(regaddr, data);
          }
          BX_LOCK(cmdfifo_mutex);
        }
        regaddr++;
        mask >>= 1;
      }
      break;
    case 3:
      nwords = (command >> 29);
      pcolor = (command >> 28) & 1;
      smode = (command >> 22) & 0x3f;
      mask = (command >> 10) & 0xff;
      nvertex = (command >> 6) & 0x0f;
      code = (command >> 3) & 0x07;
      /* copy relevant bits into the setup mode register */
      v->reg[sSetupMode].u = ((smode << 16) | mask);
      /* loop over triangles */
      for (i = 0; i < nvertex; i++) {
        reg.u = cmdfifo_r(f);
        svert.x = reg.f;
        reg.u = cmdfifo_r(f);
        svert.y = reg.f;
        if (pcolor) {
          if (mask & 0x03) {
            data = cmdfifo_r(f);
            if (mask & 0x01) {
              svert.r = (float)RGB_RED(data);
              svert.g = (float)RGB_GREEN(data);
              svert.b = (float)RGB_BLUE(data);
            }
            if (mask & 0x02) {
              svert.a = (float)RGB_ALPHA(data);
            }
          }
        } else {
          if (mask & 0x01) {
            reg.u = cmdfifo_r(f);
            svert.r = reg.f;
            reg.u = cmdfifo_r(f);
            svert.g = reg.f;
            reg.u = cmdfifo_r(f);
            svert.b = reg.f;
          }
          if (mask & 0x02) {
            reg.u = cmdfifo_r(f);
            svert.a = reg.f;
          }
        }
        if (mask & 0x04) {
          reg.u = cmdfifo_r(f);
          svert.z = reg.f;
        }
        if (mask & 0x08) {
          reg.u = cmdfifo_r(f);
          svert.wb = reg.f;
        }
        if (mask & 0x10) {
          reg.u = cmdfifo_r(f);
          svert.w0 = reg.f;
        }
        if (mask & 0x20) {
          reg.u = cmdfifo_r(f);
          svert.s0 = reg.f;
          reg.u = cmdfifo_r(f);
          svert.t0 = reg.f;
        }
        if (mask & 0x40) {
          reg.u = cmdfifo_r(f);
          svert.w1 = reg.f;
        }
        if (mask & 0x80) {
          reg.u = cmdfifo_r(f);
          svert.s1 = reg.f;
          reg.u = cmdfifo_r(f);
          svert.t1 = reg.f;
        }
        /* if we're starting a new strip, or if this is the first of a set of verts */
        /* for a series of individual triangles, initialize all the verts */
        if ((code == 1 && i == 0) || (code == 0 && i % 3 == 0)) {
          v->fbi.sverts = 1;
          v->fbi.svert[0] = v->fbi.svert[1] = v->fbi.svert[2] = svert;
        } else { /* otherwise, add this to the list */
          /* for strip mode, shuffle vertex 1 down to 0 */
          if (!(smode & 1))
            v->fbi.svert[0] = v->fbi.svert[1];

          /* copy 2 down to 1 and add our new one regardless */
          v->fbi.svert[1] = v->fbi.svert[2];
          v->fbi.svert[2] = svert;

          /* if we have enough, draw */
          if (++v->fbi.sverts >= 3) {
            BX_UNLOCK(cmdfifo_mutex);
            setup_and_draw_triangle();
            BX_LOCK(cmdfifo_mutex);
          }
        }
      }
      while (nwords--) cmdfifo_r(f);
      break;
    case 4:
      nwords = (command >> 29);
      mask = (command >> 15) & 0x3fff;
      regaddr = (command & 0x7ff8) >> 3;
      while (mask) {
        if (mask & 1) {
          data = cmdfifo_r(f);
          BX_UNLOCK(cmdfifo_mutex);
          Voodoo_reg_write(regaddr, data);
          BX_LOCK(cmdfifo_mutex);
        }
        regaddr++;
        mask >>= 1;
      }
      while (nwords--) cmdfifo_r(f);
      break;
    case 5:
      nwords = (command >> 3) & 0x7ffff;
      regaddr = (cmdfifo_r(f) & 0xffffff) >> 2;
      code = (command >> 30);
      disbytes = (command >> 22) & 0xff;
      if ((disbytes > 0) && (code != 0) && (code != 3)) {
        BX_ERROR(("CMDFIFO packet type 5: byte disable not supported yet (dest code = %d disbytes = 0x%02x)", code, disbytes));
      }
      switch (code) {
        case 0:
          regaddr <<= 2;
          w0 = 0;
          wn = nwords;
          if ((disbytes & 0xf0) > 0) {
            data = cmdfifo_r(f);
            if ((disbytes & 0xf0) == 0x30) {
              datalen = 2;
              data >>= 16;
            } else if ((disbytes & 0xf0) == 0xc0) {
              datalen = 2;
            } else if ((disbytes & 0xf0) == 0xb0) {
              datalen = 1;
              data >>= 16;
            } else if ((disbytes & 0xf0) == 0xe0) {
              datalen = 1;
            } else {
              BX_ERROR(("CMDFIFO packet type 5: byte disable not complete (dest code = 0)"));
            }
            BX_UNLOCK(cmdfifo_mutex);
            Banshee_LFB_write(regaddr, data, datalen);
            BX_LOCK(cmdfifo_mutex);
            w0++;
            regaddr += 4;
          }
          for (i = w0; i < wn; i++) {
            data = cmdfifo_r(f);
            BX_UNLOCK(cmdfifo_mutex);
            Banshee_LFB_write(regaddr, data, 4);
            BX_LOCK(cmdfifo_mutex);
            regaddr += 4;
          }
          if ((disbytes & 0x0f) > 0) {
            BX_ERROR(("CMDFIFO packet type 5: byte disable not complete (dest code = 0)"));
          }
          break;
        case 2:
          for (i = 0; i < (int)nwords; i++) {
            data = cmdfifo_r(f);
            BX_UNLOCK(cmdfifo_mutex);
            lfb_w(regaddr, data, 0xffffffff);
            BX_LOCK(cmdfifo_mutex);
            regaddr++;
          }
          break;
        case 3:
          w0 = 0;
          wn = nwords;
          if ((disbytes & 0xf0) > 0) {
            data = cmdfifo_r(f);
            if ((disbytes & 0xf0) == 0x30) {
              data >>= 16;
            } else if ((disbytes & 0xf0) == 0xc0) {
              data &= 0xffff;
            } else if ((disbytes & 0xf0) == 0xe0) {
              data &= 0xff;
            } else {
              BX_ERROR(("CMDFIFO packet type 5: byte disable not complete (dest code = 3)"));
            }
            BX_UNLOCK(cmdfifo_mutex);
            texture_w(regaddr, data);
            BX_LOCK(cmdfifo_mutex);
            w0++;
            regaddr++;
          }
          for (i = w0; i < wn; i++) {
            data = cmdfifo_r(f);
            BX_UNLOCK(cmdfifo_mutex);
            texture_w(regaddr, data);
            BX_LOCK(cmdfifo_mutex);
            regaddr++;
          }
          if ((disbytes & 0x0f) > 0) {
            BX_ERROR(("CMDFIFO packet type 5: byte disable not complete (dest code = 3)"));
          }
          break;
        default:
          BX_ERROR(("CMDFIFO packet type 5: unsupported destination type %d", code));
      }
      break;
    case 6:
      // TODO: AGP to VRAM transfer
      cmdfifo_r(f);
      cmdfifo_r(f);
      cmdfifo_r(f);
      cmdfifo_r(f);
    default:
      BX_ERROR(("CMDFIFO: unsupported packet type %d", type));
  }
  f->depth_needed = cmdfifo_calc_depth_needed(f);
  if (!f->jsr && f->depth < f->depth_needed) {
    f->cmd_ready = 0;
  }
}


#define FBI_TRICK 1
#if FBI_TRICK
bool fifo_add_fbi(Bit32u type_offset, Bit32u data)
{
  bool ret = 0;

  BX_LOCK(fifo_mutex);
  if (v->fbi.fifo.enabled) {
    fifo_add(&v->fbi.fifo, type_offset, data);
    ret = 1;
    if ((fifo_space(&v->fbi.fifo)/2) <= 0xe000)
      bx_set_sem(&fifo_wakeup);
  }
  BX_UNLOCK(fifo_mutex);
  return ret;
}

bool fifo_add_common(Bit32u type_offset, Bit32u data)
{
  bool ret = 0;

  BX_LOCK(fifo_mutex);
  if (v->fbi.fifo.enabled) {
    fifo_add(&v->fbi.fifo, type_offset, data);
    ret = 1;
    if ((fifo_space(&v->fbi.fifo)/2) <= 0xe000)
      bx_set_sem(&fifo_wakeup);
  } else
  if (v->pci.fifo.enabled) {
    fifo_add(&v->pci.fifo, type_offset, data);
    ret = 1;
    if ((fifo_space(&v->pci.fifo)/2) <= 16)
      bx_set_sem(&fifo_wakeup);
  }
  BX_UNLOCK(fifo_mutex);
  return ret;
}
#else
bool fifo_add_common(Bit32u type_offset, Bit32u data)
{
  bool ret = 0;

  BX_LOCK(fifo_mutex);
  if (v->pci.fifo.enabled) {
    fifo_add(&v->pci.fifo, type_offset, data);
    ret = 1;
    if (v->fbi.fifo.enabled) {
      if ((fifo_space(&v->pci.fifo)/2) <= 16) {
        fifo_move(&v->pci.fifo, &v->fbi.fifo);
      }
      if ((fifo_space(&v->fbi.fifo)/2) <= 0xe000) {
        bx_set_sem(&fifo_wakeup);
      }
    } else {
      if ((fifo_space(&v->pci.fifo)/2) <= 16) {
        bx_set_sem(&fifo_wakeup);
      }
    }
  }
  BX_UNLOCK(fifo_mutex);
  return ret;
}
#endif


void register_w_common(Bit32u offset, Bit32u data)
{
  Bit32u regnum  = (offset) & 0xff;
  Bit32u chips   = (offset>>8) & 0xf;

  /* Voodoo 2 CMDFIFO handling */
  if ((v->type == VOODOO_2) && v->fbi.cmdfifo[0].enabled) {
    if ((offset & 0x80000) > 0) {
      if (!FBIINIT7_CMDFIFO_MEMORY_STORE(v->reg[fbiInit7].u)) {
        BX_ERROR(("CMDFIFO-to-FIFO mode not supported yet"));
      } else {
        Bit32u fbi_offset = (v->fbi.cmdfifo[0].base + ((offset & 0xffff) << 2)) & v->fbi.mask;
        if (LOG_CMDFIFO) BX_DEBUG(("CMDFIFO write: FBI offset=0x%08x, data=0x%08x", fbi_offset, data));
        cmdfifo_w(&v->fbi.cmdfifo[0], fbi_offset, data);
      }
      return;
    } else {
      if (v->regaccess[regnum] & REGISTER_WRITETHRU) {
        BX_DEBUG(("Writing to register %s in CMDFIFO mode", v->regnames[regnum]));
      } else if (regnum == swapbufferCMD) {
        v->fbi.swaps_pending++;
        return;
      } else {
        BX_DEBUG(("Invalid attempt to write %s in CMDFIFO mode", v->regnames[regnum]));
        return;
      }
    }
  }

  if (chips == 0)
    chips = 0xf;

  /* the first 64 registers can be aliased differently */
  if ((offset & 0x800c0) == 0x80000 && v->alt_regmap)
    regnum = register_alias_map[offset & 0x3f];
  else
    regnum = offset & 0xff;

  /* first make sure this register is writable */
  if (!(v->regaccess[regnum] & REGISTER_WRITE)) {
    BX_DEBUG(("Invalid attempt to write %s", v->regnames[regnum]));
    return;
  }

  BX_DEBUG(("write chip 0x%x reg 0x%x value 0x%08x(%s)", chips, regnum<<2, data, v->regnames[regnum]));

  switch (regnum) {
    /* external DAC access -- Voodoo/Voodoo2 only */
    case dacData:
      if (v->type <= VOODOO_2 /*&& (chips & 1)*/)
      {
        poly_wait(v->poly, v->regnames[regnum]);
        if (!(data & 0x800))
          dacdata_w(&v->dac, (data >> 8) & 7, data & 0xff);
        else
          dacdata_r(&v->dac, (data >> 8) & 7);
      }
      break;

    /* vertical sync rate -- Voodoo/Voodoo2 only */
    case hSync:
    case vSync:
    case backPorch:
    case videoDimensions:
      if (v->type <= VOODOO_2 && (chips & 1))
      {
        poly_wait(v->poly, v->regnames[regnum]);
        v->reg[regnum].u = data;
        if (v->reg[hSync].u != 0 && v->reg[vSync].u != 0 && v->reg[videoDimensions].u != 0)
        {
          int htotal = ((v->reg[hSync].u >> 16) & 0x3ff) + 1 + (v->reg[hSync].u & 0xff) + 1;
          int vtotal = ((v->reg[vSync].u >> 16) & 0xfff) + (v->reg[vSync].u & 0xfff);
          int hvis = v->reg[videoDimensions].u & 0x3ff;
          int vvis = (v->reg[videoDimensions].u >> 16) & 0x3ff;
          int hbp = (v->reg[backPorch].u & 0xff) + 2;
          int vbp = (v->reg[backPorch].u >> 16) & 0xff;
          rectangle visarea;

          /* create a new visarea */
          visarea.min_x = hbp;
          visarea.max_x = hbp + hvis - 1;
          visarea.min_y = vbp;
          visarea.max_y = vbp + vvis - 1;

          /* keep within bounds */
          visarea.max_x = MIN(visarea.max_x, htotal - 1);
          visarea.max_y = MIN(visarea.max_y, vtotal - 1);

          BX_DEBUG(("hSync=%08X  vSync=%08X  backPorch=%08X  videoDimensions=%08X",
            v->reg[hSync].u, v->reg[vSync].u, v->reg[backPorch].u, v->reg[videoDimensions].u));
          BX_DEBUG(("Horiz: %d-%d (%d total)  Vert: %d-%d (%d total) -- ", visarea.min_x, visarea.max_x, htotal, visarea.min_y, visarea.max_y, vtotal));

          /* configure the new framebuffer info */
          v->fbi.width = hvis + 1;
          v->fbi.height = vvis;
          v->fbi.xoffs = hbp;
          v->fbi.yoffs = vbp;
          v->fbi.vsyncscan = (v->reg[vSync].u >> 16) & 0xfff;

          /* if changing dimensions, update video memory layout */
          if (regnum == videoDimensions)
            recompute_video_memory(v);

          Voodoo_UpdateScreenStart();
        }
      }
      break;

    /* fbiInit0 can only be written if initEnable says we can -- Voodoo/Voodoo2 only */
    case fbiInit0:
      poly_wait(v->poly, v->regnames[regnum]);
      if (v->type <= VOODOO_2 && (chips & 1) && INITEN_ENABLE_HW_INIT(v->pci.init_enable)) {
        Voodoo_Output_Enable(data & 1);
        if (v->fbi.fifo.enabled != FBIINIT0_ENABLE_MEMORY_FIFO(data)) {
          v->fbi.fifo.enabled = FBIINIT0_ENABLE_MEMORY_FIFO(data);
          BX_INFO(("memory FIFO now %sabled",
                   v->fbi.fifo.enabled ? "en" : "dis"));
        }
        v->reg[fbiInit0].u = data;
        if (FBIINIT0_GRAPHICS_RESET(data))
          soft_reset(v);
        if (FBIINIT0_FIFO_RESET(data))
          fifo_reset(&v->pci.fifo);
        recompute_video_memory(v);
      }
      break;

    /* fbiInitX can only be written if initEnable says we can -- Voodoo/Voodoo2 only */
    /* most of these affect memory layout, so always recompute that when done */
    case fbiInit1:
    case fbiInit2:
    case fbiInit4:
    case fbiInit5:
    case fbiInit6:
      poly_wait(v->poly, v->regnames[regnum]);

      if (v->type <= VOODOO_2 && (chips & 1) && INITEN_ENABLE_HW_INIT(v->pci.init_enable))
      {
        v->reg[regnum].u = data;
        recompute_video_memory(v);
        v->fbi.video_changed = 1;
        v->fbi.clut_dirty = 1;
      }
      break;

    case fbiInit3:
      poly_wait(v->poly, v->regnames[regnum]);
      if (v->type <= VOODOO_2 && (chips & 1) && INITEN_ENABLE_HW_INIT(v->pci.init_enable))
      {
        v->reg[regnum].u = data;
        v->alt_regmap = FBIINIT3_TRI_REGISTER_REMAP(data);
        v->fbi.yorigin = FBIINIT3_YORIGIN_SUBTRACT(v->reg[fbiInit3].u);
        recompute_video_memory(v);
      }
      break;

    case fbiInit7:
/*  case swapPending: -- Banshee */
      poly_wait(v->poly, v->regnames[regnum]);

      if (v->type == VOODOO_2 && (chips & 1) && INITEN_ENABLE_HW_INIT(v->pci.init_enable))
      {
        v->fbi.cmdfifo[0].count_holes = !FBIINIT7_DISABLE_CMDFIFO_HOLES(data);
        if (v->fbi.cmdfifo[0].enabled != FBIINIT7_CMDFIFO_ENABLE(data)) {
          v->fbi.cmdfifo[0].enabled = FBIINIT7_CMDFIFO_ENABLE(data);
          BX_INFO(("CMDFIFO now %sabled", v->fbi.cmdfifo[0].enabled ? "en" : "dis"));
        }
        v->reg[regnum].u = data;
      } else if (v->type >= VOODOO_BANSHEE) {
        v->fbi.swaps_pending++;
      }
      break;

    case cmdFifoBaseAddr:
      BX_LOCK(cmdfifo_mutex);
      v->fbi.cmdfifo[0].base = (data & 0x3ff) << 12;
      v->fbi.cmdfifo[0].end = (((data >> 16) & 0x3ff) + 1) << 12;
      BX_UNLOCK(cmdfifo_mutex);
      break;

    case cmdFifoRdPtr:
      BX_LOCK(cmdfifo_mutex);
      v->fbi.cmdfifo[0].rdptr = data;
      BX_UNLOCK(cmdfifo_mutex);
      break;

    case cmdFifoAMin:
/*  case colBufferAddr: -- Banshee */
      if (v->type == VOODOO_2 && (chips & 1)) {
        BX_LOCK(cmdfifo_mutex);
        v->fbi.cmdfifo[0].amin = data;
        BX_UNLOCK(cmdfifo_mutex);
      } else if (v->type >= VOODOO_BANSHEE && (chips & 1))
        v->fbi.rgboffs[1] = data & v->fbi.mask & ~0x0f;
      break;

    case cmdFifoAMax:
/*  case colBufferStride: -- Banshee */
      if (v->type == VOODOO_2 && (chips & 1)) {
        BX_LOCK(cmdfifo_mutex);
        v->fbi.cmdfifo[0].amax = data;
        BX_UNLOCK(cmdfifo_mutex);
      } else if (v->type >= VOODOO_BANSHEE && (chips & 1)) {
        if (data & 0x8000)
          v->fbi.rowpixels = (data & 0x7f) << 6;
        else
          v->fbi.rowpixels = (data & 0x3fff) >> 1;
      }
      break;

    case cmdFifoDepth:
/*  case auxBufferAddr: -- Banshee */
      if (v->type == VOODOO_2 && (chips & 1)) {
        BX_LOCK(cmdfifo_mutex);
        v->fbi.cmdfifo[0].depth = data & 0xffff;
        v->fbi.cmdfifo[0].depth_needed = BX_MAX_BIT32U;
        BX_UNLOCK(cmdfifo_mutex);
      } else if (v->type >= VOODOO_BANSHEE && (chips & 1)) {
        v->fbi.auxoffs = data & v->fbi.mask & ~0x0f;
      }
      break;

    case cmdFifoHoles:
/*  case auxBufferStride: -- Banshee */
      if (v->type == VOODOO_2 && (chips & 1)) {
        BX_LOCK(cmdfifo_mutex);
        v->fbi.cmdfifo[0].holes = data;
        BX_UNLOCK(cmdfifo_mutex);
      } else if (v->type >= VOODOO_BANSHEE && (chips & 1)) {
        Bit32u rowpixels;

        if (data & 0x8000)
          rowpixels = (data & 0x7f) << 6;
        else
          rowpixels = (data & 0x3fff) >> 1;
        if (v->fbi.rowpixels != rowpixels)
          BX_PANIC(("aux buffer stride differs from color buffer stride"));
      }
      break;

    case intrCtrl:
      BX_ERROR(("Writing to register %s not supported yet", v->regnames[regnum]));
      break;

    default:
      if (fifo_add_common(FIFO_WR_REG | offset, data)) {
        BX_LOCK(fifo_mutex);
        if ((regnum == triangleCMD) || (regnum == ftriangleCMD) || (regnum == nopCMD) ||
            (regnum == fastfillCMD) || (regnum == swapbufferCMD)) {
          v->pci.op_pending++;
          if (regnum == swapbufferCMD) {
            v->fbi.swaps_pending++;
          }
          bx_set_sem(&fifo_wakeup);
        }
        BX_UNLOCK(fifo_mutex);
      } else {
        register_w(offset, data, 0);
      }
  }
}


Bit32u register_r(Bit32u offset)
{
  Bit32u regnum  = (offset) & 0xff;
  Bit32u chips   = (offset>>8) & 0xf;

  if (!((voodoo_last_msg == regnum) && (regnum == status))) //show status reg only once
    BX_DEBUG(("read chip 0x%x reg 0x%x (%s)", chips, regnum<<2, v->regnames[regnum]));
  voodoo_last_msg = regnum;

  /* first make sure this register is readable */
  if (!(v->regaccess[regnum] & REGISTER_READ)) {
    BX_DEBUG(("Invalid attempt to read %s", v->regnames[regnum]));
    return 0;
  }
  if ((v->type == VOODOO_2) && v->fbi.cmdfifo[0].enabled && ((offset & 0x80000) > 0)) {
    BX_DEBUG(("Invalid attempt to read from CMDFIFO"));
    return 0;
  }

  Bit32u result;

  /* default result is the FBI register value */
  result = v->reg[regnum].u;

  /* some registers are dynamic; compute them */
  switch (regnum) {
    case status:

      /* start with a blank slate */
      result = 0;

      /* bits 5:0 are the PCI FIFO free space */
      if (fifo_empty_locked(&v->pci.fifo))
        result |= 0x3f << 0;
      else
      {
        BX_LOCK(fifo_mutex);
        int temp = fifo_space(&v->pci.fifo)/2;
        BX_UNLOCK(fifo_mutex);
        if (temp > 0x3f)
          temp = 0x3f;
        result |= temp << 0;
      }

      /* bit 6 is the vertical retrace */
      result |= (Voodoo_get_retrace(0) > 0) << 6;

      /* bit 7 is FBI graphics engine busy */
      if (v->pci.op_pending)
        result |= 1 << 7;

      /* bit 8 is TREX busy */
      if (v->pci.op_pending)
        result |= 1 << 8;

      /* bit 9 is overall busy */
      if (v->pci.op_pending)
        result |= 1 << 9;

      if (v->type == VOODOO_2) {
        if (v->fbi.cmdfifo[0].enabled && v->fbi.cmdfifo[0].depth > 0)
          result |= 7 << 7;
      }
      /* Banshee is different starting here */
      if (v->type < VOODOO_BANSHEE)
      {
        /* bits 11:10 specifies which buffer is visible */
        result |= v->fbi.frontbuf << 10;

        /* bits 27:12 indicate memory FIFO freespace */
        if (!v->fbi.fifo.enabled || fifo_empty_locked(&v->fbi.fifo))
          result |= 0xffff << 12;
        else
        {
          BX_LOCK(fifo_mutex);
          int temp = fifo_space(&v->fbi.fifo)/2;
          BX_UNLOCK(fifo_mutex);
          if (temp > 0xffff)
            temp = 0xffff;
          result |= temp << 12;
        }
      }
      else
      {
        /* bit 10 is 2D busy */
        if (v->banshee.blt.busy)
          result |= 3 << 9;

        /* bit 11 is cmd FIFO 0 busy */
        if (v->fbi.cmdfifo[0].enabled && v->fbi.cmdfifo[0].depth > 0)
          result |= 5 << 9;

        /* bit 12 is cmd FIFO 1 busy */
        if (v->fbi.cmdfifo[1].enabled && v->fbi.cmdfifo[1].depth > 0)
          result |= 9 << 9;
      }

      /* bits 30:28 are the number of pending swaps */
      if (v->fbi.swaps_pending > 7)
        result |= 7 << 28;
      else
        result |= v->fbi.swaps_pending << 28;

      /* bit 31 is not used */

      /* eat some cycles since people like polling here */
      cpu_eat_cycles(v->cpu, 1000);
      break;

    /* bit 2 of the initEnable register maps this to dacRead */
    case fbiInit2:
      if (INITEN_REMAP_INIT_TO_DAC(v->pci.init_enable))
        result = v->dac.read_result;
      break;

    case vRetrace:
      result = Voodoo_get_retrace(0) & 0x1fff;
      break;

    case hvRetrace:
      result = Voodoo_get_retrace(1);
      break;

    case cmdFifoBaseAddr:
      result = (v->fbi.cmdfifo[0].base >> 12) | ((v->fbi.cmdfifo[0].end >> 12) << 16);
      break;

    case cmdFifoRdPtr:
      result = v->fbi.cmdfifo[0].rdptr;
      break;

    case cmdFifoDepth:
      result = v->fbi.cmdfifo[0].depth;
      break;

    case cmdFifoAMin:
      result = v->fbi.cmdfifo[0].amin;
      break;

    case cmdFifoAMax:
      result = v->fbi.cmdfifo[0].amax;
      break;
  }

  return result;
}

Bit32u lfb_r(Bit32u offset)
{
  Bit16u *buffer;
  Bit32u bufmax;
  Bit32u bufoffs;
  Bit32u data;
  bool forcefront=false;
  int x, y, scry;
  Bit32u destbuf;

  BX_DEBUG(("read LFB offset 0x%x", offset));

  /* compute X,Y */
  x = (offset << 1) & 0x3fe;
  y = (offset >> 9) & 0x7ff;

  /* select the target buffer */
  destbuf = (v->type >= VOODOO_BANSHEE) ? (!forcefront) : LFBMODE_READ_BUFFER_SELECT(v->reg[lfbMode].u);
  switch (destbuf)
  {
    case 0:     /* front buffer */
      buffer = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.frontbuf]);
      bufmax = (v->fbi.mask + 1 - v->fbi.rgboffs[v->fbi.frontbuf]) / 2;
      break;

    case 1:     /* back buffer */
      buffer = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[v->fbi.backbuf]);
      bufmax = (v->fbi.mask + 1 - v->fbi.rgboffs[v->fbi.backbuf]) / 2;
      break;

    case 2:     /* aux buffer */
      if (v->fbi.auxoffs == (Bit32u)~0)
        return 0xffffffff;
      buffer = (Bit16u *)(v->fbi.ram + v->fbi.auxoffs);
      bufmax = (v->fbi.mask + 1 - v->fbi.auxoffs) / 2;
      break;

    default:    /* reserved */
      return 0xffffffff;
  }

  /* determine the screen Y */
  scry = y;
  if (LFBMODE_Y_ORIGIN(v->reg[lfbMode].u))
    scry = (v->fbi.yorigin - y) & v->fbi.clip_mask;

  /* advance pointers to the proper row */
  bufoffs = scry * v->fbi.rowpixels + x;
  if (bufoffs >= bufmax)
    return 0xffffffff;

  /* wait for any outstanding work to finish */
  poly_wait(v->poly, "LFB read");

  /* compute the data */
  data = buffer[bufoffs + 0] | (buffer[bufoffs + 1] << 16);

  /* word swapping */
  if (LFBMODE_WORD_SWAP_READS(v->reg[lfbMode].u))
    data = (data << 16) | (data >> 16);

  /* byte swizzling */
  if (LFBMODE_BYTE_SWIZZLE_READS(v->reg[lfbMode].u))
    data = bx_bswap32(data);

  if (LOG_LFB) BX_DEBUG(("VOODOO.%d.LFB:read (%d,%d) = %08X", v->index, x, y, data));
  return data;
}

void voodoo_w(Bit32u offset, Bit32u data, Bit32u mask)
{
  Bit32u type;

  if ((offset & (0xc00000/4)) == 0)
    register_w_common(offset, data);
  else if (offset & (0x800000/4)) {
    if (!fifo_add_common(FIFO_WR_TEX | offset, data)) {
      texture_w(offset, data);
    }
  } else {
    if (mask == 0xffffffff) {
      type = FIFO_WR_FBI_32;
    } else if (mask & 1) {
      type = FIFO_WR_FBI_16L;
    } else {
      type = FIFO_WR_FBI_16H;
    }
#if FBI_TRICK
    if (!fifo_add_fbi(type | offset, data)) {
#else
    if (!fifo_add_common(type | offset, data)) {
#endif
      lfb_w(offset, data, mask);
    }
  }
}

Bit32u voodoo_r(Bit32u offset)
{
  if (!(offset & (0xc00000/4)))
    return register_r(offset);
  else
    return lfb_r(offset);

  return 0xffffffff;
}

void init_tmu(voodoo_state *v, tmu_state *t, voodoo_reg *reg, void *memory, int tmem)
{
  /* allocate texture RAM */
  t->ram = (Bit8u *)memory;
  t->mask = tmem - 1;
  t->reg = reg;
  t->regdirty = 1;
  t->bilinear_mask = (v->type >= VOODOO_2) ? 0xff : 0xf0;

  /* mark the NCC tables dirty and configure their registers */
  t->ncc[0].dirty = t->ncc[1].dirty = 1;
  t->ncc[0].reg = &t->reg[nccTable+0];
  t->ncc[1].reg = &t->reg[nccTable+12];

  /* create pointers to all the tables */
  t->texel[0] = v->tmushare.rgb332;
  t->texel[1] = t->ncc[0].texel;
  t->texel[2] = v->tmushare.alpha8;
  t->texel[3] = v->tmushare.int8;
  t->texel[4] = v->tmushare.ai44;
  t->texel[5] = t->palette;
  t->texel[6] = (v->type >= VOODOO_2) ? t->palettea : NULL;
  t->texel[7] = NULL;
  t->texel[8] = v->tmushare.rgb332;
  t->texel[9] = t->ncc[0].texel;
  t->texel[10] = v->tmushare.rgb565;
  t->texel[11] = v->tmushare.argb1555;
  t->texel[12] = v->tmushare.argb4444;
  t->texel[13] = v->tmushare.int8;
  t->texel[14] = t->palette;
  t->texel[15] = NULL;
  t->lookup = t->texel[0];

  /* attach the palette to NCC table 0 */
  t->ncc[0].palette = t->palette;
  if (v->type >= VOODOO_2)
    t->ncc[0].palettea = t->palettea;

  /* set up texture address calculations */
  if (v->type <= VOODOO_2)
  {
    t->texaddr_mask = 0x0fffff;
    t->texaddr_shift = 3;
  } else {
    t->texaddr_mask = 0xfffff0;
    t->texaddr_shift = 0;
  }
}

void init_tmu_shared(tmu_shared_state *s)
{
  int val;

  /* build static 8-bit texel tables */
  for (val = 0; val < 256; val++) {
    int r, g, b, a;

    /* 8-bit RGB (3-3-2) */
    EXTRACT_332_TO_888(val, r, g, b);
    s->rgb332[val] = MAKE_ARGB(0xff, r, g, b);

    /* 8-bit alpha */
    s->alpha8[val] = MAKE_ARGB(val, val, val, val);

    /* 8-bit intensity */
    s->int8[val] = MAKE_ARGB(0xff, val, val, val);

    /* 8-bit alpha, intensity */
    a = ((val >> 0) & 0xf0) | ((val >> 4) & 0x0f);
    r = ((val << 4) & 0xf0) | ((val << 0) & 0x0f);
    s->ai44[val] = MAKE_ARGB(a, r, r, r);
  }

  /* build static 16-bit texel tables */
  for (val = 0; val < 65536; val++) {
    int r, g, b, a;

    /* table 10 = 16-bit RGB (5-6-5) */
    EXTRACT_565_TO_888(val, r, g, b);
    s->rgb565[val] = MAKE_ARGB(0xff, r, g, b);

    /* table 11 = 16 ARGB (1-5-5-5) */
    EXTRACT_1555_TO_8888(val, a, r, g, b);
    s->argb1555[val] = MAKE_ARGB(a, r, g, b);

    /* table 12 = 16-bit ARGB (4-4-4-4) */
    EXTRACT_4444_TO_8888(val, a, r, g, b);
    s->argb4444[val] = MAKE_ARGB(a, r, g, b);
  }
}

#define SETUP_BITBLT(num, name, flags) \
  do { \
    v->banshee.blt.rop_handler[0][num] = bitblt_rop_fwd_##name; \
    v->banshee.blt.rop_handler[1][num] = bitblt_rop_bkwd_##name; \
    v->banshee.blt.rop_flags[num] = flags; \
  } while (0);

void banshee_bitblt_init()
{
  for (int i = 0; i < 0x100; i++) {
    SETUP_BITBLT(i, nop, BX_ROP_PATTERN);
  }
  SETUP_BITBLT(0x00, 0, 0);                              // 0
  SETUP_BITBLT(0x05, notsrc_and_notdst, BX_ROP_PATTERN); // PSan
  SETUP_BITBLT(0x0a, notsrc_and_dst, BX_ROP_PATTERN);    // DPna
  SETUP_BITBLT(0x0f, notsrc, BX_ROP_PATTERN);            // Pn
  SETUP_BITBLT(0x11, notsrc_and_notdst, 0);              // DSon
  SETUP_BITBLT(0x22, notsrc_and_dst, 0);                 // DSna
  SETUP_BITBLT(0x33, notsrc, 0);                         // Sn
  SETUP_BITBLT(0x44, src_and_notdst, 0);                 // SDna
  SETUP_BITBLT(0x50, src_and_notdst, 0);                 // PDna
  SETUP_BITBLT(0x55, notdst, 0);                         // Dn
  SETUP_BITBLT(0x5a, src_xor_dst, BX_ROP_PATTERN);       // DPx
  SETUP_BITBLT(0x5f, notsrc_or_notdst, BX_ROP_PATTERN);  // DSan
  SETUP_BITBLT(0x66, src_xor_dst, 0);                    // DSx
  SETUP_BITBLT(0x77, notsrc_or_notdst, 0);               // DSan
  SETUP_BITBLT(0x88, src_and_dst, 0);                    // DSa
  SETUP_BITBLT(0x99, src_notxor_dst, 0);                 // DSxn
  SETUP_BITBLT(0xaa, nop, 0);                            // D
  SETUP_BITBLT(0xad, src_and_dst, BX_ROP_PATTERN);       // DPa
  SETUP_BITBLT(0xaf, notsrc_or_dst, BX_ROP_PATTERN);     // DPno
  SETUP_BITBLT(0xbb, notsrc_or_dst, 0);                  // DSno
  SETUP_BITBLT(0xcc, src, 0);                            // S
  SETUP_BITBLT(0xdd, src_and_notdst, 0);                 // SDna
  SETUP_BITBLT(0xee, src_or_dst, 0);                     // DSo
  SETUP_BITBLT(0xf0, src, BX_ROP_PATTERN);               // P
  SETUP_BITBLT(0xf5, src_or_notdst, BX_ROP_PATTERN);     // PDno
  SETUP_BITBLT(0xfa, src_or_dst, BX_ROP_PATTERN);        // DPo
  SETUP_BITBLT(0xff, 1, 0);                              // 1
}

void voodoo_init(Bit8u _type)
{
  int pen;
  int val;

  v->reg[lfbMode].u = 0;
  v->reg[fbiInit0].u = (1 << 4) | (0x10 << 6);
  v->reg[fbiInit1].u = (1 << 1) | (1 << 8) | (1 << 12) | (2 << 20);
  v->reg[fbiInit2].u = (1 << 6) | (0x100 << 23);
  v->reg[fbiInit3].u = (2 << 13) | (0xf << 17);
  v->reg[fbiInit4].u = (1 << 0);
  v->type = _type;
  v->chipmask = 0x01 | 0x02 | 0x04 | 0x08;
  switch (v->type) {
    case VOODOO_1:
      v->regaccess = voodoo_register_access;
      v->regnames = voodoo_reg_name;
      v->alt_regmap = 0;
      v->fbi.lfb_stride = 10;
      break;

    case VOODOO_2:
      v->regaccess = voodoo2_register_access;
      v->regnames = voodoo_reg_name;
      v->alt_regmap = 0;
      v->fbi.lfb_stride = 10;
      break;

    case VOODOO_BANSHEE:
      v->regaccess = banshee_register_access;
      v->regnames = banshee_reg_name;
      v->alt_regmap = 1;
      v->fbi.lfb_stride = 11;
      v->chipmask = 0x01 | 0x02;
      break;

    case VOODOO_3:
      v->regaccess = banshee_register_access;
      v->regnames = banshee_reg_name;
      v->alt_regmap = 1;
      v->fbi.lfb_stride = 11;
      v->chipmask = 0x01 | 0x02 | 0x04;
      break;
  }
  memset(v->dac.reg, 0, sizeof(v->dac.reg));
  v->dac.read_result = 0;
  v->dac.clk0_m = 0x37;
  v->dac.clk0_n = 0x02;
  v->dac.clk0_p = 0x03;

  /* set up the PCI FIFO */
  v->pci.fifo.base = v->pci.fifo_mem;
  v->pci.fifo.size = 64*2;
  v->pci.fifo.in = v->pci.fifo.out = 0;

  /* create a table of precomputed log2(n) values */
  /* n ranges from 1.0000 to 2.0000 */
  for (val = 0; val < (1 << LOG_LOOKUP_BITS); val++) {
    Bit32u value = (1 << LOG_LOOKUP_BITS) + val;
    voodoo_log[val] = (Bit32u)(LOGB2((double)value / (double)(1 << LOG_LOOKUP_BITS)) * (double)(1 << LOG_OUTPUT_PREC));
  }

  /* create dithering tables */
  for (int val = 0; val < 256*16*2; val++) {
    int g = (val >> 0) & 1;
    int x = (val >> 1) & 3;
    int color = (val >> 3) & 0xff;
    int y = (val >> 11) & 3;

    if (!g) {
      dither4_lookup[val] = DITHER_RB(color, dither_matrix_4x4[y * 4 + x]) >> 3;
      dither2_lookup[val] = DITHER_RB(color, dither_matrix_2x2[y * 4 + x]) >> 3;
    } else {
      dither4_lookup[val] = DITHER_G(color, dither_matrix_4x4[y * 4 + x]) >> 2;
      dither2_lookup[val] = DITHER_G(color, dither_matrix_2x2[y * 4 + x]) >> 2;
    }
  }

  /* init the pens */
  v->fbi.clut_dirty = 1;
  if (v->type <= VOODOO_2) {
    for (pen = 0; pen < 32; pen++)
      v->fbi.clut[pen] = MAKE_ARGB(pen, pal5bit(pen), pal5bit(pen), pal5bit(pen));
    v->fbi.clut[32] = MAKE_ARGB(32,0xff,0xff,0xff);
  } else {
    for (pen = 0; pen < 512; pen++)
      v->fbi.clut[pen] = MAKE_RGB(pen,pen,pen);
  }
  if (v->type < VOODOO_BANSHEE) {
    v->fbi.ram = (Bit8u*)malloc(4<<20);
    v->fbi.mask = (4<<20)-1;
  } else {
    v->fbi.ram = (Bit8u*)malloc(16<<20);
    v->fbi.mask = (16<<20)-1;
  }
  v->fbi.frontbuf = 0;
  v->fbi.backbuf = 1;
  v->fbi.width = 640;
  v->fbi.height = 480;
  v->fbi.rowpixels = v->fbi.width;
  v->fbi.fogdelta_mask = (v->type < VOODOO_2) ? 0xff : 0xfc;
  v->fbi.clip_mask = (v->type <= VOODOO_2) ? 0x3ff : 0xfff;

  /* build shared TMU tables */
  init_tmu_shared(&v->tmushare);

  init_tmu(v, &v->tmu[0], &v->reg[0x100], 0, 4 << 20);
  init_tmu(v, &v->tmu[1], &v->reg[0x200], 0, 4 << 20);

  v->tmu[0].reg = &v->reg[0x100];
  v->tmu[1].reg = &v->reg[0x200];

  if (v->type < VOODOO_BANSHEE) {
    v->tmu[0].ram = (Bit8u*)malloc(4<<20);
    v->tmu[1].ram = (Bit8u*)malloc(4<<20);
    v->tmu[0].mask = (4<<20)-1;
    v->tmu[1].mask = (4<<20)-1;
  } else {
    v->tmu[0].ram = v->fbi.ram;
    v->tmu[1].ram = v->fbi.ram;
    v->tmu[0].mask = (16<<20)-1;
    v->tmu[1].mask = (16<<20)-1;
  }

  v->tmu_config = 64;

  v->thread_stats = new stats_block[16];

  soft_reset(v);
}

void update_pens(void)
{
  int x, y;

  /* if the CLUT is dirty, recompute the pens array */
  if (v->fbi.clut_dirty) {
    Bit8u rtable[32], gtable[64], btable[32];

    /* Voodoo/Voodoo-2 have an internal 33-entry CLUT */
    if (v->type <= VOODOO_2) {
      /* kludge: some of the Midway games write 0 to the last entry when they obviously mean FF */
      if ((v->fbi.clut[32] & 0xffffff) == 0 && (v->fbi.clut[31] & 0xffffff) != 0)
        v->fbi.clut[32] = 0x20ffffff;

      /* compute the R/G/B pens first */
      for (x = 0; x < 32; x++) {
        /* treat X as a 5-bit value, scale up to 8 bits, and linear interpolate for red/blue */
        y = (x << 3) | (x >> 2);
        rtable[x] = (RGB_RED(v->fbi.clut[y >> 3]) * (8 - (y & 7)) + RGB_RED(v->fbi.clut[(y >> 3) + 1]) * (y & 7)) >> 3;
        btable[x] = (RGB_BLUE(v->fbi.clut[y >> 3]) * (8 - (y & 7)) + RGB_BLUE(v->fbi.clut[(y >> 3) + 1]) * (y & 7)) >> 3;

        /* treat X as a 6-bit value with LSB=0, scale up to 8 bits, and linear interpolate */
        y = (x * 2) + 0;
        y = (y << 2) | (y >> 4);
        gtable[x*2+0] = (RGB_GREEN(v->fbi.clut[y >> 3]) * (8 - (y & 7)) + RGB_GREEN(v->fbi.clut[(y >> 3) + 1]) * (y & 7)) >> 3;

        /* treat X as a 6-bit value with LSB=1, scale up to 8 bits, and linear interpolate */
        y = (x * 2) + 1;
        y = (y << 2) | (y >> 4);
        gtable[x*2+1] = (RGB_GREEN(v->fbi.clut[y >> 3]) * (8 - (y & 7)) + RGB_GREEN(v->fbi.clut[(y >> 3) + 1]) * (y & 7)) >> 3;
      }
    }

    /* Banshee and later have a 512-entry CLUT that can be bypassed */
    else
    {
      int mode3d = (v->banshee.io[io_vidProcCfg] >> 8) & 1;
      int which = (v->banshee.io[io_vidProcCfg] >> (12 + mode3d)) & 1;
      int bypass = (v->banshee.io[io_vidProcCfg] >> (10 + mode3d)) & 1;

      /* compute R/G/B pens first */
      for (x = 0; x < 32; x++) {
        /* treat X as a 5-bit value, scale up to 8 bits */
        y = (x << 3) | (x >> 2);
        rtable[x] = bypass ? y : RGB_RED(v->fbi.clut[which * 256 + y]);
        btable[x] = bypass ? y : RGB_BLUE(v->fbi.clut[which * 256 + y]);

        /* treat X as a 6-bit value with LSB=0, scale up to 8 bits */
        y = (x * 2) + 0;
        y = (y << 2) | (y >> 4);
        gtable[x*2+0] = bypass ? y : RGB_GREEN(v->fbi.clut[which * 256 + y]);

        /* treat X as a 6-bit value with LSB=1, scale up to 8 bits, and linear interpolate */
        y = (x * 2) + 1;
        y = (y << 2) | (y >> 4);
        gtable[x*2+1] = bypass ? y : RGB_GREEN(v->fbi.clut[which * 256 + y]);
      }
    }

    /* now compute the actual pens array */
    for (x = 0; x < 65536; x++) {
      int r = rtable[(x >> 11) & 0x1f];
      int g = gtable[(x >> 5) & 0x3f];
      int b = btable[x & 0x1f];
      v->fbi.pen[x] = MAKE_RGB(r, g, b);
    }
    /* no longer dirty */
    v->fbi.clut_dirty = 0;
  }
}
