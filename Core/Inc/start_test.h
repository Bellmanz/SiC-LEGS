//void start_test(void);
//void convert_8bit(uint8_t * buffer);
void setDAC(uint32_t);
void start_test(void);
void sic_get_data(unsigned char *buf, unsigned long len, unsigned long data_offset);
void clear_sic_buffer (void);
void sic_test_driver(void);
void readADCvalues(uint8_t);
void setDAC_voltage(uint32_t);
