/****************************************************************************
 * Piezo-LEGS communication & handler written FOR KTH MIST (SiC in space)   *
 ****************************************************************************/

/**
 *****************************************************************************
 * @file piezo.c
 * @date 2019-06-15 2021-11-28
 * @bug no known bugs
 * @brief Interface for the Piezo motor.
 *****************************************************************************
 * this file contains functions that can be used in order to interact with
 * pizo-leggs via rs485
 * Pin names renamed for V4 PCB and final release USART1_NRE_Pin and USART1_DE_Pin
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "piezo.h"
#include "usart.h"
#include "power_management.h"
#include "tools.h"


int NUMBER_OF_READ_ATTEMTS = 3;

/* predifined commands for the motor */
char xm3_buffer[4]="XM3;";
char xm4_buffer[4]="XM4;";

/* data section */
uint8_t xu6_buffer[17]; // used for sending data request, changed from 6 to 17 to avoid warning
uint8_t saveDataPointer[1];
uint8_t piezoData[200];
int piezoBufferRxInt[200];
uint8_t piezoBufferint8[(sizeof(piezoBufferRxInt) / sizeof(piezoBufferRxInt[0]) << 1)];
extern UART_HandleTypeDef huart1;
int dataLength = 0;


/**
	Sets the mode of RS-485 communication. Needs to be called before
	attempting transmitting or receiveing data.

	Always end a transaction with mode RS_MODE_DEACTIVATE.
	This command sets the transceiver to low power mode(turns off).

	Example:
		RS485(RS_MODE_RECEIVE);
		HAL_UART_Receive(&huart1, (uint8_t *)saveDataPointer, 1, 100);

		// If no more communication will be done
		RS485(RS_MODE_DEACTIVATE)

*/
void RS485(uint8_t rs485_mode){
	switch (rs485_mode) {
		case RS_MODE_RECEIVE:
			HAL_GPIO_WritePin(GPIOA, USART1_NRE_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, USART1_DE_Pin, GPIO_PIN_RESET);
                        break;

		case RS_MODE_TRANSMIT:
			HAL_GPIO_WritePin(GPIOA, USART1_NRE_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, USART1_DE_Pin, GPIO_PIN_SET);
                        break;

		case RS_MODE_DEACTIVATE:
			HAL_GPIO_WritePin(GPIOA, USART1_NRE_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, USART1_DE_Pin, GPIO_PIN_RESET);
                        break;
                
                //case RS_MODE_TRANSMIT
	}
}


void clear_piezo_buffer (void)
{
	dataLength = 0;
	//    Flush_Buffer8(piezoData, (dataLength));
	//    Flush_Buffer8(piezoBufferint8, (dataLength));
}

/**
 * @brief starts the motor by sending xm3
 */
void piezo_start_exp(void)
{
  piezo_power_on();
  HAL_Delay(3000); // time it takes for the motor to turn on.
  RS485(RS_MODE_TRANSMIT);
  HAL_UART_Transmit(&huart1, (uint8_t *)xm3_buffer, 4, 1000);
  RS485(RS_MODE_DEACTIVATE);
}

/**
 * @brief stops the motor by sending xm4
 */
void piezo_stop_exp(void)
{
  RS485(RS_MODE_TRANSMIT);
  HAL_UART_Transmit(&huart1, (uint8_t *)xm4_buffer, 4, 1000);
  dataLength = piezo_read_data_records();
  piezo_power_off();
  RS485(RS_MODE_DEACTIVATE); // Not really necessary, just added for clarity

}

/**
 * @brief retrieves the data collected data from the buffer
 * @param buffer to copy the data to
 * @param the amount of data to copy
 * @param the data offset from where to start copying
 */
void piezo_get_data(unsigned char *buf, unsigned long len, unsigned long data_offset)
{
  for (unsigned long i = data_offset; i < dataLength && i - data_offset < len; i++)
  {
    buf[i - data_offset] = piezoBufferint8[i];
  }
}

/**
 * @brief retrieves the length of the buffer
 */
int piezo_get_data_length(void)
{
  return dataLength;
}

/**
 * @brief retrieves data from pizo leggs
 * @prarm buffer to copy the data to
 * @param the data offset
 *
 * this function will read data until an empty record is found
 * or until the max read attempts is reached.
 * checksum is checked and the data is formated in 8bit integers
 */

