//kernel.c
//Joao Alves and David Rochon

void printString(char*);
void printChar(char c);
void readString(char*);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
void readFile(char* filename, char* buffer2, int* sectorsRead);
void executeProgram(char* name);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void terminate();
void handleTimerInterrupt(int segment, int sp);


// global variables from step 2
int processActive[8];
int processStackPointer[8];
int currentProcess;


void main()
{
	char line[100];
	char buffer[512];
	char buffer2[13312];
	int sectorRead;
	int i;

	char shell[6];
	shell[0] = 's';
	shell[1] = 'h';
	shell[2] = 'e';
	shell[3] = 'l';
	shell[4] = 'l';
	shell[5] = '\0';

	// step 2:
	// table contained 8 entires
	// setting Active to 0, StackPointer to 0xff00, currentProcess to -1
	currentProcess = -1;
	for(i = 0; i < 8; i++)
	{
		processActive[i] = 0;
		processStackPointer[i] = 0xff00;
	}

//	makeInterrupt21();  //crashed the program


	interrupt(0x21, 4, "shell", 0, 0);

	makeInterrupt21();

	makeTimerInterrupt();

        //call shell
 //       interrupt(0x21, 4, "shell", 0, 0);

//	makeTimerInterrupt();

	while(1); /*hang up*/
}

void printString(char* chars)
{
	//intital value
	int index = 0;
	//an array of 0 values
	char i = chars[index];

	while (i != 0)
	{	//interrupt(0x10, 0xe * 256 + 'Q', 0, 0, 0)
				//prints the letter 'Q'
		interrupt(0x10, 0xe * 256 + i, 0, 0, 0);
		i = chars[++index]; //prints out each char inside the array
					//moves onto the next char
	}				//pre-increments the index
						//value is first incremented
}

//printchar function (step 1 recommends we add this)
void printChar(char c)
{
	interrupt(0x10, 0xe * 256 + c, 0, 0, 0);
}


void readString(char* line)
{
	char buffer[];
//	char true;
	int index = 0;
	char readingString = interrupt(0x16,0 ,0 ,0 ,0);
	while(1)
	{
		//if not enter or backspace
		if(readingString != 0xd && readingString != 0x8)
		{
			//store it
			buffer[index] = readingString;
			index++;
			//write to screen
			interrupt(0x10, 0xe * 0x100 + readingString, 0, 0, 0);
		}
		//if backspace entered
		else if(readingString == 0x8)
		{
			// move one character back
			index--;
			//
		//	interrupt(0x10, 0xe*256+0x08, 0, 0, 0);
		//	interrupt(0x10, 0xe*256 + ' ', 0, 0, 0);
		//	interrupt(0x10, 0xe*256+0x08, 0, 0, 0);
			printChar(0x08);
			printChar(0x20);
			printChar(0x08);
		}
	/*	else
	//	{
			// reading from user input
	//		index = interrupt(0x16, 0, 0, 0, 0);
			//pointer to char array/char
	//		*(line + index) = i;
			// printing user's input to the screen
	//		interrupt(0x10, 0xe * 0x100 + i, 0, 0, 0);
	//		index++;
		} */
	}
	printChar(0xa);
	line[index] = 0xa;
	printChar(0x0);
	index++;
	line[index] = 0x0;
}

void readSector(char* buffer, int sector)
{
	int AH = 0x2;
	int AL = 0x1;

	//int BX = char*
	//int CH = (sector, 30);
	//int CL = (sector, 30) + 1;
	//int DH = (sector, 30);

	char* BX = buffer;
	int CH = 0x0;
	int CL = sector + 1;
	int DH = 0x0;

	int DL = 0x80;
	int AX = AH * 256 + AL;
	int CX = CH * 256 + CL;
	int DX = DH * 256 + DL;

	//interrupt(number, AX, BD, CX, DX)
	interrupt(0x13, AX, BX, CX, DX);

	//interrupt(0x13, AX, CX, DX, buffer);

}

void writeSector(char* buffer, int sector)
{
	int AH = 0x3;
        int AL = 0x1;

        char* BX = buffer;
        int CH = 0x0;
        int CL = sector + 1;
        int DH = 0x0;

        int DL = 0x80;
        int AX = AH * 256 + AL;
        int CX = CH * 256 + CL;
        int DX = DH * 256 + DL;

	interrupt(0x13, AX, BX, CX, DX);

}


//function to read a file and to load it, sector by sector, to buffer array
void readFile(char* filename, char* buffer2, int* sectorsRead)
{

	//will store the directory once it is loaded via readSector
	char directory[512];
	//iterator for for loop to find the file name in the directory
	int fileentry = 0;
	int loadIndex = 6;
	char s;

	//load the directory sector into the character array directory[]
	//use 2 as the sector because that is the sector holding the dir
	//1 is the map, 2 is the dir
	*sectorsRead = 0;
	readSector(directory, 2);

	//debugging statement
	//printString(directory);

	//loop to find the file name
	for(fileentry; fileentry < 512; fileentry += 32)
	{
//		printChar(directory[0]);
//		printChar(filename[0]);
		//printChar('a');
		if(filename[0] == directory[fileentry + 0])
		{
//			printChar(filename[0]);
//			printChar(directory[fileentry + 0]);
			if(filename[1] == directory[fileentry + 1])
			{
//			printChar(filename[1]);
//			printChar(directory[fileentry + 1]);
				if(filename[2] == directory[fileentry + 2])
				{
//					printChar(filename[2]);
//					printChar(directory[fileentry + 2]);
					if(filename[3] == directory[fileentry + 3])
					{
//						printChar(filename[3]);
//						printChar(directory[fileentry + 3]);
						if(filename[4] == directory[fileentry + 4])
						{
//							printChar(filename[4]);
//							printChar(directory[fileentry + 4]);
							if(filename[5] == directory[fileentry + 5])
							{//	printChar(filename[5]);
//								printChar(directory[fileentry + 5]);
								//printChar('!');
								//debugging statements
								//printString("Found file!");
/*								*sectorsRead = *sectorsRead + 1;*/

								//for loop to load the file
								for(loadIndex; loadIndex < 32; loadIndex++)
								{

									s = directory[fileentry + loadIndex];
									//done reading file
									if(s == 0)
										break;
									else
									{
										readSector(buffer2, s);
										buffer2 += 512;
										*sectorsRead = *sectorsRead + 1;
									}

								}
									break;
							} else
								continue;
						}
						else
							continue;
					}
					else
						continue;
				}
				else
					continue;
			}
			else
				continue;
		}
		else
			continue;
	}

}


