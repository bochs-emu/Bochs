/*
 *
 * $Id: blur.c,v 1.6 2002-04-17 02:28:33 bdenney Exp $
 *
 */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>

#define MAX 128
int array[MAX][MAX];
int array2[MAX][MAX];
#define BLUR_SIZE 3
#define BLUR_WINDOW_HALF 1

#define DEFAULT_TIMES 1000

void blur_simple()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++)
	  sum += array[x2][y2];
      array2[x][y] = sum;
    }
}

#ifdef BLUR_UNROLL_INNER
void blur_unroll_inner()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = array[x-1][y-1] + array[x-1][y] + array[x-1][y+1]
	    + array[x][y-1] + array[x][y] + array[x][y+1]
	    + array[x+1][y-1] + array[x+1][y] + array[x+1][y+1];
      array2[x][y] = sum;
    }
}
#endif

#ifdef BLUR_USE_FUNCTION_CALL
void blur_func (int *sum, int *data)
{
  *sum += *data;
}

void blur_funcall()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++)
	  blur_func (&sum, &array[x2][y2]);
      array2[x][y] = sum;
    }
}
#endif //ifdef BLUR_USE_FUNCTION_CALL

#ifdef BLUR_USE_SWITCH
/// With BLUR_USE_SWITCH, don't expect the result to match.  I had to make
//all the cases in the switch do something slightly different, so that they
//weren't all merged together by a compiler optimization.\n");
void blur_switch()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++) {
	  switch (y2)
	  {
	    // all cases are slightly different
	    case 0:
	      sum += array[x][y2]; break;
	    case 1:
	      sum += array[x][y2]; break;
	    case 2:
	      sum += array[x][y2]; break;
	    case 3:
	      sum += array[y][y2]; break;
	    case 4:
	      sum += array[y2][y2]; break;
	    case 5:
	      sum += array[y2][y2]; break;
	    case 6:
	      sum += array[y2][y2]; break;
	    case 7:
	      sum += array[x2][y2]; break;
	    case 8:
	      sum += array[x][x2]; break;
	    case 9:
	      sum += array[x][x2]; break;
	    case 10:
	      sum += array[y][x2]; break;
	    case 11:
	      sum += array[x][y]; break;
	    case 12:
	      sum += array[x][y]; break;
	    case 13:
	      sum += array[y2][x]; break;
	    case 14:
	      sum += array[y2][x]; break;
	    case 15:
	      sum += array[x2][x]; break;
	    case 16:
	      sum += array[y][x]; break;
	    case 17:
	      sum += array[y][x]; break;
	    case 18:
	      sum += array[y][x]; break;
	    case 19:
	      sum += array[x][x]; break;
	    case 20:
	      sum += array[x][x]; break;
	    case 21: 
	      sum += array[x2][x2]; break;
	    case 22: 
	      sum += array[x2][x2]; break;
	    case 23: 
	      sum += array[x2][x2]; break;
	    case 24: 
	      sum += array[x2][x2]; break;
	    case 25: 
	      sum += array[y2][x2]; break;
	    case 26:
	      sum += array[x2][y]; break;
	    case 27: 
	      sum += array[y2][y]; break;
	    case 28: 
	      sum += array[y2][y]; break;
	    case 29: 
	      sum += array[y2][y]; break;
	    case 30: 
	      sum += array[y][y]; break;
	    case 31: 
	      sum += array[y][y]; break;
	    default:
	      sum += array[x2][y2]; break;
	  }
	}
      array2[x][y] = sum;
    }
}

#endif // ifdef BLUR_USE_SWITCH

