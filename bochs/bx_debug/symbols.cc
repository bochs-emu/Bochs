/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2009  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "cpu/cpu.h"

#if BX_DEBUGGER
#if !((BX_HAVE_MAP || BX_HAVE_MAP_H) && (BX_HAVE_SET || BX_HAVE_SET_H))

static const char BX_HAVE_MAP_ERR[] = "context not implemented because BX_HAVE_MAP=0\n";

const char* bx_dbg_symbolic_address(Bit32u context, Bit32u eip, Bit32u base)
{
  static bx_bool first = true;
  if (first) {
    dbg_printf(BX_HAVE_MAP_ERR);
    first = false;
  }
  return "unk. ctxt";
}

int bx_dbg_symbol_command(const char* filename, bx_bool global, Bit32u offset)
{
  dbg_printf(BX_HAVE_MAP_ERR);
  return -1;
}

void bx_dbg_info_symbols_command(const char *symbol)
{
  dbg_printf(BX_HAVE_MAP_ERR);
}

int bx_dbg_lbreakpoint_symbol_command(const char *symbol)
{
  dbg_printf(BX_HAVE_MAP_ERR);
  return -1;
}

Bit32u bx_dbg_get_symbol_value(const char *symbol)
{
  return 0;
}

const char* bx_dbg_disasm_symbolic_address(Bit32u eip, Bit32u base)
{
  return 0;
}

#else   /* if BX_HAVE_MAP == 1 */

#if BX_HAVE_MAP
#include <map>
#elif BX_HAVE_MAP_H
#include <map.h>
#endif

#if BX_HAVE_SET
#include <set>
#elif BX_HAVE_SET_H
#include <set.h>
#endif

struct symbol_entry_t
{
  symbol_entry_t (Bit32u _start = 0, const char* _name = 0)
  {
    start = _start;
    name = _name ? strdup(_name) : 0;
  }
  ~symbol_entry_t()
  {
    if (name)
      free(name);
  }

  void trim_quotes(void);

  char* name;
  Bit32u start;

private:
  symbol_entry_t(const symbol_entry_t&);  // No definition
  void operator=(const symbol_entry_t &); // No definition
};

struct lt_symbol_entry_t
{
  bool operator()(const symbol_entry_t* s1, const symbol_entry_t* s2) const
  {
    return s1->start < s2->start;
  }
};

struct lt_rsymbol_entry_t
{
  bool operator()(const symbol_entry_t* s1, const symbol_entry_t* s2) const
  {
    return strcoll(s1->name, s2->name) < 0;
  }
};

struct context_t
{
  typedef std::set<symbol_entry_t*,lt_symbol_entry_t>  sym_set_t;
  typedef std::set<symbol_entry_t*,lt_rsymbol_entry_t> rsym_set_t;
  typedef std::map<Bit32u,context_t*> map_t;

  context_t (Bit32u);
 ~context_t();

  static context_t* get_context(Bit32u);
  symbol_entry_t* get_symbol_entry(Bit32u);
  symbol_entry_t* get_symbol_entry(const char *symbol) const;
  void add_symbol(symbol_entry_t*);
  const sym_set_t* get_all_symbols() const {return &m_syms;}
  const rsym_set_t* get_all_rsymbols() const {return &m_rsyms;}

  friend class context_dtor_t;
protected:
  static map_t s_map;
  // Forvard references (find name by address)
  sym_set_t m_syms;
  // Reverse references (find address by name)
  rsym_set_t m_rsyms;
  Bit32u m_id;
};

context_t::map_t context_t::s_map;

context_t::context_t (Bit32u id)
{
  m_id = id;
  s_map[m_id] = this;
}

context_t::~context_t()
{
  while (!m_syms.empty())
  {
    sym_set_t::iterator it = m_syms.begin();
    symbol_entry_t *sym = *it;
    m_syms.erase(it);
    delete sym;
  }
}

context_t* context_t::get_context(Bit32u i)
{
  return s_map[i];
}

symbol_entry_t* context_t::get_symbol_entry(Bit32u ip)
{
  symbol_entry_t probe(ip, 0);
  // find the first symbol whose address is greater than ip.
  if (m_syms.empty ())
      return 0;
  sym_set_t::iterator iter = m_syms.upper_bound(&probe);

  if (iter == m_syms.end()) { // No symbol found
    return 0;
  }

  --iter;
  if(iter == m_syms.end())
      return 0;
  return *iter;
}

symbol_entry_t* context_t::get_symbol_entry(const char *symbol) const
{
  if (m_rsyms.empty())
    return 0;

  symbol_entry_t probe(0, symbol);
  rsym_set_t::const_iterator iter;
  iter=m_rsyms.find(&probe);
  if(iter==m_rsyms.end()) // No symbol found
    return 0;
  return *iter;
}

void context_t::add_symbol(symbol_entry_t* sym)
{
  m_syms.insert(sym);
  m_rsyms.insert(sym);
}

void symbol_entry_t::trim_quotes(void)
{
  size_t len = strlen(name);
  if (name[0] == '\"' && name[len-1] == '\"')
  {
    memmove(name, name+1, len);
    name[len-2] = 0;
  }
}

Bit32u bx_dbg_get_symbol_value(const char *symbol)
{
  context_t* cntx = context_t::get_context(0);
  if(!cntx) // Context not found
    return 0;

  symbol_entry_t s(0, symbol);
  s.trim_quotes();

  symbol_entry_t* sym=cntx->get_symbol_entry(s.name);
  if(!sym) // symbol not found
    return 0;

  return sym->start;
}

