/*********************************************************************************************************
CSV DATALOG FILE INTEGER TO STRING CONVERTER Win32 by Dan Lindamood III 2018-01-10

Read an IDEC datalog file of ascii strings stored as comma separated 16 bit integers.
Parse each logline. Convert each integer to 2 ascii characters and write to new file.
Build 2018-01-10

The application prompts the user to open a csv file to view. Once loaded, the user is 
prompted to save and create a new text file that will contain the converted strings.
Once created, the new text file is loaded and displayed.

Notes:	Visual Studio Settings:
		Win32 Project / Windows Application / Empty 
		Single source file with no headers
		Default properties except for Character Set = Not Set
		icon.ico not inlcuded. Must add file or comment out option.


*********************************************************************************************************/


#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <fstream>

char str_store[256]; //Global Variable

const char g_szClassName[] = { "myWindowClass" };


#define IDC_MAIN_EDIT 101
#define IDC_READ_BUTTON 102
#define IDC_SAVE_BUTTON 103
#define IDC_NOTE_STATIC 104


//#define WM_SETFONT 

#define ENABLE_TRACE



//////////////// PARSE COMMA ///////////////// PARSE COMMA ///////////////// PARSE COMMA ///////////////// PARSE TAB /////////////////

const char* getfield(const char* wordline, int col_num){

  const char * pch;
  static char tmp_comma[120];
  int i = 0, q = 0;
  __w64 int start = 0;
  __w64 int end;

  pch=strchr(wordline,',');
  tmp_comma[0] = '\0'; 
  
  while (pch!=NULL){ //Search for each comma and provide get the characters between each comma and combine them to one string
	end = pch-wordline+1;
	if (i == col_num){
		for(q = 0; (q < end-start-1)&& (q < 120); q++) {
			tmp_comma[q] = wordline[start+q];
		}
		tmp_comma[q]='\0';
	}
	i++;
	start = pch-wordline+1;
	pch=strchr(pch+1,',');
  }
   return tmp_comma; // Send string back to the CONVERT LINE routine
}


//////////////// CONVERT LINE///////////////// CONVERT LINE ///////////////// CONVERT LINE ///////////////// CONVERT LINE ///////////////// 

int convert(const HWND &hwnd, const char* wordline, FILE *new_file){
	
	int msg,
		column,
		done=1,
		msg_in,
		count = 0;
	char *characters;
	char cell[16];
	char ss[3];
	ss[2] = '\0';

	
	for(column = 1; done; column++){ // Ignore 1st column (IDEC LOG FILE date stamp)
				strcpy_s(cell, 16,getfield(wordline, column)); // go to subroutine to retrieve the singe word registry 16 bit integer
				if (strlen(cell) > 0){
						msg_in = atoi(cell); //Check value of word registry integer
						if ((msg_in > 0) /* && (msg_in != 2560)*/){ //Ignore NULL values or single LF character. Windows handles this.
							msg = msg_in >> 8; msg |= msg_in << 8; // shift bits to retrieve each ascii character(2)
							characters = (char *)&msg;
							ss[0] =  characters[0];
							ss[1] = characters[1];
							fprintf(new_file, ss); // Check if file exists
							count++;
						}
				}
				else done = 0;
		}
		return count; // No need to return anything to the MAIN routine
}


//////////////// READ FILE ///////////////// READ FILE  ///////////////// READ FILE  ///////////////// 

BOOL read_file(const HWND &hwnd, char *read_file_name, char *save_file_name)
{
	char fileline[1024];
	FILE* log_file;
	FILE* new_file;
	int stop = 0;
	int char_count = 0;
	BOOL bSuccess = FALSE;
	
	HWND hEdit = GetDlgItem (hwnd, IDC_MAIN_EDIT);
	
	fopen_s(&log_file, read_file_name, "r"); // Check if file exists
	fopen_s(&new_file, save_file_name, "w+"); // Check if file exists
	
 
	if ((log_file == NULL) || (new_file == NULL)){
		//OutputDebugString("\r\nNot A valid file\r\n");//For Degug Testing
		return bSuccess; // Return failed status
	}

	else{
	
		while (fgets(fileline, 1024, log_file)&& !stop && (char_count < 9400)) {
			convert(hwnd, fileline, new_file); //Send 1 line to sub routine for processing
		if(GetAsyncKeyState(VK_ESCAPE)){
			stop++;
				}
				//OutputDebugString(fileline); //For Degug Testing
			}
			fclose(log_file); // Close the file at completion
			fclose(new_file); // Close the file at completion
			bSuccess = TRUE; // It worked!
		}

return bSuccess;
}

