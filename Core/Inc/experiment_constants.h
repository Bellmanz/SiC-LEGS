#define EXPERIMENTSIZE sizeof(experiments)
#define DACSTEPS 100
#define DACMAXVOLTAGE 3000 // millivolts
#define DACMINIMUMVOLTAGE 300 // millivolts
#define EXPERIMENTPOINTS ((DACMAXVOLTAGE - DACMINIMUMVOLTAGE)/DACSTEPS + 1)
#define BUFFERLENGTH (EXPERIMENTPOINTS * 4 * 2 * 2)// Number of data points * 4 variables * 2 bytes per variable * 2 Devices Si and SiC
