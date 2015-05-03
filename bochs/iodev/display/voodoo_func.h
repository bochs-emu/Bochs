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

/* fast dither lookup */
static Bit8u dither4_lookup[256*16*2];
static Bit8u dither2_lookup[256*16*2];

/* fast reciprocal+log2 lookup */
Bit32u voodoo_reciplog[(2 << RECIPLOG_LOOKUP_BITS) + 2];


/***************************************************************************
    RASTERIZER MANAGEMENT
***************************************************************************/

/*-------------------------------------------------
    generic_0tmu - generic rasterizer for 0 TMUs
-------------------------------------------------*/

RASTERIZER(generic_0tmu, 0, v->reg[fbzColorPath].u, v->reg[fbzMode].u, v->reg[alphaMode].u,
      v->reg[fogMode].u, 0, 0)


/*-------------------------------------------------
    generic_1tmu - generic rasterizer for 1 TMU
-------------------------------------------------*/

RASTERIZER(generic_1tmu, 1, v->reg[fbzColorPath].u, v->reg[fbzMode].u, v->reg[alphaMode].u,
      v->reg[fogMode].u, v->tmu[0].reg[textureMode].u, 0)


/*-------------------------------------------------
    generic_2tmu - generic rasterizer for 2 TMUs
-------------------------------------------------*/

RASTERIZER(generic_2tmu, 2, v->reg[fbzColorPath].u, v->reg[fbzMode].u, v->reg[alphaMode].u,
      v->reg[fogMode].u, v->tmu[0].reg[textureMode].u, v->tmu[1].reg[textureMode].u)


/*************************************
 *
 *  Specific rasterizers
 *
 *************************************/

