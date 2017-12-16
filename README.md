CSV DATALOG FILE INTEGER TO STRING CONVERTER by Dan Lindamood III 2017-12-15

A late model PLC controller can store 16 bit registers in a datalog file as comma separated values. A new datalog file is created each day with the date stamp as the file name and stored on an SD card. The datalog files can store thousands of strings such as cycle prints. These strings are stored as 16 bit integers.

There are two versions of the program. One will display the results only. The other will create a new text file with the converted values.

This program will read a datalog file. Each line is parsed to remove the comma and separate each integer. The integer is then converted to 2 ascii characters. The converted data is stored in a new file while leaving the original datalog unchanged.

The programs are written for Windows where the end of line characters are edited for LF (Line Feed) only.

The programs can be compiled using the Visual Studios VS Command prompt. Compile Command Example: cl csv_to_string_file.c
