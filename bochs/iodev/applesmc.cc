/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2026  The Bochs Project
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//

// Apple SMC (System Management Controller) emulation.
//
// Real macOS x86 guests probe this device early in boot; without a response
// the kernel hangs/panics. Port layout, command protocol and the minimal
// key set below are ported from QEMU's isa-applesmc device
// (hw/misc/applesmc.c, Copyright (c) 2007 Alexander Graf, LGPLv2.1+), which
// is the reference implementation the hackintosh-on-QEMU community has used
// for years.
//
// The OSK0/OSK1 keys are the two halves of Apple's "OSK" ROM key, which is
// proprietary Apple data extracted from real hardware -- it is not
// redistributed here, same as upstream QEMU does not hardcode it. A dummy
// placeholder is used unless the real key is supplied via the "applesmc"
// bochsrc addon option (see applesmc_options_parser below).

#define BX_PLUGGABLE

#include "iodev.h"
#include "applesmc.h"

#define LOG_THIS theAppleSMC->

bx_applesmc_c *theAppleSMC = NULL;

enum {
  APPLESMC_IO_BASE      = 0x300,
  APPLESMC_DATA_PORT    = APPLESMC_IO_BASE + 0x00,
  APPLESMC_CMD_PORT     = APPLESMC_IO_BASE + 0x04,
  APPLESMC_ERR_PORT     = APPLESMC_IO_BASE + 0x1e
};

enum {
  APPLESMC_READ_CMD              = 0x10
};

enum {
  APPLESMC_ST_CMD_DONE            = 0x00,
  APPLESMC_ST_DATA_READY          = 0x01,
  APPLESMC_ST_ACK                 = 0x04,
  APPLESMC_ST_NEW_CMD             = 0x08
};

enum {
  APPLESMC_ST_1E_CMD_INTRUPTED    = 0x80,
  APPLESMC_ST_1E_STILL_BAD_CMD    = 0x81,
  APPLESMC_ST_1E_BAD_CMD          = 0x82,
  APPLESMC_ST_1E_NOEXIST          = 0x84
};

// BX_APPLESMC_OSK_LENGTH + 1: room for the implicit trailing NUL the string
// literal below carries; only the first BX_APPLESMC_OSK_LENGTH bytes are
// ever copied out (the NUL is never used as OSK data).
static const char default_osk[BX_APPLESMC_OSK_LENGTH + 1] =
  "This is a dummy key. Enter the real key "
  "using the -osk parameter";

// static key table, ported verbatim from QEMU's isa-applesmc. OSK0/OSK1 are
// filled in from theAppleSMC->s.osk at runtime instead of a fixed buffer.
static const Bit8u applesmc_rev_data[6] = {0x01, 0x13, 0x0f, 0x00, 0x00, 0x03};
static const Bit8u applesmc_natj_data[1] = {0x00};
static const Bit8u applesmc_mssp_data[1] = {0x00};
static const Bit8u applesmc_mssd_data[1] = {0x03};

const bx_applesmc_key_t bx_applesmc_c::static_keys[] = {
  {{'R','E','V',' '}, 6, applesmc_rev_data},
  {{'N','A','T','J'}, 1, applesmc_natj_data},
  {{'M','S','S','P'}, 1, applesmc_mssp_data},
  {{'M','S','S','D'}, 1, applesmc_mssd_data}
};

Bit32s applesmc_options_parser(const char *context, int num_params, char *params[])
{
  if (!strcmp(params[0], "applesmc")) {
    for (int i = 1; i < num_params; i++) {
      if (!strncmp(params[i], "osk=", 4)) {
        const char *osk = params[i] + 4;
        if (strlen(osk) != BX_APPLESMC_OSK_LENGTH) {
          BX_ERROR(("%s: applesmc osk must be exactly %d characters, ignoring",
                    context, BX_APPLESMC_OSK_LENGTH));
        } else if (theAppleSMC != NULL) {
          theAppleSMC->set_osk(osk);
        }
      } else {
        BX_ERROR(("%s: unknown parameter for applesmc ignored.", context));
      }
    }
  } else {
    BX_PANIC(("%s: unknown directive '%s'", context, params[0]));
  }
  return 0;
}

// device plugin entry point