//function to execute a program by first reading the file name of the program, loading it into memory, and then executing it
void executeProgram(char* name)
{
/*  	// --- OLD executeProgram ---

	//buffer to hold the file name
	char bufferForFile[13312];
	int index;
	int sectorRead;
	//read file into the buffer
//	printChar('#');
	readFile(name, bufferForFile, &sectorRead);

	if( sectorRead == 0)
		return;
//	printChar('%');

	//load the file into memory using putInMemory from kernel.asm
	//;void putInMemory (int segment, int address, char character)
	for(index = 0; index < 13312; index++)
	{
		putInMemory(0x2000, index, bufferForFile[index]);
	}

//	printChar('*');
	launchProgram(0x2000);

*/


	//buffer to hold the file name
	char buffer[13312];
	int index;

	//step 3 execute reconstruction
	int i;
	int j;
	int segment;

 	char dataseg;
	dataseg = setKernelDataSegment();
//	currentProcess = -1;
	restoreDataSegment(dataseg);

	//reading file into buffer
        readFile(name, buffer);

	setKernelDataSegment();

	//inside processActive array, looking for a free entry
	for(i = 0; i < 8; i++)
	{
		if(processActive[i] == 0)
			break;
	}

	restoreDataSegment();
	//with segment: take the entry [i], add 2, multiply by 0x1000
	segment = 0x1000 * (i+2);

	//copying the buffer into segment with putInMemory
	for(j = 0; j < 13312; j++);
	{
		putInMemory(segment, j, buffer[j]);
	}

	// calling initializeProgram
	initializeProgram(segment);

	setKernelDataSegment();
	//set processActive to 1 & processStackPointer to 0xff00
	processActive[i] = 1;
	processStackPointer[i] = 0xff00;
	restoreDataSegment();

}



//makes interrupt 21 based on function in kernel.asm. Stores our code in the interrupt vector table at the base of memory
//when interrupt 21 happens, goes to the table in memory, executes our code
void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	//debugging statement
	//printString("Interrupt 21 Works!");

	//if ax equals zero, then call printString()
	if(ax == 0)
	{
		printString(bx);
	}

	//else readString() is called
	else if(ax == 1)
	{
		readString(bx);
	}

	//else read sector
	else if(ax == 2)
	{
		readSector(bx, cx);
	}
	else if(ax == 3)
	{
		readFile(bx, cx, dx);
	}
	else if(ax == 4)
	{
		executeProgram(bx);
	}
	else if(ax == 5)
	{
		terminate();
	}
	else if(ax >= 6)
	{
		printString("Invalid value for AX. No function available! Please try again.");
	}
}


void terminate()
{
	//updated terminate for step 5

	setKernelDataSegment();
	processActive[currentProcess] = 0;
	//processStackPointer[currentProcess] = 0xff00;
	restoreDataSegment();

	while(1);
/*
	char shellname[6];
	shellname[0] = 's';
	shellname[1] = 'h';
	shellname[2] = 'e';
	shellname[3] = 'l';
	shellname[4] = 'l';
	shellname[5] = '\0';

	//executeProgram(shellname, 0x2000);
	interrupt(0x21, 4, shellname, 0x2000, 0);

	while(1);
*/
}


void handleTimerInterrupt(int segment, int sp)
{
	printString("tic");
	returnFromTimer(segment, sp);

/*
	int activeRun = 0;
	int i = 0;


	// saving sp parameter into processStackPointer indexed by currentProcess
	// only do this if segment is not -1 
	if(segment!= -1)
	{
		setKernelDataSegment();
		processStackPointer[currentProcess] = sp;
		restoreDataSegment();
	
		setKernelDataSegment();
	        currentProcess++;
	        restoreDataSegment();

	}
	//finding in activeRun, loop through 8 currentProcess
	setKernelDataSegment();
	while(i < 8 && activeRun == 0)
	{
		// checking if processActive entry for currentProcess is 1
		if(processActive[currentProcess] == 1)
		{
			// setting segment: step 4
			setKernelDataSegment();
			segment = (2 + currentProcess) * 0x1000;
			restoreDataSegment();

			// setting sp: step 4
			setKernelDataSegment();
			sp = processStackPointer[currentProcess];	
			restoreDataSegment();
			
		  	activeRun = 1;
		}

		else
		{
			currentProcess++;
			
			//if currentProcess reaches 8, loop back to 0
			
			if(currentProcess > 7) 
			{
				currentProcess = 0;
			}
		}
		i++;
	}

	restoreDataSegment();
	returnFromTimer(segment, sp);
*/
}