#ifdef BLUR_USE_SWITCH_CALL
void blur_func00 (int *sum, int *data) { *sum += *data; }
void blur_func01 (int *sum, int *data) { *sum += *data; }
void blur_func02 (int *sum, int *data) { *sum += *data; }
void blur_func03 (int *sum, int *data) { *sum += *data; }
void blur_func04 (int *sum, int *data) { *sum += *data; }
void blur_func05 (int *sum, int *data) { *sum += *data; }
void blur_func06 (int *sum, int *data) { *sum += *data; }
void blur_func07 (int *sum, int *data) { *sum += *data; }
void blur_func08 (int *sum, int *data) { *sum += *data; }
void blur_func09 (int *sum, int *data) { *sum += *data; }
void blur_func10 (int *sum, int *data) { *sum += *data; }
void blur_func11 (int *sum, int *data) { *sum += *data; }
void blur_func12 (int *sum, int *data) { *sum += *data; }
void blur_func13 (int *sum, int *data) { *sum += *data; }
void blur_func14 (int *sum, int *data) { *sum += *data; }
void blur_func15 (int *sum, int *data) { *sum += *data; }
void blur_func16 (int *sum, int *data) { *sum += *data; }
void blur_func17 (int *sum, int *data) { *sum += *data; }
void blur_func18 (int *sum, int *data) { *sum += *data; }
void blur_func19 (int *sum, int *data) { *sum += *data; }
void blur_func20 (int *sum, int *data) { *sum += *data; }
void blur_func21 (int *sum, int *data) { *sum += *data; }
void blur_func22 (int *sum, int *data) { *sum += *data; }
void blur_func23 (int *sum, int *data) { *sum += *data; }
void blur_func24 (int *sum, int *data) { *sum += *data; }
void blur_func25 (int *sum, int *data) { *sum += *data; }
void blur_func26 (int *sum, int *data) { *sum += *data; }
void blur_func27 (int *sum, int *data) { *sum += *data; }
void blur_func28 (int *sum, int *data) { *sum += *data; }
void blur_func29 (int *sum, int *data) { *sum += *data; }
void blur_func30 (int *sum, int *data) { *sum += *data; }
void blur_func31 (int *sum, int *data) { *sum += *data; }

void blur_switch_call()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++) {
	  switch (y2)
	  {
	    case 0: blur_func00 (&sum, &array[x2][y2]); break;
	    case 1: blur_func01 (&sum, &array[x2][y2]); break;
	    case 2: blur_func02 (&sum, &array[x2][y2]); break;
	    case 3: blur_func03 (&sum, &array[x2][y2]); break;
	    case 4: blur_func04 (&sum, &array[x2][y2]); break;
	    case 5: blur_func05 (&sum, &array[x2][y2]); break;
	    case 6: blur_func06 (&sum, &array[x2][y2]); break;
	    case 7: blur_func07 (&sum, &array[x2][y2]); break;
	    case 8: blur_func08 (&sum, &array[x2][y2]); break;
	    case 9: blur_func09 (&sum, &array[x2][y2]); break;
	    case 10: blur_func10 (&sum, &array[x2][y2]); break;
	    case 11: blur_func11 (&sum, &array[x2][y2]); break;
	    case 12: blur_func12 (&sum, &array[x2][y2]); break;
	    case 13: blur_func13 (&sum, &array[x2][y2]); break;
	    case 14: blur_func14 (&sum, &array[x2][y2]); break;
	    case 15: blur_func15 (&sum, &array[x2][y2]); break;
	    case 16: blur_func16 (&sum, &array[x2][y2]); break;
	    case 17: blur_func17 (&sum, &array[x2][y2]); break;
	    case 18: blur_func18 (&sum, &array[x2][y2]); break;
	    case 19: blur_func19 (&sum, &array[x2][y2]); break;
	    case 20: blur_func20 (&sum, &array[x2][y2]); break;
	    case 21: blur_func21 (&sum, &array[x2][y2]); break;
	    case 22: blur_func22 (&sum, &array[x2][y2]); break;
	    case 23: blur_func23 (&sum, &array[x2][y2]); break;
	    case 24: blur_func24 (&sum, &array[x2][y2]); break;
	    case 25: blur_func25 (&sum, &array[x2][y2]); break;
	    case 26: blur_func26 (&sum, &array[x2][y2]); break;
	    case 27: blur_func27 (&sum, &array[x2][y2]); break;
	    case 28: blur_func28 (&sum, &array[x2][y2]); break;
	    case 29: blur_func29 (&sum, &array[x2][y2]); break;
	    case 30: blur_func30 (&sum, &array[x2][y2]); break;
	    case 31: blur_func31 (&sum, &array[x2][y2]); break;
	    default:
		     sum += array[x2][y2];
	  }
	}
      array2[x][y] = sum;
    }
}
#endif //ifdef BLUR_USE_SWITCH_CALL