PLUGIN_ENTRY_FOR_MODULE(applesmc)
{
  if (mode == PLUGIN_INIT) {
    theAppleSMC = new bx_applesmc_c();
    bx_devices.pluginAppleSMC = theAppleSMC;
    BX_REGISTER_DEVICE_DEVMODEL(plugin, type, theAppleSMC, BX_PLUGIN_APPLESMC);
    SIM->register_addon_option("applesmc", applesmc_options_parser, NULL);
  } else if (mode == PLUGIN_FINI) {
    bx_devices.pluginAppleSMC = &bx_devices.stubAppleSMC;
    delete theAppleSMC;
    SIM->unregister_addon_option("applesmc");
  } else if (mode == PLUGIN_PROBE) {
    return (int)PLUGTYPE_OPTIONAL;
  }
  return(0); // Success
}

// the device object

bx_applesmc_c::bx_applesmc_c()
{
  put("applesmc", "SMC");
  memcpy(s.osk, default_osk, BX_APPLESMC_OSK_LENGTH);
}

bx_applesmc_c::~bx_applesmc_c()
{
  SIM->get_bochs_root()->remove("applesmc");
  BX_DEBUG(("Exit"));
}

void bx_applesmc_c::init(void)
{
  DEV_register_ioread_handler(this, read_handler, APPLESMC_DATA_PORT, "Apple SMC data", 1);
  DEV_register_ioread_handler(this, read_handler, APPLESMC_CMD_PORT, "Apple SMC cmd", 1);
  DEV_register_ioread_handler(this, read_handler, APPLESMC_ERR_PORT, "Apple SMC err", 1);
  DEV_register_iowrite_handler(this, write_handler, APPLESMC_DATA_PORT, "Apple SMC data", 1);
  DEV_register_iowrite_handler(this, write_handler, APPLESMC_CMD_PORT, "Apple SMC cmd", 1);

  BX_INFO(("Apple SMC present at ioport 0x%04x", (unsigned) APPLESMC_IO_BASE));
}

void bx_applesmc_c::reset(unsigned type)
{
  s.cmd = 0;
  s.status = APPLESMC_ST_CMD_DONE;
  s.status_1e = APPLESMC_ST_CMD_DONE;
  s.last_ret = 0;
  s.read_pos = 0;
  s.data_len = 0;
  s.data_pos = 0;
}

void bx_applesmc_c::register_state(void)
{
  bx_list_c *list = new bx_list_c(SIM->get_bochs_root(), "applesmc", "Apple SMC State");
  BXRS_HEX_PARAM_FIELD(list, cmd, s.cmd);
  BXRS_HEX_PARAM_FIELD(list, status, s.status);
  BXRS_HEX_PARAM_FIELD(list, status_1e, s.status_1e);
  BXRS_HEX_PARAM_FIELD(list, last_ret, s.last_ret);
  BXRS_DEC_PARAM_FIELD(list, read_pos, s.read_pos);
  BXRS_DEC_PARAM_FIELD(list, data_len, s.data_len);
  BXRS_DEC_PARAM_FIELD(list, data_pos, s.data_pos);
}

const bx_applesmc_key_t* bx_applesmc_c::find_key(const char key[4])
{
  for (unsigned i = 0; i < sizeof(static_keys)/sizeof(static_keys[0]); i++) {
    if (!memcmp(static_keys[i].key, key, 4)) return &static_keys[i];
  }
  if (!memcmp("OSK0", key, 4)) {
    static bx_applesmc_key_t osk0 = {{'O','S','K','0'}, 32, NULL};
    osk0.data = (const Bit8u*) s.osk;
    return &osk0;
  }
  if (!memcmp("OSK1", key, 4)) {
    static bx_applesmc_key_t osk1 = {{'O','S','K','1'}, 32, NULL};
    osk1.data = (const Bit8u*) (s.osk + 32);
    return &osk1;
  }
  return NULL;
}

// static IO port read/write callback handlers
// redirect to non-static class handlers to avoid virtual functions

Bit32u bx_applesmc_c::read_handler(void *this_ptr, Bit32u address, unsigned io_len)
{
#if !BX_USE_APPLESMC_SMF
  bx_applesmc_c *class_ptr = (bx_applesmc_c *) this_ptr;
  return class_ptr->read(address, io_len);
}

