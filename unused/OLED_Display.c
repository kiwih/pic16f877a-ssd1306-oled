
/************ OLED Display.c *********/

#include "OLED_Display.h"

#include "i2c.h"

char OLEDSetUp ( void )
{
#if defined RESET_PIN_NEEDED
	__delay_ms ( 1 );				// As OLDE VDD goes high sooner than VCC,
	OLED_RESET_PIN = OFF;			// we must wait to stabilize VCC
	__delay_ms ( 1 );				// OLED Reset pin.
	OLED_RESET_PIN = ON;			
#endif

// 1. Fundamental Commands.
	OLEDCommand ( OLED_CONTRAST );				// 0x81
	OLEDCommand ( 0x4F );						// was 0x9F. ******
	OLEDCommand ( OLED_DISPLAY_ALL_ON_RESUME );	// 0xA5
	OLEDCommand ( OLED_NORMAL_DISPLAY );		// 0xA6
	OLEDCommand ( OLED_DISPLAY_ON );			// 0xAF

// 2. Scrolling.

// 3. Addressing.
	OLEDCommand ( OLED_MEMORY_MODE );			// 0x20
	OLEDCommand ( OLED_HORIZONTAL_MODE );
		
// 4. Hardware Configuration.
	OLEDCommand ( OLED_SET_START_LINE | 0x00 );	// line #0
	OLEDCommand ( OLED_SEGMENT_REMAP | 0x01 );	// WTF???
	OLEDCommand ( OLED_SET_MULTIPLEX_RATIO );	// 0xA8
	OLEDCommand ( 0x3F );
	OLEDCommand ( OLED_COM_SCAN_DECREMENT );	// WTF???
	OLEDCommand ( OLED_DISPLAY_OFFSET );		// 0xD3
	OLEDCommand ( 0x0 );						// no offset
	OLEDCommand ( OLED_SET_COM_PINS );			// 0xDA
	OLEDCommand ( 0x12 );

// 5. Timing.
	OLEDCommand ( OLED_DISPLAY_CLOCK_RATIO );	// 0xD5
	OLEDCommand ( OLED_CLOCK_RESET_RATIO );
	OLEDCommand ( OLED_SET_PRECHARGE );			// 0xd9
	OLEDCommand ( OLED_PRECHARGE_P1_P2 );		// 0xF1
	OLEDCommand ( OLED_SET_VCOM_H );			// 0xDB
	OLEDCommand ( 0x40 );						// WTF???

// 6?. Other 
	OLEDCommand ( OLED_CHARGE_PUMP );			// 0x8D
	OLEDCommand ( OLED_ENABLE_CHARGE );			// 0x14

	OLEDStopScroll ( );
	
	return 0x00;
	}
	
	
void OLEDCommand ( unsigned char c )
{
	unsigned char command = 0x00;	// Co = 0, D/C=0, followed by 00 0000.
	
	I2C_Begin();
	I2C_Write ( OLED_ADDRESS );
	I2C_Write ( command );
	I2C_Write ( c );
	I2C_End();
	}
	

/********** prints out the Data to the GLCD ********/
void OLEDData ( unsigned char c ) 
{
	unsigned char control = 0x40;   // Co = 0, D/C = 1

	I2C_Begin();
	I2C_Write ( OLED_ADDRESS );
	I2C_Write ( control );
	I2C_Write ( c );
	I2C_End();
	}


