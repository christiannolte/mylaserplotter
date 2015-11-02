// Stepper.h: Schnittstelle für die Klasse CStepper.
//
//////////////////////////////////////////////////////////////////////


class CStepper  
{
public:
	int GetOutputs(unsigned char *out,unsigned char *enables);
	void Down();
	void Up();
	CStepper();
	virtual ~CStepper();
private:
	int state;
};