#define RASTERIZER_ENTRY(fbzcp, alpha, fog, fbz, tex0, tex1) \
  RASTERIZER(fbzcp##_##alpha##_##fog##_##fbz##_##tex0##_##tex1, (((tex0) == 0xffffffff) ? 0 : ((tex1) == 0xffffffff) ? 1 : 2), fbzcp, fbz, alpha, fog, tex0, tex1)

#include "voodoo_raster.h"

#undef RASTERIZER_ENTRY



/*************************************
 *
 *  Rasterizer table
 *
 *************************************/

#define RASTERIZER_ENTRY(fbzcp, alpha, fog, fbz, tex0, tex1) \
  { NULL, raster_##fbzcp##_##alpha##_##fog##_##fbz##_##tex0##_##tex1, FALSE, 0, 0, 0, fbzcp, alpha, fog, fbz, tex0, tex1 },

static const raster_info predef_raster_table[] =
{
#include "voodoo_raster.h"
  { 0 }
};

#undef RASTERIZER_ENTRY



/***************************************************************************
    RASTERIZER MANAGEMENT
***************************************************************************/

/*-------------------------------------------------
    add_rasterizer - add a rasterizer to our
    hash table
-------------------------------------------------*/

static raster_info *add_rasterizer(voodoo_state *v, const raster_info *cinfo)
{
  raster_info *info = &v->rasterizer[v->next_rasterizer++];
  int hash = compute_raster_hash(cinfo);

  if (v->next_rasterizer > MAX_RASTERIZERS)
  BX_PANIC(("Out of space for new rasterizers!"));

  /* make a copy of the info */
  *info = *cinfo;

  /* fill in the data */
  info->hits = 0;
  info->polys = 0;

  /* hook us into the hash table */
  info->next = v->raster_hash[hash];
  v->raster_hash[hash] = info;

  if (LOG_RASTERIZERS) {
    printf("Adding rasterizer @ %p : %08X %08X %08X %08X %08X %08X (hash=%d)\n",
        info->callback,
        info->eff_color_path, info->eff_alpha_mode, info->eff_fog_mode, info->eff_fbz_mode,
        info->eff_tex_mode_0, info->eff_tex_mode_1, hash);

    printf("RASTERIZER_ENTRY( 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X)\n",
      info->eff_color_path,
      info->eff_alpha_mode,
      info->eff_fog_mode,
      info->eff_fbz_mode,
      info->eff_tex_mode_0,
      info->eff_tex_mode_1,
      info->is_generic ? '*' : ' ',
      info->polys,
      info->hits);
  }
  return info;
}


/*-------------------------------------------------
    find_rasterizer - find a rasterizer that
    matches  our current parameters and return
    it, creating a new one if necessary
-------------------------------------------------*/

static raster_info *find_rasterizer(voodoo_state *v, int texcount)
{
  raster_info *info, *prev = NULL;
  raster_info curinfo;
  int hash;

  /* build an info struct with all the parameters */
  curinfo.eff_color_path = normalize_color_path(v->reg[fbzColorPath].u);
  curinfo.eff_alpha_mode = normalize_alpha_mode(v->reg[alphaMode].u);
  curinfo.eff_fog_mode = normalize_fog_mode(v->reg[fogMode].u);
  curinfo.eff_fbz_mode = normalize_fbz_mode(v->reg[fbzMode].u);
  curinfo.eff_tex_mode_0 = (texcount >= 1) ? normalize_tex_mode(v->tmu[0].reg[textureMode].u) : 0xffffffff;
  curinfo.eff_tex_mode_1 = (texcount >= 2) ? normalize_tex_mode(v->tmu[1].reg[textureMode].u) : 0xffffffff;

  /* compute the hash */
  hash = compute_raster_hash(&curinfo);

  /* find the appropriate hash entry */
  for (info = v->raster_hash[hash]; info; prev = info, info = info->next)
    if (info->eff_color_path == curinfo.eff_color_path &&
      info->eff_alpha_mode == curinfo.eff_alpha_mode &&
      info->eff_fog_mode == curinfo.eff_fog_mode &&
      info->eff_fbz_mode == curinfo.eff_fbz_mode &&
      info->eff_tex_mode_0 == curinfo.eff_tex_mode_0 &&
      info->eff_tex_mode_1 == curinfo.eff_tex_mode_1)
    {
      /* got it, move us to the head of the list */
      if (prev)
      {
        prev->next = info->next;
        info->next = v->raster_hash[hash];
        v->raster_hash[hash] = info;
      }

      /* return the result */
      return info;
    }

  /* generate a new one using the generic entry */
  curinfo.callback = (texcount == 0) ? raster_generic_0tmu : (texcount == 1) ? raster_generic_1tmu : raster_generic_2tmu;
  curinfo.is_generic = TRUE;
  curinfo.display = 0;
  curinfo.polys = 0;
  curinfo.hits = 0;
  curinfo.next = 0;

  return add_rasterizer(v, &curinfo);
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
  Bit64s texdx, texdy;
  Bit32s lodbase;

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

  /* compute (ds^2 + dt^2) in both X and Y as 28.36 numbers */
  texdx = (Bit64s)(t->dsdx >> 14) * (Bit64s)(t->dsdx >> 14) + (Bit64s)(t->dtdx >> 14) * (Bit64s)(t->dtdx >> 14);
  texdy = (Bit64s)(t->dsdy >> 14) * (Bit64s)(t->dsdy >> 14) + (Bit64s)(t->dtdy >> 14) * (Bit64s)(t->dtdy >> 14);

  /* pick whichever is larger and shift off some high bits -> 28.20 */
  if (texdx < texdy)
    texdx = texdy;
  texdx >>= 16;

  /* use our fast reciprocal/log on this value; it expects input as a */
  /* 16.32 number, and returns the log of the reciprocal, so we have to */
  /* adjust the result: negative to get the log of the original value */
  /* plus 12 to account for the extra exponent, and divided by 2 to */
  /* get the log of the square root of texdx */
  (void)fast_reciplog(texdx, &lodbase);
  return (-lodbase + (12 << 8)) / 2;
}


BX_CPP_INLINE Bit32s round_coordinate(float value)
{
  Bit32s result = (Bit32s)floor(value);
  return result + (value - (float)result > 0.5f);
}

Bit32u poly_render_triangle(void *dest, const rectangle *cliprect, poly_draw_scanline_func callback, int paramcount, const poly_vertex *v1, const poly_vertex *v2, const poly_vertex *v3, poly_extra_data *extra)
{
  float dxdy_v1v2, dxdy_v1v3, dxdy_v2v3;
  const poly_vertex *tv;
  Bit32s curscan, scaninc=1;

  Bit32s v1yclip, v3yclip;
  Bit32s v1y, v3y /*, v1x */;
  Bit32s pixels = 0;
//  Bit32u startunit;

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
//  v1x = round_coordinate(v1->x);
  v1y = round_coordinate(v1->y);
  v3y = round_coordinate(v3->y);

  /* clip coordinates */
  v1yclip = v1y;
  v3yclip = v3y;// + ((poly->flags & POLYFLAG_INCLUDE_BOTTOM_EDGE) ? 1 : 0);
  if (cliprect != NULL)
  {
    v1yclip = MAX(v1yclip, cliprect->min_y);
    v3yclip = MIN(v3yclip, cliprect->max_y + 1);
  }
  if (v3yclip - v1yclip <= 0)
    return 0;

  /* allocate a new polygon */
//  polygon = allocate_polygon(poly, v1yclip, v3yclip);

  /* fill in the polygon information */
  //polygon->poly = poly;
  //polygon->dest = dest;
  //polygon->callback = callback;
  //polygon->extra = poly->extra[poly->extra_next - 1];
  //polygon->numparams = paramcount;
  //polygon->numverts = 3;

  /* set the start X/Y coordinates */
  //polygon->xorigin = v1x;
  //polygon->yorigin = v1y;

  /* compute the slopes for each portion of the triangle */
  dxdy_v1v2 = (v2->y == v1->y) ? 0.0f : (v2->x - v1->x) / (v2->y - v1->y);
  dxdy_v1v3 = (v3->y == v1->y) ? 0.0f : (v3->x - v1->x) / (v3->y - v1->y);
  dxdy_v2v3 = (v3->y == v2->y) ? 0.0f : (v3->x - v2->x) / (v3->y - v2->y);

  /* compute the X extents for each scanline */
//  startunit = poly->unit_next;

//  tri_work_unit *unit = new tri_work_unit;
  poly_extent extent;
  int extnum=0;
  for (curscan = v1yclip; curscan < v3yclip; curscan += scaninc)
  {
//    Bit32u bucketnum = ((Bit32u)curscan / SCANLINES_PER_BUCKET) % TOTAL_BUCKETS;
//    Bit32u unit_index = poly->unit_next++;
//    tri_work_unit *unit = &poly->unit[unit_index]->tri;


    /* determine how much to advance to hit the next bucket */
//    scaninc = SCANLINES_PER_BUCKET - (Bit32u)curscan % SCANLINES_PER_BUCKET;

    /* fill in the work unit basics */
//    unit->shared.polygon = polygon;
//    unit->shared.count_next = MIN(v3yclip - curscan, scaninc);
//    unit->shared.scanline = curscan;
//    unit->shared.previtem = poly->unit_bucket[bucketnum];
//    poly->unit_bucket[bucketnum] = unit_index;

    /* iterate over extents */
//    for (extnum = 0; extnum < unit->shared.count_next; extnum++)
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

      /* include the right edge if requested */
//      if (poly->flags & POLYFLAG_INCLUDE_RIGHT_EDGE)
//        istopx++;

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
//      unit->extent[extnum].startx = istartx;
//      unit->extent[extnum].stopx = istopx;
      extent.startx = istartx;
      extent.stopx = istopx;
      (callback)(dest,curscan,&extent,extra,0);

      pixels += istopx - istartx;
    }
  }

//  delete unit;

  /* return the total number of pixels in the triangle */
//  poly->triangles++;
//  poly->pixels += pixels;
  return pixels;
}

Bit32s triangle_create_work_item(/*voodoo_state *v,*/ Bit16u *drawbuf, int texcount)
{
  poly_extra_data extra;// = (poly_extra_data *)poly_get_extra_data(v->poly);
//  raster_info *info = new raster_info;
//  info=find_rasterizer(v, info, texcount);
  raster_info *info = find_rasterizer(v, texcount);
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
  extra.info = info;

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
    v->stats.texture_mode[TEXMODE_FORMAT(v->tmu[0].reg[textureMode].u)]++;

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
      v->stats.texture_mode[TEXMODE_FORMAT(v->tmu[1].reg[textureMode].u)]++;
    }
  }

  /* farm the rasterization out to other threads */
  info->polys++;
  retval = poly_render_triangle(drawbuf, NULL, info->callback, 0, &vert[0], &vert[1], &vert[2], &extra);

