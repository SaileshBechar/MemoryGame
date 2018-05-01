#include <iostream>
#include <ugpio/ugpio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <ctime>


using namespace std;
int checkbuttons(int gpio0, int gpio1, int gpio2, int gpio3, int rnum[], int numlights, int loopcounter, int position, int stime);
void gameover(int gpio0, int gpio1, int gpio2, int gpio3);
int displaylights(int gpio0, int gpio1, int gpio2, int gpio3, int rnum[], int numlights, int stime);
void scoreBoardPrinter(int point,int difficulty,int playerMode);
int logging(string s);


int displaylights(int gpio0, int gpio1, int gpio2, int gpio3, int rnum[], int numlights, int stime){
	cout << "Displaying lights" << endl;
		for (int x = 0; x < numlights; x++){
			gpio_direction_output(gpio0, 0);
			gpio_direction_output(gpio1, 0);
			gpio_direction_output(gpio2, 0);
			gpio_direction_output(gpio3, 0);


			usleep(stime);
			
			rnum[x] = rand() % 4 +1;

			if (rnum[x] == 1){
				gpio_set_value(gpio0, 1);
			}
			if (rnum[x] == 2){
				gpio_set_value(gpio1, 1);
			}
			if (rnum[x] == 3){
				gpio_set_value(gpio2, 1);
			}
			if (rnum[x] == 4){
				gpio_set_value(gpio3, 1);
			}
			cout << "Light num: " << rnum[x] << endl;
			usleep(stime);
		}
	return 0;
}
void gameover(int gpio0, int gpio1, int gpio2, int gpio3){
	int value = 0;
	for (value = 0; value < 11; value++){
		gpio_direction_output(gpio0, value % 2);
		gpio_direction_output(gpio1, value % 2);
		gpio_direction_output(gpio2, value % 2);
		gpio_direction_output(gpio3, value % 2);
		usleep(100000);
	}
}
int checkbuttons(int gpio0, int gpio1, int gpio2, int gpio3, int rnum[], int numlights, int loopcounter, int position, int stime){
	int activated = 0;
	if (loopcounter > (stime/1000)){
		cout << "Timed out" << endl;
		return -1;
	}
	if (gpio_get_value(gpio0) == 0 && gpio_get_value(gpio1) == 0 && gpio_get_value(gpio2) == 0 && gpio_get_value(gpio3) == 0){
			usleep(5000);
		return checkbuttons(gpio0, gpio1, gpio2, gpio3, rnum, numlights, ++loopcounter, position, stime);
	}
	else{	

		if (gpio_get_value(gpio0) == 1){
			activated = 1;
		} 
		if (gpio_get_value(gpio1) == 1){
			activated = 2;
		} 
		if (gpio_get_value(gpio2) == 1){
			activated = 3;
		} 
		if (gpio_get_value(gpio3) == 1){
			activated = 4;
		}
		if (activated == rnum[position]){
			cout << "Correct! Activated : " << activated << endl;
		}
		
		if (activated != rnum[position]){
			cout << "Wrong Input! " << activated << "\t\t\t" << rnum[position] << endl;
			return -1;
		}
	
		return 1;
	}
}
void scoreBoardPrinter(int point,int difficultyOG,int playerMode){
 	string playerName;
 	cin >> playerName;
 	int period;


 	int playerPoint=point;
 	int difficulty=difficultyOG;
 	int mode=playerMode;
 	ofstream scoreBoard;
 
	scoreBoard.open("Single_Player_Score_Board.txt");
	if (!scoreBoard.is_open()){
		return -1;
	}
	scoreBoard << "	" << playerName << "		Difficulty Level " << difficulty << "|" << "Game Mode:" << mode << playerPoint << " rounds completed!" << "Congratulations!";
 	 
 	
 }
int logging(string s){
	int period;
	char newfilename[8] = "log.txt";

	
	struct tm * timeinfo;
	time_t rawtime;
	time (&rawtime);
	timeinfo = localtime (&rawtime);

	ofstream outfile;
	outfile.open(newfilename);
	if (!outfile.is_open()){
		return -1;
	}

	outfile << __DATE__ << " " << __TIME__ <<  " " << s;

	return 1;	
}