unsigned char piezo_read_data_records(void)
{
  bool isThereMoreData = true;
  int record_counter = 0;
  uint16_t dataOffset = 0;
  int a=0;

  //read data records until a empty record is read.
  while(isThereMoreData)
  {
    //printf("Gather");
    //retry reading data until max attempts is reached if checksum was not ok.
    for (a =0; a<NUMBER_OF_READ_ATTEMTS; a++)
    {

      //we already know there is more data to read since we pased while, need to
     //be reset because of checksum check since we want to exit if we fail checksum test more than max read attempts
      isThereMoreData = true;


      sprintf((char *)xu6_buffer, "XU6,%d\r", record_counter);
      //printf("%d", xu6_buffer);
      int i=0;
      RS485(RS_MODE_TRANSMIT); // Set transceiver to transmit
      HAL_Delay(10);
      HAL_UART_Transmit(&huart1, (uint8_t *)xu6_buffer, 6, 100);

      //HAL_Delay(10);
      RS485(RS_MODE_RECEIVE); // Set transceiver to receive
      //HAL_Delay(10);
      while(1)
      {
        if(HAL_UART_Receive(&huart1, (uint8_t *)saveDataPointer, 1, 100) != HAL_OK)
          //printf("Abort reception of data");
          break; // stop reading
        piezoData[i++] = saveDataPointer[0];
        //printf("%d\n", piezoData[i]);
        if ('\r' == saveDataPointer[0]||i == sizeof(piezoData) - 1)//so we do not write outside array
           //printf("r received");
           break; // stop reading
      }
      
      HAL_Delay(1000);
      RS485(RS_MODE_DEACTIVATE); // Turn off communication

      //check if record was empty, or we have received too much data
      if(record_was_empty((char *)&piezoData[6]) || dataOffset + 9 >= sizeof(piezoBufferRxInt))
      {
        isThereMoreData = false;
        break; // break the attempt loop
      }
      //convert to integers, skip (xu6:) therefore 4
      ascii_to_int((char *)&piezoData[4], (int *)&piezoBufferRxInt[dataOffset]);

      //calulate checksum
      if(piezo_checksum((int *)&piezoBufferRxInt[dataOffset]))
      {
        //if the checksum was correct, read the next record.
        dataOffset += 9;
        record_counter++;
        break; // break the attempt loop
      }
      else
      {
        //flush the last recived incorrect values
        Flush_Buffer8(piezoData, i);
        isThereMoreData = false;
      }
    }
  }
  // Used to be dataOffset*2 here which is incorrect since that is the amount
  // of indices in piezoBufferint8, not piezoBufferRxInt
  convert_to_big_endian(piezoBufferint8, dataOffset);
  // Return value should be dataOffset*2 since it is the length of piezoBufferint8
  // which is the data sent
  return dataOffset*2;
}

/**
 * @brief check if the record that was read was empty, either by containg 0,0,0
 * or by containing null.
 * @prarm buffer to copy the data to
 * @param the data offset
 */
bool record_was_empty(char * bufferIn)
{
   // Previous implementation assumed that all values being equal to 384 means
   // that all values are '0', this is not always correct
   for (int i = 0; i <= 14; i += 2) {
       if (bufferIn[i] != '0') {
           for (int j = 0; j <= 14; j += 2) {
               if (bufferIn[j] != '\0')
                   return false;
           }
           return true;
       }
   }
   return true;
}



/**
 * @brief calculated the checksum for a command that was recived by from piezo
 * @prarm the buffer to calculate the checksum from
 * @param if the checksum was correct
 */
bool piezo_checksum(int *piezoBufferRx)
{
    piezoBufferRx++;
    int a=0;
    for(int x=0; x<9; x++)
    {
        a = a ^ *piezoBufferRx;

        piezoBufferRx++;
    }
    if (a==*piezoBufferRx)
    {
      return true;
    }
   return false;
}

/**
 * @brief Converts a series of nine ascii encoded integers into nine decimal integers.
 *        The ascii integers should be separated by a non-numerical character
 * @prarm buffer with ascii characters
 * @param buffer that the integer valued should be written to
 * @note Assumes that each ascii integer fits within an 'int', otherwise the converted
 *       value will be undefined
 */
void ascii_to_int (char * bufferIn, int * bufferOut)
{
  for (int i = 0; i < 9; i++)
  {
      int temp = 0;
      while(((*bufferIn >= '0')&&(*bufferIn <= '9')))
      {
          temp = temp*10;
          temp = temp + (*bufferIn - '0');
          bufferIn++;
      }
      *bufferOut=temp;
      bufferOut++;
      bufferIn++;
  }
}

void convert_to_big_endian(uint8_t * buffer, uint16_t length)
{
  for (int i = 0; i < length; i++)
  {
      buffer[(i << 1)] = (piezoBufferRxInt[i] >> 8) & 0xFF;
      buffer[(i << 1) + 1] = piezoBufferRxInt[i]  & 0xFF;
  }
}
