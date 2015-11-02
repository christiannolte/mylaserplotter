#include"xyplotter.h"
#include <unistd.h>  
#include <stdio.h>

#define MAXSTEPS 500
#define MAXWAYMM 38

extern int yyparse(void);

CXYPlotter my_plotter;

main()
{
	yyparse();

}

int mygoto(float x,float y)
{
	my_plotter.GotoA((int)(x*MAXSTEPS/MAXWAYMM),(int) (y*MAXSTEPS/MAXWAYMM));
}

void mymotoroff()
{
	my_plotter.MotorStop();
}

void mySetStepDelay(double speed)
{
	unsigned int calcdelayi;
	double calcdelay;
	
	calcdelay=(MAXWAYMM*60)/(speed*MAXSTEPS);
	printf("calcdelay ist %f\n",calcdelay);


	calcdelay*=1000000;
	printf("calcdelay ist %f\n",calcdelay);


	calcdelayi=(unsigned int)calcdelay;
	printf("calcdelayi ist %d\n",calcdelayi);
	my_plotter.SetStepDelay(calcdelayi);
	
}

void mylaseroff()
{
    my_plotter.LaserOFF();
}

void mylaseron()
{
    my_plotter.LaserON();
	my_plotter.MotorStop();

	printf("5 Sekunden Zeit zum einbrennen!!!!!!\n");
	sleep(5);  //pause zum einbrennen
}
