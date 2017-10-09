#include "Wiegand.h"
#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <time.h>
#include <unistd.h>
#include <memory.h>


volatile unsigned long WIEGAND::_cardTempHigh=0;
volatile unsigned long WIEGAND::_cardTemp=0;
volatile unsigned long WIEGAND::_lastWiegand=0;
unsigned long WIEGAND::_code=0;
volatile int WIEGAND::_bitCount=0;	
int WIEGAND::_wiegandType=0;

WIEGAND::WIEGAND()
{
}

unsigned long WIEGAND::getCode()
{
	return _code;
}

int WIEGAND::getWiegandType()
{
	return _wiegandType;
}

bool WIEGAND::available()
{
	bool ret;
	ret=DoWiegandConversion();
	return ret;
}

void WIEGAND::begin()
{
	begin(0,1);
}

void WIEGAND::begin(int pinD0, int pinD1)
{
	_lastWiegand = 0;
	_cardTempHigh = 0;
	_cardTemp = 0;
	_code = 0;
	_wiegandType = 0;
	_bitCount = 0;  

	// Setup wiringPi
	wiringPiSetup() ;

	pinMode(pinD0, INPUT);					// Set D0 pin as input
	pinMode(pinD1, INPUT);					// Set D1 pin as input
	wiringPiISR(pinD0, INT_EDGE_FALLING, ReadD0 );
	wiringPiISR(pinD1, INT_EDGE_FALLING, ReadD1);
}

void WIEGAND::ReadD0 ()
{
	_bitCount++;				// Increament bit count for Interrupt connected to D0
	if (_bitCount>31)			// If bit count more than 31, process high bits
	{
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	//	shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp <<=1;
	}
	else
	{
		_cardTemp <<= 1;		// D0 represent binary 0, so just left shift card data
	}
	_lastWiegand = millis();	// Keep track of last wiegand bit received
}

void WIEGAND::ReadD1()
{
	_bitCount ++;				// Increment bit count for Interrupt connected to D1
	if (_bitCount>31)			// If bit count more than 31, process high bits
	{
		_cardTempHigh |= ((0x80000000 & _cardTemp)>>31);	// shift value to high bits
		_cardTempHigh <<= 1;
		_cardTemp |= 1;
		_cardTemp <<=1;
	}
	else
	{
		_cardTemp |= 1;			// D1 represent binary 1, so OR card data with 1 then
		_cardTemp <<= 1;		// left shift card data
	}
	_lastWiegand = millis();	// Keep track of last wiegand bit received
}

unsigned long WIEGAND::GetCardId (volatile unsigned long *codehigh, volatile unsigned long *codelow, char bitlength)
{
	unsigned long cardID=0;

	if (bitlength==26)								// EM tag
		cardID = (*codelow & 0x1FFFFFE) >>1;

	if (bitlength==34)								// Mifare 
	{
		*codehigh = *codehigh & 0x03;				// only need the 2 LSB of the codehigh
		*codehigh <<= 30;							// shift 2 LSB to MSB		
		*codelow >>=1;
		cardID = *codehigh | *codelow;
	}
	return cardID;
}

char translateEnterEscapeKeyPress(char originalKeyPress) {
	switch(originalKeyPress) {
	case 0x0b:        // 11 or * key
		return 0x0d;  // 13 or ASCII ENTER

	case 0x0a:        // 10 or # key
		return 0x1b;  // 27 or ASCII ESCAPE

	default:
		return originalKeyPress;
	}
}

bool WIEGAND::DoWiegandConversion ()
{
	unsigned long cardID;
	unsigned long sysTick = millis();

	if ((sysTick - _lastWiegand) > 25)								// if no more signal coming through after 25ms
	{
		//printf("Bit Count: %d",_bitCount);
		if ((_bitCount==26) || (_bitCount==34) || (_bitCount==8) || (_bitCount==4)) 	// bitCount for keypress=4 or 8, Wiegand 26=26, Wiegand 34=34
		{
			_cardTemp >>= 1;			// shift right 1 bit to get back the real value - interrupt done 1 left shift in advance
			if (_bitCount>32)			// bit count more than 32 bits, shift high bits right to make adjustment
				_cardTempHigh >>= 1;	

			if((_bitCount>=26) || (_bitCount==34))		// wiegand 26 or wiegand 34
			{
				cardID = GetCardId (&_cardTempHigh, &_cardTemp, _bitCount);
				_wiegandType=_bitCount;
				_bitCount=0;
				_cardTemp=0;
				_cardTempHigh=0;
				_code=cardID;
				return true;				
			}
		}
		else
		{
			// well time over 25 ms and bitCount !=8 , !=26, !=34 , must be noise or nothing then.
			_lastWiegand=sysTick;
			_bitCount=0;			
			_cardTemp=0;
			_cardTempHigh=0;
			return false;
		}	
	}
	else
		return false;
}