/********** prints out a pixel on the GLCD ********/
void OLEDPixel ( int x, int y, char color )
{
	unsigned char *ptr;
	unsigned char page;
	unsigned int axisX;
	unsigned int axisY;
	signed char bitNumber, bitPlace;
	unsigned int address;


	if ( ( x < ARRAY_WIDHT && y <= ARRAY_LENGHT ) && 
		( x >= 0x00 && y >= 0x00 ) )
	{
		axisY = y / 8;							// Getting the row value.
		axisX = x;								// Getting the column value.
		page = abs ( axisY );					// Getting the Page number.
		bitNumber = ( y % 8 );					// Getting the bit value on the column.
		
/*		printf ( " \nX:%d, seg#:%d", x, axisX );
		printf ( " \nY:%d, PAGE:%d", y, page );
		printf ( " \nbit #:%d", bitNumber );
*/
		int temp;
		temp =  y / 8;
		temp *= ARRAY_WIDHT;
		temp += x;
		ptr = ( char * ) TEST;

	// Set Column Address.
		OLEDCommand ( OLED_COLUMN_ADDRESS );		
		OLEDCommand ( axisX );					// Column start address.
		OLEDCommand ( axisX );					// Column end address.
	// Set Page Address.
		OLEDCommand ( OLED_PAGE_ADDRESS );		
		OLEDCommand ( page );					// Page start Address.
		OLEDCommand ( page );					// Page end Address.
		
		if ( color )	// bitSet ( x, x ) equivalent.
//			OLEDData ( *( ptr + temp ) |=  ( bitWise ( y % 8 ) ) );
			*( ptr + temp ) |=  ( bitWise ( y % 8 ) );

		else			// bitClear ( x, x ) equivalent.	
//			OLEDData ( *( ptr + temp ) &=  ~( bitWise ( y % 8 ) ) );
			*( ptr + temp ) &=  ~( bitWise ( y % 8 ) );

		}	// If the axies are whithin the OLED margin.
	}	// test Pixel.	



/********** prints out the 'color' Data to the GLCD ********/
void OLEDFill ( unsigned char color )
{
//	OLEDClearBuffer ( );

	unsigned char *ptr;
	unsigned char i, j;

	ptr = ( char * ) TEST;

	for ( unsigned int i = 0x00; i < ARRAY_HEIGHT; i++ )
		for ( unsigned int j = 0x00; j < ARRAY_WIDHT; j++ )
			*( ptr + ( j + i * ARRAY_WIDHT ) ) = color;


// Set the Display with the 'color' Data.
	OLEDCommand ( OLED_LOW_COLUMN_START );		//set lower column address.
	OLEDCommand ( OLED_HIGH_COLUMN_START );		//set higher column address.
	OLEDCommand ( OLED_PAGE_START );			//set page address.

	for ( j = 0x00; j < ARRAY_HEIGHT; j++ )
	{
		OLEDCommand ( OLED_LOW_COLUMN_START );	//set lower column address.
		OLEDCommand ( OLED_HIGH_COLUMN_START );	//set higher column address.
		OLEDCommand ( OLED_PAGE_START + j );	//set page address.
		
		for ( i = 0x00; i < ARRAY_WIDHT; i++ )
			OLEDData ( *( ptr + ( j + i * ARRAY_WIDHT ) ) = color );
//			OLEDData ( color );
		}	// Printing out the rows an then the columns.

	}	// OLEDFill ( ).


void OLEDClearBuffer ( void )
{
	unsigned char *ptr;
	unsigned char ColumnAddress, PageAddress;

	ptr = ( char * ) TEST;

	for ( PageAddress = 0x00; PageAddress < ARRAY_HEIGHT; PageAddress++ )
		for ( ColumnAddress = 0x00; ColumnAddress < ARRAY_WIDHT; ColumnAddress++ )
			*( ptr + ( ColumnAddress + PageAddress * ARRAY_WIDHT ) ) = 0;
	
	}	




void OLEDUpdateDisplay ( unsigned char clear )
{
	unsigned char ColumnAddress, PageAddress;
	unsigned char *ptr;
	
	ptr = ( char * ) TEST;

// Set Page Address.
	OLEDCommand ( OLED_PAGE_ADDRESS );		
	OLEDCommand ( 0x00 );					// Page start Address.
	OLEDCommand ( 0x07 );					// Page end Address.

// Set Column Address.
	OLEDCommand ( OLED_COLUMN_ADDRESS );		
	OLEDCommand ( 0x00 );					// Column start address.
	OLEDCommand ( 0x7F );					// Column end address.

	for ( PageAddress = 0x00; PageAddress < ARRAY_HEIGHT; PageAddress++ )	
	{
// Set Page Address.
		OLEDCommand ( OLED_PAGE_ADDRESS );		
		OLEDCommand ( 0x00 + PageAddress );	// Page start Address.
		OLEDCommand ( 0x07 );				// Page end Address.
// Set Column Address.
		OLEDCommand ( OLED_COLUMN_ADDRESS );		
		OLEDCommand ( 0x00 );			// Column start address.
		OLEDCommand ( 0x7F );			// Column end address.

		I2C_Begin ( );
		I2C_Write ( OLED_ADDRESS );
		I2C_Write ( OLED_SET_START_LINE );
		for ( ColumnAddress = 0x00; ColumnAddress < ARRAY_WIDHT; ColumnAddress++ )
			I2C_Write ( *( ptr + ( ( PageAddress * ARRAY_WIDHT) + ColumnAddress ) ) );
		I2C_End ( );
		}	// Printing out the rows an then the columns.

	if ( clear )				// if erased DDRAM wanted on every Update.
		OLEDClearBuffer ( );
	
	}	// OLEDUpdateDisplay.


	
