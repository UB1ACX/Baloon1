#include "gps.h"

//char cmd[40] = ("PMTK000\0");
//char cmd[40] = ("PMTK605\0");
//char cmd[40] = ("PMTK604*30\r\n"); // unsupported EB-500
//	char cmd[50] = ("PMTK314,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0\0");


/* GPS init */
void GPS_Init(void)
{
	USART2_Init(GPS_BAUDRATE);
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPS_POWER_ON;
	RCC->AHBENR &= ~RCC_AHBENR_GPIOBEN;
	while ((ResetFlags & 0x80000000) != 0x80000000);
//	GPS_Comm(&cmd[0]);
}

void GPS_Comm(char *pstr)
{
	uint8_t CheckSum;
	uint16_t i;
	CheckSum = 0;
	i = 0;
	USART2_SendChar('$');
	while (pstr[i] != 0)
	{
		USART2_SendChar(pstr[i]);
		CheckSum ^= pstr[i++];
	}
	USART2_SendChar('*');
	if ((CheckSum >> 4) <= 0x09) USART2_SendChar((CheckSum >> 4) | '0');
	else 	USART2_SendChar((CheckSum >> 4) - 10 + 'A');
	if ((CheckSum & 0x0F) <= 0x09) USART2_SendChar((CheckSum & 0x0F) | '0');
	else 	USART2_SendChar((CheckSum & 0x0F) - 10 + 'A');
	USART2_SendChar('\r');
	USART2_SendChar('\n');
}
/*
GGA - essential fix data which provide 3D location and accuracy data.

 $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
			       4 = Real Time Kinematic
			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
			       7 = Manual input mode
			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
     *47          the checksum data, always begins with *

If the height of geoid is missing then the altitude should be suspect. Some non-standard implementations report altitude with respect to the ellipsoid rather than geoid altitude. Some units do not report negative altitudes at all. This is the only sentence that reports altitude.

GSA - GPS DOP and active satellites. This sentence provides details on the nature of the fix. It includes the numbers of the satellites being used in the current solution and the DOP. DOP (dilution of precision) is an indication of the effect of satellite geometry on the accuracy of the fix. It is a unitless number where smaller is better. For 3D fixes using 4 satellites a 1.0 would be considered to be a perfect number, however for overdetermined solutions it is possible to see numbers below 1.0.

There are differences in the way the PRN's are presented which can effect the ability of some programs to display this data. For example, in the example shown below there are 5 satellites in the solution and the null fields are scattered indicating that the almanac would show satellites in the null positions that are not being used as part of this solution. Other receivers might output all of the satellites used at the beginning of the sentence with the null field all stacked up at the end. This difference accounts for some satellite display programs not always being able to display the satellites being tracked. Some units may show all satellites that have ephemeris data without regard to their use as part of the solution but this is non-standard.

  $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39

Where:
     GSA      Satellite status
     A        Auto selection of 2D or 3D fix (M = manual) 
     3        3D fix - values include: 1 = no fix
                                       2 = 2D fix
                                       3 = 3D fix
     04,05... PRNs of satellites used for fix (space for 12) 
     2.5      PDOP (dilution of precision) 
     1.3      Horizontal dilution of precision (HDOP) 
     2.1      Vertical dilution of precision (VDOP)
     *39      the checksum data, always begins with *

RMC - NMEA has its own version of essential gps pvt (position, velocity, time) data. It is called RMC, The Recommended Minimum, which will look similar to:

$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A

Where:
     RMC          Recommended Minimum sentence C
     123519       Fix taken at 12:35:19 UTC
     A            Status A=active or V=Void.
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     022.4        Speed over the ground in knots
     084.4        Track angle in degrees True
     230394       Date - 23rd of March 1994
     003.1,W      Magnetic Variation
     *6A          The checksum data, always begins with *

Note that, as of the 2.3 release of NMEA, there is a new field in the RMC sentence at the end just prior to the checksum. For more information on this field see here. 
*/
