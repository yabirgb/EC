#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <string.h>	// para strncmp()/strlen()
#include <sys/time.h>	// para gettimeofday(), struct timeval

char password[] = "Q..W.r\n";
int passcode = 8746;


void boom(){
	printf("***************\n");
	printf("*** BOOM!!! ***\n");
	printf("***************\n");
	exit(-1);
}

void defused(){
	printf("·························\n");
	printf("··· bomba desactivada ···\n");
	printf("·························\n");
	exit(0);
}

int main(){
#define SIZE 100
	char pass[SIZE];
	int  pasv;

#define TLIM 7
	struct timeval tv1,tv2;	// gettimeofday() secs-usecs
	int i;
	gettimeofday(&tv1, NULL);

	printf("Itroduzca la contraseña: ");
	fgets(pass, SIZE, stdin);

	if(strlen(pass) != strlen(password))
	  boom();
	
        for(i = 0; i < strlen(password); i++){
	  if(password[i] - pass[i] != 0)
	    boom();
	}
	

	gettimeofday(&tv2,NULL);
	if (tv2.tv_sec - tv1.tv_sec > TLIM)
	    boom();

	printf("Introduzca el código: ");
	scanf("%i",&pasv);
        pasv -= passcode;

	if(pasv != 0)
	  boom();

       	gettimeofday(&tv1,NULL);
	if (tv1.tv_sec - tv2.tv_sec > TLIM)
	  boom();

	defused();

	
	
}
