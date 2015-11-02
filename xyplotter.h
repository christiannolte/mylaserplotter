#include "stepper.h"

class CXYPlotter  
{
public:
	void GotoA(int x, int y);
	void DrawCircle(int x0, int y0,int radius);
	void MotorStop(void);
	void LaserON(void);
	void LaserOFF(void);
	void SetStepDelay(unsigned int new_delay);
	int Sync(void);
	void Move(signed char dx, signed char dy);
	CXYPlotter(void);
	virtual ~CXYPlotter(void);

private:
	int cur_y;
	int cur_x;
	CStepper y_crtl;
	CStepper x_crtl;
	unsigned char LaserState;
//	HANDLE hComm;
	int serial_fd;
	unsigned int stepdelay;
};