#ifdef BLUR_FNPTR_SWITCH
void blur_func00 (int *sum, int *data) { *sum += *data; }
void blur_func01 (int *sum, int *data) { *sum += *data; }
void blur_func02 (int *sum, int *data) { *sum += *data; }
void blur_func03 (int *sum, int *data) { *sum += *data; }
void blur_func04 (int *sum, int *data) { *sum += *data; }
void blur_func05 (int *sum, int *data) { *sum += *data; }
void blur_func06 (int *sum, int *data) { *sum += *data; }
void blur_func07 (int *sum, int *data) { *sum += *data; }
void blur_func08 (int *sum, int *data) { *sum += *data; }
void blur_func09 (int *sum, int *data) { *sum += *data; }
void blur_func10 (int *sum, int *data) { *sum += *data; }
void blur_func11 (int *sum, int *data) { *sum += *data; }
void blur_func12 (int *sum, int *data) { *sum += *data; }
void blur_func13 (int *sum, int *data) { *sum += *data; }
void blur_func14 (int *sum, int *data) { *sum += *data; }
void blur_func15 (int *sum, int *data) { *sum += *data; }
void blur_func16 (int *sum, int *data) { *sum += *data; }
void blur_func17 (int *sum, int *data) { *sum += *data; }
void blur_func18 (int *sum, int *data) { *sum += *data; }
void blur_func19 (int *sum, int *data) { *sum += *data; }
void blur_func20 (int *sum, int *data) { *sum += *data; }
void blur_func21 (int *sum, int *data) { *sum += *data; }
void blur_func22 (int *sum, int *data) { *sum += *data; }
void blur_func23 (int *sum, int *data) { *sum += *data; }
void blur_func24 (int *sum, int *data) { *sum += *data; }
void blur_func25 (int *sum, int *data) { *sum += *data; }
void blur_func26 (int *sum, int *data) { *sum += *data; }
void blur_func27 (int *sum, int *data) { *sum += *data; }
void blur_func28 (int *sum, int *data) { *sum += *data; }
void blur_func29 (int *sum, int *data) { *sum += *data; }
void blur_func30 (int *sum, int *data) { *sum += *data; }
void blur_func31 (int *sum, int *data) { *sum += *data; }