/********** prints out on the UART PORT the Arrays ********/
/********** For Testing Purposes ********/
void printArrayTest ( void )
{
	for ( unsigned char i = 0; i < ARRAY_HEIGHT; i++ )	
	{
		printf ( "\n" );
		for ( unsigned char j = 0; j < ARRAY_WIDHT; j++ )
			printf ( "[%2X]", TEST [ i ] [ j ] );
		}		
	printf ( "\n" );
	}



void OLEDInvertedDisplay ( unsigned char complement )
{
	if ( complement )
		OLEDCommand ( OLED_INVERTED_DISPLAY );
	else
		OLEDCommand ( OLED_NORMAL_DISPLAY );
	}	
	



/*Continuous Horizontal Scroll Setup */
// The bigger the frequency, the longer the time to scroll.
void OLEDScrollToRight ( unsigned char start, unsigned char stop, unsigned char frequency  )
{
	OLEDCommand ( OLED_RIGHT_HORIZONTAL_SCROLL );
	OLEDCommand ( OLED_DUMMY_BYTE );				// A, Dummy byte.
	OLEDCommand ( start );							// B, Define start page address.
	OLEDCommand ( frequency );						// C, Frame frequency.
// Step D must be larger or equal to B.
	OLEDCommand ( stop );							// D, Define end page address.
	OLEDCommand ( OLED_DUMMY_BYTE );				// E, Dummy byte (Set as 00h).
	OLEDCommand ( 0xFF );							// F, Dummy byte (Set as ffh)
	OLEDCommand ( OLED_ACTIVATE_SCROLL );			// Start scrolling.
	}
	
void OLEDScrollToLeft ( unsigned char start, unsigned char stop, unsigned char frequency )
{
	OLEDCommand ( OLED_LEFT_HORIZONTAL_SCROLL );
	OLEDCommand ( OLED_DUMMY_BYTE );				// A, Dummy byte.
	OLEDCommand ( start );							// B, Define start page address.
	OLEDCommand ( frequency );						// C, Frame frequency.
// Step D must be larger or equal to B.
	OLEDCommand ( stop );							// D, Define end page address.
	OLEDCommand ( OLED_DUMMY_BYTE );				// E, Dummy byte (Set as 00h).
	OLEDCommand ( 0xFF );							// F, Dummy byte (Set as ffh)
	OLEDCommand ( OLED_ACTIVATE_SCROLL );			// Start scrolling.
	}	
	
	
void OLEDScrollVerticalAndRight ( unsigned char start, unsigned char stop, 
								unsigned char frequency, unsigned char offset )
{
	OLEDCommand ( OLED_VERTICAL_RIGHT );
	OLEDCommand ( OLED_DUMMY_BYTE );				// A, Dummy byte.
	OLEDCommand ( start );							// B, Define start page address.
	OLEDCommand ( frequency );						// C, Frame frequency.
// Step D must be larger or equal to B.
	OLEDCommand ( stop );							// D, Define end page address.
//No continuous vertical scrolling is available.
	OLEDCommand ( offset );							// E, Vertical scrolling offset.
	OLEDCommand ( OLED_ACTIVATE_SCROLL );			// Start scrolling.
	}
	
	
void OLEDScrollVerticalAndLeft ( unsigned char start, unsigned char stop, 
								unsigned char frequency, unsigned char offset )
{
	OLEDCommand ( OLED_VERTICAL_LEFT );
	OLEDCommand ( OLED_DUMMY_BYTE );				// A, Dummy byte.
	OLEDCommand ( start );							// B, Define start page address.
	OLEDCommand ( frequency );						// C, Frame frequency.
// Step D must be larger or equal to B.
	OLEDCommand ( stop );							// D, Define end page address.
//No continuous vertical scrolling is available.
	OLEDCommand ( offset );							// E, Vertical scrolling offset.
	OLEDCommand ( OLED_ACTIVATE_SCROLL );			// Start scrolling.
	}	
	
	
