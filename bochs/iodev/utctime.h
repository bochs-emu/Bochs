/////////////////////////////////////////////////////////////////////////
// $Id: utctime.h
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2023  The Bochs Project
//
//  Portable gmtime()/timegm() clones by Michele Giacomone
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
/////////////////////////////////////////////////////////////////////////

#ifndef UTCTIME_H
#define UTCTIME_H

/*

  I wanted a couple of functions that could mimic the behavior of localtime()/mktime() but being linear (no timezone or DST issues)  and portable.
  For convenience I wrote them so that they behave as if the local timezone was UTC, which means that my "localtime" function - which will be utctime() - must provide identical results as gmtime(), and my "mktime" function - which will be timeutc() - must act exactly as the non-standard function timegm() or mktime() called with TZ set to UTC.
  This allows a more realistic rtc/cmos simulation since the internally kept time value can no longer bounce around due to external factors.
  Also, by doing this, across every system, a given internal value will always mean the same time and date setting.
  If the user selects the cmos to be set up with their local time, the calculated difference between the current epoch value and bochs internal value could be communicated to them in the "startup time" debug message. The same goes if they choose to specify an epoch value, since that has to be interpreted as the epoch value which would set their corresponding local time, to ensure compatibility with previous bochs' behavior. If the user selects to set the current UTC time value instead, the value returned by time(NULL) is already what needed.

A few considerations on operand sizes:

These are the contents of "struct tm":

tm_sec    int seconds after the minute  0-61*
tm_min    int minutes after the hour    0-59
tm_hour   int hours since midnight      0-23
tm_mday   int day of the month          1-31
tm_mon    int months since January      0-11
tm_year   int years since 1900
tm_wday   int days since Sunday         0-6
tm_yday   int days since January 1      0-365
tm_isdst  int Daylight Saving Time flag

*60 in C11. Technically to account for leap seconds. Since they have the same epoch value as the (normal) following second, and my functions can treat out of range values correctly, I can safely assume 0-59.

I use Bit64s instead of time_t to represent the number of seconds since 1970-1-1 00:00:00 (which I just call "epoch") since the latter is not well defined, and 32bits won't fully cover dates that a human might want to input (==at least a few hundred years both ways).

I also need to redefine "struct tm" to protect against different "int" sizes. As said below, to get everything to work properly the size of its elements has to be less than the size of the epoch, but must allow all in-range values, so Bit32s and Bit16s are the options. I choose Bit16s since bochs can't handle years less than 0 or more than 25599 (valid values: -1900 to 23699, included).

The following will work if division does round towards zero.
Everything will operate correctly for all the valid (values in range, or convertible to in range values) struct tm settings.
Despite the fact that "invalid" (values out of range and not convertible to in range values) struct tm settings could be used to get otherwise unreachable epoch values through timeutc() (but the reverse operation through utctime() would not be allowed since it can only return in-range values), timeutc() has to return -1 to properly mimic timegm()/mktime(). I'm not too happy with this since -1 means a possibly useful date (last second of 1969), and a better value would have been (1<<63), so that can be changed in the future if this -1 behavior is not used by bochs.

With the above considerations I tested that:

utctime() supports all values that can be converted to a VALID struct configuration (as gmtime() does):
MIN utctime() value:     -1036267257600 =>     -30868/01/01 FRI  0:00:00
MAX utctime() value:      1031849193599 =>      34667/12/31 TUE 23:59:59
Values out of this range will not set anything and a NULL pointer will be returned.

timeutc() supports all the same values, making it the exact inverse of utctime():
MIN utctime() value:     -1036267257600 =>     -30868/01/01 FRI  0:00:00
MAX utctime() value:      1031849193599 =>      34667/12/31 TUE 23:59:59
Out of range or not set values will be fixed through a call to utcime() before returning.
Invalid struct settings will cause timeutc() to return -1 and the struct not to be set.

A utctime_ext() function is provided. It is identical to utctime() but allows to use an external structure (in fact, it is called by utctime() to set the globally available struct utctm).

An ascutc() function is provided. It serves as an ascitime() clone. It makes up a string with no additional checks.

A pushtm() function is provided. It's used to copy a struct tm into the struct uctm globally available.

Bochs current CMOS code can only handle properly the following range (Gregorian calendar):
MIN     setting             -62167219200 =>          0/01/01 SAT  0:00:00
MAX BCD setting             253402300799 =>       9999/12/31 FRI 23:59:59
MAX BIN setting             745690751999 =>      25599/12/31 FRI 23:59:59
With binary mode only accessible by loading a cmos memory map file
Either way, settings out of the currently valid range must wrap around correctly, giving the impression of a simple overflow. Luckily the valid range starts on a Saturday and ends on a Friday.
The allowed epoch values could all fit in 40bits
*/

