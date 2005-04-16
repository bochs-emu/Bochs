%{
/////////////////////////////////////////////////////////////////////////
// $Id: lexer.l,v 1.5 2005-04-16 21:43:06 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include "debug.h"
#if BX_DEBUGGER

#include "parser.h"


int bx_yyinput(char *buf, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf, ret, max_size) (ret = bx_yyinput(buf, max_size))

static char    *lex_input_ptr = NULL;
static unsigned lex_input_size = 0;
%}

%x EXAMINE
%x DISASM

%%
<*>[ \t]+       ; // eat up whitespace
set             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SET); }
dis             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DIS); }
on              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ON); }
off             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_OFF); }
crc             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CRC); }
c               |
cont            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CONTINUE); }
stepi           |
step            |
s               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_STEPN); }
next            |
n               |
p               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_STEP_OVER); }
blist           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LIST_BREAK); }
vb              |
vbreak          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_VBREAKPOINT); }
lb              |
lbreak          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LBREAKPOINT); }
break           |
b               |
pb              |
pbreak          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PBREAKPOINT); }
info            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_INFO); }
program         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PROGRAM); }
cr		|
creg		|
cregs		{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_CONTROL_REGS); }
r		|
reg		|
registers       { bxlval.sval = strdup(bxtext); return(BX_TOKEN_REGISTERS); }
fpu             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_FPU); }
cpu             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CPU); }
all             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ALL); }
idt		{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_IDT); }
ivt		{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_IVT); }
gdt		{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_GDT); }
ldt		{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_LDT); }
tss		{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_TSS); }
dirty           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DIRTY); }
linux           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LINUX); }
dump_cpu        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DUMP_CPU); }
delete          |
del             |
d               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DEL_BREAKPOINT); }
bpe             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ENABLE_BREAKPOINT); }
bpd             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DISABLE_BREAKPOINT); }
quit            |
exit            |
q               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_QUIT); }
x               |
xp              { BEGIN(EXAMINE); bxlval.sval = strdup(bxtext); return(BX_TOKEN_EXAMINE); }
setpmem         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SETPMEM); }
query           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_QUERY); }
pending         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PENDING); }
take            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TAKE); }
dma             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DMA); }
irq             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_IRQ); }
pic             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PIC); }
set_cpu         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SET_CPU); }
u               |
disas           |
disassemble     { BEGIN(DISASM); bxlval.sval = strdup(bxtext); return(BX_TOKEN_DISASSEMBLE); }
instrument      { bxlval.sval = strdup(bxtext); return(BX_TOKEN_INSTRUMENT); }
start           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_START); }
stop            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_STOP); }
reset           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_RESET); }
print           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PRINT); }
loader          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LOADER); }
doit            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DOIT); }
trace-on        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TRACEON); }
trace-off       { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TRACEOFF); }
trace-reg-on	{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_TRACEREGON); }
trace-reg-off	{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_TRACEREGOFF); }
ptime           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PTIME); }
sb              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TIMEBP); }
sba             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TIMEBP_ABSOLUTE); }
record          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_RECORD); }
playback        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PLAYBACK); }
modebp          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_MODEBP); }
print-stack     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PRINT_STACK); }
watch           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_WATCH); }
unwatch         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_UNWATCH); }
read            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_READ); }
write           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_WRITE); }
continue        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CONTINUE); }
show            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SHOW); }
load-symbols    { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYMBOL); }
symbols         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYMBOLS); }
slist           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LIST_SYMBOLS); }
global          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_GLOBAL); }
where           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_WHERE); }
print-string    { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PRINT_STRING); }
diff-memory     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DIFF_MEMORY); }
sync-memory     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYNC_MEMORY); }
sync-cpu        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYNC_CPU); }
fast-forward    { bxlval.sval = strdup(bxtext); return(BX_TOKEN_FAST_FORWARD); }
phy2log         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PHY_2_LOG); }
addr-info       { bxlval.sval = strdup(bxtext); return(BX_TOKEN_INFO_ADDRESS); }
ne2k            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_NE2000); }
ne2000          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_NE2000); }
page            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PAGE); }
vga             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_VGA); }
al              { bxlval.reg=rAL; return (BX_TOKEN_REG_AL);}
bl              { bxlval.reg=rBL; return (BX_TOKEN_REG_BL);}
cl              { bxlval.reg=rCL; return (BX_TOKEN_REG_CL);}
dl              { bxlval.reg=rDL; return (BX_TOKEN_REG_DL);}
ah              { bxlval.reg=rAH; return (BX_TOKEN_REG_AH);}
bh              { bxlval.reg=rBH; return (BX_TOKEN_REG_BH);}
ch              { bxlval.reg=rCH; return (BX_TOKEN_REG_CH);}
dh              { bxlval.reg=rDH; return (BX_TOKEN_REG_DH);}
ax              { bxlval.reg=rAX; return (BX_TOKEN_REG_AX);}
bx              { bxlval.reg=rBX; return (BX_TOKEN_REG_BX);}
cx              { bxlval.reg=rCX; return (BX_TOKEN_REG_CX);}
dx              { bxlval.reg=rDX; return (BX_TOKEN_REG_DX);}
eax             { bxlval.reg=rEAX; return (BX_TOKEN_REG_EAX);}
ebx             { bxlval.reg=rEBX; return (BX_TOKEN_REG_EBX);}
ecx             { bxlval.reg=rECX; return (BX_TOKEN_REG_ECX);}
edx             { bxlval.reg=rEDX; return (BX_TOKEN_REG_EDX);}
si              { bxlval.reg=rSI; return (BX_TOKEN_REG_SI);}
di              { bxlval.reg=rDI; return (BX_TOKEN_REG_DI);}
bp              { bxlval.reg=rBP; return (BX_TOKEN_REG_BP);}
sp              { bxlval.reg=rSP; return (BX_TOKEN_REG_SP);}
ip              { bxlval.reg=rIP; return (BX_TOKEN_REG_IP);}
esi             { bxlval.reg=rESI; return (BX_TOKEN_REG_ESI);}
edi             { bxlval.reg=rEDI; return (BX_TOKEN_REG_EDI);}
ebp             { bxlval.reg=rEBP; return (BX_TOKEN_REG_EBP);}
esp             { bxlval.reg=rESP; return (BX_TOKEN_REG_ESP);}
eip             { bxlval.reg=rEIP; return (BX_TOKEN_REG_EIP);}
cs              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CS); }
es              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ES); }
ss              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SS); }
ds              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DS); }
fs              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_FS); }
gs              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_GS); }
flags           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_FLAGS); }
force-check     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ALWAYS_CHECK); }
help            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_HELP); }
\?              |
calc            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CALC); }
<EXAMINE>\/[0-9]+                 { BEGIN(INITIAL); bxlval.sval = strdup(bxtext); return(BX_TOKEN_XFORMAT); }
<EXAMINE>\/[0-9]*[mxduotcsibhwg]+ { BEGIN(INITIAL); bxlval.sval = strdup(bxtext); return(BX_TOKEN_XFORMAT); }
<DISASM>\/[0-9]+        { BEGIN(INITIAL); bxlval.sval = strdup(bxtext); return(BX_TOKEN_DISFORMAT); }
"+"             { return ('+'); }
"-"             { return ('-'); }
"*"             { return ('*'); }
"/"             { return ('/'); }
">>"            { return (BX_TOKEN_RSHIFT); }
"<<"            { return (BX_TOKEN_LSHIFT); }
"&"             { return ('&'); }
"|"             { return ('|'); }
"^"             { return ('^'); }
"!"             { return ('!'); }
"("             { return ('('); }
")"             { return (')'); }
\'([^\\\'\n]|(\\.))*\'    |
\"([^\\\"\n]|(\\.))*\"    { bxlval.sval = strdup(bxtext); return(BX_TOKEN_STRING); }
0x[0-9a-fA-F]+  { bxlval.uval = strtoul(bxtext+2, NULL, 16); return(BX_TOKEN_NUMERIC); }
0[0-7]+         { bxlval.uval = strtoul(bxtext+1, NULL, 8); return(BX_TOKEN_NUMERIC); }
[0-9]+L         { bxlval.ulval = strtoull(bxtext, NULL, 10); return(BX_TOKEN_LONG_NUMERIC); }
[0-9]+          { bxlval.uval = strtoul(bxtext, NULL, 10); return(BX_TOKEN_NUMERIC); }
$[a-zA-Z_][a-zA-Z0-9_]* { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYMBOLNAME); }
<*>";"          { return ('\n'); }
<*>\n           { return('\n'); }
[#][^\n]*    ; // eat up comments '//'
.               { return(bxtext[0]); }
<EXAMINE,DISASM>. { BEGIN(INITIAL); unput(*bxtext); }
%%

// [A-Za-z_][A-Za-z0-9_]*   { bxlval.sval = strdup(bxtext); return(BX_TOKEN_GENERIC); }

  int
bx_yyinput(char *buf, int max_size)
{
  int len;

  if (lex_input_size == 0) {
    fprintf(stderr, "lex: no characters in string input buffer.\n");
    exit(1);
  }

  len = strlen(lex_input_ptr) + 1;
  if (len > max_size)
    len = max_size;

  memcpy(buf, lex_input_ptr, len);

  return(len);
}

  void
bx_add_lex_input(char *buf)
{
  lex_input_ptr  = buf;
  lex_input_size = strlen(buf);

  // Since we're parsing from strings, flush out
  // all current buffer state, so the next read
  // requests from yyinput

  bx_flush_buffer( YY_CURRENT_BUFFER );
}

#endif /* if BX_DEBUGGER */
