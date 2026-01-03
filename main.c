
#include "lib.h"

/********************* GLOBAL VARIABLES DEFINITION *****************************/

TIndexTable GlobalIndex;
int N_IO_READ = 0;
int N_IO_WRITE = 0;

char MainFileName[]  = "STUDENTS_ESI.BIN";
char IndexFileName[] = "StudentID_INDEX.idx";
char CPFileName[]    = "STUDENTS_CP.BIN";
char NamesFileName[64] = "NAMES.txt";

TNameEntry NamesArray[MAX_NAMES];
int NamesCount = 0;
int UsedIDs[9001];

char *Wilaya_List[59] = {
    "", "Adrar","Chlef","Laghouat","Oum El Bouaghi","Batna",
    "Bejaia","Biskra","Bechar","Blida","Bouira",
    "Tamanrasset","Tebessa","Tlemcen","Tiaret","Tizi Ouzou",
    "Algiers","Djelfa","Jijel","Setif","Saida",
    "Skikda","Sidi Bel Abbes","Annaba","Guelma","Constantine",
    "Medea","Mostaganem","M'Sila","Mascara","Ouargla",
    "Oran","El Bayadh","Illizi","Bordj Bou Arreridj","Boumerdes",
    "El Tarf","Tindouf","Tissemsilt","El Oued","Khenchela",
    "Souk Ahras","Tipaza","Mila","Ain Defla","Naama",
    "Ain Temouchent","Ghardaia","Relizane",
    "Timimoun","Bordj Badji Mokhtar","Ouled Djellal","Beni Abbes",
    "In Salah","In Guezzam","Touggourt","Djanet","El Mghair","El Menia"
};

char *Speciality_List[4] = {"SIT","SIQ","SIL","SID"};

/********************** UTILITIES *****************************/