//  delete info;

  return retval;
}


void draw_line (int x1, int y1, int x2, int y2, Bit16u *buf)
{
  int i, deltax, deltay, numpixels,
      d, dinc1, dinc2,
      x, xinc1, xinc2,
      y, yinc1, yinc2;
  Bit16u color = 0xffff;
  int XRes = v->fbi.rowpixels;
  int YRes = v->fbi.height;
  Bit16u *dest;
  dest = buf;

  deltax = abs(x2 - x1);
  deltay = abs(y2 - y1);

  if (deltax >= deltay)
  {
      numpixels = deltax + 1;
      d = (deltay<<1) - deltax;
      dinc1 = deltay<<1;
      dinc2 = (deltay - deltax) << 1;
      xinc1 = 1;
      xinc2 = 1;
      yinc1 = 0;
      yinc2 = 1;
  }
  else
  {
      numpixels = deltay + 1;
      d = (deltax<<1) - deltay;
      dinc1 = deltax << 1;
      dinc2 = (deltax - deltay) << 1;
      xinc1 = 0;
      xinc2 = 1;
      yinc1 = 1;
      yinc2 = 1;
  }

  if (x1 > x2)
  {
      xinc1 = - xinc1;
      xinc2 = - xinc2;
  }
  if (y1 > y2)
  {
      yinc1 = - yinc1;
      yinc2 = - yinc2;
  }

  x = x1;
  y = y1;

  for (i=0; i < numpixels; i++)
  {
      if ((x>=0) && (x<XRes) && (y>=0) && (y<YRes))
          dest [x + y*XRes] = color;

      if (d < 0)
      {
          d = d + dinc1;
          x = x + xinc1;
          y = y + yinc1;
      }
      else
      {
          d = d + dinc2;
          x = x + xinc2;
          y = y + yinc2;
      }
  }
}

Bit32s triangle(voodoo_state *v)
{
  int texcount = 0;
  Bit16u *drawbuf;
  int destbuf;
  int pixels;

//  profiler_mark_start(PROFILER_USER2);

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

  /* wait for any outstanding work to finish */
//  poly_wait(v->poly, "triangle");

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
      break;

    default:  /* reserved */
      return TRIANGLE_SETUP_CLOCKS;
  }

//  draw_line(v->fbi.ax/16,v->fbi.ay/16,v->fbi.bx/16,v->fbi.by/16,drawbuf);
//  draw_line(v->fbi.ax/16,v->fbi.ay/16,v->fbi.cx/16,v->fbi.cy/16,drawbuf);
//  draw_line(v->fbi.cx/16,v->fbi.cy/16,v->fbi.bx/16,v->fbi.by/16,drawbuf);

  /* find a rasterizer that matches our current state */
  pixels = triangle_create_work_item(/*v, */drawbuf, texcount);

  /* update stats */
  v->reg[fbiTrianglesOut].u++;

  /* update stats */
  v->stats.total_triangles++;

