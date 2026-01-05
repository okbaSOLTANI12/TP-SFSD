#ifndef LIB_H
#define LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 40
#define MAX_INDEX 5000
#define MAX_NAMES 5000
#define INDEX_BLOCK_SIZE 40

/********************* STRUCTURES *****************************/

typedef struct
{
    int Student_ID;
    char Family_Name[30];
    char First_Name[30];
    int Day_Birth;
    int Month_Birth;
    int Year_Birth;
    int Wilaya_Code;
    char Wilaya_Name[30];
    int Gender;
    int Blood_Type_Code;
    char Blood_Type[4];
    int Year_Study_Code;
    char Year_Study[4];
    char Speciality[40];
    int Resident_UC;
    int deleted;
} TStudent;

typedef TStudent TArray[MAX];

/* LNOF Structure - for STUDENTS_ESI.BIN */
typedef struct
{
    TArray records;
    int nb_records;
    int next_block;
} TBlock;

typedef struct
{
    int first_block;
    int total_records;
    int last_block;
} THeader;

typedef struct
{
    FILE *file;
    THeader *header;
} TLNOF;

/* LOF Structure - for STUDENTS_CP.BIN */
typedef struct
{
    TArray records;
    int nb_records;
    int next_block;
} TLOFBlock;

typedef struct
{
    FILE *file;
    THeader *header;
} TLOF;

/* Index Structures */
typedef struct
{
    int key;
    int block_num;
    int position;
} TIndexEntry;

typedef struct
{
    TIndexEntry entries[MAX_INDEX];
    int count;
} TIndexTable;

typedef TIndexEntry TIndexBlock[INDEX_BLOCK_SIZE];

typedef struct
{
    TIndexBlock entries;
    int nb_entries;
    int next_block;
} TTOFIndexBlock;

typedef struct
{
    char family[30];
    char first[60];
    int gender;
} TNameEntry;

/********************* GLOBAL VARIABLES *****************************/

extern TIndexTable GlobalIndex;
extern int N_IO_READ;
extern int N_IO_WRITE;

extern char MainFileName[];
extern char IndexFileName[];
extern char CPFileName[];
extern char NamesFileName[64];

extern TNameEntry NamesArray[MAX_NAMES];
extern int NamesCount;
extern int UsedIDs[9001];

extern char *WilayaList[59];
extern char *SpecialityList[4];

/********************* FUNCTION DECLARATIONS *****************************/

// Utilities
void ClearScreen();
void Pause();
void PrintLine(int length);
void ShowHeader();
int IsLeapYear(int y);
int DaysInMonth(int m, int y);
void MapBloodType(int code, char bt[4]);
void MapYearStudy(int code, char ys[4], char spec[40]);
int GetMaxBirthYearForStudyLevel(int year_study_code);
int GetBirthYearForStudyLevel(int year_study_code);
int IsValidBirthYearForLevel(int birth_year, int year_study_code);
void PrintWilayaList();

// Names
void LoadNames();

// LNOF File Operations (for STUDENTS_ESI.BIN)
void OpenFile(TLNOF *file, char filename[30], const char mode);
void CloseFile(TLNOF *file);
void WriteBlock(TLNOF *file, int block_index, TBlock *buffer);
void ReadBlock(TLNOF *file, int block_index, TBlock *buffer);
void SetHeader(TLNOF *file, int field_num, int value);
int GetHeader(TLNOF *file, int field_num);
void AllocateBlock(TBlock *buffer);

// LOF File Operations (for STUDENTS_CP.BIN)
void OpenLOFFile(TLOF *file, char filename[30], const char mode);
void CloseLOFFile(TLOF *file);
void WriteLOFBlock(TLOF *file, int block_index, TLOFBlock *buffer);
void ReadLOFBlock(TLOF *file, int block_index, TLOFBlock *buffer);
void SetLOFHeader(TLOF *file, int field_num, int value);
int GetLOFHeader(TLOF *file, int field_num);
void AllocateLOFBlock(TLOFBlock *buffer);

// Index Operations
void InitIndex();
void InsertIndexSorted(int key, int block_num, int position);
int SearchIndex(int key, int *block_num, int *position);
void DeleteFromIndex(int key);
void UpdateIndexAfterDeletion(int block_num, int deleted_pos);
void SaveIndexToFile();
void LoadIndexFromFile();
void SearchFreePosition(TLNOF *file, int *free_pos, int *block_num, int *found);

// Student Operations
int GenerateUniqueID();
void GenerateRandomStudent(TStudent *S);
void DisplayStudent(TStudent *S);

// Main Operations
void CreateAndInitialLoad();
void SearchStudent();
void InsertStudent();
void DeleteStudent();
void ModifyFirstName();

// Queries
void QueryBloodTypeResident();
void QuerySpeciality();
void QueryUnder20YearsOld();
void QueryYearOfStudy();

// Advanced
void CreateCPFile();

// Display Functions (work for both LNOF and LOF)
void DisplayFileHeader();
void DisplaySpecificBlock();
void DisplayAllRecords();

// Menu
void ShowMainMenu();

#endif
