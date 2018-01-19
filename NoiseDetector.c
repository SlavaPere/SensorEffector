#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <pcf8591.h>
#include <math.h>

#define PCF       120

int main (void)
{
	int value1, value2;
	int data1, data2;
	int counter = 0;
	int step = 1; //1
	int i = 0;
	int offset = 85;  //85
	int deviation = 10; //10
	int noiseCounter = 0;
	int noiseDuration = 5; //20
	int noiseOffCounter = 0;
	int noiseOffDuration = 1000; //1000
	int noise = 0;

	if (wiringPiSetup () == -1) {
		printf("Error at wiringPiSetup()");
    		return 1 ;
	}
 
	pinMode (0, OUTPUT) ;         // aka BCM_GPIO pin 17
	pinMode (1, OUTPUT) ;	// aka BCM_GPIO pin 18
 

	// Setup pcf8591 on base pin 120, and address 0x48
//	printf("%d\n", ++step);

	if (pcf8591Setup (PCF, 0x48) == -1) {
		printf("Error at pcf8591Setup()");
    		return 1 ;
	}
	
       printf("Lowest noise level \t\t\t%d\n", offset);
       printf("Data has been generalized with weight value of %d\n", step);

	while(1) // loop forever
	{
		data1 += analogRead  (PCF + 0);	//white - local
		data2 += analogRead  (PCF + 1);	//yellow - remote
		counter++;
		if(noise) {
			noiseOffCounter++;
		}

		if(counter == step ) {
			counter = 0;
			value1 = round((double)data1 / step);
			value2 = round((double)data2 / step);
			data1 = 0;
			data2 = 0;

			if ((value1 < offset) || (value2 < offset)) {

				if( abs (value1 - value2) < deviation)  {
					if (++noiseCounter > noiseDuration) {
						noiseCounter = 0;
						noiseOffCounter = 0;
						noise = 1;
					}
				}
				else  {
					if(noise) {
						if(noiseOffCounter > noiseOffDuration) {
							noiseOffCounter = 0;
							noise = 0;						
						}
					}
					else	if( --noiseCounter < 0)  {
							noiseCounter = 0;
					}
				}

				if(noise) {
					printf("\n\tNOISE !!!!  #1:%3d #2:%3d", value1, value2);
					//both microphones are sensing noise - red 
					digitalWrite (0, LOW);
					digitalWrite (1, HIGH);
				}
				else {
					printf("\n\tVoice In!!  #1:%3d #2:%3d", value1, value2);
					// Voice in at any of microphones - green
					digitalWrite (0, HIGH);
					digitalWrite (1, LOW);
				}

				for(i = offset; (i > offset - value1) && (i > 0); i--) {
					printf(" ");
				}
				for(i = offset; i > value1; i--) {
					printf("-");
				}
				printf("|");
				for(i = offset; i > value2; i--) {
					printf("+");
				}
			}
			else  {
				if(!noise) {
					digitalWrite (0, LOW);
					digitalWrite (1, LOW);
				}
			}
		}
	}

	return 0;
}