const char* bx_dbg_symbolic_address(Bit32u context, Bit32u eip, Bit32u base)
{
  static char buf[80];

  // Look up this context
  context_t* cntx = context_t::get_context(context);
  if (!cntx) {
    // Try global context
    cntx = context_t::get_context(0);
    if (!cntx) {
      snprintf (buf, 80, "unk. ctxt");
      return buf;
    }
  }
  // full linear address not only eip (for nonzero based segments)
  symbol_entry_t* entr = cntx->get_symbol_entry(base+eip);
  if (!entr) {
    snprintf (buf, 80, "no symbol");
    return buf;
  }
  snprintf (buf, 80, "%s+%x", entr->name, (base+eip) - entr->start);
  return buf;
}

const char* bx_dbg_disasm_symbolic_address(Bit32u eip, Bit32u base)
{
  static char buf[80];

  // Try global context
  context_t* cntx = context_t::get_context(0);
  if (!cntx) {
    return 0;
  }

  // full linear address not only eip (for nonzero based segments)
  symbol_entry_t* entr = cntx->get_symbol_entry(base+eip);
  if (!entr) {
    return 0;
  }
  snprintf (buf, 80, "%s+%x", entr->name, (base+eip) - entr->start);
  return buf;
}

int bx_dbg_symbol_command(const char* filename, bx_bool global, Bit32u offset)
{
  symbol_entry_t file(0, filename);
  file.trim_quotes();

  // Install symbols in correct context (page table)
  // The file format should be
  // address symbol (example '00002afe _StartLoseNT')

  Bit32u context_id = (global) ? 0 : (BX_CPU(dbg_cpu)->cr3) >> 12;

  context_t* cntx = context_t::get_context(context_id);

  if (!cntx) {
    cntx = new context_t(context_id);
  }

  FILE* fp = fopen(file.name, "rt"); // 't' is need for win32, unixes simply ignore it
  if (!fp) {
    dbg_printf ("Could not open symbol file '%s'\n", file.name);
    return -1;
  }

  // C++/C# symbols can be long
  char buf[512];
  int  line_num = 1;

  while (fgets(buf, sizeof(buf), fp)) {
    // handle end of line (before error messages)
    int len = strlen(buf);
    bool whole_line = (buf[len - 1] == '\n');
    if (whole_line)
      buf[len - 1] = 0;

    // parse
    char* sym_name;
    Bit32u addr = strtoul(buf, &sym_name, 16);

    if (!isspace(*sym_name)) {
      if (*sym_name == 0)
        dbg_printf("%s:%d: missing symbol name\n", file.name, line_num);
      else
        dbg_printf("%s:%d: syntax error near '%s'\n", file.name, line_num, sym_name);
      return -1;
    }
    ++sym_name;

    symbol_entry_t* sym = new symbol_entry_t(addr + offset, sym_name);
    cntx->add_symbol(sym);

    // skip the rest of long line
    while (!whole_line) {
      if (!fgets(buf, sizeof(buf), fp))
        break;
      // actually, last line can end without newline, but then
      // we'll just break at the next iteration because of EOF
      whole_line = (buf[strlen(buf)-1] == '\n');
    }
    ++line_num;
  }
  return 0;
}

// check if s1 is prefix of s2
static bool bx_dbg_strprefix(const char *s1, const char *s2)
{
  if(!s1 || !s2)
    return false;

  size_t len=strlen(s1);

  if(len>strlen(s2))
    return false;
  return strncmp(s1, s2, len)==0;
}

void bx_dbg_info_symbols_command(const char *symbol)
{
  context_t* cntx = context_t::get_context(0);

  if(!cntx) {
    dbg_printf ("Global context not available\n");
    return;
  }

  if(symbol) {
    const context_t::rsym_set_t* rsyms;

    rsyms=cntx->get_all_rsymbols();
    if (rsyms->empty ()) {
      dbg_printf ("symbols not loaded\n");
      return;
    }

    symbol_entry_t probe(0, symbol);
    // remove leading and trailing quotas
    probe.trim_quotes();
    context_t::rsym_set_t::const_iterator iter;
    iter=rsyms->lower_bound(&probe);

    if(iter==rsyms->end() || !bx_dbg_strprefix(probe.name, (*iter)->name))
      dbg_printf ("No symbols found\n");
    else {
      for(;iter!=rsyms->end() && bx_dbg_strprefix(probe.name, (*iter)->name);++iter) {
        dbg_printf ("%08x: %s\n", (*iter)->start, (*iter)->name);
      }
    }
  }
  else {
    const context_t::sym_set_t* syms;

    syms=cntx->get_all_symbols();
    if (syms->empty()) {
      dbg_printf ("symbols not loaded\n");
      return;
    }

    context_t::sym_set_t::const_iterator iter;
    for(iter = syms->begin();iter!=syms->end();++iter) {
      dbg_printf ("%08x: %s\n", (*iter)->start, (*iter)->name);
    }
  }
}

int bx_dbg_lbreakpoint_symbol_command(const char *symbol)
{
  context_t* cntx = context_t::get_context(0);
  if(!cntx) {
    dbg_printf ("Global context not available\n");
    return -1;
  }
  symbol_entry_t probe(0, symbol);
  // remove leading and trailing quotes
  probe.trim_quotes();

  const symbol_entry_t* sym=cntx->get_symbol_entry(probe.name);
  if(sym)
    return bx_dbg_lbreakpoint_command(bkRegular, sym->start);
  dbg_printf ("symbol not found\n");
  return -1;
}

#endif
#endif
