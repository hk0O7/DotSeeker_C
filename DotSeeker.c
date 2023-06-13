#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "titlescreen.h"


int vlines = 24;
int hchars = 80;
int autopilot_enabled = 0;
int frame_time_30 = 33333;
int frame_time_60 = 16667;
//int frame_time = 33333; // 30fps
int frame_time = 16667; // 60fps
int time_limit = 60;
int debug_enabled = 0;


void clearScreen(){
	for(int i = 1; i <= vlines; i++){
		printf("\e[%i;0H\e[K", i);
	}
}

int safeExit(int exitCode){
	system("stty sane");
	system("tput cnorm");
	system("stty echo");
	printf("\n");
	exit(exitCode);
}

int elegantExit(){
	clearScreen();
	printf("\e[1;1H");
	safeExit(0);
}

void clearPos(int cpos_y, int cpos_x){
	printf("\e[%i;%iH  ", cpos_y, cpos_x);
}

void drawPos(int cpos_y, int cpos_x, int colorCode){
	printf("\e[%i;%iH", cpos_y, cpos_x);
	printf("\e[1;4%im  \e[0m", colorCode);
	printf("\e[1;1H\n");
}

void clearLine(int lineNo){
	printf("\e[%i;1H\e[K", lineNo);
}

void drawHUD(int score,
  int delay,
  int currentPos_y, int currentPos_x,
  int center_y, int center_x,
  int coinPos_y, int coinPos_x,
  int boundary_y, int boundary_x,
  char keyPressed, int timer
){
	int i = 3;
	if(autopilot_enabled){
		clearLine(1);
		printf("[AUTOPILOT ENABLED]");
		if(debug_enabled){
			clearLine(i++);
			printf("keyPressed: %c", keyPressed);
		}
	}
	if(debug_enabled){
		clearLine(i++);
		printf("Delay: %i", delay);
		clearLine(i++);
		printf("currentPos: %i %i", currentPos_y, currentPos_x);
		clearLine(i++);
		printf("coinPos: %i %i", coinPos_y, coinPos_x);
		clearLine(i++);
		printf("vlines: %i hchars: %i", vlines, hchars);
		clearLine(i++);
		printf("center: %i %i", center_y, center_x);
		clearLine(i++);
		printf("boundary: %i %i", boundary_y, boundary_x);
	}
	printf("\e[30m");
	int score_length = printf("%i", score);
	int timer_seconds = timer / 1000000;
	printf("\e[2;%iH\e[1;34m%i\e[0m", hchars - ++score_length, score);
	printf("\e[2;3H\e[1;32m%i\e[0m", timer_seconds);
	if(timer_seconds == 9) printf(" ");
}

void drawCoin(int pos_y, int pos_x){
	drawPos(pos_y, pos_x, 3);
}

void spawnCoin(int *coinPos_y, int *coinPos_x,
  int currentPos_y, int currentPos_x
){
	while(1){
		*coinPos_y = (rand() % vlines) + 1;
		*coinPos_x = ((rand() % (hchars / 2)) * 2) + 1;
		if(*coinPos_y != currentPos_y || *coinPos_x != currentPos_x) break;
	}
	drawCoin(*coinPos_y, *coinPos_x);
}

void alarmHandler(){
	signal(SIGALRM, SIG_IGN);
}

void interruptHandler(){
	safeExit(130);
}

char autoPilot(int currentPos_y, int currentPos_x,
  int coinPos_y, int coinPos_x
){
	if(currentPos_y != coinPos_y){
		if(currentPos_y > coinPos_y) return 'w';
		else return 's';
	}else{
		if(currentPos_x > coinPos_x) return 'a';
		else return 'd';
	}
}

void toggleAutoPilot(int *autopilot_enabled) {
	if(*autopilot_enabled) {
		*autopilot_enabled = 0;
	} else {
		*autopilot_enabled = 1;
	}
	clearScreen();
}

void toggleDebug(int *debug_enabled){
	if(*debug_enabled){
		*debug_enabled = 0;
		clearScreen();
	}else{
		*debug_enabled = 1;
	}
}

void centerPrint(char msg[], int voffset, int charoffset){
	int vCenter = (vlines / 2) + voffset;
	int hCenter = hchars / 2;
	printf("\e[1;1H");
	int msgLength = printf("%s", msg) - charoffset;
	printf("\e[1;1H\e[K");
	int hPos = hCenter - (msgLength / 2) + 1;
	if(hchars % 2) hPos++;
	printf("\e[%i;%iH%s\n", vCenter, hPos, msg);
}