void OLEDScrollVertical ( unsigned char startRow, unsigned char stopRow, unsigned char frequency )
{
	OLEDCommand ( OLED_VERTICAL_SCROLL );
	OLEDCommand ( startRow );					// A, Define start row address.
	OLEDCommand ( stopRow );					// B, Define end row address.

//	OLEDCommand ( OLED_VERTICAL_LEFT );
//	OLEDCommand ( OLED_DUMMY_BYTE );			// A, Dummy byte.
//	OLEDCommand ( startRow );					// B, Define start page address.
//	OLEDCommand ( frequency );					// C, Frame frequency.
// Step D must be larger or equal to B.
//	OLEDCommand ( stopRow );					// D, Define end page address.
//No continuous vertical scrolling is available.
//	OLEDCommand ( 0x01 );						// E, Vertical scrolling offset.

	OLEDCommand ( OLED_ACTIVATE_SCROLL );		// Start scrolling.
	
	}


void OLEDStopScroll ( void )
{
	OLEDCommand ( OLED_DEACTIVATE_SCROLL );
	}	

	
	
	
void OLEDLine ( unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, char color )
{
	int  x, y, addx, addy, dx, dy;
	int P;
	int i;

	dx = abs ( ( int ) ( x2 - x1 ) );
	dy = abs ( ( int ) ( y2 - y1 ) );
	x = x1;
	y = y1;

	if ( x1 > x2 )
		addx = -1;
	else
		addx = 1;
	
	if ( y1 > y2 )
		addy = -1;
	else
		addy = 1;

	if ( dx >= dy )
	{
		P = 2 * dy - dx;

		for ( i = 0; i <= dx; ++i )
		{
			OLEDPixel ( x, y, color );

			if ( P < 0 )
			{
				P += 2 * dy;
				x += addx;
				}
			else
			{
				P += 2 * dy - 2 * dx;
				x += addx;
				y += addy;
				}
			}
		}
	else
	{
		P = 2 * dx - dy;

		for ( i = 0; i <= dy; ++i )
		{
			OLEDPixel ( x, y, color );

			if ( P < 0 )
			{
				P += 2 * dx;
				y += addy;
				}
			else
			{
				P += 2 * dx - 2 * dy;
				x += addx;
				y += addy;
				}
			}
		}
	}


void OLEDRectangular ( unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char fill, char color )
{
	if ( fill )
	{
		unsigned char y, ymax;                          // Find the y min and max
		
		if ( y1 < y2 )
		{
			y = y1;
			ymax = y2;
			}
		else
		{
			y = y2;
			ymax = y1;
			}

		for ( ; y <= ymax; ++y )                    // Draw lines to fill the rectangle
			OLEDLine ( x1, y, x2, y, color );
		}
	else
	{
		OLEDLine ( x1, y1, x2, y1, color );      // Draw the 4 sides
		OLEDLine ( x1, y2, x2, y2, color );
		OLEDLine ( x1, y1, x1, y2, color );
		OLEDLine ( x2, y1, x2, y2, color );
		}
	}


void OLEDAreaFill ( unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned char fill, char color )
{
	unsigned char y, ymax;                          // Find the y min and max
	
	if ( y1 < y2 )
	{
		y = y1;
		ymax = y2;
		}
	else
	{
		y = y2;
		ymax = y1;
		}

	for ( ; y <= ymax; ++y )                    // Draw lines to fill the rectangle
		OLEDLine ( x1, y, x2, y, color );
	}	


void OLEDCircle ( int x, int y, int radius, char fill, char color )
{
	int a, b, P;
	a = 0x00;
	b = radius;
	P = 0x01 - radius;
	
	do
	{
		if( fill )
		{
			OLEDLine ( x - a, y + b, x + a, y + b, color );
			OLEDLine ( x - a, y - b, x + a, y - b, color );
			OLEDLine ( x - b, y + a, x + b, y + a, color );
			OLEDLine ( x - b, y - a, x + b, y - a, color );
			}
		else
		{
			OLEDPixel ( a + x, b + y, color );
			OLEDPixel ( b + x, a + y, color );
			OLEDPixel ( x - a, b + y, color );
			OLEDPixel ( x - b, a + y, color );
			OLEDPixel ( b + x, y - a, color );
			OLEDPixel ( a + x, y - b, color );
			OLEDPixel ( x - a, y - b, color );
			OLEDPixel ( x - b, y - a, color );
      		}

		if ( P < 0 )
			P += 3 + 2 * a++;
		else
			P += 5 + 2 * ( a++ - b-- );
		
		} while ( a <= b );
	}