void bluf_fnptr_switch()
{
  int sum;
  int x,y,x2,y2;
  void (*ptr)(int *sum, int *data) = NULL;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++) {
	  switch (y2) {
	    case 0:  ptr = blur_func00; break;
	    case 1:  ptr = blur_func01; break;
	    case 2:  ptr = blur_func02; break;
	    case 3:  ptr = blur_func03; break;
	    case 4:  ptr = blur_func04; break;
	    case 5:  ptr = blur_func05; break;
	    case 6:  ptr = blur_func06; break;
	    case 7:  ptr = blur_func07; break;
	    case 8:  ptr = blur_func08; break;
	    case 9:  ptr = blur_func09; break;
	    case 10:  ptr = blur_func10; break;
	    case 11:  ptr = blur_func11; break;
	    case 12:  ptr = blur_func12; break;
	    case 13:  ptr = blur_func13; break;
	    case 14:  ptr = blur_func14; break;
	    case 15:  ptr = blur_func15; break;
	    case 16:  ptr = blur_func16; break;
	    case 17:  ptr = blur_func17; break;
	    case 18:  ptr = blur_func18; break;
	    case 19:  ptr = blur_func19; break;
	    case 20:  ptr = blur_func20; break;
	    case 21:  ptr = blur_func21; break;
	    case 22:  ptr = blur_func22; break;
	    case 23:  ptr = blur_func23; break;
	    case 24:  ptr = blur_func24; break;
	    case 25:  ptr = blur_func25; break;
	    case 26:  ptr = blur_func26; break;
	    case 27:  ptr = blur_func27; break;
	    case 28:  ptr = blur_func28; break;
	    case 29:  ptr = blur_func29; break;
	    case 30:  ptr = blur_func30; break;
	    case 31:  ptr = blur_func31; break;
	    case 32:  ptr = blur_func00; break;
	    case 33:  ptr = blur_func01; break;
	    case 34:  ptr = blur_func02; break;
	    case 35:  ptr = blur_func03; break;
	    case 36:  ptr = blur_func04; break;
	    case 37:  ptr = blur_func05; break;
	    case 38:  ptr = blur_func06; break;
	    case 39:  ptr = blur_func07; break;
	    case 40:  ptr = blur_func08; break;
	    case 41:  ptr = blur_func09; break;
	    case 42:  ptr = blur_func10; break;
	    case 43:  ptr = blur_func11; break;
	    case 44:  ptr = blur_func12; break;
	    case 45:  ptr = blur_func13; break;
	    case 46:  ptr = blur_func14; break;
	    case 47:  ptr = blur_func15; break;
	    case 48:  ptr = blur_func16; break;
	    case 49:  ptr = blur_func17; break;
	    case 50:  ptr = blur_func18; break;
	    case 51:  ptr = blur_func19; break;
	    case 52:  ptr = blur_func20; break;
	    case 53:  ptr = blur_func21; break;
	    case 54:  ptr = blur_func22; break;
	    case 55:  ptr = blur_func23; break;
	    case 56:  ptr = blur_func24; break;
	    case 57:  ptr = blur_func25; break;
	    case 58:  ptr = blur_func26; break;
	    case 59:  ptr = blur_func27; break;
	    case 60:  ptr = blur_func28; break;
	    case 61:  ptr = blur_func29; break;
	    case 62:  ptr = blur_func30; break;
	    case 63:  ptr = blur_func31; break;
	    case 64:  ptr = blur_func00; break;
	    case 65:  ptr = blur_func01; break;
	    case 66:  ptr = blur_func02; break;
	    case 67:  ptr = blur_func03; break;
	    case 68:  ptr = blur_func04; break;
	    case 69:  ptr = blur_func05; break;
	    case 70:  ptr = blur_func06; break;
	    case 71:  ptr = blur_func07; break;
	    case 72:  ptr = blur_func08; break;
	    case 73:  ptr = blur_func09; break;
	    case 74:  ptr = blur_func10; break;
	    case 75:  ptr = blur_func11; break;
	    case 76:  ptr = blur_func12; break;
	    case 77:  ptr = blur_func13; break;
	    case 78:  ptr = blur_func14; break;
	    case 79:  ptr = blur_func15; break;
	    case 80:  ptr = blur_func16; break;
	    case 81:  ptr = blur_func17; break;
	    case 82:  ptr = blur_func18; break;
	    case 83:  ptr = blur_func19; break;
	    case 84:  ptr = blur_func20; break;
	    case 85:  ptr = blur_func21; break;
	    case 86:  ptr = blur_func22; break;
	    case 87:  ptr = blur_func23; break;
	    case 88:  ptr = blur_func24; break;
	    case 89:  ptr = blur_func25; break;
	    case 90:  ptr = blur_func26; break;
	    case 91:  ptr = blur_func27; break;
	    case 92:  ptr = blur_func28; break;
	    case 93:  ptr = blur_func29; break;
	    case 94:  ptr = blur_func30; break;
	    case 95:  ptr = blur_func31; break;
	    case 96:  ptr = blur_func00; break;
	    case 97:  ptr = blur_func01; break;
	    case 98:  ptr = blur_func02; break;
	    case 99:  ptr = blur_func03; break;
	    case 100:  ptr = blur_func04; break;
	    case 101:  ptr = blur_func05; break;
	    case 102:  ptr = blur_func06; break;
	    case 103:  ptr = blur_func07; break;
	    case 104:  ptr = blur_func08; break;
	    case 105:  ptr = blur_func09; break;
	    case 106:  ptr = blur_func10; break;
	    case 107:  ptr = blur_func11; break;
	    case 108:  ptr = blur_func12; break;
	    case 109:  ptr = blur_func13; break;
	    case 110:  ptr = blur_func14; break;
	    case 111:  ptr = blur_func15; break;
	    case 112:  ptr = blur_func16; break;
	    case 113:  ptr = blur_func17; break;
	    case 114:  ptr = blur_func18; break;
	    case 115:  ptr = blur_func19; break;
	    case 116:  ptr = blur_func20; break;
	    case 117:  ptr = blur_func21; break;
	    case 118:  ptr = blur_func22; break;
	    case 119:  ptr = blur_func23; break;
	    case 120:  ptr = blur_func24; break;
	    case 121:  ptr = blur_func25; break;
	    case 122:  ptr = blur_func26; break;
	    case 123:  ptr = blur_func27; break;
	    case 124:  ptr = blur_func28; break;
	    case 125:  ptr = blur_func29; break;
	    case 126:  ptr = blur_func30; break;
	    case 127:  ptr = blur_func31; break;
	    default: ptr=NULL;
	  }
	  (*ptr)(&sum, &array[x2][y2]);
	}
      array2[x][y] = sum;
    }
}
#endif //ifdef BLUR_FNPTR_SWITCH