int main(int argc, char **argv){
	int gpio;
	int i;
	int rq; 
	int rv1;
	int rv2;
	int rv3;
	int rv4;
	int rq1;
	int rq2;
	int rq3;
	int rq4;
	int gpio0;
	int gpio1;
	int gpio2;
	int gpio3;
	int rnum[100];
	int numlights = 1;
	int activated = 0;
	bool failed = false;
	int sleeptime = 0;
	int difficulty;
	int mode = 1;
	int j;
	int k = 0;
	ofstream outfile; 
    	//outfile.open(logFileName.c_str());

	//Initialize all gpio pins
	gpio0 = 0;
	gpio1 = 1;
	gpio2 = 2;
	gpio3 = 3;

	//Create seed for original random numbers
	srand(time(NULL));

	//Error Check Inputs
	if (argc > 3 || argc < 2){
		scoreBoardPrinter(numlights,difficulty,mode);
		logging("ERROR! Incorrect difficulty entered!");
		failed = true;
	}

	if (argc == 3){
		mode = atoi(argv[2]);
	}
	if (mode < 1 || mode > 2){
		scoreBoardPrinter(numlights,difficulty,mode);
		logging("ERROR! Please enter a valid mode (1 or 2)!");
		failed = true;
	}

	difficulty = atoi(argv[1]);	

	cout << "Difficulty: " << difficulty << " Mode: " << mode << endl;

	if (difficulty == 1){
		sleeptime = 1000000;
	}
	else if (difficulty == 2){
		sleeptime = 500000;
	}
	else if (difficulty == 3){
		sleeptime = 200000;
	}
	else{
		logging("ERROR! Please enter a valid input between (1 - 3)!");
		failed = true;
	}

	
	//Request GPIO Pins for reading LEDs and Button inputs
	if ((rq1 = gpio_is_requested(gpio0)) < 0)
	{
		logging("gpio_is_requested");
		return EXIT_FAILURE;
	}
	if ((rq2 = gpio_is_requested(gpio1)) < 0)
	{
		logging("gpio_is_requested");
		return EXIT_FAILURE;
	}

	if ((rq3 = gpio_is_requested(gpio2)) < 0)
	{
		logging("gpio_is_requested");
		return EXIT_FAILURE;
	}

	if ((rq4 = gpio_is_requested(gpio3)) < 0)
	{
		logging("gpio_is_requested");
		return EXIT_FAILURE;
	}

	//Request lights
	gpio_is_requested(gpio0);
	gpio_is_requested(gpio1);
	gpio_is_requested(gpio2);
	gpio_is_requested(gpio3);


	// export the gpio
	if (!rq1) {
		logging("exporting gpio");
		if ((rv1 = gpio_request(gpio0, NULL)) < 0)
		{
			logging("gpio_request");
			return EXIT_FAILURE;
		}
	}
	if (!rq2) {
		logging("exporting gpio");
		if ((rv2 = gpio_request(gpio1, NULL)) < 0)
		{
			logging("gpio_request");
			return EXIT_FAILURE;
		}
	}
	if (!rq3) {
		logging("exporting gpio");
		if ((rv3 = gpio_request(gpio2, NULL)) < 0)
		{
			logging("gpio_request");
			return EXIT_FAILURE;
		}
	}
	if (!rq4) {
		logging("exporting gpio");
		if ((rv4 = gpio_request(gpio3, NULL)) < 0)
		{
			logging("gpio_request");
			return EXIT_FAILURE;
		}
	}

	//Display Intro Light Show
	for (i = 0; i < 4; i++) 
	{ 
		gpio_direction_output(gpio0, 0);
		gpio_direction_output(gpio1, 0);
		gpio_direction_output(gpio2, 0);
		gpio_direction_output(gpio3, 0);
	 	gpio_direction_output(i, 1);

		usleep(100000);
	}
	for (i=3; i >=0; i--) 
	{ 
		gpio_direction_output(gpio0, 0);
		gpio_direction_output(gpio1, 0);
		gpio_direction_output(gpio2, 0);
		gpio_direction_output(gpio3, 0);
	 	gpio_direction_output(i, 1);

		
		usleep(100000);
	}


	gpio_direction_output(gpio0, 0);
	gpio_direction_output(gpio1, 0);
	gpio_direction_output(gpio2, 0);
	gpio_direction_output(gpio3, 0);

	sleep(1);

	//Begin Game
	while (failed == false){ 

		//Display the series of lights
		displaylights(gpio0, gpio1, gpio2, gpio3, rnum, numlights, sleeptime);
		usleep(300000);
		//Display flicker of lights to signify round number and the change of displaying to reading
		for (int w = 0; w < numlights*2; w++){
			gpio_set_value(gpio0, w % 2);
			gpio_set_value(gpio1, w % 2);
			gpio_set_value(gpio2, w % 2);
			gpio_set_value(gpio3, w % 2);
			usleep(10000);
		}

		j = 0;
		i = 0;
		gpio_set_value(gpio0,0);
		gpio_set_value(gpio1,0);
		gpio_set_value(gpio2,0);
		gpio_set_value(gpio3,0);
		usleep(200000);
	
		logging("Reading Lights");
		gpio_direction_input(gpio0);
        gpio_direction_input(gpio1);
        gpio_direction_input(gpio2);
        gpio_direction_input(gpio3);

        if (mode == 1){
			while (i < numlights && failed == false){
				cout << "Position: " << i + 1 << endl;
				j = checkbuttons(gpio0, gpio1, gpio2, gpio3, rnum, numlights, 0, i, sleeptime);
				if (j < 0){
					logging("Game Over");
					failed = true;
					scoreBoardPrinter(numlights,difficulty,mode);
					gameover(gpio0, gpio1, gpio2, gpio3);
				}
				i++;
				usleep(500000);
			}
		}
		else{
			k = numlights -1;
			while (k >= 0 && failed == false){
				cout << "Position: " << i + 1 << endl;
				j = checkbuttons(gpio0, gpio1, gpio2, gpio3, rnum, numlights, 0, k, sleeptime);
				if (j < 0){
					logging("GAME OVER!");
					failed = true;
					scoreBoardPrinter(numlights,difficulty,mode);
					gameover(gpio0, gpio1, gpio2, gpio3);
				}
				k--;
				usleep(500000);
			}
		}

		sleep(1);
		numlights++;

	
	}

	
		gpio_direction_output(gpio0, 0);
		gpio_direction_output(gpio1, 0);
		gpio_direction_output(gpio2, 0);
		gpio_direction_output(gpio3, 0);
	

	// unexport the gpio
	if (!rq1) {
		logging("> unexporting gpio");
		if (gpio_free(gpio0) < 0)
		{
			logging("gpio_free");
		}
	}
	if (!rq2) {
		logging("> unexporting gpio");
		if (gpio_free(gpio1) < 0)
		{
			logging("gpio_free");
		}
	}
	if (!rq3) {
		logging("> unexporting gpio");
		if (gpio_free(gpio2) < 0)
		{
			logging("gpio_free");
		}
	}
	if (!rq4) {
		logging("unexporting gpio");
		if (gpio_free(gpio3) < 0)
		{
			logging("gpio_free");
		}
	}
	return 0;

}