//  profiler_mark_end();

  /* 1 pixel per clock, plus some setup time */
  if (LOG_REGISTERS) BX_DEBUG(("cycles = %d", TRIANGLE_SETUP_CLOCKS + pixels));
  return TRIANGLE_SETUP_CLOCKS + pixels;
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
    scry = (v->fbi.yorigin - y) & 0x3ff;

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


Bit32u poly_render_triangle_custom(/*poly_manager *poly, */void *dest, const rectangle *cliprect, poly_draw_scanline_func callback, int startscanline, int numscanlines, const poly_extent *extents, poly_extra_data *extra)
{
  Bit32s curscan, scaninc;
//  polygon_info *polygon;
  Bit32s v1yclip, v3yclip;
  Bit32s pixels = 0;
//  Bit32u startunit;

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

  /* allocate a new polygon */
//  polygon = allocate_polygon(poly, v1yclip, v3yclip);

  /* fill in the polygon information */
  //polygon->poly = poly;
  //polygon->dest = dest;
  //polygon->callback = callback;
  //polygon->extra = poly->extra[poly->extra_next - 1];
  //polygon->numparams = 0;
  //polygon->numverts = 3;

  /* compute the X extents for each scanline */
//  startunit = poly->unit_next;
  for (curscan = v1yclip; curscan < v3yclip; curscan += scaninc)
  {
//    Bit32u bucketnum = ((Bit32u)curscan / SCANLINES_PER_BUCKET) % TOTAL_BUCKETS;
//    Bit32u unit_index = poly->unit_next++;
//    tri_work_unit *unit = &poly->unit[unit_index]->tri;
//    tri_work_unit unit;

    int extnum=0;

    /* determine how much to advance to hit the next bucket */
//    scaninc = SCANLINES_PER_BUCKET - (Bit32u)curscan % SCANLINES_PER_BUCKET;
    scaninc = 1;

    /* fill in the work unit basics */
    //unit->shared.polygon = polygon;
    //unit->shared.count_next = MIN(v3yclip - curscan, scaninc);
    //unit->shared.scanline = curscan;
    //unit->shared.previtem = poly->unit_bucket[bucketnum];
    //poly->unit_bucket[bucketnum] = unit_index;

    /* iterate over extents */
//    for (extnum = 0; extnum < unit->shared.count_next; extnum++)
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
//      unit.extent[extnum].startx = istartx;
//      unit.extent[extnum].stopx = istopx;
      (callback)(dest,curscan,extent,extra,0);
      if (istartx < istopx)
        pixels += istopx - istartx;
    }
  }
#if KEEP_STATISTICS
  poly->unit_max = MAX(poly->unit_max, poly->unit_next);
#endif


  /* enqueue the work items */
//  if (poly->queue != NULL)
//    osd_work_item_queue_multiple(poly->queue, poly_item_callback, poly->unit_next - startunit, poly->unit[startunit], poly->unit_size, WORK_ITEM_FLAG_AUTO_RELEASE);

  /* return the total number of pixels in the object */
//  poly->triangles++;
//  poly->pixels += pixels;
  return pixels;
}

Bit32s fastfill(voodoo_state *v)
{
  int sx = (v->reg[clipLeftRight].u >> 16) & 0x3ff;
  int ex = (v->reg[clipLeftRight].u >> 0) & 0x3ff;
  int sy = (v->reg[clipLowYHighY].u >> 16) & 0x3ff;
  int ey = (v->reg[clipLowYHighY].u >> 0) & 0x3ff;
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

  poly_extra_data extra; //(poly_extra_data *)poly_get_extra_data(v->poly);
  /* iterate over blocks of extents */
  for (y = sy; y < ey; y += ARRAY_LENGTH(extents))
  {
    int count = MIN(ey - y, (int) ARRAY_LENGTH(extents));

    extra.state = v;
    memcpy(extra.dither, dithermatrix, sizeof(extra.dither));

    pixels += poly_render_triangle_custom(drawbuf, NULL, raster_fastfill, y, count, extents, &extra);
  }

  /* 2 pixels per clock */
  return pixels / 2;
}

