#include "eVehicle.hpp"

WIEGAND wg;
unsigned long lastCard=0;
unsigned long currCard=0;

using namespace cv;
using namespace std;

int main(){

	wg.begin();
	while(1){
		if(wg.available())
		{
			currCard = wg.getCode();
			if(currCard != lastCard)
			{
				char name[50];
				char tBuff[26];
				struct tm* tm_info;
				time_t now = time(0);
				tm_info = localtime(&now);
				strftime(tBuff, 26, "%d%m%y_%H%M%S", tm_info);
				sprintf(name, "Snaps/V_%lu_%s.png", currCard, tBuff);
				getPic(name);
				printf("Snap name: %s, Card# %lx\n",name, currCard);
				lastCard = currCard;
			}
		}
		usleep(10000);
	}
}