void OLEDBar ( int x1, int y1, int x2, int y2, int width, char color )
{
	signed int  x, y, addx, addy, j;
	signed int P, dx, dy, c1, c2;
	char i;

	dx = abs ( ( signed int ) ( x2 - x1 ) );
	dy = abs ( ( signed int ) ( y2 - y1 ) );
	x = x1;
	y = y1;
	c1 = -dx * x1 - dy * y1;
	c2 = -dx * x2 - dy * y2;

	if ( x1 > x2 )
	{
		addx = -1;
		c1 = -dx * x2 - dy * y2;
		c2 = -dx * x1 - dy * y1;
		}
	else
		addx = 1;
	
	if ( y1 > y2 )
	{
		addy = -1;
		c1 = -dx*x2 - dy*y2;
		c2 = -dx*x1 - dy*y1;
		}
	else
		addy = 1;

	if ( dx >= dy )
	{
		P = 2*dy - dx;

		for ( i = 0; i <= dx; ++i )
		{
			for ( j =- ( width / 2 ); j < width / 2 + width % 2; ++j )
			{
				if ( dx * x + dy * ( y + j ) + c1 >= 0 && dx * x + dy * ( y + j ) + c2 <= 0 )
					OLEDPixel ( x, y + j, color );
				}
			
			if ( P < 0 )
			{
				P += 2 * dy;
				x += addx;
				}
         	else
			{
				P += 2 * dy - 2 * dx;
				x += addx;
				y += addy;
				}
			}
		}
	else
	{
		P = 2 * dx - dy;

		for ( i = 0; i <= dy; ++i )
		{
			if ( P < 0 )
			{
				P += 2 * dx;
				y += addy;
				}
			else
			{
				P += 2 * dx - 2 * dy;
				x += addx;
				y += addy;
				}
			
			for ( j =- ( width / 2 ); j < width / 2 + width % 2; ++j )
			{
				if ( dx * x + dy * ( y + j ) + c1 >= 0 && dx * x + dy * ( y + j ) + c2 <= 0 )
				OLEDPixel ( x + j, y, color );
				}
			}
		}
	}



void OLEDText ( int x, int y, char* textptr, int size, char color )
{
	unsigned char i, j, k, l, m;						// Loop counters
	unsigned char pixelData [ 5 ];						// Stores character data

	for ( i = 0; textptr [ i ] != 0x00; ++i, ++x )		// Loop through the passed string
	{
		if ( textptr [ i ] < 'S') // Checks if the letter is in the first text array
			memcpy ( pixelData, TEXT [ textptr [ i ] - ' '], 5 );
		
		else if(textptr[i] <= '~') // Check if the letter is in the second array
		{
			if ( ( textptr[i] >= 'a' ) && ( textptr[i] <= 'z' ) )
				memcpy ( pixelData, TEXT2 [ textptr [ i ] - 'S' - 1 ], 5 );
			
			else
				memcpy ( pixelData, TEXT2 [ textptr [ i ] - 'S' ], 5 );

			}
		
		else
			memcpy ( pixelData, TEXT [ 0 ], 5 );		// Default to space

		if ( x + 5 * size >= ARRAY_WIDHT )				// Performs character wrapping
		{	
			x = 0;										// Set x at far left position
			y += 7*size + 1;							// Set y at next position down
      		}
      
		for ( j = 0; j < 5; ++j, x += size )			// Loop through character byte data
		{
			for ( k = 0; k <7 *size; ++k )				// Loop through the vertical pixels
			{
				if ( pixelData [ j ] & 1<<(k%8) )	// Check if the pixel should be set
				{
					for ( l = 0; l < size; ++l )		// The next two loops change the
					{									// character's size
						for ( m = 0; m < size; ++m )
						{
							OLEDPixel ( x + m, y + k * size + l, color ); // Draws the pixel
							}
						}
					}
				}
			}
		}
	}
			