Bit32u bx_applesmc_c::read(Bit32u address, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif

  switch (address) {
    case APPLESMC_CMD_PORT:
      return BX_APPLESMC_THIS s.status;

    case APPLESMC_ERR_PORT:
      // NOTE: read does not clear the 1e status
      return BX_APPLESMC_THIS s.status_1e;

    case APPLESMC_DATA_PORT:
      if (BX_APPLESMC_THIS s.cmd != APPLESMC_READ_CMD) {
        BX_APPLESMC_THIS s.status = APPLESMC_ST_CMD_DONE;
        BX_APPLESMC_THIS s.status_1e = APPLESMC_ST_1E_STILL_BAD_CMD;
        break;
      }
      if (!(BX_APPLESMC_THIS s.status & APPLESMC_ST_DATA_READY)) break;
      if (BX_APPLESMC_THIS s.data_pos < BX_APPLESMC_THIS s.data_len) {
        BX_APPLESMC_THIS s.last_ret = BX_APPLESMC_THIS s.data[BX_APPLESMC_THIS s.data_pos];
        BX_APPLESMC_THIS s.data_pos++;
        if (BX_APPLESMC_THIS s.data_pos == BX_APPLESMC_THIS s.data_len) {
          BX_APPLESMC_THIS s.status = APPLESMC_ST_CMD_DONE;
        } else {
          BX_APPLESMC_THIS s.status = APPLESMC_ST_ACK | APPLESMC_ST_DATA_READY;
        }
      }
      break;

    default:
      BX_ERROR(("unsupported applesmc read, address=0x%04x!", (unsigned) address));
      return 0;
  }
  return BX_APPLESMC_THIS s.last_ret;
}


void bx_applesmc_c::write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len)
{
#if !BX_USE_APPLESMC_SMF
  bx_applesmc_c *class_ptr = (bx_applesmc_c *) this_ptr;
  class_ptr->write(address, value, io_len);
}

void bx_applesmc_c::write(Bit32u address, Bit32u value, unsigned io_len)
{
#else
  UNUSED(this_ptr);
#endif

  switch (address) {
    case APPLESMC_CMD_PORT:
      if (value == APPLESMC_READ_CMD) {
        Bit8u status = BX_APPLESMC_THIS s.status & 0x0f;
        if (status == APPLESMC_ST_CMD_DONE || status == APPLESMC_ST_NEW_CMD) {
          BX_APPLESMC_THIS s.cmd = (Bit8u) value;
          BX_APPLESMC_THIS s.status = APPLESMC_ST_NEW_CMD | APPLESMC_ST_ACK;
        } else {
          BX_APPLESMC_THIS s.status = APPLESMC_ST_NEW_CMD;
          BX_APPLESMC_THIS s.status_1e = APPLESMC_ST_1E_CMD_INTRUPTED;
        }
      } else {
        BX_APPLESMC_THIS s.status = APPLESMC_ST_NEW_CMD;
        BX_APPLESMC_THIS s.status_1e = APPLESMC_ST_1E_BAD_CMD;
      }
      BX_APPLESMC_THIS s.read_pos = 0;
      BX_APPLESMC_THIS s.data_pos = 0;
      break;

    case APPLESMC_DATA_PORT:
      if (BX_APPLESMC_THIS s.cmd != APPLESMC_READ_CMD) {
        BX_APPLESMC_THIS s.status = APPLESMC_ST_CMD_DONE;
        BX_APPLESMC_THIS s.status_1e = APPLESMC_ST_1E_STILL_BAD_CMD;
        break;
      }
      if ((BX_APPLESMC_THIS s.status & 0x0f) == APPLESMC_ST_CMD_DONE) break;
      if (BX_APPLESMC_THIS s.read_pos < 4) {
        BX_APPLESMC_THIS s.key[BX_APPLESMC_THIS s.read_pos] = (char) value;
        BX_APPLESMC_THIS s.status = APPLESMC_ST_ACK;
      } else if (BX_APPLESMC_THIS s.read_pos == 4) {
        const bx_applesmc_key_t *k = BX_APPLESMC_THIS find_key(BX_APPLESMC_THIS s.key);
        if (k != NULL) {
          memcpy(BX_APPLESMC_THIS s.data, k->data, k->len);
          BX_APPLESMC_THIS s.data_len = k->len;
          BX_APPLESMC_THIS s.data_pos = 0;
          BX_APPLESMC_THIS s.status = APPLESMC_ST_ACK | APPLESMC_ST_DATA_READY;
          BX_APPLESMC_THIS s.status_1e = APPLESMC_ST_CMD_DONE; // clear on valid key
        } else {
          BX_DEBUG(("READ_CMD: key '%c%c%c%c' not found!", BX_APPLESMC_THIS s.key[0],
                    BX_APPLESMC_THIS s.key[1], BX_APPLESMC_THIS s.key[2], BX_APPLESMC_THIS s.key[3]));
          BX_APPLESMC_THIS s.status = APPLESMC_ST_CMD_DONE;
          BX_APPLESMC_THIS s.status_1e = APPLESMC_ST_1E_NOEXIST;
        }
      }
      BX_APPLESMC_THIS s.read_pos++;
      break;

    default:
      BX_ERROR(("unsupported applesmc write, address=0x%04x!", (unsigned) address));
  }
}
