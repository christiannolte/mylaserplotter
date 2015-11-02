%{
#include "y.tab.hh"
%}

%%
-?[0-9]+(\.[0-9]+)?	{ 
			yylval.dval = atof (yytext); 
			//printf ("scanned the number %f\n", yylval.dval);
			return NUMBER; 
		}
[\n\r] {
			//printf("Newline gefunden\n");
			return NEWLINE;
		}
[Gg]	    {
			//printf("G gefunden\n");
			return G;
		}
[Mm]	    {
			//printf("M gefunden\n");
			return M;
		}
[Xx]	    {
			//printf("X gefunden\n");
			return X;
		}
[Yy]	    {
			//printf("Y gefunden\n");
			return Y;
		}
[Ff]	    {
			//printf("F gefunden\n");
			return F;
		}
[Ii]	    {
			//printf("I gefunden\n");
			return I;
		}
[Jj]	    {
			//printf("J gefunden\n");
			return J;
		}
[ \t]   {
			//printf("Skipping spaces\n");
		}
%%