struct utctm {
  Bit16s tm_sec;  // seconds after the minute 0-59
  Bit16s tm_min;  // minutes after the hour   0-59
  Bit16s tm_hour; // hours since midnight     0-23
  Bit16s tm_mday; // day of the month         1-31
  Bit16s tm_mon;  // months since January     0-11
  Bit16s tm_year; // years since 1900
  Bit16s tm_wday; // days since Sunday        0-6
  Bit16s tm_yday; // days since January 1     0-365
};

char *ascutc(struct utctm *src)
{
  static const char wdaystr[8][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "N/D"};
  static const char monstr[13][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", "N/D"};
  static char timestr[28];
  sprintf(timestr,"%.3s %.3s %2d %2d:%02d:%02d %6d\n", wdaystr[((src->tm_wday>=0)&&(src->tm_wday<7))?src->tm_wday:7], monstr[((src->tm_mon>=0)&&(src->tm_mon<12))?src->tm_mon:12], ((Bit16u)src->tm_mday)%100, ((Bit16u)src->tm_hour)%100, ((Bit16u)src->tm_min)%100, ((Bit16u)src->tm_sec)%100, (Bit32s)src->tm_year+1900);  //Set the string up using extra caution to protect against bogus values, since snprintf() might not be available
  return (char*) &timestr;
}

struct utctm *utctime_ext(const Bit64s *a,struct utctm *trgt)
{
  static const Bit32s monthlydays[2][13]={{0,31,59,90,120,151,181,212,243,273,304,334,365},{0,31,60,91,121,152,182,213,244,274,305,335,366}}; //Days elapsed between the start of the selected month and the start of the year
  Bit8u isleap=0;                                                       //Leap year flag
  struct utctm bdt;                                                     //Structure to temporary store the output
  Bit64s etmp=*a;                                                       //Temporary variable, epoch since 1970
  Bit64s tsec;                                                          //Temporary variable, seconds
  Bit64s eyear=2001;                                                    //Temporary variable, year

  tsec=etmp%(24*60*60);                                                 //Get time of day
  etmp/=(24*60*60);                                                     //Get days number
  etmp-=11323;                                                          //Use 2001-01-01 as the base of the days number, it being the nearest first non-leap year of a 400yrs cycle
  if(tsec<0){etmp--;tsec+=(24*60*60);};                                 //Get a positive time_of_day number and a properly rounded days number

  bdt.tm_sec=tsec%60;                                                   //Set the seconds value
  tsec/=60;
  bdt.tm_min=tsec%60;                                                   //Set the minutes value
  tsec/=60;
  bdt.tm_hour=tsec;                                                     //Set the hour value

  bdt.tm_wday=(etmp-6)%7;
  if(bdt.tm_wday<0) bdt.tm_wday+=7;                                     //Set the day of the week value

  if(etmp<0){eyear+=400*(etmp/146097-1);etmp%=146097;etmp+=146097;};    //Years before 2001 accounted for

  eyear+=400*(etmp/146097);                                             //Add the number of 400yr cycles
  etmp%=146097;
  eyear+=100*(etmp/36524);                                              //Add the number of 100yr cycles
  etmp%=36524;
  eyear+=4*(etmp/1461);                                                 //Add the number of   4yr cycles
  etmp%=1461;
  while((eyear%4)&&(etmp>=365)){eyear++;etmp-=365;}                     //Add the number of remaining years;

  isleap|=((eyear%400)? 0: 2);
  isleap|=((eyear%  4)? 0: 1);
  isleap&=((eyear%100)?~0:~1);
  isleap=(isleap?1:0);                                                  //Find out if the year is leap

  eyear-=1900;
  bdt.tm_year=eyear;                                                    //Set the year value

  bdt.tm_yday=etmp;                                                     //Set the day of the year value
  bdt.tm_mon=0;
  while(etmp>=monthlydays[isleap][bdt.tm_mon+1]) bdt.tm_mon++;          //Set the month value
  etmp-=monthlydays[isleap][bdt.tm_mon];
  bdt.tm_mday=etmp+1;                                                   //Set the day of the month value

  if(eyear != bdt.tm_year) return NULL;                                 //If the calculated year is too high fail

  trgt->tm_sec=bdt.tm_sec;                                            //Else write back in the structure proper values
  trgt->tm_min=bdt.tm_min;                                            //And return its address
  trgt->tm_hour=bdt.tm_hour;
  trgt->tm_wday=bdt.tm_wday;
  trgt->tm_yday=bdt.tm_yday;
  trgt->tm_mday=bdt.tm_mday;
  trgt->tm_mon=bdt.tm_mon;
  trgt->tm_year=bdt.tm_year;
  return trgt;
}

Bit64s timeutc(struct utctm *bdt)
{
  static const Bit32s monthlydays[2][13]={{0,31,59,90,120,151,181,212,243,273,304,334,365},{0,31,60,91,121,152,182,213,244,274,305,335,366}}; //Days elapsed between the start of the selected month and the start of the year
  Bit8u   isleap=3;                                                     //Leap year flag
  Bit32s  tmon;                                                         //Temporary month value
  Bit64s  epoch=0;                                                      //Value to return
  Bit64s  etmp;                                                         //Temporary variable

  etmp=bdt->tm_year;
  tmon=bdt->tm_mon;

  etmp+=tmon/12;
  tmon%=12;
  if(tmon<0){etmp--;tmon+=12;};                                         //Broken month value accounted for

  etmp-=101;                                                            //Years passed since 2001
  if(etmp<0){epoch+=(146097*(etmp/400 - 1)); etmp%=400; etmp+=400;};    //Years before 2001 accounted for

  epoch+=(etmp/400)*146097;                                             //Add in epoch the number of days corresponding to completed 400yr cycles
  etmp%=400;
  isleap&=((etmp==399)?~0:~2);                                          //Clear bit1 if the year can not be divided by 400
  epoch+=(etmp/100)*36524;                                              //Add in epoch the number of days corresponding to completed 100yr cycles
  etmp%=100;
  isleap&=((etmp==99)?~1:~0);                                           //Clear bit0 if the year can be divided by 100
  epoch+=(etmp/4)*1461;                                                 //Add in epoch the number of days corresponding to completed   4yr cycles
  etmp%=4;
  isleap&=((etmp==3)?~0:~1);                                            //Clear bit0 if the year can not divided by 4
  isleap=(isleap?1:0);                                                  //Shrink the flag to a single bit
  epoch+=etmp*365;                                                      //Add in epoch the number of days corresponding to completed years

  //Now we have in epoch the number (positive or negative) of days between the start of the current year and the start of 2001 and isleap set if the year is leap

  epoch+=monthlydays[isleap][tmon];
  epoch+=bdt->tm_mday-1;                                                //Now we have in epoch the number of entire days between the current date and 2001-01-01 00:00
  epoch*=24;
  epoch+=bdt->tm_hour;                                                  //Now we have in epoch the number of hours
  epoch*=60;
  epoch+=bdt->tm_min;                                                   //Now we have in epoch the number of minutes
  epoch*=60;
  epoch+=bdt->tm_sec;                                                   //Now we have a positive or negative number of seconds between the input time and 2001-01-01 00:00:00
  epoch+=978307200;                                                     //Now we have a positive or negative number of seconds between the input time and 1970-01-01 00:00:00

  if(utctime_ext(&epoch,bdt)) return epoch;                             //Set if possible all fields so they are in their proper ranges
  else return -1;
}

static struct utctm timedata;

struct utctm *utctime(const Bit64s *a)
{
  return utctime_ext(a,&timedata);
}

struct utctm *pushtm(struct tm *src)
{
  timedata.tm_sec=src->tm_sec;
  timedata.tm_min=src->tm_min;
  timedata.tm_hour=src->tm_hour;
  timedata.tm_wday=src->tm_wday;
  timedata.tm_yday=src->tm_yday;
  timedata.tm_mday=src->tm_mday;
  timedata.tm_mon=src->tm_mon;
  timedata.tm_year=src->tm_year;
  return &timedata;
}

#endif // UTCTIME_H