#ifdef BLUR_FNPTR_TABLE
void blur_func00 (int *sum, int *data) { *sum += *data; }
void blur_func01 (int *sum, int *data) { *sum += *data; }
void blur_func02 (int *sum, int *data) { *sum += *data; }
void blur_func03 (int *sum, int *data) { *sum += *data; }
void blur_func04 (int *sum, int *data) { *sum += *data; }
void blur_func05 (int *sum, int *data) { *sum += *data; }
void blur_func06 (int *sum, int *data) { *sum += *data; }
void blur_func07 (int *sum, int *data) { *sum += *data; }
void blur_func08 (int *sum, int *data) { *sum += *data; }
void blur_func09 (int *sum, int *data) { *sum += *data; }
void blur_func10 (int *sum, int *data) { *sum += *data; }
void blur_func11 (int *sum, int *data) { *sum += *data; }
void blur_func12 (int *sum, int *data) { *sum += *data; }
void blur_func13 (int *sum, int *data) { *sum += *data; }
void blur_func14 (int *sum, int *data) { *sum += *data; }
void blur_func15 (int *sum, int *data) { *sum += *data; }
void blur_func16 (int *sum, int *data) { *sum += *data; }
void blur_func17 (int *sum, int *data) { *sum += *data; }
void blur_func18 (int *sum, int *data) { *sum += *data; }
void blur_func19 (int *sum, int *data) { *sum += *data; }
void blur_func20 (int *sum, int *data) { *sum += *data; }
void blur_func21 (int *sum, int *data) { *sum += *data; }
void blur_func22 (int *sum, int *data) { *sum += *data; }
void blur_func23 (int *sum, int *data) { *sum += *data; }
void blur_func24 (int *sum, int *data) { *sum += *data; }
void blur_func25 (int *sum, int *data) { *sum += *data; }
void blur_func26 (int *sum, int *data) { *sum += *data; }
void blur_func27 (int *sum, int *data) { *sum += *data; }
void blur_func28 (int *sum, int *data) { *sum += *data; }
void blur_func29 (int *sum, int *data) { *sum += *data; }
void blur_func30 (int *sum, int *data) { *sum += *data; }
void blur_func31 (int *sum, int *data) { *sum += *data; }

