%{
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "parser.h"

int bx_yyinput(char *buf, int max_size);
#undef YY_INPUT
#define YY_INPUT(buf, ret, max_size) (ret = bx_yyinput(buf, max_size))

static char    *lex_input_ptr = NULL;
static unsigned lex_input_size = 0;
%}


%%
[ \t]+          ; // eat up whitespace
set             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SET); }
dis             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DIS); }
on              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ON); }
off             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_OFF); }
crc             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CRC); }
c               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CONTINUE); }
stepi           |
step            |
si              |
s               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_STEPN); }
vb              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_VBREAKPOINT); }
vbreak          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_VBREAKPOINT); }
lb              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LBREAKPOINT); }
lbreak          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LBREAKPOINT); }
pb              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PBREAKPOINT); }
pbreak          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PBREAKPOINT); }
info            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_INFO); }
break           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PBREAKPOINT); }
b               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PBREAKPOINT); }
program         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PROGRAM); }
cr		|
cregs		{ bxlval.sval = strdup(bxtext); return(BX_TOKEN_CONTROL_REGS); }
r		|
reg		|
registers       { bxlval.sval = strdup(bxtext); return(BX_TOKEN_REGISTERS); }
dirty           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DIRTY); }
linux           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LINUX); }
dump_cpu        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DUMP_CPU); }
delete          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DEL_BREAKPOINT); }
del             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DEL_BREAKPOINT); }
d               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DEL_BREAKPOINT); }
quit            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_QUIT); }
q               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_QUIT); }
x               { bxlval.sval = strdup(bxtext); return(BX_TOKEN_EXAMINE); }
xp              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_EXAMINE); }
setpmem         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SETPMEM); }
query           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_QUERY); }
pending         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PENDING); }
take            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TAKE); }
dma             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DMA); }
irq             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_IRQ); }
set_cpu         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SET_CPU); }
disas           |
disassemble     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DISASSEMBLE); }
maths           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_MATHS); }
add             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ADD); }
sub             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SUB); }
mul             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_MUL); }
div             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DIV); }
instrument      { bxlval.sval = strdup(bxtext); return(BX_TOKEN_INSTRUMENT); }
start           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_START); }
stop            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_STOP); }
reset           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_RESET); }
print           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PRINT); }
loader          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_LOADER); }
doit            { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DOIT); }
trace-on        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TRACEON); }
trace-off       { bxlval.sval = strdup(bxtext); return(BX_TOKEN_TRACEOFF); }
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
global          { bxlval.sval = strdup(bxtext); return(BX_TOKEN_GLOBAL); }
where           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_WHERE); }
print-string    { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PRINT_STRING); }
diff-memory     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DIFF_MEMORY); }
sync-memory     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYNC_MEMORY); }
sync-cpu        { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYNC_CPU); }
fast-forward    { bxlval.sval = strdup(bxtext); return(BX_TOKEN_FAST_FORWARD); }
phy2log         { bxlval.sval = strdup(bxtext); return(BX_TOKEN_PHY_2_LOG); }
addr-info       { bxlval.sval = strdup(bxtext); return(BX_TOKEN_INFO_ADDRESS); }
cs              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_CS); }
es              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ES); }
ss              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SS); }
ds              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_DS); }
fs              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_FS); }
gs              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_GS); }
force-check     { bxlval.sval = strdup(bxtext); return(BX_TOKEN_ALWAYS_CHECK); }
v2l             { bxlval.sval = strdup(bxtext); return(BX_TOKEN_V2L); }
\"[^\"\n]*\"    { bxlval.sval = strdup(bxtext); return(BX_TOKEN_STRING); }
\/[0-9]*[xduotcsibhwg][xduotcsibhwg] { bxlval.sval = strdup(bxtext); return(BX_TOKEN_XFORMAT); }
\/[0-9]*[xduotcsibhwg]              { bxlval.sval = strdup(bxtext); return(BX_TOKEN_XFORMAT); }
\/[0-9]+                           { bxlval.sval = strdup(bxtext); return(BX_TOKEN_XFORMAT); }
0x[0-9a-fA-F]+  { bxlval.uval = strtoul(bxtext+2, NULL, 16); return(BX_TOKEN_NUMERIC); }
0[0-7]+         { bxlval.uval = strtoul(bxtext+1, NULL, 8); return(BX_TOKEN_NUMERIC); }
[0-9]+L         { bxlval.ulval = strtoull(bxtext, NULL, 10); return(BX_TOKEN_LONG_NUMERIC); }
[0-9]+          { bxlval.uval = strtoul(bxtext, NULL, 10); return(BX_TOKEN_NUMERIC); }
$[a-zA-Z_][a-zA-Z0-9_]* { bxlval.sval = strdup(bxtext); return(BX_TOKEN_SYMBOLNAME); }
\n              { return('\n'); }
[#][^\n]*    ; // eat up comments '//'
.               { return(bxtext[0]); }
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

  len = strlen(lex_input_ptr);
  if (len > max_size)
    max_size = len;

  memcpy(buf, lex_input_ptr, max_size);
  lex_input_size -= max_size;

  return(max_size);
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
