// Joao Alves & David Rochon
int fileType(char* string);
int executeFile(char* string);
int directoryList(char* string);
int main2();

enableInterrupts();


int main()
{
	main2();
}

char type [] = "type";
char execute [] = "exec";
char dir [] = "dir";

char lines[100];
char badCommand [] = "File Not Found, Try again \r \n";
char buffer[13312];
char directory[512];

int main2()
{
	enableInterrupts();

	while(1)
	{
		char getfilename;
		int filename;
		int sectorsRead;

		syscall( 0, "SHELL> ", 0, 0); //promt for shell
                syscall( 1, lines, 0 , 0); //read user's string to the screen

		if(directoryList(lines) == 1)
		{
			syscall(2, directory, buffer, &filename);

			if(directory != 0)
				{
					syscall( 2, buffer, 0 , 0);
				}
			else
				syscall(0, "directory not found", 0, 0);
		}


		// read the file
		else if(fileType(lines) == 1)
		{
			syscall( 3, lines + 5, buffer, &filename);

			if(filename > 0)
				syscall( 0, buffer, 0, 0);
			else
				syscall(0,"file not found", 0 , 0);
		}

		// executes the file
		else if(executeFile(lines) == 1)
		{
			syscall(0, "exec program \r \n", 0 , 0);
			syscall(0, lines + 5 , 0, 0);
			syscall(4, lines + 5, buffer);
			syscall(0, "file not found!", 0 ,0);
			//syscall( 0, buffer, 0, 0);
		}

		else
		{
			syscall(0, badCommand, 0, 0);
			// prints error message if no file matches the user input
		}
	}
}


//function to recognize 'type'
int fileType(char* string)
{

	int i = 0;
	int index = 1;

	while(i < 4) //searching with 'type'
	{
		if (string[i] != type[i])  //comparing the first letter with user input
		{
			index = 0; //if it is not equal, start over
			break;
		}

		i++; //move onto the next letter

	}
	return index;
}

// function to recognize 'exec'
int executeFile(char* string)
{
        int i = 0;
        int index = 1;

        while(i < 4) //searching with 'exec'
        {
                if (string[i] != execute[i])
                {
                        index = 0;
                        break;
                }

                i++;

        }
        return index;
}

// function to recognize 'dir'
int directoryList(char* string)
{
        int i = 0;
        int index = 1;

        while(i < 3) //searching with 'dir'
        {
                if (string[i] != dir[i])
                {
                        index = 0;
                        break;
                }

                i++;

        }
        return index;
}

