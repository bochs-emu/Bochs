/////////////////////////////////////////////////////////////////////////
// $Id: pcivga.h,v 1.3 2003-01-27 21:11:55 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2002,2003  Mike Nordell
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

#if BX_USE_PCIVGA_SMF
#  define BX_PCIVGA_THIS thePciVgaAdapter->
#else
#  define BX_PCIVGA_THIS this->
#endif


class bx_pcivga_c : public bx_devmodel_c
{
public:
  bx_pcivga_c(void);
  ~bx_pcivga_c(void);
  virtual void   init(void);
  virtual void   reset(unsigned type);

private:

  struct {
    Bit8u pci_conf[256];
  } s;

  static Bit32u pci_read_handler(void *this_ptr, Bit8u address, unsigned io_len);
  static void   pci_write_handler(void *this_ptr, Bit8u address, Bit32u value, unsigned io_len);
#if !BX_USE_PCIVGA_SMF
  Bit32u pci_read(Bit8u address, unsigned io_len);
  void   pci_write(Bit8u address, Bit32u value, unsigned io_len);
#endif
};
