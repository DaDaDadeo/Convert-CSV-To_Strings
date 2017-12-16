/*********************************************************************************************************

CSV DATALOG FILE INTEGER TO STRING CONVERTER by Dan Lindamood III 2017-12-15

Read an IDEC datalog file of ascii strings stored as comma separated 16 bit integers.
Parse each logline. Convert each integer to 2 ascii characters and write to new file.


Build 2017-12-15

Notes: Comment out 71 and 72 and uncomment 73 if compiled for Linux. 


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

int convert(char* wordline, FILE* to_file){
	
    int msg,
		column,
		done=1,
		msg_in;
    char *characters,cell[16];
	
        for(column = 1; done; column++){ // Ignore 1st column (IDEC LOG FILE date stamp)
                strcpy(cell, getfield(wordline, column)); // go to subroutine to retrieve the singe word registry 16 bit integer
                if (strlen(cell) > 0){
                        msg_in = atoi(cell); //Check value of word registry integer
						if ((msg_in > 0) && (msg_in != 2560)){ //Ignore NULL values or single LF character. Windows handles this.
							msg = msg_in >> 8; msg |= msg_in << 8; // shift bits to retrieve each ascii character(2)
							characters = (char *)&msg;
							if (msg_in == 3338)fprintf(to_file,"%c", characters[0]); //Send only single LF character for Windows
							else fprintf(to_file,"%c%c", characters[0],characters[1]); // Reserved for Windows only
//							fprintf(to_file,"%c%c", characters[0],characters[1]); //Linux will use the carriage return
						}
                }
				else done = 0;
		}
        return 0; // No need to return anything to the MAIN routine
}

//////////////// MAIN ///////////////// MAIN ///////////////// MAIN ///////////////// MAIN ///////////////// MAIN /////////////////
int main()
{


	char fileline[1024];
	char read_file_name[128];
	char write_file_name[128];
	FILE* log_file;
	FILE* out_file;


	printf("CSV DATALOG FILE INTEGER TO STRING CONVERTER by Dan Lindamood III 2017-12-15\n\n\
This program will read a datalog file of ascii strings stored as comma separated 16 bit integers.\n\
Each line is parsed to remove the comma and separate each integer. The integer is then converted to 2 ascii characters.\n\
The converted data is stored in a new file while leaving the original datalog unchanged.\n\n\
Enter the datalog file name. \nInclude path if file is located in a separate folder from this application\nExample: C:\\Datalog_Folder\\ReadFile.csv\n\nREAD FILE : ");

	scanf("%s", read_file_name); //Entered read file name
	
    log_file = fopen(read_file_name, "r"); // Check if file exists

	if (log_file == NULL) { // Check if log file exists. Close application if the file is not there.
		printf("Log File entered (%s) is not readable\n\n", read_file_name);
	    system("pause");// Comment Out for Linux
		exit (0);
	}
	
	else{
		printf("\n\nEnter new file name. Include path if file is located in a separate folder from this application\nExample: C:\\Datalog_Folder\\NewFile.txt\n\n\nNEW FILE : ");
		scanf("%s", write_file_name); //Entered write file name
		
		out_file = fopen(write_file_name, "w+"); // Check if file exists
		
		if (out_file == NULL) {// Check if new file was created. Close application if something went wrong.
			printf("New file (%s) cannot be created.\n\n", write_file_name);
			system("pause");// Comment Out for Linux
			exit (0);
		}
		else {
			while (fgets(fileline, 1024, log_file)){ // Run through file one line at a time
				convert(fileline, out_file); //Send 1 line to sub routine for processing
			}
		}
		fclose(out_file); // Close the new file at completion
		
		out_file = fopen(write_file_name, "r"); // Verify that the file creation worked
		if (out_file == NULL) {// Check if new file was created. Close application if something went wrong.
			printf("New file (%s) was not created.\n\n", write_file_name);
			system("pause");// Comment Out for Linux
			exit (0);
		}
		else printf("\n\n\nFile creation was successful\n");
		fclose(out_file); // Close the new file finally
		
		fclose(log_file); // Close the file at completion
	}
	printf("\n\n"); //add a few empty lines after completion
    system("pause"); // Comment Out for Linux
	exit (0);
}