void ClearScreen()
{
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void Pause()
{
    printf("\n  Press ENTER to continue...");
    getchar();
    getchar();
}

void PrintLine(int length)
{
    int i;
    for(i=0; i<length; i++) printf("=");
    printf("\n");
}

void ShowHeader()
{
    ClearScreen();
    PrintLine(90);
    printf("                        ESI STUDENTS MANAGEMENT SYSTEM - LNOF\n");
    PrintLine(90);
    printf("  Lab: File Structures and Data Systems (FSDS)\n");
    printf("  Presented by: SOLTANI Okba & ZERARI Tarek\n");
    PrintLine(90);
    printf("\n");
}

int IsLeapYear(int y)
{
    return ((y%4==0 && y%100!=0) || (y%400==0));
}

int DaysInMonth(int m, int y)
{
    int days[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(m==2 && IsLeapYear(y)) return 29;
    return days[m];
}

void MapBloodType(int code, char bt[4])
{
    switch(code)
    {
        case 1: strcpy(bt,"O+"); break;
        case 2: strcpy(bt,"A+"); break;
        case 3: strcpy(bt,"B+"); break;
        case 4: strcpy(bt,"O-"); break;
        case 5: strcpy(bt,"A-"); break;
        case 6: strcpy(bt,"AB+"); break;
        case 7: strcpy(bt,"B-"); break;
        case 8: strcpy(bt,"AB-"); break;
        default: strcpy(bt,""); break;
    }
}

void MapYearStudy(int code, char ys[4], char spec[40])
{
    int spec_code;
    switch(code)
    {
        case 1:
            strcpy(ys,"1CP");
            strcpy(spec,"Integrated Preparatory Classes");
            break;
        case 2:
            strcpy(ys,"2CP");
            strcpy(spec,"Integrated Preparatory Classes");
            break;
        case 3:
            strcpy(ys,"1CS");
            strcpy(spec,"Common Core");
            break;
        case 4:
            strcpy(ys,"2CS");
            spec_code = rand()%4;
            strcpy(spec,Speciality_List[spec_code]);
            break;
        case 5:
            strcpy(ys,"3CS");
            spec_code = rand()%4;
            strcpy(spec,Speciality_List[spec_code]);
            break;
        default:
            strcpy(ys,"");
            strcpy(spec,"");
            break;
    }
}

int GetMaxBirthYearForStudyLevel(int year_study_code)
{
    switch(year_study_code)
    {
        case 1: return 2008;
        case 2: return 2007;
        case 3: return 2006;
        case 4: return 2005;
        case 5: return 2004;
        default: return 2006;
    }
}

int GetBirthYearForStudyLevel(int year_study_code)
{
    int max_year = GetMaxBirthYearForStudyLevel(year_study_code);
    int variation = rand() % 3;
    return max_year - variation;
}

int IsValidBirthYearForLevel(int birth_year, int year_study_code)
{
    int max_year = GetMaxBirthYearForStudyLevel(year_study_code);
    return (birth_year <= max_year && birth_year >= (max_year - 2));
}

void PrintWilayaList()
{
    int i;
    printf("\n  ALGERIAN WILAYAS:\n");
    PrintLine(70);
    for(i=1; i<=29; i++)
        printf("  %2d.%-20s  %2d.%-20s\n",i,Wilaya_List[i],i+29,(i+29<=58)?Wilaya_List[i+29]:"");
    PrintLine(70);
}

/********************** LOAD NAMES *****************************/

void LoadNames()
{
    FILE *f;
    char line[200];
    char *paren;
    int gender;
    char *token;
    char family[30];
    char first[60];
    int is_first_word;

    NamesCount = 0;
    f = fopen(NamesFileName,"r");
    if(!f)
    {
        printf("  [!] Error: %s not found.\n",NamesFileName);
        return;
    }

    while(NamesCount<MAX_NAMES && fgets(line,sizeof(line),f))
    {
        size_t len = strlen(line);
        if(len>0 && (line[len-1]=='\n' || line[len-1]=='\r')) line[--len]='\0';
        if(len==0) continue;

        paren = strchr(line,'(');
        if(paren==NULL) continue;

        gender = 1;
        if(strstr(paren,"(W)") || strstr(paren,"(w)") || strstr(paren,"(F)") || strstr(paren,"(f)"))
            gender = 2;
        else if(strstr(paren,"(M)") || strstr(paren,"(m)"))
            gender = 1;

        *paren = '\0';

        while(paren>line && *(paren-1)==' ') {
            paren--;
            *paren = '\0';
        }

        family[0] = '\0';
        first[0] = '\0';
        is_first_word = 1;

        token = strtok(line," \t");
        while(token != NULL)
        {
            if(is_first_word)
            {
                strncpy(family, token, 29);
                family[29] = '\0';
                is_first_word = 0;
            }
            else
            {
                if(strlen(first) > 0)
                    strcat(first, " ");
                strncat(first, token, 59 - strlen(first));
            }
            token = strtok(NULL, " \t");
        }

        if(strlen(family) == 0) strcpy(family, "UNKNOWN");
        if(strlen(first) == 0) strcpy(first, "X");

        strncpy(NamesArray[NamesCount].family, family, 29);
        NamesArray[NamesCount].family[29] = '\0';
        strncpy(NamesArray[NamesCount].first, first, 59);
        NamesArray[NamesCount].first[59] = '\0';
        NamesArray[NamesCount].gender = gender;
        NamesCount++;
    }
    fclose(f);
    printf("  [+] Successfully loaded %d names from %s\n",NamesCount,NamesFileName);
}

/*********************** FILE OPERATIONS *****************************/

void OpenFile(TLNOF *file, char filename[30], const char mode)
{
    file->header = (THeader*)malloc(sizeof(THeader));
    if((mode=='n') || (mode=='N'))
    {
        file->file = fopen(filename,"wb+");
        if(!file->file)
        {
            free(file->header);
            file->header = NULL;
            return;
        }
        file->header->first_block = 0;
        file->header->total_records = 0;
        file->header->last_block = 0;
        fwrite(file->header, sizeof(THeader), 1, file->file);
    }
    else if((mode=='a') || (mode=='A'))
    {
        file->file = fopen(filename,"rb+");
        if(file->file == NULL)
        {
            printf("  [!] File %s does not exist.\n",filename);
        }
        else
        {
            fread(file->header, sizeof(THeader), 1, file->file);
        }
    }
}

void CloseFile(TLNOF *file)
{
    if(file->file != NULL)
    {
        rewind(file->file);
        fwrite(file->header, sizeof(THeader), 1, file->file);
        fclose(file->file);
    }
    free(file->header);
}

void WriteBlock(TLNOF *file, int block_index, TBlock *buffer)
{
    rewind(file->file);
    fseek(file->file, sizeof(THeader) + (block_index-1)*sizeof(TBlock), SEEK_SET);
    fwrite(buffer, sizeof(TBlock), 1, file->file);
    N_IO_WRITE++;
}

void ReadBlock(TLNOF *file, int block_index, TBlock *buffer)
{
    rewind(file->file);
    fseek(file->file, sizeof(THeader) + (block_index-1)*sizeof(TBlock), SEEK_SET);
    fread(buffer, sizeof(TBlock), 1, file->file);
    N_IO_READ++;
}

void SetHeader(TLNOF *file, int field_num, int value)
{
    switch(field_num)
    {
        case 1: file->header->first_block = value; break;
        case 2: file->header->total_records = value; break;
        case 3: file->header->last_block = value; break;
    }
}

int GetHeader(TLNOF *file, int field_num)
{
    int val = 0;
    switch(field_num)
    {
        case 1: val = file->header->first_block; break;
        case 2: val = file->header->total_records; break;
        case 3: val = file->header->last_block; break;
    }
    return val;
}

void AllocateBlock(TBlock *buffer)
{
    int i;
    buffer->next_block = 0;
    buffer->nb_records = 0;
    for(i=0; i<MAX; i++)
        buffer->records[i].deleted = 1;
}

/***************** INDEX OPERATIONS ***************************/

void InitIndex()
{
    GlobalIndex.count = 0;
}

void InsertIndexSorted(int key, int block_num, int position)
{
    int i = GlobalIndex.count - 1;
    if(GlobalIndex.count >= MAX_INDEX) return;

    while((i >= 0) && (GlobalIndex.entries[i].key > key))
    {
        GlobalIndex.entries[i+1] = GlobalIndex.entries[i];
        i--;
    }
    GlobalIndex.entries[i+1].key = key;
    GlobalIndex.entries[i+1].block_num = block_num;
    GlobalIndex.entries[i+1].position = position;
    GlobalIndex.count++;
}

int SearchIndex(int key, int *block_num, int *position)
{
    int lower = 0, upper = GlobalIndex.count - 1, middle;
    while(lower <= upper)
    {
        middle = (lower + upper) / 2;
        if(GlobalIndex.entries[middle].key == key)
        {
            *block_num = GlobalIndex.entries[middle].block_num;
            *position = GlobalIndex.entries[middle].position;
            return 1;
        }
        else if(GlobalIndex.entries[middle].key < key)
            lower = middle + 1;
        else
            upper = middle - 1;
    }
    return 0;
}

void DeleteFromIndex(int key)
{
    int i, j;
    for(i=0; i<GlobalIndex.count; i++)
    {
        if(GlobalIndex.entries[i].key == key)
        {
            for(j=i; j<GlobalIndex.count-1; j++)
                GlobalIndex.entries[j] = GlobalIndex.entries[j+1];
            GlobalIndex.count--;
            return;
        }
    }
}

void UpdateIndexAfterDeletion(int block_num, int deleted_pos)
{
    int i;
    for(i=0; i<GlobalIndex.count; i++)
    {
        if(GlobalIndex.entries[i].block_num == block_num &&
           GlobalIndex.entries[i].position > deleted_pos)
        {
            GlobalIndex.entries[i].position--;
        }
    }
}

void SaveIndexToFile()
{
    FILE *idx;
    int C31 = 0;

    idx = fopen(IndexFileName,"wb");
    if(idx == NULL)
    {
        printf("  [!] Error saving index.\n");
        return;
    }
    fwrite(&GlobalIndex.count, sizeof(int), 1, idx);
    C31++;
    fwrite(GlobalIndex.entries, sizeof(TIndexEntry), GlobalIndex.count, idx);
    C31++;
    fclose(idx);

    printf("  [+] Index saved successfully\n");
    printf("  [COST] C31 = %d write operations\n", C31);
}

void LoadIndexFromFile()
{
    FILE *idx;
    int C32 = 0;

    idx = fopen(IndexFileName,"rb");
    if(idx == NULL)
    {
        printf("  [!] Index file not found. Empty index initialized.\n");
        InitIndex();
        return;
    }
    fread(&GlobalIndex.count, sizeof(int), 1, idx);
    C32++;
    fread(GlobalIndex.entries, sizeof(TIndexEntry), GlobalIndex.count, idx);
    C32++;
    fclose(idx);

    printf("  [+] Index loaded successfully\n");
    printf("  [COST] C32 = %d read operations\n", C32);
}

void SearchFreePosition(TLNOF *file, int *free_pos, int *block_num, int *found)
{
    int i, j;
    TBlock buffer;
    *found = 0;
    i = GetHeader(file, 1);
    if(i == 0) return;

    while(i != 0 && *found == 0)
    {
        ReadBlock(file, i, &buffer);

        if(buffer.nb_records < MAX)
        {
            for(j=1; j<=MAX && *found==0; j++)
            {
                if(buffer.records[j-1].deleted == 1)
                {
                    *found = 1;
                    *block_num = i;
                    *free_pos = j;
                }
            }
        }
        i = buffer.next_block;
    }
}

/********************** RANDOM STUDENT *****************************/

int GenerateUniqueID()
{
    int id;
    do {
        id = 1000 + rand()%8001;
    } while(UsedIDs[id]);
    UsedIDs[id] = 1;
    return id;
}

void GenerateRandomStudent(TStudent *S)
{
    int index = rand() % NamesCount;

    S->Student_ID = GenerateUniqueID();
    strncpy(S->Family_Name, NamesArray[index].family, 29);
    S->Family_Name[29] = '\0';
    strncpy(S->First_Name, NamesArray[index].first, 29);
    S->First_Name[29] = '\0';
    S->Gender = NamesArray[index].gender;

    S->Year_Study_Code = 1 + rand()%5;
    MapYearStudy(S->Year_Study_Code, S->Year_Study, S->Speciality);

    S->Year_Birth = GetBirthYearForStudyLevel(S->Year_Study_Code);
    S->Month_Birth = 1 + rand()%12;
    S->Day_Birth = 1 + rand()%DaysInMonth(S->Month_Birth, S->Year_Birth);

    S->Wilaya_Code = 1 + rand()%58;
    strcpy(S->Wilaya_Name, Wilaya_List[S->Wilaya_Code]);

    S->Blood_Type_Code = 1 + rand()%8;
    MapBloodType(S->Blood_Type_Code, S->Blood_Type);

    S->Resident_UC = rand()%2;
    S->deleted = 0;
}

void DisplayStudent(TStudent *S)
{
    int current_year = 2026;
    int age = current_year - S->Year_Birth;

    printf("\n");
    PrintLine(80);
    printf("  STUDENT DETAILS\n");
    PrintLine(80);
    printf("  ID              : %d\n", S->Student_ID);
    printf("  Name            : %s %s\n", S->First_Name, S->Family_Name);
    printf("  Gender          : %s\n", (S->Gender==2)?"Female":"Male");
    printf("  Birth Date      : %02d/%02d/%d (Age: %d)\n", S->Day_Birth, S->Month_Birth, S->Year_Birth, age);
    printf("  Wilaya          : %s (%d)\n", S->Wilaya_Name, S->Wilaya_Code);
    printf("  Blood Type      : %s\n", S->Blood_Type);
    printf("  Year of Study   : %s\n", S->Year_Study);
    printf("  Speciality      : %s\n", S->Speciality);
    printf("  Resident UC     : %s\n", (S->Resident_UC==1)?"Yes":"No");
    PrintLine(80);
}

/********************** MAIN OPERATIONS *****************************/

void CreateAndInitialLoad()
{
    ShowHeader();
    TLNOF file;
    TBlock buffer;
    TStudent student;
    int N, i;
    int block_count, record_count, position;

    if(NamesCount == 0)
    {
        printf("  [!] No names loaded. Please check %s\n", NamesFileName);
        Pause();
        return;
    }

    printf("  CREATE AND INITIAL LOAD\n");
    PrintLine(80);
    printf("\n  Number of records to create: ");
    scanf("%d", &N);

    printf("\n  [*] Creating file with %d records...\n", N);

    memset(UsedIDs, 0, sizeof(UsedIDs));
    N_IO_READ = 0;
    N_IO_WRITE = 0;
    InitIndex();

    OpenFile(&file, MainFileName, 'n');
    if(file.file == NULL)
    {
        printf("  [!] Error creating file.\n");
        Pause();
        return;
    }

    AllocateBlock(&buffer);
    block_count = 1;
    record_count = 0;
    position = 1;
    SetHeader(&file, 1, 1);
    SetHeader(&file, 3, 1);

    for(i=0; i<N; i++)
    {
        if(position > MAX)
        {
            WriteBlock(&file, block_count, &buffer);
            block_count++;
            AllocateBlock(&buffer);
            buffer.next_block = 0;
            position = 1;
        }

        GenerateRandomStudent(&student);
        buffer.records[position-1] = student;
        InsertIndexSorted(student.Student_ID, block_count, position);
        buffer.nb_records++;
        position++;
        record_count++;

        if((i+1) % 500 == 0)
            printf("  Progress: %d/%d records\n", i+1, N);
    }

    WriteBlock(&file, block_count, &buffer);
    SetHeader(&file, 2, record_count);
    SetHeader(&file, 3, block_count);
    SetHeader(&file, 1, 1);

    CloseFile(&file);

    printf("\n");
    PrintLine(80);
    printf("  [+] File created successfully!\n");
    printf("      Records: %d | Blocks: %d (numbered 1-%d)\n", record_count, block_count, block_count);
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C2     : %d\n", N_IO_READ + N_IO_WRITE);
    PrintLine(80);
    Pause();
}

void SearchStudent()
{
    ShowHeader();
    int search_id, block_num, pos, found;
    TLNOF file;
    TBlock buffer;

    printf("  SEARCH STUDENT BY ID\n");
    PrintLine(80);
    printf("\n  Enter Student ID: ");
    scanf("%d", &search_id);

    N_IO_READ = 0;
    N_IO_WRITE = 0;

    found = SearchIndex(search_id, &block_num, &pos);

    if(!found)
    {
        printf("\n  [!] Student with ID %d not found.\n", search_id);
        printf("\n  [COST] C33 = %d (0 Read + 0 Write)\n", N_IO_READ + N_IO_WRITE);
        Pause();
        return;
    }

    OpenFile(&file, MainFileName, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    ReadBlock(&file, block_num, &buffer);
    CloseFile(&file);

    if(buffer.records[pos-1].deleted == 0 && buffer.records[pos-1].Student_ID == search_id)
    {
        DisplayStudent(&buffer.records[pos-1]);
        printf("  Storage: Block %d, Position %d\n", block_num, pos);
        printf("\n  [COST BREAKDOWN]\n");
        printf("      Read blocks  : %d\n", N_IO_READ);
        printf("      Write blocks : %d\n", N_IO_WRITE);
        printf("      Total C33    : %d\n", N_IO_READ + N_IO_WRITE);
    }
    else
    {
        printf("\n  [!] Student record deleted.\n");
        printf("\n  [COST] C33 = %d\n", N_IO_READ + N_IO_WRITE);
    }
    Pause();
}

void InsertStudent()
{
    ShowHeader();
    TLNOF file;
    TBlock buffer;
    TStudent student;
    int new_id, i, j, found, free_pos, block_num;
    int spec_choice;
    int max_year;

    printf("  INSERT NEW STUDENT\n");
    PrintLine(80);

    memset(UsedIDs, 0, sizeof(UsedIDs));
    do {
        new_id = 1000 + rand()%8001;
    } while(SearchIndex(new_id, &i, &j));

    student.Student_ID = new_id;
    printf("\n  Generated ID: %d\n\n", new_id);

    printf("  Family Name: ");
    scanf("%s", student.Family_Name);
    printf("  First Name: ");
    scanf(" %[^\n]", student.First_Name);

    printf("\n  Year of Study: 1.1CP  2.2CP  3.1CS  4.2CS  5.3CS\n  Choice: ");
    scanf("%d", &student.Year_Study_Code);

    max_year = GetMaxBirthYearForStudyLevel(student.Year_Study_Code);
    printf("\n  [INFO] For %s students:\n",
           (student.Year_Study_Code==1)?"1CP":(student.Year_Study_Code==2)?"2CP":
           (student.Year_Study_Code==3)?"1CS":(student.Year_Study_Code==4)?"2CS":"3CS");
    printf("         MAX birth year is %d (can't be %d or later)\n", max_year, max_year+1);
    printf("         Valid range: %d to %d\n\n", max_year-2, max_year);

    printf("  Birth Year: ");
    scanf("%d", &student.Year_Birth);

    if(!IsValidBirthYearForLevel(student.Year_Birth, student.Year_Study_Code))
    {
        printf("  [!] ERROR: Birth year %d is INVALID for this study level!\n", student.Year_Birth);
        printf("      Maximum allowed: %d\n", max_year);
        printf("      Setting to %d (maximum allowed)\n", max_year);
        student.Year_Birth = max_year;
    }

    printf("  Birth Month (1-12): ");
    scanf("%d", &student.Month_Birth);
    printf("  Birth Day: ");
    scanf("%d", &student.Day_Birth);

    PrintWilayaList();
    printf("  Wilaya (1-58): ");
    scanf("%d", &student.Wilaya_Code);
    if(student.Wilaya_Code < 1 || student.Wilaya_Code > 58)
        student.Wilaya_Code = 16;
    strcpy(student.Wilaya_Name, Wilaya_List[student.Wilaya_Code]);

    printf("\n  Gender: 1.Male  2.Female\n  Choice: ");
    scanf("%d", &student.Gender);

    printf("\n  Blood Type: 1.O+  2.A+  3.B+  4.O-  5.A-  6.AB+  7.B-  8.AB-\n  Choice: ");
    scanf("%d", &student.Blood_Type_Code);
    MapBloodType(student.Blood_Type_Code, student.Blood_Type);

    if(student.Year_Study_Code == 1 || student.Year_Study_Code == 2)
    {
        strcpy(student.Year_Study, (student.Year_Study_Code==1)?"1CP":"2CP");
        strcpy(student.Speciality, "Integrated Preparatory Classes");
    }
    else if(student.Year_Study_Code == 3)
    {
        strcpy(student.Year_Study, "1CS");
        strcpy(student.Speciality, "Common Core");
    }
    else if(student.Year_Study_Code == 4 || student.Year_Study_Code == 5)
    {
        strcpy(student.Year_Study, (student.Year_Study_Code==4)?"2CS":"3CS");
        printf("\n  Speciality: 1.SIT  2.SIQ  3.SIL  4.SID\n  Choice: ");
        scanf("%d", &spec_choice);
        if(spec_choice >= 1 && spec_choice <= 4)
            strcpy(student.Speciality, Speciality_List[spec_choice-1]);
        else
            strcpy(student.Speciality, "SIT");
    }

    printf("\n  Resident UC: 1.Yes  0.No\n  Choice: ");
    scanf("%d", &student.Resident_UC);

    student.deleted = 0;
    N_IO_READ = 0;
    N_IO_WRITE = 0;

    OpenFile(&file, MainFileName, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    SearchFreePosition(&file, &free_pos, &block_num, &found);

    if(!found)
    {
        TBlock last_block;
        int last_block_num = GetHeader(&file, 3);

        if(last_block_num == 0)
        {
            block_num = 1;
            SetHeader(&file, 1, 1);
            SetHeader(&file, 3, 1);
        }
        else
        {
            ReadBlock(&file, last_block_num, &last_block);
            last_block.next_block = last_block_num + 1;
            WriteBlock(&file, last_block_num, &last_block);
            block_num = last_block_num + 1;
            SetHeader(&file, 3, block_num);
        }

        AllocateBlock(&buffer);
        buffer.records[0] = student;
        buffer.records[0].deleted = 0;
        buffer.nb_records = 1;
        WriteBlock(&file, block_num, &buffer);
        free_pos = 1;
    }
    else
    {
        ReadBlock(&file, block_num, &buffer);
        buffer.records[free_pos-1] = student;
        buffer.records[free_pos-1].deleted = 0;
        buffer.nb_records++;
        WriteBlock(&file, block_num, &buffer);
    }

    SetHeader(&file, 2, GetHeader(&file, 2) + 1);
    CloseFile(&file);
    InsertIndexSorted(new_id, block_num, free_pos);

    printf("\n  [+] Student inserted successfully!\n");
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C34    : %d\n", N_IO_READ + N_IO_WRITE);
    Pause();
}

void DeleteStudent()
{
    ShowHeader();
    int delete_id, block_num, pos, found;
    TLNOF file;
    TBlock buffer, prev_buffer;
    int i;
    int prev_block_num;

    printf("  DELETE STUDENT (PHYSICAL DELETION)\n");
    PrintLine(80);
    printf("\n  Enter Student ID: ");
    scanf("%d", &delete_id);

    N_IO_READ = 0;
    N_IO_WRITE = 0;

    found = SearchIndex(delete_id, &block_num, &pos);

    if(!found)
    {
        printf("\n  [!] Student not found.\n");
        printf("\n  [COST] C35 = %d\n", N_IO_READ + N_IO_WRITE);
        Pause();
        return;
    }

    OpenFile(&file, MainFileName, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    ReadBlock(&file, block_num, &buffer);

    if(buffer.records[pos-1].deleted == 0 && buffer.records[pos-1].Student_ID == delete_id)
    {
        for(i = pos-1; i < buffer.nb_records - 1; i++)
        {
            buffer.records[i] = buffer.records[i + 1];
        }

        buffer.records[buffer.nb_records - 1].deleted = 1;
        buffer.nb_records--;

        if(buffer.nb_records == 0)
        {
            printf("\n  [*] Block %d is now empty, freeing it...\n", block_num);

            prev_block_num = 0;

            if(block_num == GetHeader(&file, 1))
            {
                SetHeader(&file, 1, buffer.next_block);

                if(buffer.next_block == 0)
                {
                    SetHeader(&file, 1, 0);
                    SetHeader(&file, 3, 0);
                }
            }
            else
            {
                int current = GetHeader(&file, 1);
                while(current != 0 && current != block_num)
                {
                    ReadBlock(&file, current, &prev_buffer);
                    if(prev_buffer.next_block == block_num)
                    {
                        prev_block_num = current;
                        break;
                    }
                    current = prev_buffer.next_block;
                }

                if(prev_block_num != 0)
                {
                    prev_buffer.next_block = buffer.next_block;
                    WriteBlock(&file, prev_block_num, &prev_buffer);
                }
            }

            if(block_num == GetHeader(&file, 3))
            {
                SetHeader(&file, 3, prev_block_num);
            }

            printf("  [+] Block %d freed successfully!\n", block_num);
        }
        else
        {
            WriteBlock(&file, block_num, &buffer);
        }

        SetHeader(&file, 2, GetHeader(&file, 2) - 1);
        CloseFile(&file);

        DeleteFromIndex(delete_id);
        UpdateIndexAfterDeletion(block_num, pos);

        printf("\n  [+] Student deleted successfully!\n");
        printf("\n  [COST BREAKDOWN]\n");
        printf("      Read blocks  : %d\n", N_IO_READ);
        printf("      Write blocks : %d\n", N_IO_WRITE);
        printf("      Total C35    : %d\n", N_IO_READ + N_IO_WRITE);
    }
    else
    {
        CloseFile(&file);
        printf("\n  [!] Student already deleted.\n");
        printf("\n  [COST] C35 = %d\n", N_IO_READ + N_IO_WRITE);
    }
    Pause();
}

void ModifyFirstName()
{
    ShowHeader();
    int modify_id, block_num, pos, found;
    TLNOF file;
    TBlock buffer;
    char new_name[30];

    printf("  MODIFY FIRST NAME\n");
    PrintLine(80);
    printf("\n  Enter Student ID: ");
    scanf("%d", &modify_id);
    printf("  New First Name: ");
    scanf(" %[^\n]", new_name);

    N_IO_READ = 0;
    N_IO_WRITE = 0;

    found = SearchIndex(modify_id, &block_num, &pos);

    if(!found)
    {
        printf("\n  [!] Student not found.\n");
        printf("\n  [COST] C36 = %d\n", N_IO_READ + N_IO_WRITE);
        Pause();
        return;
    }

    OpenFile(&file, MainFileName, 'a');
    ReadBlock(&file, block_num, &buffer);

    if(buffer.records[pos-1].deleted == 0 && buffer.records[pos-1].Student_ID == modify_id)
    {
        strcpy(buffer.records[pos-1].First_Name, new_name);
        WriteBlock(&file, block_num, &buffer);

        printf("\n  [+] First name modified successfully!\n");
    }
    else
    {
        printf("\n  [!] Student deleted. Cannot modify.\n");
    }

    CloseFile(&file);
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C36    : %d\n", N_IO_READ + N_IO_WRITE);
    Pause();
}

/********************** QUERIES *****************************/

void QueryBloodTypeResident()
{
    ShowHeader();
    int blood_code, i, j, count;
    TLNOF file;
    TBlock buffer;
    char blood_type[4];
    char fullname[50];
    int total_blocks;

    printf("  QUERY: BLOOD TYPE & RESIDENT UC\n");
    PrintLine(80);
    printf("\n  Blood: 1.O+  2.A+  3.B+  4.O-  5.A-  6.AB+  7.B-  8.AB-\n  Choice: ");
    scanf("%d", &blood_code);
    MapBloodType(blood_code, blood_type);

    N_IO_READ = 0;
    N_IO_WRITE = 0;
    count = 0;

    OpenFile(&file, MainFileName, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    total_blocks = GetHeader(&file, 3);

    printf("\n  [*] Searching through %d blocks...\n\n", total_blocks);
    PrintLine(130);
    printf("%-6s | %-30s | %-10s | %-16s | %-6s | %-4s | %-22s\n",
           "ID","Name","Birth","Wilaya","Gender","Year","Speciality");
    PrintLine(130);

    for(i = 1; i <= total_blocks; i++)
    {
        ReadBlock(&file, i, &buffer);

        for(j=0; j<buffer.nb_records; j++)
        {
            if(buffer.records[j].deleted == 0 &&
               buffer.records[j].Blood_Type_Code == blood_code &&
               buffer.records[j].Resident_UC == 1)
            {
                sprintf(fullname, "%s %s", buffer.records[j].Family_Name, buffer.records[j].First_Name);
                printf("%-6d | %-30s | %02d/%02d/%d | %-16s | %-6s | %-4s | %-22s\n",
                       buffer.records[j].Student_ID, fullname,
                       buffer.records[j].Day_Birth, buffer.records[j].Month_Birth, buffer.records[j].Year_Birth,
                       buffer.records[j].Wilaya_Name,
                       (buffer.records[j].Gender==2)?"Female":"Male",
                       buffer.records[j].Year_Study,
                       buffer.records[j].Speciality);
                count++;
            }
        }
    }

    CloseFile(&file);

    PrintLine(130);
    printf("  Total: %d students found (Blood Type: %s, Resident: Yes)\n", count, blood_type);
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C41    : %d\n", N_IO_READ + N_IO_WRITE);
    PrintLine(130);
    Pause();
}

void QuerySpeciality()
{
    ShowHeader();
    char speciality_keyword[40];
    int i, j, count;
    TLNOF file;
    TBlock buffer;
    char fullname[50];
    int total_blocks;

    printf("  QUERY: BY SPECIALITY\n");
    PrintLine(80);
    printf("\n  Enter keyword (Common/SIT/SIQ/SIL/SID/Preparatory): ");
    scanf("%s", speciality_keyword);

    N_IO_READ = 0;
    N_IO_WRITE = 0;
    count = 0;

    OpenFile(&file, MainFileName, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    total_blocks = GetHeader(&file, 3);

    printf("\n  [*] Searching through %d blocks...\n\n", total_blocks);
    PrintLine(130);
    printf("%-6s | %-30s | %-10s | %-6s | %-4s | %-4s | %-8s | %-22s\n",
           "ID","Name","Birth","Gender","Blood","Year","Resident","Speciality");
    PrintLine(130);

    for(i = 1; i <= total_blocks; i++)
    {
        ReadBlock(&file, i, &buffer);

        for(j=0; j<buffer.nb_records; j++)
        {
            if(buffer.records[j].deleted == 0 &&
               strstr(buffer.records[j].Speciality, speciality_keyword) != NULL)
            {
                sprintf(fullname, "%s %s", buffer.records[j].Family_Name, buffer.records[j].First_Name);
                printf("%-6d | %-30s | %02d/%02d/%d | %-6s | %-4s | %-4s | %-8s | %-22s\n",
                       buffer.records[j].Student_ID, fullname,
                       buffer.records[j].Day_Birth, buffer.records[j].Month_Birth, buffer.records[j].Year_Birth,
                       (buffer.records[j].Gender==2)?"Female":"Male",
                       buffer.records[j].Blood_Type, buffer.records[j].Year_Study,
                       (buffer.records[j].Resident_UC==1)?"Yes":"No",
                       buffer.records[j].Speciality);
                count++;
            }
        }
    }

    CloseFile(&file);

    PrintLine(130);
    printf("  Total: %d students found (Speciality: %s)\n", count, speciality_keyword);
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C42    : %d\n", N_IO_READ + N_IO_WRITE);
    PrintLine(130);
    Pause();
}

void QueryUnder20YearsOld()
{
    ShowHeader();
    int year1, year2, i, j, count;
    int current_year = 2026;
    TLNOF file;
    TBlock buffer;
    char fullname[50];
    int total_blocks;

    printf("  QUERY: UNDER 20 YEARS OLD\n");
    PrintLine(80);
    printf("\n  Birth year range [Y1 Y2]: ");
    scanf("%d%d", &year1, &year2);

    N_IO_READ = 0;
    N_IO_WRITE = 0;
    count = 0;

    OpenFile(&file, MainFileName, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    total_blocks = GetHeader(&file, 3);

    printf("\n  [*] Searching through %d blocks...\n\n", total_blocks);
    PrintLine(130);
    printf("%-6s | %-30s | %-10s | %-3s | %-6s | %-4s | %-4s | %-16s\n",
           "ID","Name","Birth","Age","Gender","Blood","Year","Wilaya");
    PrintLine(130);

    for(i = 1; i <= total_blocks; i++)
    {
        ReadBlock(&file, i, &buffer);

        for(j=0; j<buffer.nb_records; j++)
        {
            if(buffer.records[j].deleted == 0)
            {
                int age = current_year - buffer.records[j].Year_Birth;
                if(age < 20 && buffer.records[j].Year_Birth >= year1 && buffer.records[j].Year_Birth <= year2)
                {
                    sprintf(fullname, "%s %s", buffer.records[j].Family_Name, buffer.records[j].First_Name);
                    printf("%-6d | %-30s | %02d/%02d/%d | %-3d | %-6s | %-4s | %-4s | %-16s\n",
                           buffer.records[j].Student_ID, fullname,
                           buffer.records[j].Day_Birth, buffer.records[j].Month_Birth, buffer.records[j].Year_Birth,
                           age,
                           (buffer.records[j].Gender==2)?"Female":"Male",
                           buffer.records[j].Blood_Type, buffer.records[j].Year_Study,
                           buffer.records[j].Wilaya_Name);
                    count++;
                }
            }
        }
    }

    CloseFile(&file);

    PrintLine(130);
    printf("  Total: %d students found (Age < 20, Birth years %d-%d)\n", count, year1, year2);
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C43    : %d\n", N_IO_READ + N_IO_WRITE);
    PrintLine(130);
    Pause();
}

void QueryYearOfStudy()
{
    ShowHeader();
    int year_code, i, j, count;
    TLNOF file;
    TBlock buffer;
    char year_study[4], speciality[40];
    char fullname[50];
    int total_blocks;

    printf("  QUERY: BY YEAR OF STUDY\n");
    PrintLine(80);
    printf("\n  Year: 1.1CP  2.2CP  3.1CS  4.2CS  5.3CS\n  Choice: ");
    scanf("%d", &year_code);
    MapYearStudy(year_code, year_study, speciality);

    N_IO_READ = 0;
    N_IO_WRITE = 0;
    count = 0;

    OpenFile(&file, MainFileName, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    total_blocks = GetHeader(&file, 3);

    printf("\n  [*] Searching through %d blocks for %s students...\n\n", total_blocks, year_study);
    PrintLine(130);
    printf("%-6s | %-30s | %-10s | %-6s | %-4s | %-8s | %-22s\n",
           "ID","Name","Birth","Gender","Blood","Resident","Speciality");
    PrintLine(130);

    for(i = 1; i <= total_blocks; i++)
    {
        ReadBlock(&file, i, &buffer);

        for(j=0; j<buffer.nb_records; j++)
        {
            if(buffer.records[j].deleted == 0 && buffer.records[j].Year_Study_Code == year_code)
            {
                sprintf(fullname, "%s %s", buffer.records[j].Family_Name, buffer.records[j].First_Name);
                printf("%-6d | %-30s | %02d/%02d/%d | %-6s | %-4s | %-8s | %-22s\n",
                       buffer.records[j].Student_ID, fullname,
                       buffer.records[j].Day_Birth, buffer.records[j].Month_Birth, buffer.records[j].Year_Birth,
                       (buffer.records[j].Gender==2)?"Female":"Male",
                       buffer.records[j].Blood_Type,
                       (buffer.records[j].Resident_UC==1)?"Yes":"No",
                       buffer.records[j].Speciality);
                count++;
            }
        }
    }

    CloseFile(&file);

    PrintLine(130);
    printf("  Total: %d students found (Year: %s)\n", count, year_study);
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C44    : %d\n", N_IO_READ + N_IO_WRITE);
    PrintLine(130);
    Pause();
}

/********************** ADVANCED OPERATIONS *****************************/

void CreateCPFile()
{
    ShowHeader();
    TLNOF source_file, destination_file;
    TBlock source_buffer, dest_buffer;
    int i, j, dest_block_count, dest_record_count, position;
    float load_factor;

    printf("  CREATE CP FILE (1CP + 2CP)\n");
    PrintLine(80);

    N_IO_READ = 0;
    N_IO_WRITE = 0;

    OpenFile(&source_file, MainFileName, 'a');
    if(source_file.file == NULL)
    {
        free(source_file.header);
        Pause();
        return;
    }

    OpenFile(&destination_file, CPFileName, 'n');
    if(destination_file.file == NULL)
    {
        free(destination_file.header);
        CloseFile(&source_file);
        Pause();
        return;
    }

    AllocateBlock(&dest_buffer);
    dest_block_count = 1;
    dest_record_count = 0;
    position = 1;
    SetHeader(&destination_file, 1, 1);
    SetHeader(&destination_file, 3, 1);

    printf("\n  [*] Extracting 1CP and 2CP students...\n");

    i = GetHeader(&source_file, 1);
    while(i != 0)
    {
        ReadBlock(&source_file, i, &source_buffer);

        for(j=0; j<source_buffer.nb_records; j++)
        {
            if(source_buffer.records[j].deleted == 0 &&
               (source_buffer.records[j].Year_Study_Code == 1 ||
                source_buffer.records[j].Year_Study_Code == 2))
            {
                if(position > MAX)
                {
                    WriteBlock(&destination_file, dest_block_count, &dest_buffer);
                    dest_block_count++;
                    AllocateBlock(&dest_buffer);
                    position = 1;
                }
                dest_buffer.records[position-1] = source_buffer.records[j];
                dest_buffer.nb_records++;
                position++;
                dest_record_count++;
            }
        }
        i = source_buffer.next_block;
    }

    WriteBlock(&destination_file, dest_block_count, &dest_buffer);
    SetHeader(&destination_file, 2, dest_record_count);
    SetHeader(&destination_file, 3, dest_block_count);
    SetHeader(&destination_file, 1, 1);

    CloseFile(&source_file);
    CloseFile(&destination_file);

    load_factor = (float)dest_record_count / (dest_block_count*MAX) * 100.0f;

    printf("\n  [+] CP file created successfully!\n");
    printf("      Records: %d | Blocks: %d (numbered 1-%d) | Load: %.2f%%\n",
           dest_record_count, dest_block_count, dest_block_count, load_factor);
    printf("\n  [COST BREAKDOWN]\n");
    printf("      Read blocks  : %d\n", N_IO_READ);
    printf("      Write blocks : %d\n", N_IO_WRITE);
    printf("      Total C5     : %d\n", N_IO_READ + N_IO_WRITE);
    Pause();
}

/********************** DISPLAY FUNCTIONS *****************************/

void DisplayFileHeader(char filename[30])
{
    ShowHeader();
    TLNOF file;

    OpenFile(&file, filename, 'a');
    if(file.file != NULL)
    {
        printf("  FILE HEADER: %s\n", filename);
        PrintLine(80);
        printf("  First Block     : %d\n", GetHeader(&file, 1));
        printf("  Active Records  : %d\n", GetHeader(&file, 2));
        printf("  Last Block      : %d\n", GetHeader(&file, 3));
        PrintLine(80);
    }
    if(file.header) CloseFile(&file);
    Pause();
}

void DisplaySpecificBlock(char filename[30])
{
    ShowHeader();
    TLNOF file;
    TBlock buffer;
    int block_number, i, count;
    char fullname[50];

    printf("  DISPLAY SPECIFIC BLOCK\n");
    PrintLine(80);
    printf("\n  Block number: ");
    scanf("%d", &block_number);

    OpenFile(&file, filename, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    ReadBlock(&file, block_number, &buffer);

    printf("\n  BLOCK %d (Records: %d, Next: %d)\n", block_number, buffer.nb_records, buffer.next_block);
    PrintLine(80);

    count = 0;
    for(i=1; i<=buffer.nb_records; i++)
    {
        if(buffer.records[i-1].deleted == 0)
        {
            sprintf(fullname, "%s %s", buffer.records[i-1].Family_Name, buffer.records[i-1].First_Name);
            printf("  [%2d] ID:%-6d | %s\n", i, buffer.records[i-1].Student_ID, fullname);
            count++;
        }
    }

    if(count == 0) printf("  (Empty)\n");

    PrintLine(80);
    CloseFile(&file);
    Pause();
}

void DisplayAllRecords(char filename[30])
{
    ShowHeader();
    TLNOF file;
    TBlock buffer;
    int i, j, block_record_count, total_displayed = 0;
    int total_blocks;
    char fullname[50];

    OpenFile(&file, filename, 'a');
    if(file.file == NULL)
    {
        free(file.header);
        Pause();
        return;
    }

    total_blocks = GetHeader(&file, 3);

    printf("  ALL RECORDS (Block by Block)\n");
    printf("  Total Blocks in file: %d\n", total_blocks);
    printf("  Press ENTER after each block to continue...\n\n");

    for(i = 1; i <= total_blocks; i++)
    {
        ReadBlock(&file, i, &buffer);

        PrintLine(140);
        printf("  BLOCK %d/%d | Records: %d | Next: %d\n", i, total_blocks, buffer.nb_records, buffer.next_block);
        PrintLine(140);
        printf("%-6s | %-30s | %-10s | %-16s | %-6s | %-4s | %-4s | %-22s | %-3s\n",
               "ID","Name","Birth","Wilaya","Gender","Blood","Year","Speciality","UC");
        PrintLine(140);

        block_record_count = 0;
        for(j=0; j<buffer.nb_records; j++)
        {
            if(buffer.records[j].deleted == 0)
            {
                TStudent *student = &buffer.records[j];
                sprintf(fullname, "%s %s", student->Family_Name, student->First_Name);

                printf("%-6d | %-30s | %02d/%02d/%d | %-16s | %-6s | %-4s | %-4s | %-22s | %-3s\n",
                       student->Student_ID,
                       fullname,
                       student->Day_Birth, student->Month_Birth, student->Year_Birth,
                       student->Wilaya_Name,
                       (student->Gender==2)?"Female":"Male",
                       student->Blood_Type,
                       student->Year_Study,
                       student->Speciality,
                       (student->Resident_UC==1)?"Yes":"No");
                block_record_count++;
                total_displayed++;
            }
        }

        if(block_record_count == 0)
            printf("  (No active records)\n");

        PrintLine(140);
        printf("  Block %d: %d records | Total displayed so far: %d\n", i, block_record_count, total_displayed);
        PrintLine(140);

        if(i < total_blocks)
        {
            printf("\n  Press ENTER to view next block...");
            char ch;
            while((ch = getchar()) != '\n' && ch != EOF);
            getchar();
            printf("\n");
        }
    }

    printf("\n");
    PrintLine(140);
    printf("  === END OF FILE ===\n");
    printf("  Total Active Records Displayed: %d\n", total_displayed);
    printf("  Total Blocks Read: %d\n", total_blocks);
    PrintLine(140);

    CloseFile(&file);
    Pause();
}

/***************************** MENU *********************************/

void ShowMainMenu()
{
    int choice;
    do
    {
        ShowHeader();
        printf("  +-----------------------------------------------------------------------+\n");
        printf("  |                          MAIN MENU                                    |\n");
        printf("  +-----------------------------------------------------------------------+\n");
        printf("  |  FILE OPERATIONS                                                      |\n");
        printf("  |    1. Create & Initial Load           2. Save Index                   |\n");
        printf("  |    3. Load Index                                                      |\n");
        printf("  +-----------------------------------------------------------------------+\n");
        printf("  |  STUDENT OPERATIONS                                                   |\n");
        printf("  |    4. Search Student                  5. Insert Student               |\n");
        printf("  |    6. Delete Student (Physical)       7. Modify First Name            |\n");
        printf("  +-----------------------------------------------------------------------+\n");
        printf("  |  QUERIES                                                              |\n");
        printf("  |    8. Blood Type & Resident           9. By Speciality                |\n");
        printf("  |   10. Under 20 Years Old             11. By Year of Study             |\n");
        printf("  +-----------------------------------------------------------------------+\n");
        printf("  |  ADVANCED                                                             |\n");
        printf("  |   12. Create CP File (1CP+2CP)                                        |\n");
        printf("  +-----------------------------------------------------------------------+\n");
        printf("  |  DISPLAY                                                              |\n");
        printf("  |   13. File Header                    14. Specific Block               |\n");
        printf("  |   15. All Records (Block by Block)                                    |\n");
        printf("  +-----------------------------------------------------------------------+\n");
        printf("  |    0. Exit                                                            |\n");
        printf("  +-----------------------------------------------------------------------+\n");
        printf("\n  Enter your choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1: CreateAndInitialLoad(); break;
            case 2: ShowHeader(); SaveIndexToFile(); Pause(); break;
            case 3: ShowHeader(); LoadIndexFromFile(); Pause(); break;
            case 4: SearchStudent(); break;
            case 5: InsertStudent(); break;
            case 6: DeleteStudent(); break;
            case 7: ModifyFirstName(); break;
            case 8: QueryBloodTypeResident(); break;
            case 9: QuerySpeciality(); break;
            case 10: QueryUnder20YearsOld(); break;
            case 11: QueryYearOfStudy(); break;
            case 12: CreateCPFile(); break;
            case 13: DisplayFileHeader(MainFileName); break;
            case 14: DisplaySpecificBlock(MainFileName); break;
            case 15: DisplayAllRecords(MainFileName); break;
            case 0:
                ShowHeader();
                printf("  Thank you for using ESI Students Management System!\n");
                printf("  Lab FSDS - SOLTANI Okba & ZERARI Tarek\n\n");
                break;
            default:
                ShowHeader();
                printf("  [!] Invalid choice. Try again.\n");
                Pause();
                break;
        }
    } while(choice != 0);
}

/********************************* MAIN ***********************************/

int main()
{
    srand(time(NULL));
    InitIndex();

    ClearScreen();
    PrintLine(90);
    printf("\n");
    printf("         ======================================================================\n");
    printf("         ||                                                                  ||\n");
    printf("         ||          ESI STUDENTS MANAGEMENT SYSTEM - LNOF                  ||\n");
    printf("         ||                                                                  ||\n");
    printf("         ||          Linked Non-Ordered File Implementation                 ||\n");
    printf("         ||                                                                  ||\n");
    printf("         ======================================================================\n");
    printf("\n");
    PrintLine(90);
    printf("\n  Lab: File Structures and Data Systems (FSDS)\n");
    printf("  Presented by: SOLTANI Okba & ZERARI Tarek\n");
    printf("  Year: 2025-2026\n\n");
    PrintLine(90);
    printf("\n  Enter names filename [default: NAMES.txt]: ");

    char temp_filename[64];
    if(scanf("%63s", temp_filename) == 1 && strlen(temp_filename) > 0 && strcmp(temp_filename, "default") != 0)
        strcpy(NamesFileName, temp_filename);

    printf("\n");
    LoadNames();
    printf("\n");
    Pause();

    ShowMainMenu();
    return 0;
}
