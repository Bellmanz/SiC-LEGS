//this file was written by  @author: Simon Lagerqvist.
#include "msp_exp_handler.h"
#include <stdint.h>
#include "main.h"
#include "start_test.h"
#include "msp_opcodes.h"
#include <stdbool.h>
#include "piezo.h"
#include "start_test.h"
#include "power_management.h"
#include "sicpiezo_global.h"
#include <interface_flags.h>
#include "experiment_constants.h"


/* uint8_t sicBuffer[64];
uint8_t piezoBuffer[30];
extern bool volatile sic;
extern bool volatile piezo;
extern int piezoTick;
extern bool getData;
extern char piezoBufferRx[50];
extern int piezoBufferRxInt[50];
extern int aTxBuffer[100];
extern uint16_t buffLength; */

/* bool volatile debuggvar = false;
bool debuggvar2 = false;
bool debuggvar3 = false;
extern piezo_sic_type volatile piezo_sic;
bool piezo_error = false;
bool sic_error = false; */


void msp_expsend_start(unsigned char opcode, unsigned long *len)
{
  if (opcode == REQ_PIEZO)
  {
    *len = piezo_get_data_length();
  }
  else if (opcode == REQ_SIC)
  {
    *len = BUFFERLENGTH;
  }
}

void msp_expsend_data(unsigned char opcode, unsigned char *buf, unsigned long len, unsigned long offset)
{
  if (opcode == REQ_PIEZO)
  {
    piezo_get_data(buf, len, offset);
  }
  else if (opcode == REQ_SIC)
  {
     sic_get_data(buf, len, offset);
  }
}

void msp_expsend_complete(unsigned char opcode)
{
// add code to clear buffers
  if (opcode == REQ_PIEZO)
  {
    clear_piezo_buffer();
  }
  else if (opcode == REQ_SIC)
  {
     clear_sic_buffer();
  }
}

void msp_expsend_error(unsigned char opcode, int error)
{

}

void msp_exprecv_start(unsigned char opcode, unsigned long len)
{

}

void msp_exprecv_data(unsigned char opcode, const unsigned char *buf, unsigned long len, unsigned long offset)
{

}

void msp_exprecv_complete(unsigned char opcode)
{

}

void msp_exprecv_error(unsigned char opcode, int error)
{

}

void msp_exprecv_syscommand(unsigned char opcode)
{
  switch(opcode)
  {
    case START_EXP_PIEZO:
      piezo_start_exp();
      break;

    case STOP_EXP_PIEZO:
      piezo_stop_exp();
      break;

    case START_EXP_SIC:
      start_test();
      break;

    case MSP_OP_POWER_OFF:  // Added turn off for all voltages
      save_seqflags();
      turn_off_vbat();
      turn_off_48v();
      turn_off_10v();
      turn_off_5v();
      break;

    case SIC_10V_OFF:  // The following are for testing and should probably be excluded after debug
      turn_off_10v();
      break;

    case PIEZO_5V_OFF:
      turn_off_5v();
      break;

    case PIEZO_48V_OFF:
      turn_off_48v();
      break;

    case VBAT_OFF:
      turn_off_vbat();
      break;

    case SIC_10V_ON:
      turn_on_vbat();  // necessary for 10 V
      turn_on_10v();
      break;

    case PIEZO_5V_ON:
      turn_on_5v();
      break;

    case PIEZO_48V_ON:
      turn_on_vbat();  // necessary for 48 V
      turn_on_48v();
      break;

    case VBAT_ON:
      turn_on_vbat();
      break;
  }
}
