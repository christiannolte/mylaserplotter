// Stepper.cpp: Implementierung der Klasse CStepper.
//
//////////////////////////////////////////////////////////////////////

#include "Stepper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CStepper::CStepper()
{
	state=0;
}

CStepper::~CStepper()
{

}

void CStepper::Up()
{
	if(state==7)
	{
		state=0;
	}
	else
	{
		state++;
	}
}

void CStepper::Down()
{
	if(state==0)
	{
		state=7;
	}
	else
	{
		state--;
	}
}

int CStepper::GetOutputs(unsigned char *out, unsigned char *enables)
{
	switch(state)
	{
	case 0:
		*out=0x1;
		*enables=0x1;
		break;
	case 1:
		*out=0x5;
		*enables=0x3;
		break;
	case 2:
		*out=0x4;
		*enables=0x2;
		break;
	case 3:
		*out=0x6;
		*enables=0x3;
		break;
	case 4:
		*out=0x2;
		*enables=0x1;
		break;
	case 5:
		*out=0xA;
		*enables=0x3;
		break;
	case 6:
		*out=0x8;
		*enables=0x2;
		break;
	case 7:
		*out=0x9;
		*enables=0x3;
		break;
	default:
		*out=0;
		*enables=0;
		return(0);
	}
		return(1);
}
