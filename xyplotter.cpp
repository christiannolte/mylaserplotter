// XYPlotter1.cpp: Implementierung der Klasse CXYPlotter.
//
//////////////////////////////////////////////////////////////////////
#include "XYPlotter.h"

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>                                                       
#include <termios.h>
#include <unistd.h>                                                      
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TIMEOUT 5



//#define COMPORT "\\\\.\\com8"
#define COMPORT "\\\\.\\com2"
#define MINX	0
#define MAXX	500
//#define MAXX 1000000000
#define MINY	0
#define MAXY	500
//#define MAXY 1000000000

///Speicherbereich um die Schnittstelleneinstellungen zu sichern
struct termios oldtio;
///Speicherbereich um die Schnittstelleneinstellungen zu setzen
struct termios newtio;                                                  



//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CXYPlotter::CXYPlotter(void)
{
 serial_fd=-1;
 while(serial_fd==-1)
 {
	serial_fd=open(COMPORT,O_RDWR|O_NONBLOCK| O_NOCTTY );
		if(serial_fd==-1){
		printf("Unable to open Serial Device %s\n",COMPORT);
		//exit(-1);
		sleep(1);
	};
  }
  tcgetattr(serial_fd,&oldtio);
  bzero(&newtio,sizeof(newtio));
  newtio.c_cflag=B9600|CS8|CLOCAL|PARENB|CREAD|CSTOPB;
  newtio.c_iflag=0;
  newtio.c_oflag=0;
  newtio.c_lflag=0;
  newtio.c_cc[VEOF]=0;
  newtio.c_cc[VMIN]=0;
  /*
   Raw output.
  */
  newtio.c_oflag = 0;
  cfsetospeed(&newtio,B9600);
  cfsetispeed(&newtio,B9600);

  tcflush(serial_fd, TCIFLUSH);
  tcsetattr(serial_fd,TCSANOW,&newtio);

	LaserState=0;
	cur_x=0;
	cur_y=0;
	stepdelay=100;
//	stepdelay=0;
}

CXYPlotter::~CXYPlotter(void)
{
	GotoA(0,0);
	MotorStop();
//	CloseHandle(hComm);
	close(serial_fd);
}

void CXYPlotter::Move(signed char dx, signed char dy)
{
	unsigned char b1,b2,b3,b4,xtemp1,xtemp2,ytemp1,ytemp2;
	
	unsigned char buffer[5];
	unsigned char answer;
	unsigned long dwWritten;
	unsigned long dwLen;
	static unsigned long dxfree=0,dyfree=0;
	static unsigned int fehler=0;
	
	printf("Move %d %d \n",dx,dy);
//	Sync();
	
	if(dx==1)
	{
		if(cur_x<=MAXX)
		{
			x_crtl.Up();
			cur_x++;
		}
	}
	if(dx==-1)
	{
		if(cur_x>MINX)
		{
			x_crtl.Down();
			cur_x--;
		}
	}
	if(dy==1)
	{
		if(cur_y<=MAXY)
		{
			y_crtl.Up();
			cur_y++;
		}
	}
	if(dy==-1)
	{
		if(cur_y>MINY)
		{
			y_crtl.Down();
			cur_y--;
		}
	}
	x_crtl.GetOutputs(&xtemp1,&xtemp2);
	y_crtl.GetOutputs(&ytemp1,&ytemp2);
	if(dx==0)
	{
		dxfree++;
		if((dxfree%5)&&(dxfree>5))
		{
			xtemp2=0;    //Zuerst immer dann nur alle 5 y Schritte X Ausrichten
		}
	}
	else
	{
		dxfree=0;
	}
	if(dy==0)
	{
		dyfree++;
		if((dyfree%5)&&(dyfree>5))
		{
			ytemp2=0;    //Zuerst immer dann nur alle 5 x Schritte y Ausrichten
		}
	}
	else
	{
		dyfree=0;
	}


	b1=(xtemp2&0x3)|(ytemp2<<2);
	b2=xtemp1|(ytemp1<<4);
	if(LaserState)
	{
		b1|=0x30;					//Laser Einschalten
	}
	b3=0;							//erstmal
	b4=b1+b2+b3;
	//Schreiben
	buffer[0]='a';
	buffer[1]=b1;
	buffer[2]=b2;
//	buffer[3]=b3;
//	buffer[4]=b4;
	buffer[3]=b4;
	dwLen=4;
//	WriteFile( hComm, buffer, dwLen, &dwWritten, NULL );
	write(serial_fd,buffer,4);
	//Warten
//	if(LaserState)Sleep(5);
	answer='X';
	read(serial_fd,&answer,1);
	if(answer!='O')
	{
		fehler++;
	}
//	printf("%c gelesen %d fehler\n",answer,fehler);
//	printf("stepdelay ist %d\n",stepdelay);
	if((dx!=0)&&(dy!=0))
	{	
		usleep((useconds_t)(stepdelay*1.41));   //Bein fahren einer Diagonale ist der Weg laenger, deshalb wird auch laenger gewartet
	}
	else
	{
		usleep(stepdelay);
	}
}