///////// LOAD FILE  ///////// LOAD FILE  ///////// LOAD FILE  ///////// LOAD FILE  ///////// LOAD FILE  

BOOL LoadTextFileToEdit(HWND hEdit, LPTSTR pszFileName){

	HANDLE hFile;
	BOOL bSuccess = FALSE;
	hFile = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE){
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize != 0xFFFFFFFF){
			LPSTR pszFileText;
			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)	{
				DWORD dwRead;
				if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))	{
					pszFileText[dwFileSize] = 0; // Add null terminator
					// Set Font
					HFONT hfont = CreateFont (16, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
						OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
						DEFAULT_PITCH | FF_DONTCARE, TEXT("Consolas"));
					SendMessageA(hEdit, WM_SETFONT,(WPARAM)hfont, TRUE);

					if(SetWindowTextA(hEdit, (LPCSTR)pszFileText))
						bSuccess = TRUE; // It worked!
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(hFile);
	}
	return bSuccess;
}

///////// CREATE WINDOWS	///////// CREATE WINDOWS	///////// CREATE WINDOWS	///////// CREATE WINDOWS	

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg){
		case WM_CREATE: {  ////////// CREATE OBJECTS ////////////////////////////////////////////
			HFONT hfont;

			HWND hedit,btnread,btnsave,hnote;

			hedit = CreateWindowEx(//////// TEXT WINDOW
				WS_EX_CLIENTEDGE,
				"EDIT",
				"		CSV DATALOG FILE INTEGER TO STRING CONVERTER (Win32)\r\n\r\n\
		By Dan Lindamood III 2018-01-10\r\n\r\n\
		Conversion application to change CSV log file to ascii strings\r\n\r\n\
		This application will convert individual 16 bit integers to 2 ascii characters.\r\n\
		The characters are orginally stored as integers from an automation PLC generated csv log file\r\n\
		The characters are compliled and stored in a new file to resemble the original format.\r\n\r\n\
		Use the instructions at the top left to continue.",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_MULTILINE ,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				hwnd, 
				(HMENU)IDC_MAIN_EDIT, 
				GetModuleHandle(NULL), 
				NULL);

			hnote = CreateWindowEx(//////// TEXT WINDOW
				WS_EX_LTRREADING,
				"STATIC",
				"START: Press the READ button to open csv log file.",
				WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE ,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				hwnd, 
				(HMENU)IDC_NOTE_STATIC, 
				GetModuleHandle(NULL), 
				NULL);

			btnread = CreateWindowEx(//////// READ BUTTON
				WS_EX_CLIENTEDGE, 
				"BUTTON", 
				"READ", 
				WS_CHILD | WS_VISIBLE|WS_BORDER|WS_TABSTOP,
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				hwnd, 
				(HMENU)IDC_READ_BUTTON, 
				GetModuleHandle(NULL), 
				NULL);

			btnsave = CreateWindowEx(//////// SAVE BUTTON
				WS_EX_CLIENTEDGE, 
				"BUTTON", 
				"SAVE", 
				WS_CHILD | WS_VISIBLE , 
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				CW_USEDEFAULT, 
				hwnd, 
				(HMENU)IDC_SAVE_BUTTON, 
				GetModuleHandle(NULL), 
				NULL);

			if (hedit == NULL){
				MessageBox(NULL, "Could not create edit box", "Error!", MB_ICONEXCLAMATION | MB_OK);
				return 0;
			}

			hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(hedit, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(FALSE, 0));

		}
		break;

		case WM_COMMAND:  ////////// DO SOMETHING WHEN OBJECTS CHANGE  ////////////////////////////////////////////

			switch (LOWORD(wParam)){
		
				case IDC_READ_BUTTON: { ///////////  READ BUTTON PRESSED
					OPENFILENAME ofn;
					char szFileName_read[MAX_PATH] = "";
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
					ofn.hwndOwner = hwnd;
					ofn.lpstrFilter = "Text Files (*.CSV)\0*.csv\0All Files (*.*)\0*.*\0";
					ofn.lpstrFile = szFileName_read;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
					ofn.lpstrDefExt = "csv";
		
					if (GetOpenFileName(&ofn)){	
						strncpy(str_store, szFileName_read, sizeof(str_store)-1 );

						HWND btnread = GetDlgItem(hwnd, IDC_READ_BUTTON); // Hide READ button
						ShowWindow( btnread, SW_HIDE);

						HWND btn_save = GetDlgItem(hwnd, IDC_SAVE_BUTTON); // Show SAVE button
						ShowWindow( btn_save, SW_SHOW);

						HWND hEdit;
						hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
						LoadTextFileToEdit(hEdit, szFileName_read);
						//OutputDebugString(str_store); //For Degug Testing
			
						HWND hnote = GetDlgItem(hwnd, IDC_NOTE_STATIC);
						SetWindowTextA(hnote, "CSV LOADED: Press the SAVE button to convert and create new text file.");
					}
				}
				break;
				
				

				case IDC_SAVE_BUTTON:{ ///////////  SAVE BUTTON PRESSED
					OPENFILENAME ofn;
					char szFileName_save[MAX_PATH] = "";
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
					ofn.hwndOwner = hwnd;
					ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
					ofn.lpstrFile = szFileName_save;
					ofn.nMaxFile = MAX_PATH;
					ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
					ofn.lpstrDefExt = "txt";
		
					if (GetSaveFileName(&ofn)){	// Do something usefull with the filename stored in szFileName
						if (
							/* >>>>>>>>>>>>>>>> This is where most of the action starts <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
							read_file(hwnd, str_store,szFileName_save )){

							HWND btnread = GetDlgItem(hwnd, IDC_READ_BUTTON); // Show READ button
							ShowWindow( btnread, SW_SHOW);

							HWND btn_save = GetDlgItem(hwnd, IDC_SAVE_BUTTON); // Hide SAVE button
							ShowWindow( btn_save, SW_HIDE);

							HWND hEdit;
							hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT); 
							LoadTextFileToEdit(hEdit, szFileName_save); // Start the process of converting and saving new file

							HWND hnote = GetDlgItem(hwnd, IDC_NOTE_STATIC); // Indicate that the process of finished
							SetWindowTextA(hnote, "NEW TEXT FILE CREATED AND DISPLAYED: Press READ to convert more.");
						}
					}
		
				}
				break;
				
			default:
			break;
				
			}
		break;


		case WM_SIZE: { ////////// SET OBJECT SIZES ////////////////////////////////////////////	
			HWND hEdit,btnread,btnsave,hnote;
			RECT rcClient;
			GetClientRect(hwnd, &rcClient);
			hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
			btnread = GetDlgItem(hwnd, IDC_READ_BUTTON);
			btnsave = GetDlgItem(hwnd, IDC_SAVE_BUTTON);
			hnote = GetDlgItem(hwnd, IDC_NOTE_STATIC);
			SetWindowPos(hEdit, NULL, 10, 50, rcClient.right-20, rcClient.bottom-60, SWP_NOZORDER);
			SetWindowPos(btnread, NULL, rcClient.right-130, 10, 120, 30, SWP_NOZORDER);
			SetWindowPos(btnsave, NULL, rcClient.right - 130, 10, 120, 30, SWP_NOZORDER);
			SetWindowPos(hnote, NULL, 10, 10, rcClient.right - 150, 30, SWP_NOZORDER);
		}
		break;

		case WM_CLOSE:  ////////// HANDLE APPLICATION CLOSING ////////////////////////////////////////////
			DestroyWindow(hwnd);
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		
		default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	
return 0;
}

/*~~~~~~~~~~~ MAIN ~~~~~~~~~~~ MAIN ~~~~~~~~~~~ MAIN ~~~~~~~~~~~ MAIN ~~~~~~~~~~~ MAIN ~~~~~~~~~~~ MAIN ~~~~~~~~~~~ MAIN */

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR  lpCmdLine, int nCmdShow) {

	WNDCLASSEX wc;
	HWND hwnd;
	MSG msg;

	//step 1: registering the window class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);



	if (!RegisterClassEx(&wc)){
		MessageBox(NULL, "Window Registration failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//step 2: Creating the Window
	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"CSV Integers to Strings Conversion",
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		800,
		800,
		NULL,
		NULL,
		hinstance,
		NULL);

	if (hwnd == NULL){
		MessageBox(NULL, "Window Registration failed", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//LoadIcon(hinstance,MAKEINTRESOURCE(IDI_ICON));
	HANDLE hIcon = LoadImage(0, _T("./icon.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	if (hIcon) {
		//Change both icons to the same icon handle.
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessageA(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);

		//This will ensure that the application icon gets changed too.
		SendMessageA(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		SendMessageA(GetWindow(hwnd, GW_OWNER), WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	}


	HWND btn_save = GetDlgItem(hwnd, IDC_SAVE_BUTTON);
	ShowWindow( btn_save, SW_HIDE);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//Step-3: the message loop
	while (GetMessage(&msg, NULL, 0, 0)>0){
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
