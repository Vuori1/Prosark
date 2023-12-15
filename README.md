# Prosark
Prosessoriarkkitehtuuri kurssin projekti
abstract:
This project implements a multi-process pipeline to analyze a given test string for missing and found 
alphabets. The pipeline consists of three main tasks:
Get_Input (P0): This task is performed by the parent process (P0) and involves acquiring the test string from the 
user.
Clean_Input (C0): The child process (C0) removes numbers and special characters from the test string.
Find_Missing (P1): Another process (P1) finds the missing and found alphabets from the cleaned string.
