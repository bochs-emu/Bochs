/*
 * Emulator of an Intel 8254/82C54 Programmable Interval Timer.
 * Greg Alexander <yakovlev@usa.com>
 *
 * This code is not yet linked into Bochs, but has been included so
 * that you can experiment with it.  (bbd)
 *
 * 
 * Things I am unclear on (greg):
 * 1.)What happens if both the status and count registers are latched,
 *  but the first of the two count registers has already been read?
 *  I.E.: 
 *   latch count 0 (16-bit)
 *   Read count 0 (read LSByte)
 *   READ_BACK status of count 0
 *   Read count 0 - do you get MSByte or status?
 *  This will be flagged as an error.
 * 2.)What happens when we latch the output in the middle of a 2-part
 *  unlatched read?
 * 3.)I assumed that programming a counter removes a latched status.
 * 4.)I implemented the 8254 description of mode 0, not the 82C54 one.
 * 5.)clock() calls represent a rising clock edge followed by a falling
 *  clock edge.
 * 6.)What happens when we trigger mode 1 in the middle of a 2-part 
 *  write?
 */

#include "bochs.h"
#include "pit82c54.h"
#define LOG_THIS this->

  void pit_82C54::latch_counter(counter_type & thisctr) {
    if(thisctr.count_LSB_latched || thisctr.count_MSB_latched) {
      //Do nothing because previous latch has not been read.;
    } else {
      switch(thisctr.read_state) {
      case MSByte:
	thisctr.outlatch=thisctr.count & 0xFFFF;
	thisctr.count_MSB_latched=1;
      case LSByte:
	thisctr.outlatch=thisctr.count & 0xFFFF;
	thisctr.count_LSB_latched=1;
      case LSByte_multiple:
	thisctr.outlatch=thisctr.count & 0xFFFF;
	thisctr.count_LSB_latched=1;
	thisctr.count_MSB_latched=1;
      case MSByte_multiple:
	BX_ERROR(("Unknown behavior when latching during 2-part read."));
	//I guess latching and resetting to LSB first makes sense;
	thisctr.read_state=LSByte_multiple;
	thisctr.outlatch=thisctr.count & 0xFFFF;
	thisctr.count_LSB_latched=1;
	thisctr.count_MSB_latched=1;
	break;
      default:
	BX_ERROR(("Unknown read mode found during latch command."));
	break;
      }
    }
  }

  void pit_82C54::set_OUT (counter_type & thisctr, bool data) {
    //This will probably have a callback, so I put it here.
    thisctr.OUT=data;
  }

  void pit_82C54::decrement (counter_type & thisctr) {
    if(thisctr.bcd_mode) {
      if(!thisctr.count) {
	thisctr.count=0x9999;
      } else {
	int d0=thisctr.count & 0xF;
	int d1=(thisctr.count>>4)&0xF;
	int d2=(thisctr.count>>8)&0xF;
	int d3=(thisctr.count>>12)&0xF;
	if(d0) {
	  d0--;
	} else {
	  d0=0x9;
	  if(d1) {
	    d1--;
	  } else {
	    d1=0x9;
	    if(d2) {
	      d2--;
	    } else {
	      d2=0x9;
	      d3--;  //if d3==0, we would use the above zero case.;
	    }
	  }
	}
      }
    } else {
      if(!thisctr.count) {
	thisctr.count=0xFFFF;
      } else {
	thisctr.count--;
      }
    }
  }

  void pit_82C54::init (void) {
    Bit8u i;
    for(i=0;i<3;i++) {
      counter[i].read_state=LSByte;
      counter[i].write_state=LSByte;
      counter[i].GATE=1;
      counter[i].OUT=1;
      counter[i].triggerGATE=0;
      counter[i].mode=4;
      counter[i].first_pass=0;
      counter[i].bcd_mode=0;
      counter[i].count=0;
      counter[i].state_bit_1=0;
      counter[i].state_bit_2=0;
      counter[i].null_count=0;
      counter[i].rw_mode=1;
      counter[i].count_written=1;
      counter[i].count_LSB_latched=0;
      counter[i].count_MSB_latched=0;
      counter[i].status_latched=0;
    }
  }

  pit_82C54::pit_82C54 (void) {
    init();
  }

  void pit_82C54::clock(Bit8u cnum) {
    if(cnum>MAX_COUNTER) {
      BX_ERROR(("Counter number too high in clock"));
    } else {
      counter_type & thisctr = counter[cnum];
      switch(thisctr.mode) {
      case 0:
	if(thisctr.count_written) {
	  if(thisctr.null_count) {
	    thisctr.count=thisctr.inlatch;
	    thisctr.null_count=0;
	  } else {
	    if(thisctr.GATE && (thisctr.write_state!=MSByte_multiple)) {
	      decrement(thisctr);
	      if((!thisctr.count) && (!thisctr.OUT)) {
		set_OUT(thisctr,1);
	      }
	    }
	  }
	}
	thisctr.triggerGATE=0;
	break;
      case 1:
	if(thisctr.count_written) {
	  if(thisctr.triggerGATE) {
	    thisctr.count=thisctr.inlatch;
	    thisctr.null_count=0;
	    set_OUT(thisctr,0);
	    if(thisctr.write_state==MSByte_multiple) {
	      BX_ERROR(("Undefined behavior when loading a half loaded count."));
	    }
	  } else {
	    decrement(thisctr);
	    if((thisctr.count==0) && (!thisctr.OUT)) {
	      set_OUT(thisctr,1);
	    }
	  }
	}
	thisctr.triggerGATE=0;
	break;
      case 2:
	if(thisctr.count_written) {
	  if(thisctr.triggerGATE || thisctr.first_pass) {
	    thisctr.count=thisctr.inlatch;
	    thisctr.null_count=0;
	    if(thisctr.inlatch==1) {
	      BX_ERROR(("ERROR: count of 1 is invalid in pit mode 2."));
	    }
	    if(!thisctr.OUT) {
	      set_OUT(thisctr,1);
	    }
	    if(thisctr.write_state==MSByte_multiple) {
	      BX_ERROR(("Undefined behavior when loading a half loaded count."));
	    }
	    thisctr.first_pass=0;
	  } else {
	    if(thisctr.GATE) {
	      decrement(thisctr);
	      if(thisctr.count==1) {
		set_OUT(thisctr,0);
		thisctr.first_pass=1;
	      }
	    }
	  }
	}
	thisctr.triggerGATE=0;
	break;
      case 3:
	if(thisctr.count_written) {
	  if( (thisctr.triggerGATE || thisctr.first_pass
	     || thisctr.state_bit_2) && thisctr.GATE ) {
	    thisctr.count=thisctr.inlatch & 0xFFFE;
	    thisctr.state_bit_1=thisctr.inlatch & 0x1;
	    thisctr.null_count=0;
	    if(thisctr.inlatch==1) {
	      BX_ERROR(("Count of 1 is invalid in pit mode 3."));
	    }
	    if(!thisctr.OUT) {
	      set_OUT(thisctr,1);
	    } else if(thisctr.OUT && !thisctr.first_pass) {
	      set_OUT(thisctr,0);
	    }
	    if(thisctr.write_state==MSByte_multiple) {
	      BX_ERROR(("Undefined behavior when loading a half loaded count."));
	    }
	    thisctr.state_bit_2=0;
	    thisctr.first_pass=0;
	  } else {
	    if(thisctr.GATE) {
	      decrement(thisctr);
	      decrement(thisctr);
	      if(thisctr.count==0) {
		thisctr.state_bit_2=1;
	      }
	      if( (thisctr.count==2) &&
		 ( (!thisctr.OUT) || (!(thisctr.state_bit_1)))
		 ) {
		thisctr.state_bit_2=1;
	      }
	    }
	  }
	}
	thisctr.triggerGATE=0;
	break;
      case 4:
	if(thisctr.count_written) {
	  if(!thisctr.OUT) {
	    set_OUT(thisctr,1);
	  }
	  if(thisctr.null_count) {
	    thisctr.count=thisctr.inlatch;
	    thisctr.null_count=0;
	    if(thisctr.write_state==MSByte_multiple) {
	      BX_ERROR(("Undefined behavior when loading a half loaded count."));
	    }
	    thisctr.first_pass=1;
	  } else {
	    if(thisctr.GATE) {
	      decrement(thisctr);
	      if( (!thisctr.count) && thisctr.first_pass) {
		set_OUT(thisctr,0);
		thisctr.first_pass=0;
	      }
	    }
	  }
	}
	thisctr.triggerGATE=0;
	break;
      case 5:
	if(thisctr.count_written) {
	  if(!thisctr.OUT) {
	    set_OUT(thisctr,1);
	  }
	  if(thisctr.triggerGATE) {
	    thisctr.count=thisctr.inlatch;
	    thisctr.null_count=0;
	    if(thisctr.write_state==MSByte_multiple) {
	      BX_ERROR(("Undefined behavior when loading a half loaded count."));
	    }
	    thisctr.first_pass=1;
	  } else {
	    if(thisctr.GATE) {
	      decrement(thisctr);
	      if( (!thisctr.count) && thisctr.first_pass) {
		set_OUT(thisctr,0);
		thisctr.first_pass=0;
	      }
	    }
	  }
	}
	thisctr.triggerGATE=0;
	break;
      default:
	BX_ERROR(("Mode not implemented."));
	thisctr.triggerGATE=0;
	break;
      }
    }
  }

  void pit_82C54::clock_all(void) {
    clock(0);
    clock(1);
    clock(2);
  }

  Bit8u pit_82C54::read(Bit8u address) {
    if(address>MAX_ADDRESS) {
      BX_ERROR(("Counter address incorrect in data read."));
    } else if(address==CONTROL_ADDRESS) {
      //Read from control word register;
      /* This might be okay.  If so, 0 seems the most logical
       *  return value from looking at the docs.
       */
      BX_ERROR(("Read from control word register not defined."));
      return 0;
    } else {
      //Read from a counter;
      counter_type & thisctr=counter[address];
      if(thisctr.status_latched) {
	//Latched Status Read;
	if(thisctr.count_MSB_latched &&
	   (thisctr.read_state==MSByte_multiple) ) {
	  BX_ERROR(("Undefined output when status latched and count half read."));
	} else {
	  thisctr.status_latched=0;
	  return thisctr.status_latch;
	}
      } else {
	//Latched Count Read;
	if(thisctr.count_LSB_latched) {
	  //Read Least Significant Byte;
	  if(thisctr.read_state==LSByte_multiple) {
	    thisctr.read_state=MSByte_multiple;
	  }
	  thisctr.count_LSB_latched=0;
	  return (thisctr.outlatch & 0xFF);
	} else if(thisctr.count_MSB_latched) {
	  //Read Most Significant Byte;
	  if(thisctr.read_state==MSByte_multiple) {
	    thisctr.read_state=LSByte_multiple;
	  }
	  thisctr.count_MSB_latched=0;
	  return ((thisctr.outlatch>>8) & 0xFF);
	} else {
	  //Unlatched Count Read;
	  if(!(thisctr.read_state & 0x1)) {
	    //Read Least Significant Byte;
	    if(thisctr.read_state==LSByte_multiple) {
	      thisctr.read_state=MSByte_multiple;
	    }
	    return (thisctr.count & 0xFF);
	  } else {
	    //Read Most Significant Byte;
	    if(thisctr.read_state==MSByte_multiple) {
	      thisctr.read_state=LSByte_multiple;
	    }
	    return ((thisctr.count>>8) & 0xFF);
	  }
	}
      }
    }
    //Should only get here on errors;
    return 0;
  }

  void pit_82C54::write(Bit8u address, Bit8u data) {
    if(address>MAX_ADDRESS) {
      BX_ERROR(("Counter address incorrect in data write."));
    } else if(address==CONTROL_ADDRESS) {
      Bit8u SC, RW, M, BCD;
      controlword=data;
      SC = (controlword>>6) & 0x3;
      RW = (controlword>>4) & 0x3;
      M = (controlword>>1) & 0x7;
      BCD = controlword & 0x1;
      if(SC == 3) {
	//READ_BACK command;
	int i;
	for(i=0;i<=MAX_COUNTER;i++) {
	  if((M>>i) & 0x1) {
	    //If we are using this counter;
	    counter_type & thisctr=counter[i];
	    if(!((controlword>>5) & 1)) {
	      //Latch Count;
	      latch_counter(thisctr);
	    }
	    if(!((controlword>>4) & 1)) {
	      //Latch Status;
	      if(thisctr.status_latched) {
		//Do nothing because latched status has not been read.;
	      } else {
		thisctr.status_latch=
		  ((thisctr.OUT & 0x1) << 7) |
		  ((thisctr.null_count & 0x1) << 6) |
		  ((thisctr.rw_mode & 0x3) << 4) |
		  ((thisctr.mode & 0x7) << 1) |
		  (thisctr.bcd_mode&0x1)
		  ;
		thisctr.status_latched=1;
	      }
	    }
	  }
	}
      } else {
	counter_type & thisctr = counter[SC];
	if(!RW) {
	  //Counter Latch command;
	  latch_counter(thisctr);
	} else {
	  //Counter Program Command;
	  thisctr.null_count=1;
	  thisctr.count_LSB_latched=0;
	  thisctr.count_MSB_latched=0;
	  thisctr.status_latched=0;
	  thisctr.inlatch=0;
	  thisctr.count_written=0;
	  thisctr.first_pass=1;
	  thisctr.rw_mode=RW;
	  thisctr.bcd_mode=BCD;
	  thisctr.mode=M;
	  switch(RW) {
	  case 0x1:
	    thisctr.read_state=LSByte;
	    thisctr.write_state=LSByte;
	    break;
	  case 0x2:
	    thisctr.read_state=MSByte;
	    thisctr.write_state=MSByte;
	    break;
	  case 0x3:
	    thisctr.read_state=LSByte_multiple;
	    thisctr.write_state=LSByte_multiple;
	    break;
	  default:
	    BX_ERROR(("RW field invalid in control word write."));
	    break;
	  }
	  //All modes except mode 0 have initial output of 1.;
	  if(M) {
	    set_OUT(thisctr, 1);
	  } else {
	    set_OUT(thisctr, 0);
	  }
	}
      }
    } else {
      //Write to counter initial value.
      counter_type & thisctr = counter[address];
      switch(thisctr.write_state) {
      case LSByte_multiple:
	thisctr.inlatch=(thisctr.inlatch & (0xFF<<8)) | data;
	thisctr.write_state=MSByte_multiple;
	break;
      case LSByte:
	thisctr.inlatch=(thisctr.inlatch & (0xFF<<8)) | data;
	thisctr.null_count=1;
	thisctr.count_written=1;
	break;
      case MSByte_multiple:
	thisctr.write_state=LSByte_multiple;
      case MSByte: //shared between MSB_multiple and MSByte
	thisctr.inlatch=(thisctr.inlatch & 0xFF) | (data<<8);
	thisctr.null_count=1;
	thisctr.count_written=1;
	break;
      default:
	BX_ERROR(("write counter in invalid write state."));
	break;
      }
      switch(thisctr.mode) {
      case 0:
	if(thisctr.write_state==MSByte_multiple) {
	  set_OUT(thisctr,0);
	}
	break;
      case 1:
	break;
      case 6:
      case 2:
	break;
      case 7:
      case 3:
	break;
      case 4:
	break;
      case 5:
	break;
      }
    }
  }

  void pit_82C54::set_GATE(Bit8u cnum, bool data) {
    if(cnum>MAX_COUNTER) {
      BX_ERROR(("Counter number incorrect in 82C54 set_GATE"));
    } else {
      counter_type & thisctr = counter[cnum];
      if((!thisctr.GATE)&&data) {
	thisctr.triggerGATE=1;
      }
      thisctr.GATE=data;
      switch(thisctr.mode) {
      case 2:
	if(!data) {
	  set_OUT(thisctr,1);
	}
	break;
      case 3:
	if(!data) {
	  set_OUT(thisctr,1);
	  thisctr.first_pass=1;
	}
	break;
      default:
	break;
      }
    }
  }

  bool pit_82C54::read_OUT(Bit8u cnum) {
    if(cnum>MAX_COUNTER) {
      BX_ERROR(("Counter number incorrect in 82C54 read_OUT"));
      return 0;
    } else {
      return counter[cnum].OUT;
    }
  }

  bool pit_82C54::read_GATE(Bit8u cnum) {
    if(cnum>MAX_COUNTER) {
      BX_ERROR(("Counter number incorrect in 82C54 read_GATE"));
      return 0;
    } else {
      return counter[cnum].GATE;
    }
  }