typedef void (*blurfn)(int *sum, int *data);
blurfn fntable[MAX] = {
 blur_func00,
 blur_func01,
 blur_func02,
 blur_func03,
 blur_func04,
 blur_func05,
 blur_func06,
 blur_func07,
 blur_func08,
 blur_func09,
 blur_func10,
 blur_func11,
 blur_func12,
 blur_func13,
 blur_func14,
 blur_func15,
 blur_func16,
 blur_func17,
 blur_func18,
 blur_func19,
 blur_func20,
 blur_func21,
 blur_func22,
 blur_func23,
 blur_func24,
 blur_func25,
 blur_func26,
 blur_func27,
 blur_func28,
 blur_func29,
 blur_func30,
 blur_func31,
 blur_func00,
 blur_func01,
 blur_func02,
 blur_func03,
 blur_func04,
 blur_func05,
 blur_func06,
 blur_func07,
 blur_func08,
 blur_func09,
 blur_func10,
 blur_func11,
 blur_func12,
 blur_func13,
 blur_func14,
 blur_func15,
 blur_func16,
 blur_func17,
 blur_func18,
 blur_func19,
 blur_func20,
 blur_func21,
 blur_func22,
 blur_func23,
 blur_func24,
 blur_func25,
 blur_func26,
 blur_func27,
 blur_func28,
 blur_func29,
 blur_func30,
 blur_func31,
 blur_func00,
 blur_func01,
 blur_func02,
 blur_func03,
 blur_func04,
 blur_func05,
 blur_func06,
 blur_func07,
 blur_func08,
 blur_func09,
 blur_func10,
 blur_func11,
 blur_func12,
 blur_func13,
 blur_func14,
 blur_func15,
 blur_func16,
 blur_func17,
 blur_func18,
 blur_func19,
 blur_func20,
 blur_func21,
 blur_func22,
 blur_func23,
 blur_func24,
 blur_func25,
 blur_func26,
 blur_func27,
 blur_func28,
 blur_func29,
 blur_func30,
 blur_func31,
 blur_func00,
 blur_func01,
 blur_func02,
 blur_func03,
 blur_func04,
 blur_func05,
 blur_func06,
 blur_func07,
 blur_func08,
 blur_func09,
 blur_func10,
 blur_func11,
 blur_func12,
 blur_func13,
 blur_func14,
 blur_func15,
 blur_func16,
 blur_func17,
 blur_func18,
 blur_func19,
 blur_func20,
 blur_func21,
 blur_func22,
 blur_func23,
 blur_func24,
 blur_func25,
 blur_func26,
 blur_func27,
 blur_func28,
 blur_func29,
 blur_func30,
 blur_func31,
};

void blur_fnptr_table()
{
  int sum;
  int x,y,x2,y2;
  void (*ptr)(int *sum, int *data) = NULL;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++) {
	  ptr = fntable[y2];
	  (*ptr)(&sum, &array[x2][y2]);
	}
      array2[x][y] = sum;
    }
}
#endif // ifdef BLUR_FNPTR_TABLE

void fill_array()
{
  int x,y;
  for (x=0; x<MAX; x++)
    for (y=0; y<MAX; y++)
      array[x][y] = (x*17+y*31)%29;
}

void dump_array (FILE *fp, int ptr[MAX][MAX])
{
  int x,y;
  for (x=0; x<MAX; x++) {
    for (y=0; y<MAX; y++) {
      fprintf (fp, "%3d ", ptr[x][y]);
    }
    fprintf (fp, "\n");
  }
}

struct timeval start, stop;
#define start_timer() gettimeofday (&start, NULL);
#define stop_timer() gettimeofday (&stop, NULL);

void report_time (FILE *fp, int iters)
{
  int usec_duration = 
    (stop.tv_sec*1000000 + stop.tv_usec)
    - (start.tv_sec*1000000 + start.tv_usec);
  double sec = (double)usec_duration / 1.0e3;
  double sec_per_iter = sec / (double)iters;
  fprintf (fp, "Total time elapsed = %f msec\n", sec);
  fprintf (fp, "Iterations = %d\n", iters);
  fprintf (fp, "Time per iteration = %f msec\n", sec_per_iter);
}

int main (int argc, char *argv[])
{
  int i;
  int times = 0;
  FILE *out;
  if (argc>1) {
    assert (sscanf (argv[1], "%d", &times) == 1);
  } else {
    times = DEFAULT_TIMES;
  }
  fill_array();
  //dump_array (stderr);
  start_timer();
  for (i=0; i<times; i++)  {
#if defined BLUR_UNROLL_INNER
    blur_unroll_inner ();
#elif defined BLUR_USE_FUNCTION_CALL
    blur_funcall ();
#elif defined BLUR_USE_SWITCH
    blur_switch();
    /// With BLUR_USE_SWITCH, don't expect the result to match.  I had to make
    //all the cases in the switch do something slightly different, so that they
    //weren't all merged together by a compiler optimization.\n");
#elif defined BLUR_USE_SWITCH_CALL
    blur_switch_call();
#elif defined BLUR_FNPTR_SWITCH
    bluf_fnptr_switch();
#elif defined BLUR_FNPTR_TABLE
    blur_fnptr_table();
#else
    blur_simple();
#endif
  }
  stop_timer();
  report_time (stdout, times);
  //fprintf (stderr, "-----------------------------------\n");
  out = fopen ("blur.out", "w");
  assert (out != NULL);
  dump_array (out, array2);
  fclose (out);
  return 0;
}
