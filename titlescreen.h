int hchars, frame_time, frame_time_30, frame_time_60, autopilot_enabled;

void drawFramerateBox() {
	short int framerate;
	centerPrint("--------------", 1, 40);
	centerPrint("| Frame rate |", 2, 40);
	centerPrint("|            |", 3, 40);
	printf("\e[1;39m");
	if(frame_time == frame_time_30){
		centerPrint("30", 3, 36);
		printf("\e[1;30m");
		centerPrint("60", 3, 44);
	}else{
		centerPrint("60", 3, 44);
		printf("\e[1;30m");
		centerPrint("30", 3, 36);
	}
	printf("\e[0m");
	centerPrint("--------------", 4, 40);
}

void titleScreen() {
	printf("\e[1;33m");
	char dotAnimChar[hchars];
	dotAnimChar[0] = '(';
	for(int i = 1; i != hchars-1; i++){
		dotAnimChar[i] = ' ';
	}
	dotAnimChar[hchars-4] = ')';
	dotAnimChar[hchars-3] = '\0';
	dotAnimChar[0] = ' ';
	dotAnimChar[1] = '(';
	for(int i = 4; i < hchars - 3; i += 2){
		dotAnimChar[hchars-i+1] = '\0';
		dotAnimChar[hchars-i] = ' ';
		dotAnimChar[hchars-i-1] = ')';
		safe_usleep(frame_time);
		centerPrint(dotAnimChar, -6, -3);
	}
	safe_usleep(frame_time);
	centerPrint(" 0 ", -6, -4);
	usleep(frame_time);
	centerPrint("·", -6, -2);
	safe_usleep(250000);
	printf("\e[1;39m");
	centerPrint("|\\", -7, -11);
	centerPrint("|/", -6, -11);
	centerPrint("(", -7, -3);
	centerPrint(")", -6, -2);
	safe_usleep(250000);
	printf("\e[0m");
	centerPrint("OT", -6, -7);
	centerPrint("EEKER", -6, 4);
	safe_usleep(250000);
	centerPrint("Objective: Get 30 dots within a minute.", -2, 0);
	centerPrint("Controls:                             ", 1, 0);
	centerPrint("W,A,S,D (move)", 1, 0);
	centerPrint("Q - Quit game", 2, -2);
	centerPrint("F - Change", 3, -4);
	centerPrint("------->", 3, 16);
	drawFramerateBox();
	char pressKeyFinal[] = "Press any key to start!";
	int pressKeySize = sizeof(pressKeyFinal);
	char pressKeyCurrent[pressKeySize];
	for(int i = 0; i < pressKeySize; i++){
		pressKeyCurrent[i] = ' ';
	}
	pressKeyCurrent[pressKeySize-1] = '\0';
	for(int i = 0; i != pressKeySize - 1; i++){
		pressKeyCurrent[i] = pressKeyFinal[i];
		safe_usleep(frame_time);
		centerPrint(pressKeyCurrent, 8, 0);
	}
	while(1){
		char keystroke = getchar();
		switch(keystroke){
			case 'q':
			case 'Q':
				elegantExit();
				break;
			case 3:
				safeExit(130);
				break;
			case 'f':
				if(frame_time == frame_time_30) frame_time = frame_time_60;
				else frame_time = frame_time_30;
				drawFramerateBox();
				break;
			case 'b':
				autopilot_enabled = 1;
				goto done;
				break;
			default:
				goto done;
				break;
		}
	}
	done: clearScreen();
}