void swap_buffers(voodoo_state *v)
{
  int count;
//  if (LOG_VBLANK_SWAP) BX_DEBUG(("--- swap_buffers @ %d", video_screen_get_vpos(v->screen)));

  /* force a partial update */
//  video_screen_update_partial(v->screen, video_screen_get_vpos(v->screen));
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
  else
    v->fbi.rgboffs[0] = v->reg[leftOverlayBuf].u & v->fbi.mask & ~0x0f;

  /* decrement the pending count and reset our state */
  if (v->fbi.swaps_pending)
    v->fbi.swaps_pending--;
  v->fbi.vblank_count = 0;
  v->fbi.vblank_swap_pending = 0;

  /* reset the last_op_time to now and start processing the next command */
  if (v->pci.op_pending)
  {
//    v->pci.op_end_time = timer_get_time(v->device->machine);
//    flush_fifos(v, v->pci.op_end_time);
  }

  /* we may be able to unstall now */
//  if (v->pci.stall_state != NOT_STALLED)
//    check_stalled_cpu(v, timer_get_time(v->device->machine));

  /* periodically log rasterizer info */
  v->stats.swaps++;
//  if (LOG_RASTERIZERS && v->stats.swaps % 100 == 0)
//    dump_rasterizer_stats(v);

  /* update the statistics (debug) */
  if (v->stats.display)
  {
/*
    const rectangle *visible_area = video_screen_get_visible_area(v->screen);
    int screen_area = (visible_area->max_x - visible_area->min_x + 1) * (visible_area->max_y - visible_area->min_y + 1);
    char *statsptr = v->stats.buffer;
    int pixelcount;
    int i;

    update_statistics(v, TRUE);
    pixelcount = v->stats.total_pixels_out;

    statsptr += sprintf(statsptr, "Swap:%6d\n", v->stats.swaps);
    statsptr += sprintf(statsptr, "Hist:%08X\n", v->reg[fbiSwapHistory].u);
    statsptr += sprintf(statsptr, "Stal:%6d\n", v->stats.stalls);
    statsptr += sprintf(statsptr, "Rend:%6d%%\n", pixelcount * 100 / screen_area);
    statsptr += sprintf(statsptr, "Poly:%6d\n", v->stats.total_triangles);
    statsptr += sprintf(statsptr, "PxIn:%6d\n", v->stats.total_pixels_in);
    statsptr += sprintf(statsptr, "POut:%6d\n", v->stats.total_pixels_out);
    statsptr += sprintf(statsptr, "Clip:%6d\n", v->stats.total_clipped);
    statsptr += sprintf(statsptr, "Stip:%6d\n", v->stats.total_stippled);
    statsptr += sprintf(statsptr, "Chro:%6d\n", v->stats.total_chroma_fail);
    statsptr += sprintf(statsptr, "ZFun:%6d\n", v->stats.total_zfunc_fail);
    statsptr += sprintf(statsptr, "AFun:%6d\n", v->stats.total_afunc_fail);
    statsptr += sprintf(statsptr, "RegW:%6d\n", v->stats.reg_writes);
    statsptr += sprintf(statsptr, "RegR:%6d\n", v->stats.reg_reads);
    statsptr += sprintf(statsptr, "LFBW:%6d\n", v->stats.lfb_writes);
    statsptr += sprintf(statsptr, "LFBR:%6d\n", v->stats.lfb_reads);
    statsptr += sprintf(statsptr, "TexW:%6d\n", v->stats.tex_writes);
    statsptr += sprintf(statsptr, "TexM:");
    for (i = 0; i < 16; i++)
      if (v->stats.texture_mode[i])
        *statsptr++ = "0123456789ABCDEF"[i];
    *statsptr = 0;
*/
  }

  /* update statistics */
  v->stats.stalls = 0;
  v->stats.total_triangles = 0;
  v->stats.total_pixels_in = 0;
  v->stats.total_pixels_out = 0;
  v->stats.total_chroma_fail = 0;
  v->stats.total_zfunc_fail = 0;
  v->stats.total_afunc_fail = 0;
  v->stats.total_clipped = 0;
  v->stats.total_stippled = 0;
  v->stats.reg_writes = 0;
  v->stats.reg_reads = 0;
  v->stats.lfb_writes = 0;
  v->stats.lfb_reads = 0;
  v->stats.tex_writes = 0;
  memset(v->stats.texture_mode, 0, sizeof(v->stats.texture_mode));
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
//  if (!(data & 1))
  {
    swap_buffers(v);
    return 0;
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

  /* apply emulation statistics */
  v->stats.total_pixels_in += stats->pixels_in;
  v->stats.total_pixels_out += stats->pixels_out;
  v->stats.total_chroma_fail += stats->chroma_fail;
  v->stats.total_zfunc_fail += stats->zfunc_fail;
  v->stats.total_afunc_fail += stats->afunc_fail;
  v->stats.total_clipped += stats->clip_fail;
  v->stats.total_stippled += stats->stipple_count;
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

//  logerror("VOODOO.%d.VIDMEM: buffer_pages=%X  fifo=%X-%X  tiles=%X  rowpix=%d\n", v->index, buffer_pages, fifo_start_page, fifo_last_page, v->fbi.x_tiles, v->fbi.rowpixels);

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
        //v->fbi.auxoffs = ~0;
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

/*  mame_printf_debug("rgb[0] = %08X   rgb[1] = %08X   rgb[2] = %08X   aux = %08X\n",
            v->fbi.rgboffs[0], v->fbi.rgboffs[1], v->fbi.rgboffs[2], v->fbi.auxoffs);*/

  /* compute the memory FIFO location and size */
  if (fifo_last_page > v->fbi.mask / 0x1000)
    fifo_last_page = v->fbi.mask / 0x1000;

  /* is it valid and enabled? */
  if (fifo_start_page <= fifo_last_page && FBIINIT0_ENABLE_MEMORY_FIFO(v->reg[fbiInit0].u))
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
  fifo_reset(&v->fbi.fifo);

  /* reset our front/back buffers if they are out of range */
  if (v->fbi.rgboffs[2] == (Bit32u)~0)
  {
    if (v->fbi.frontbuf == 2)
      v->fbi.frontbuf = 0;
    if (v->fbi.backbuf == 2)
      v->fbi.backbuf = 0;
  }
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
          if ((d->data_size == 1) && ((data & 0x21) == 0x21)) {
            d->clk0_freq = (Bit32u)((14318.0 * (d->clk0_m + 2)) / ((1 << d->clk0_p) * (d->clk0_n + 2)));
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

voodoo_reg reg;

void register_w(Bit32u offset, Bit32u data)
{
  Bit32u regnum  = (offset) & 0xff;
  Bit32u chips   = (offset>>8) & 0xf;
  reg.u = data;

//  Bit32u origdata = data;
//  Bit32s cycles = 0;
  Bit64s data64;

  BX_DEBUG(("write chip 0x%x reg 0x%x value 0x%08x(%s)", chips, regnum<<2, data, voodoo_reg_name[regnum]));
  voodoo_last_msg=regnum;

  if (chips == 0)
    chips = 0xf;

  /* the first 64 registers can be aliased differently */
  if ((offset & 0x800c0) == 0x80000 && v->alt_regmap)
    regnum = register_alias_map[offset & 0x3f];
  else
    regnum = offset & 0xff;

  /* first make sure this register is writable */
  if (!(v->regaccess[regnum] & REGISTER_WRITE)) {
    BX_ERROR(("Invalid attempt to write %s", v->regnames[regnum]));
    return;
  }

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
/*      cycles = */ triangle(v);
      break;

    case ftriangleCMD:
      v->fbi.cheating_allowed = 1;
      v->fbi.sign = data;
/*      cycles = */ triangle(v);
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
/*      cycles = */ fastfill(v);
      break;

    case swapbufferCMD:
      poly_wait(v->poly, v->regnames[regnum]);
/*      cycles = */ swapbuffer(v, data);
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
            v->fbi.clut[index] = data;
            v->fbi.clut_dirty = 1;
          }
        }
        else
          BX_DEBUG(("clutData ignored because video timing reset = 1"));
      }
      break;
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

#if 0
          /* compute the new period for standard res, medium res, and VGA res */
//        attoseconds_t refresh = video_screen_get_frame_period(v->screen).attoseconds;
          attoseconds_t stdperiod, medperiod, vgaperiod;
          attoseconds_t stddiff, meddiff, vgadiff;

//          stdperiod = HZ_TO_ATTOSECONDS(15750) * vtotal;
//          medperiod = HZ_TO_ATTOSECONDS(25000) * vtotal;
//          vgaperiod = HZ_TO_ATTOSECONDS(31500) * vtotal;

          /* compute a diff against the current refresh period */
          stddiff = stdperiod - refresh;
          if (stddiff < 0) stddiff = -stddiff;
          meddiff = medperiod - refresh;
          if (meddiff < 0) meddiff = -meddiff;
          vgadiff = vgaperiod - refresh;
          if (vgadiff < 0) vgadiff = -vgadiff;

          /* configure the screen based on which one matches the closest */
          if (stddiff < meddiff && stddiff < vgadiff)
          {
//            video_screen_configure(v->screen, htotal, vtotal, &visarea, stdperiod);
            BX_DEBUG(("Standard resolution, %f Hz", ATTOSECONDS_TO_HZ(stdperiod)));
          }
          else if (meddiff < vgadiff)
          {
//            video_screen_configure(v->screen, htotal, vtotal, &visarea, medperiod);
            BX_DEBUG(("Medium resolution, %f Hz", ATTOSECONDS_TO_HZ(medperiod)));
          }
          else
          {
//            video_screen_configure(v->screen, htotal, vtotal, &visarea, vgaperiod);
            BX_DEBUG(("VGA resolution, %f Hz", ATTOSECONDS_TO_HZ(vgaperiod)));
          }
#endif

          /* configure the new framebuffer info */
          v->fbi.width = hvis + 1;
          v->fbi.height = vvis;
          v->fbi.xoffs = hbp;
          v->fbi.yoffs = vbp;
          v->fbi.vsyncscan = (v->reg[vSync].u >> 16) & 0xfff;

          /* recompute the time of VBLANK */
//          adjust_vblank_timer(v);

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
    case fbiInit7:
      poly_wait(v->poly, v->regnames[regnum]);

      if (v->type <= VOODOO_2 && (chips & 1) && INITEN_ENABLE_HW_INIT(v->pci.init_enable))
      {
        v->reg[regnum].u = data;
        recompute_video_memory(v);
        v->fbi.video_changed = 1;
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

  /* statistics */
  v->stats.tex_writes++;

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
      if (TEXMODE_SEQ_8_DOWNLD(v->tmu[0].reg/*t->reg*/[textureMode].u))
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
//  Bit32u mem_mask=0xffffffff;
  Bit32u forcefront=0;

  int sr[2], sg[2], sb[2], sa[2], sw[2];
  int x, y, scry, mask;
  int pix, destbuf;

  BX_DEBUG(("write LFB offset 0x%x value 0x%08x", offset, data));

  /* statistics */
  v->stats.lfb_writes++;

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
  destbuf = (v->type >= VOODOO_BANSHEE) ? (!forcefront) : LFBMODE_WRITE_BUFFER_SELECT(v->reg[lfbMode].u);
//  BX_DEBUG(("destbuf %X lfbmode %X",destbuf, v->reg[lfbMode].u));
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
      scry = (v->fbi.yorigin - y) & 0x3ff;

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
      scry = (v->fbi.yorigin - y) & 0x3ff;

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
          if (x < (int)((v->reg[clipLeftRight].u >> 16) & 0x3ff) ||
            x >= (int)(v->reg[clipLeftRight].u & 0x3ff) ||
            scry < (int)((v->reg[clipLowYHighY].u >> 16) & 0x3ff) ||
            scry >= (int)(v->reg[clipLowYHighY].u & 0x3ff))
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

Bit32u register_r(Bit32u offset)
{
  Bit32u regnum  = (offset) & 0xff;
  Bit32u chips   = (offset>>8) & 0xf;

  if (!((voodoo_last_msg == regnum) && (regnum == status))) //show status reg only once
    BX_DEBUG(("read chip 0x%x reg 0x%x (%s)", chips, regnum<<2, voodoo_reg_name[regnum]));
  voodoo_last_msg = regnum;

  /* first make sure this register is readable */
  if (!(v->regaccess[regnum] & REGISTER_READ)) {
    BX_ERROR(("Invalid attempt to read %s", v->regnames[regnum]));
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
      if (fifo_empty(&v->pci.fifo))
        result |= 0x3f << 0;
      else
      {
        int temp = fifo_space(&v->pci.fifo)/2;
        if (temp > 0x3f)
          temp = 0x3f;
        result |= temp << 0;
      }

      /* bit 6 is the vertical retrace */
      result |= (Voodoo_get_retrace() > 0) << 6;

      /* bit 7 is FBI graphics engine busy */
      if (v->pci.op_pending)
        result |= 1 << 7;

      /* bit 8 is TREX busy */
      if (v->pci.op_pending)
        result |= 1 << 8;

      /* bit 9 is overall busy */
      if (v->pci.op_pending)
        result |= 1 << 9;

      /* Banshee is different starting here */
      if (v->type < VOODOO_BANSHEE)
      {
        /* bits 11:10 specifies which buffer is visible */
        result |= v->fbi.frontbuf << 10;

        /* bits 27:12 indicate memory FIFO freespace */
        if (!FBIINIT0_ENABLE_MEMORY_FIFO(v->reg[fbiInit0].u) || fifo_empty(&v->fbi.fifo))
          result |= 0xffff << 12;
        else
        {
          int temp = fifo_space(&v->fbi.fifo)/2;
          if (temp > 0xffff)
            temp = 0xffff;
          result |= temp << 12;
        }
      }
      else
      {
        /* bit 10 is 2D busy */

        /* bit 11 is cmd FIFO 0 busy */
        if (v->fbi.cmdfifo[0].enable && v->fbi.cmdfifo[0].depth > 0)
          result |= 1 << 11;

        /* bit 12 is cmd FIFO 1 busy */
        if (v->fbi.cmdfifo[1].enable && v->fbi.cmdfifo[1].depth > 0)
          result |= 1 << 12;
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
    case hvRetrace:
      result = Voodoo_get_retrace() & 0x1fff;
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

  /* statistics */
  v->stats.lfb_reads++;

  /* compute X,Y */
  x = (offset << 1) & 0x3fe;
  y = (offset >> 9) & 0x7ff;

  /* select the target buffer */
  destbuf = (v->type >= VOODOO_BANSHEE) ? (!forcefront) : LFBMODE_READ_BUFFER_SELECT(v->reg[lfbMode].u);
//  BX_DEBUG(("destbuf %X lfbmode %X",destbuf, v->reg[lfbMode].u));
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
    scry = (v->fbi.yorigin - y) & 0x3ff;

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

void voodoo_w(Bit32u offset, Bit32u data, Bit32u mask) {

  if ((offset & (0xc00000/4)) == 0)
    register_w(offset, data);
  else if (offset & (0x800000/4))
    texture_w(offset, data);
  else
    lfb_w(offset, data, mask);
}

Bit32u voodoo_r(Bit32u offset) {

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

void voodoo_init(Bit8u _type)
{
  int pen;
  int val;
  const raster_info *info;

  v->reg[lfbMode].u = 0;
  v->reg[fbiInit0].u = (1 << 4) | (0x10 << 6);
  v->reg[fbiInit1].u = (1 << 1) | (1 << 8) | (1 << 12) | (2 << 20);
  v->reg[fbiInit2].u = (1 << 6) | (0x100 << 23);
  v->reg[fbiInit3].u = (2 << 13) | (0xf << 17);
  v->reg[fbiInit4].u = (1 << 0);
  v->alt_regmap = 0;
  v->fbi.lfb_stride = 10;
  v->type = _type;
  if (v->type == VOODOO_2) {
    v->regaccess = voodoo2_register_access;
  } else {
    v->regaccess = voodoo_register_access;
  }
  v->regnames = voodoo_reg_name;
  v->chipmask = 0x01 | 0x02 | 0x04 | 0x08;
  memset(v->dac.reg, 0, sizeof(v->dac.reg));
  v->dac.read_result = 0;
  v->dac.clk0_m = 0x37;
  v->dac.clk0_n = 0x02;
  v->dac.clk0_p = 0x03;

  /* create a table of precomputed 1/n and log2(n) values */
  /* n ranges from 1.0000 to 2.0000 */
  for (val = 0; val <= (1 << RECIPLOG_LOOKUP_BITS); val++) {
    Bit32u value = (1 << RECIPLOG_LOOKUP_BITS) + val;
    voodoo_reciplog[val*2 + 0] = (1 << (RECIPLOG_LOOKUP_PREC + RECIPLOG_LOOKUP_BITS)) / value;
    voodoo_reciplog[val*2 + 1] = (Bit32u)(LOGB2((double)value / (double)(1 << RECIPLOG_LOOKUP_BITS)) * (double)(1 << RECIPLOG_LOOKUP_PREC));
  }

  /* build the rasterizer table */
  for (info = predef_raster_table; info->callback; info++)
    add_rasterizer(v, info);

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
  v->fbi.ram = (Bit8u*)malloc(4<<20); 
  v->fbi.mask = (4<<20)-1;
  v->fbi.frontbuf = 0;
  v->fbi.backbuf = 1;
  v->fbi.width = 640;
  v->fbi.height = 480;
  v->fbi.rowpixels = v->fbi.width;
  v->fbi.fogdelta_mask = (v->type < VOODOO_2) ? 0xff : 0xfc;

  /* build shared TMU tables */
  init_tmu_shared(&v->tmushare);

  init_tmu(v, &v->tmu[0], &v->reg[0x100], 0, 4 << 20);
  init_tmu(v, &v->tmu[1], &v->reg[0x200], 0, 4 << 20);

  v->tmu[0].reg = &v->reg[0x100];
  v->tmu[1].reg = &v->reg[0x200];

  v->tmu[0].ram = (Bit8u*)malloc(4<<20);
  v->tmu[1].ram = (Bit8u*)malloc(4<<20);
  v->tmu[0].mask = (4<<20)-1;
  v->tmu[1].mask = (4<<20)-1;

  v->tmu_config = 64;

  v->thread_stats = new stats_block[16];

  soft_reset(v);
}

bx_bool voodoo_update(/*running_device *device, bitmap_t *bitmap, */const rectangle *cliprect)
{
//  voodoo_state *v = get_safe_token(device);
  bx_bool changed = v->fbi.video_changed;
//  int drawbuf = v->fbi.frontbuf;
//  int statskey;
  int x, y;

  /* reset the video changed flag */
  v->fbi.video_changed = 0;

  /* if we are blank, just fill with black */
  if (v->type <= VOODOO_2 && FBIINIT1_SOFTWARE_BLANK(v->reg[fbiInit1].u)) {
//    bitmap_fill(bitmap, cliprect, 0);
    return changed;
  }

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
      int which = (v->banshee.io[io_vidProcCfg] >> 13) & 1;
      int bypass = (v->banshee.io[io_vidProcCfg] >> 11) & 1;

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
    changed = 1;
  }

  /* debugging! */
//  if (input_code_pressed(device->machine, KEYCODE_L))
//    drawbuf = v->fbi.backbuf;

  /* copy from the current front buffer */
/*
  for (y = cliprect->min_y; y <= cliprect->max_y; y++)
    if (y >= v->fbi.yoffs)
    {
      Bit16u *src = (Bit16u *)(v->fbi.ram + v->fbi.rgboffs[drawbuf]) + (y - v->fbi.yoffs) * v->fbi.rowpixels - v->fbi.xoffs;
      Bit32u *dst = BITMAP_ADDR32(bitmap, y, 0);
      for (x = cliprect->min_x; x <= cliprect->max_x; x++)
        dst[x] = v->fbi.pen[src[x]];
    }
*/
  /* update stats display */
/*
  statskey = (input_code_pressed(device->machine, KEYCODE_BACKSLASH) != 0);
  if (statskey && statskey != v->stats.lastkey)
    v->stats.display = !v->stats.display;
  v->stats.lastkey = statskey;
*/
  /* display stats */
/*
  if (v->stats.display)
    popmessage(v->stats.buffer, 0, 0);
*/
  /* update render override */
/*
  v->stats.render_override = input_code_pressed(device->machine, KEYCODE_ENTER);
  if (DEBUG_DEPTH && v->stats.render_override)
  {
    for (y = cliprect->min_y; y <= cliprect->max_y; y++)
    {
      Bit16u *src = (Bit16u *)(v->fbi.ram + v->fbi.auxoffs) + (y - v->fbi.yoffs) * v->fbi.rowpixels - v->fbi.xoffs;
      Bit32u *dst = BITMAP_ADDR32(bitmap, y, 0);
      for (x = cliprect->min_x; x <= cliprect->max_x; x++)
        dst[x] = ((src[x] << 8) & 0xff0000) | ((src[x] >> 0) & 0xff00) | ((src[x] >> 8) & 0xff);
    }
  }
*/
  return changed;
}