int CXYPlotter::Sync(void)
{
	char c=' ';
	int i;
	
    fd_set rfds;
    struct timeval tv;
    int retval=1;

	printf("5 Byte schreiben");
	for(i=0;i<5;i++)
	{
		c='X';
		write(serial_fd,&c,1);
	}

	i=1;
	/*
	while(i>0)
	{
		i=read(serial_fd,&c,1);
		printf("%d Byte %c gelesen\n",i,c);
	}
	*/

	while((retval==1)&&(i==1))
	{

    FD_ZERO(&rfds);
    FD_SET(serial_fd, &rfds);

    tv.tv_sec = 1;
    tv.tv_usec = 0;

   retval = select(serial_fd+1, &rfds, NULL, NULL, &tv);
   printf("select liefert %d zurueck\n",retval);

   if (retval == -1)
        perror("select()");
    else if (retval)
	{
        printf("Data is available now.\n");
		i=read(serial_fd,&c,1);
		printf("%d Byte %c gelesen\n",i,c);
	}
    else
        printf("No data within five seconds.\n");
	}
	
}


void CXYPlotter::LaserOFF(void)
{
	LaserState=0;
	Move(0,0);
}

void CXYPlotter::LaserON(void)
{
	LaserState=1;
	Move(0,0);
}

void CXYPlotter::SetStepDelay(unsigned int new_delay)
{
	stepdelay=new_delay;
	printf("Setze Stepdelay auf %d\n",stepdelay);
}


void CXYPlotter::MotorStop(void)
{
	
	unsigned char buffer[5];
	unsigned long dwWritten;
	unsigned long dwLen;
		
	
	//Schreiben
	buffer[0]='a';
	buffer[1]=0;
	buffer[2]=0;
	buffer[3]=0;
	buffer[4]=0;
		if(LaserState)
	{
		buffer[1]=0x30;					//Laser Einschalten
		buffer[3]=0x30;
	}

	dwLen=5;
//	WriteFile( hComm, buffer, dwLen, &dwWritten, NULL );
	write(serial_fd,buffer,4);

}

void CXYPlotter::DrawCircle(int x0, int y0, int radius)
{
	int i;
	int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

	int array1[125];
	int array2[125];
	int steps=0;

	array1[0]=0;
	array2[0]=radius;
	GotoA(x0,y0+radius);
    while(x < y) 
    {
      if(f >= 0) 
      {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;
 
      GotoA(x0 + x, y0 + y);
	  steps++;
	  array1[steps]=x;
	  array2[steps]=y;
    }
	//2
	for(i=0;i<=steps;i++)
	{
		GotoA(x0+array2[steps-i],y0+array1[steps-i]);
	}
	//3
	for(i=0;i<=steps;i++)
	{
		GotoA(x0+array2[i],y0-array1[i]);
	}
	//4
	for(i=0;i<=steps;i++)
	{
		GotoA(x0+array1[steps-i],y0-array2[steps-i]);
	}
	//5
	for(i=0;i<=steps;i++)
	{
		GotoA(x0-array1[i],y0-array2[i]);
	}
	//6
	for(i=0;i<=steps;i++)
	{
		GotoA(x0-array2[steps-i],y0-array1[steps-i]);
	}
	//7
	for(i=0;i<=steps;i++)
	{
		GotoA(x0-array2[i],y0+array1[i]);
	}
	//8
	for(i=0;i<=steps;i++)
	{
		GotoA(x0-array1[steps-i],y0+array2[steps-i]);
	}


}

int sgn(int x){
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}


void CXYPlotter::GotoA(int xend, int yend)
{
/*--------------------------------------------------------------
 * Bresenham-Algorithmus: Linien auf Rastergeräten zeichnen
 *
 * Eingabeparameter:
 *    int xstart, ystart        = Koordinaten des Startpunkts
 *    int xend, yend            = Koordinaten des Endpunkts
 *
 * Ausgabe:
 *    void SetPixel(int x, int y) setze ein Pixel in der Grafik
 *         (wird in dieser oder aehnlicher Form vorausgesetzt)
 *---------------------------------------------------------------
 */
    int x, y, t, dx, dy, incx, incy, pdx, pdy, ddx, ddy, es, el, err;
	int xstart=cur_x;
	int ystart=cur_y;
	
/* Entfernung in beiden Dimensionen berechnen */
   dx = xend - xstart;
   dy = yend - ystart;
 
/* Vorzeichen des Inkrements bestimmen */
   incx = sgn(dx);
   incy = sgn(dy);
   if(dx<0) dx = -dx;
   if(dy<0) dy = -dy;
 
/* feststellen, welche Entfernung größer ist */
   if (dx>dy)
   {
      /* x ist schnelle Richtung */
      pdx=incx; pdy=0;    /* pd. ist Parallelschritt */
      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
      es =dy;   el =dx;   /* Fehlerschritte schnell, langsam */
   } else
   {
      /* y ist schnelle Richtung */
      pdx=0;    pdy=incy; /* pd. ist Parallelschritt */
      ddx=incx; ddy=incy; /* dd. ist Diagonalschritt */
      es =dx;   el =dy;   /* Fehlerschritte schnell, langsam */
   }
 
/* Initialisierungen vor Schleifenbeginn */
   x = xstart;
   y = ystart;
   err = el/2;
//   SetPixel(x,y);  //Das haben wir schon gemacht
 
/* Pixel berechnen */
   for(t=0; t<el; ++t) /* t zaehlt die Pixel, el ist auch Anzahl */
   {
      /* Aktualisierung Fehlerterm */
      err -= es; 
      if(err<0)
      {
          /* Fehlerterm wieder positiv (>=0) machen */
          err += el;
          /* Schritt in langsame Richtung, Diagonalschritt */
          x += ddx;
          y += ddy;
      } else
      {
          /* Schritt in schnelle Richtung, Parallelschritt */
          x += pdx;
          y += pdy;
      }
	  Move(x-cur_x,y-cur_y);
      //SetPixel(x,y);
   }
} /* gbham() */
