%{
#include"xyplotter.h"
#include <unistd.h>  
#include <stdio.h>
#include "my_engrave.h"
extern "C"
{
//        int yyparse(void);
        int yylex(void);  
        int yywrap()
        {
                return 1;
        }

}

/* Added because panther doesn't have liby.a installed. */
int yyerror (char *msg) {
	return fprintf (stderr, "YACC: %s\n", msg);
	}


%}
%union
{
	char *string;
	double dval;
}
%token <dval> NUMBER
%token NEWLINE
%token G
%token M
%token X
%token Y
%token F
%token I
%token J
%%
kommandos:	kommandos kommando
	| kommando;

kommando:	G NUMBER X NUMBER Y NUMBER
			{
				int i=(int)$2;
				//printf("GXY G%.0f X nach %.4f Y nach %.4f\n",$2,$4,$6);
				switch(i)
				{
					case 0:
						printf("schneller Vorschub nach %f/%f\n",$4,$6);
						mySetStepDelay(500);
						mygoto($4,$6);
						break;
					case 1:
					case 2:
					case 3:
						printf("Gehe zu %.4f/%.4f\n",$4,$6);
						mygoto($4,$6);
						break;
					default:
						printf("unbekannter Befehl: G%d %f %f\n",i,$4,$6);
				}
			}
			| I NUMBER J NUMBER
			{
				//printf("Bogenparameter von voriger Aktion\n");
			}
			| F NUMBER
			{
				printf("Setting Feed Rate to %f\n",$2);
				mySetStepDelay($2);
			}
			| G NUMBER
			{ 
				int i=(int)$2;
				switch(i)
				{
					case 1:
						//printf("G1 Kommando ohne Koordinaten\n");
						break;
					case 20:
						printf("Benutze inch zur Adressierung\n");
						printf("Modus wird (noch) nicht unterstuetzt!\n");
						exit(0);
						break;
					case 21:
						printf("Benutze mm zur Adressierung\n");
						break;
					case 90:
						printf("absolute Adressierung\n");
						break;
					case 91:
						printf("inkrementelle Adressierung\n");
						break;
					default:
						printf("Unbekanntes G Kommando %d\n",i);
					
				}
			}
			| M NUMBER
			{
				//printf("M %.0f\n",$2);
				int i=(int)$2;
				
				switch(i)
				{
					case 5:
						printf("Laser Aus\n");
						mylaseroff();
						break;
					
					case 3:
						printf("Laser An\n");
						mylaseron();
						break;
						
					case 2:
						printf("PROGRAMM STOP\n");
						break;
				}
			}
			|NEWLINE;
%%