void endScreen(int score){
	int scoreLength = printf("%i", score);
	clearScreen();
	if(score >= 30){
		printf("\e[%i;%iH", (vlines / 2) - 1, (hchars / 2) - 6);
		printf("\e[1;32mYOU ARE WINNER\e[0m");
		printf("\e[%i;%iH", (vlines / 2) + 1, (hchars / 2) - ((7 + scoreLength) / 2) + 1);
		printf("\e[1;32mScore: \e[1;34m%i\e[0m\n", score);
	}else{
		printf("\e[%i;%iH", vlines / 2, (hchars / 2) - 4);
		printf("\e[1;31mYOU LOSE!\e[0m\n");
	}
	char keystroke;
	signal(SIGALRM, alarmHandler);
	ualarm(500000, 0);
	while(keystroke != -1){
		keystroke = getchar();
		if(keystroke == 'q' || keystroke == 'Q'){
			goto done;
		}else if(keystroke == 3) safeExit(130);
	}
	getchar();
	done:;
}

int safe_usleep(int sleep_time){
	int keystroke;
	signal(SIGALRM, alarmHandler);
	ualarm(sleep_time, 0);
	while(keystroke != -1){
		keystroke = getchar();
		if(keystroke == 3) safeExit(130);
	}
}


int main(){
	system("stty raw");
	system("tput civis");
	system("stty -echo");
	signal(SIGTERM, interruptHandler);

	clearScreen();
	titleScreen();
	
	int score = 0;
	int center_y = vlines / 2;
	int center_x;
	if((hchars / 2) % 2) center_x = (hchars / 2) + 2;
	else center_x = (hchars / 2) + 1;
	int boundary_y = vlines;
	int boundary_x;
	if(hchars % 2) boundary_x = hchars - 2;
	else boundary_x = hchars - 1;
	int currentPos_y = center_y;
	int currentPos_x = center_x;
	int previousPos_y = currentPos_y;
	int previousPos_x = currentPos_x;
	int coinPos_y, coinPos_x;
	int colorCode = 7;
	char keyPressed;
	char lastKeyPressed;
	srand((unsigned)time(NULL));
	spawnCoin(&coinPos_y, &coinPos_x, currentPos_y, currentPos_x);
	int timer = time_limit * 1000000;

	while(1){
		previousPos_y = currentPos_y;
		previousPos_x = currentPos_x;
		signal(SIGALRM, alarmHandler);
		int delay = ualarm(frame_time, 0);
		char keyPressed = getchar();
		if(autopilot_enabled && delay == 0 && keyPressed == -1){
				keyPressed = autoPilot(currentPos_y, currentPos_x, coinPos_y, coinPos_x);
		}
		if(keyPressed == -1 && lastKeyPressed != -1){
			keyPressed = lastKeyPressed;
		}
		usleep(delay);
		switch(keyPressed){
			case 'w':
			case 'W':
				if(currentPos_y == 1) break;
				currentPos_y--;
				break;
			case 's':
			case 'S':
				if(currentPos_y == boundary_y) break;
				currentPos_y++;
				break;
			case 'a':
			case 'A':
				if(currentPos_x == 1) break;
				currentPos_x -= 2;
				break;
			case 'd':
			case 'D':
				if(currentPos_x == boundary_x) break;
				currentPos_x += 2;
				break;
			default:
				keyPressed = lastKeyPressed;
				break;
			case 'k':
			case 'K':
				toggleDebug(&debug_enabled);
				keyPressed = lastKeyPressed;
				break;
			case 'b':
				toggleAutoPilot(&autopilot_enabled);
				keyPressed = lastKeyPressed;
				break;
			case 'q':
			case 'Q':
				elegantExit();
				break;
			case 3:
				safeExit(130);
				break;
		}
		lastKeyPressed = keyPressed;
		if(currentPos_y == coinPos_y && currentPos_x == coinPos_x){
			score ++;
			spawnCoin(&coinPos_y, &coinPos_x, currentPos_y, currentPos_x);
		}
		if(timer <= 0) break;
		timer -= frame_time;
		drawHUD(score, delay, currentPos_y, currentPos_x, center_y, center_x, coinPos_y, coinPos_x, boundary_y, boundary_x, keyPressed, timer);
		if(previousPos_y != coinPos_y || previousPos_x != coinPos_x){
			clearPos(previousPos_y, previousPos_x);
		}
		drawPos(currentPos_y, currentPos_x, colorCode);
		drawCoin(coinPos_y, coinPos_x);
	}
	
	endScreen(score);
	elegantExit();
	return(0);
}

