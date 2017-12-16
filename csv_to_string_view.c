/*********************************************************************************************************

CSV DATALOG FILE INTEGER TO STRING CONVERTER VIEWER by Dan Lindamood III 2017-12-15

Read an IDEC datalog file of ascii strings stored as comma separated 16 bit integers.
Parse each logline. Convert each integer to 2 ascii characters and display them.


Build 2017-12-15


*********************************************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//////////////// PARSE COMMA ///////////////// PARSE COMMA ///////////////// PARSE COMMA ///////////////// PARSE TAB /////////////////

const char* getfield(char* tmpline, int col_num){

  char * pch;
  static char tmp_comma[120];
  int i = 0, q = 0;
  int start = 0;
  int end;

  pch=strchr(tmpline,',');
  tmp_comma[0] = '\0'; 
  
  while (pch!=NULL){ //Search for each comma and provide get the characters between each comma and combine them to one string
    end = pch-tmpline+1;
    if (i == col_num){
        for(q = 0; (q < end-start-1)&& (q < 120); q++) {
			tmp_comma[q] = tmpline[start+q];
		}
		tmp_comma[q]='\0';
    }
    i++;
    start = pch-tmpline+1;
    pch=strchr(pch+1,',');
  }
   return tmp_comma; // Send string back to the CONVERT LINE routine
}


//////////////// CONVERT LINE///////////////// CONVERT LINE ///////////////// CONVERT LINE ///////////////// CONVERT LINE ///////////////// 

int convert(char* wordline){
	
    int msg,
		column,
		done=1,
		msg_in;
    char *ptr,cell[16];
	
        for(column = 1; done; column++){ // Ignore 1st column (IDEC LOG FILE date stamp)
                strcpy(cell, getfield(wordline, column)); // go to subroutine to retrieve the singe word registry 16 bit integer
                if (strlen(cell) > 0){
                        msg_in = atoi(cell); //Check value of word registry integer
                        msg = msg_in >> 8; msg |= msg_in << 8; // shift bits to retrieve each ascii character(2)
                        ptr = (char *)&msg;  
                        printf("%c%c", ptr[0],ptr[1]); //send both asii character to screen
                }
				else done = 0;
		}

      printf("\r");
        return 0; // No need to return anything to the MAIN routine
}

//////////////// MAIN ///////////////// MAIN ///////////////// MAIN ///////////////// MAIN ///////////////// MAIN /////////////////
int main()
{


	char fileline[1024];
	char file_name[128];
	FILE* log_file;


	printf("CSV DATALOG FILE INTEGER TO STRING CONVERTER VIEWER	by Dan Lindamood III 2017-12-15\n\n\
This program will read a datalog file of ascii strings stored as comma separated 16 bit integers.\n\
Each line is parsed to remove the comma and separate each integer. The integer is then converted to 2 ascii characters.\n\
The converted data is displayed on the screen.\n\n\
Enter the datalog file name. \nInclude path if file is located in a separate folder from this application\nExample: C:\\Datalog_Folder\\ReadFile.csv\n\nREAD FILE : ");

	scanf("%s", file_name); //Entered file name
	
    log_file = fopen(file_name, "r"); // Check if file exists

	if (log_file == NULL) {// Check if log file exists. Close application if the file is not there.
		printf("Log File entered (%s) is not readable\n\n", file_name);
	    system("pause");// Comment Out for Linux
		exit (0);
	}
	
	else{
		printf("\n\n\n"); //Add a few lines after entering the file name 
		
		while (fgets(fileline, 1024, log_file)){ // Run through file one line at a time
			convert(fileline); //Send 1 line to sub routine for processing
		}
		fclose(log_file); // Close the file at completion
	}
	printf("\n\n\n"); //add a few empty lines after completion
    system("pause");// Comment Out for Linux
	exit (0);
}
