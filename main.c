#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 40
#define MAX_INDEX 5000
#define MAX_NAMES 5000

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

typedef TStudent tab[MAX];

typedef struct
{
    tab t;
    int NbArticle;
    int suiv;
} TBloc;

typedef struct
{
    int num_Btete;
    int nb_enreg;
    int nb_bloc;
} TEn_Tete;

typedef struct
{
    FILE *f;
    TEn_Tete *en_tete;
} LNOF;

typedef struct
{
    int key;
    int num_bloc;
    int pos;
} IndexEntry;

typedef struct
{
    IndexEntry entries[MAX_INDEX];
    int n;
} IndexTable;

typedef struct
{
    char family[30];
    char first[60];
    int gender;
} NameEntry;

/********************* GLOBALS *****************************/

IndexTable IndexMem;
int N_IO = 0;

char MainFileName[]  = "STUDENTS_ESI.BIN";
char IndexFileName[] = "StudentID_INDEX.idx";
char CPFileName[]    = "STUDENTS_CP.BIN";
char NamesFileName[64] = "NAMES.txt";

NameEntry NamesArray[MAX_NAMES];
int NamesCount = 0;

/********************** WILAYA LIST *****************************/

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

char *Speciality_2CS_3CS[4] = {"SIT","SIQ","SIL","SID"};

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
    for(i=0;i<length;i++) printf("=");
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

int isLeap(int y){return ((y%4==0 && y%100!=0) || (y%400==0));}

int DaysInMonth(int m,int y)
{
    int d[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
    if(m==2 && isLeap(y)) return 29;
    return d[m];
}

void MapBloodType(int code,char bt[4])
{
    switch(code)
    {
        case 1:strcpy(bt,"O+");break;
        case 2:strcpy(bt,"A+");break;
        case 3:strcpy(bt,"B+");break;
        case 4:strcpy(bt,"O-");break;
        case 5:strcpy(bt,"A-");break;
        case 6:strcpy(bt,"AB+");break;
        case 7:strcpy(bt,"B-");break;
        case 8:strcpy(bt,"AB-");break;
        default:strcpy(bt,"");break;
    }
}

void MapYearStudy(int code,char ys[4],char spec[40])
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
            strcpy(spec,Speciality_2CS_3CS[spec_code]);
            break;
        case 5:
            strcpy(ys,"3CS");
            spec_code = rand()%4;
            strcpy(spec,Speciality_2CS_3CS[spec_code]);
            break;
        default:
            strcpy(ys,"");
            strcpy(spec,"");
            break;
    }
}

void PrintWilayaList()
{
    int i;
    printf("\n  ALGERIAN WILAYAS:\n");
    PrintLine(70);
    for(i=1;i<=29;i++)
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

void ouvrir (LNOF *fichier,char nom_fich[30],const char mode)
{
    fichier->en_tete = (TEn_Tete*)malloc(sizeof(TEn_Tete));
    if ((mode=='n')||(mode=='N'))
    {
        fichier->f=fopen(nom_fich,"wb+");
        if(!fichier->f)
        {
            free(fichier->en_tete);
            fichier->en_tete=NULL;
            return;
        }
        fichier->en_tete->num_Btete=-1;
        fichier->en_tete->nb_enreg=0;
        fichier->en_tete->nb_bloc=-1;
        fwrite(fichier->en_tete,sizeof(TEn_Tete),1,fichier->f);
    }
    else if ((mode=='a')||(mode=='A'))
    {
        fichier->f=fopen(nom_fich,"rb+");
        if (fichier->f==NULL)
        {
            printf("  [!] File %s does not exist.\n",nom_fich);
        }
        else
        {
            fread(fichier->en_tete,sizeof(TEn_Tete),1,fichier->f);
        }
    }
}

void fermer (LNOF *fichier)
{
    if (fichier->f!=NULL)
    {
        rewind(fichier->f);
        fwrite(fichier->en_tete,sizeof(TEn_Tete),1,fichier->f);
        fclose(fichier->f);
    }
    free(fichier->en_tete);
}

void ecrire_dire(LNOF *fichier,int i,TBloc *buf)
{
    rewind(fichier->f);
    fseek(fichier->f,sizeof(TEn_Tete)+i*sizeof(TBloc),SEEK_SET);
    fwrite(buf,sizeof(TBloc),1,fichier->f);
    N_IO++;
}

void lire_dire(LNOF *fichier,int i,TBloc *buf)
{
    rewind(fichier->f);
    fseek(fichier->f,sizeof(TEn_Tete)+i*sizeof(TBloc),SEEK_SET);
    fread(buf,sizeof(TBloc),1,fichier->f);
    N_IO++;
}

void af_entete(LNOF *fichier,int num_caract,int val)
{
    switch(num_caract)
    {
        case 1:fichier->en_tete->num_Btete=val; break;
        case 2:fichier->en_tete->nb_enreg=val; break;
        case 3:fichier->en_tete->nb_bloc=val; break;
    }
}

int en_tete(LNOF *fichier,int num_caract )
{
    int val=-1;
    switch(num_caract)
    {
        case 1:val=fichier->en_tete->num_Btete; break;
        case 2:val=fichier->en_tete->nb_enreg; break;
        case 3:val=fichier->en_tete->nb_bloc; break;
    }
    return val;
}

void alloc_bloc(TBloc *buf)
{
    int i;
    buf->suiv=-1;
    buf->NbArticle=0;
    for(i=0;i<MAX;i++)
        buf->t[i].deleted=1;
}

/***************** INDEX OPERATIONS ***************************/

void InitIndex()
{
    IndexMem.n = 0;
}

void InsertIndexSorted(int key,int num_bloc,int pos)
{
    int i=IndexMem.n-1;
    if (IndexMem.n>=MAX_INDEX) return;
    while((i>=0)&&(IndexMem.entries[i].key>key))
    {
        IndexMem.entries[i+1]=IndexMem.entries[i];
        i--;
    }
    IndexMem.entries[i+1].key=key;
    IndexMem.entries[i+1].num_bloc=num_bloc;
    IndexMem.entries[i+1].pos=pos;
    IndexMem.n++;
}

int SearchIndex(int key,int *num_bloc,int *pos)
{
    int inf=0,sup=IndexMem.n-1,m;
    while(inf<=sup)
    {
        m=(inf+sup)/2;
        if(IndexMem.entries[m].key==key)
        {
            *num_bloc=IndexMem.entries[m].num_bloc;
            *pos=IndexMem.entries[m].pos;
            return 1;
        }
        else if(IndexMem.entries[m].key<key) inf=m+1;
        else sup=m-1;
    }
    return 0;
}

void DeleteFromIndex(int key)
{
    int i,j;
    for(i=0;i<IndexMem.n;i++)
    {
        if(IndexMem.entries[i].key==key)
        {
            for(j=i;j<IndexMem.n-1;j++)
                IndexMem.entries[j]=IndexMem.entries[j+1];
            IndexMem.n--;
            return;
        }
    }
}

void SaveIndexToFile(int *C31)
{
    FILE *idx;
    *C31=0;
    idx=fopen(IndexFileName,"wb");
    if(idx==NULL)
    {
        printf("  [!] Error saving index.\n");
        return;
    }
    fwrite(&IndexMem.n,sizeof(int),1,idx); (*C31)++;
    fwrite(IndexMem.entries,sizeof(IndexEntry),IndexMem.n,idx); (*C31)++;
    fclose(idx);
    printf("  [+] Index saved successfully (I/O: %d)\n",*C31);
}

void LoadIndexFromFile(int *C32)
{
    FILE *idx;
    *C32=0;
    idx=fopen(IndexFileName,"rb");
    if(idx==NULL)
    {
        printf("  [!] Index file not found. Empty index initialized.\n");
        InitIndex();
        return;
    }
    fread(&IndexMem.n,sizeof(int),1,idx); (*C32)++;
    fread(IndexMem.entries,sizeof(IndexEntry),IndexMem.n,idx); (*C32)++;
    fclose(idx);
    printf("  [+] Index loaded successfully (I/O: %d)\n",*C32);
}

void rech_pos_lib(LNOF *fichier,int *pos_lib,int *num_bloc,int *trouv)
{
    int i,j;
    TBloc buf;
    *trouv=0;
    i=en_tete(fichier,1);
    if(i==-1) return;
    while(i!=-1 && *trouv==0)
    {
        lire_dire(fichier,i,&buf);
        if(buf.NbArticle<MAX)
        {
            for(j=0;j<MAX && *trouv==0;j++)
            {
                if(buf.t[j].deleted==1)
                {
                    *trouv=1;
                    *num_bloc=i;
                    *pos_lib=j;
                }
            }
        }
        i=buf.suiv;
    }
}

/********************** RANDOM STUDENT *****************************/

int UsedIDs[9001];

int GenerateUniqueID()
{
    int id;
    do {
        id = 1000 + rand()%8001;
    } while(UsedIDs[id]);
    UsedIDs[id] = 1;
    return id;
}

void GenerateRandomStudentFromNames(TStudent *S)
{
    int idx = rand()%NamesCount;

    S->Student_ID = GenerateUniqueID();
    strncpy(S->Family_Name, NamesArray[idx].family, 29);
    S->Family_Name[29] = '\0';
    strncpy(S->First_Name, NamesArray[idx].first, 29);
    S->First_Name[29] = '\0';
    S->Gender = NamesArray[idx].gender;

    S->Year_Birth = 2003 + rand()%6;
    S->Month_Birth = 1 + rand()%12;
    S->Day_Birth = 1 + rand()%DaysInMonth(S->Month_Birth,S->Year_Birth);

    S->Wilaya_Code = 1 + rand()%58;
    strcpy(S->Wilaya_Name,Wilaya_List[S->Wilaya_Code]);

    S->Blood_Type_Code = 1 + rand()%8;
    MapBloodType(S->Blood_Type_Code,S->Blood_Type);

    S->Year_Study_Code = 1 + rand()%5;
    MapYearStudy(S->Year_Study_Code,S->Year_Study,S->Speciality);

    S->Resident_UC = rand()%2;
    S->deleted = 0;
}

void DisplayStudent(TStudent *S)
{
    printf("\n");
    PrintLine(80);
    printf("  STUDENT DETAILS\n");
    PrintLine(80);
    printf("  ID              : %d\n", S->Student_ID);
    printf("  Name            : %s %s\n", S->First_Name, S->Family_Name);
    printf("  Gender          : %s\n", (S->Gender==2)?"Female":"Male");
    printf("  Birth Date      : %02d/%02d/%d\n", S->Day_Birth, S->Month_Birth, S->Year_Birth);
    printf("  Wilaya          : %s (%d)\n", S->Wilaya_Name, S->Wilaya_Code);
    printf("  Blood Type      : %s\n", S->Blood_Type);
    printf("  Year of Study   : %s\n", S->Year_Study);
    printf("  Speciality      : %s\n", S->Speciality);
    printf("  Resident UC     : %s\n", (S->Resident_UC==1)?"Yes":"No");
    PrintLine(80);
}

/********************** MAIN OPERATIONS *****************************/

void Create_Initial_Load()
{
    ShowHeader();
    LNOF fichier;
    TBloc buf;
    TStudent S;
    int N,i;
    int nb_bloc,nb_enreg;
    int C2;

    if(NamesCount==0)
    {
        printf("  [!] No names loaded. Please check %s\n",NamesFileName);
        Pause();
        return;
    }

    printf("  CREATE AND INITIAL LOAD\n");
    PrintLine(80);
    printf("\n  Number of records to create: ");
    scanf("%d",&N);

    printf("\n  [*] Creating file with %d records...\n",N);

    memset(UsedIDs,0,sizeof(UsedIDs));
    N_IO=0;
    InitIndex();

    ouvrir(&fichier,MainFileName,'n');
    if(fichier.f==NULL)
    {
        printf("  [!] Error creating file.\n");
        Pause();
        return;
    }

    alloc_bloc(&buf);
    nb_bloc=0;
    nb_enreg=0;
    af_entete(&fichier,1,0);
    af_entete(&fichier,3,0);

    for(i=0;i<N;i++)
    {
        if(buf.NbArticle==MAX)
        {
            ecrire_dire(&fichier,nb_bloc,&buf);
            nb_bloc++;
            alloc_bloc(&buf);
            buf.suiv=-1;
        }

        GenerateRandomStudentFromNames(&S);
        buf.t[buf.NbArticle]=S;
        InsertIndexSorted(S.Student_ID,nb_bloc,buf.NbArticle);
        buf.NbArticle++;
        nb_enreg++;

        if((i+1)%500==0) printf("  Progress: %d/%d records\n",i+1,N);
    }

    ecrire_dire(&fichier,nb_bloc,&buf);
    af_entete(&fichier,2,nb_enreg);
    af_entete(&fichier,3,nb_bloc);
    af_entete(&fichier,1,0);

    fermer(&fichier);
    C2=N_IO;

    printf("\n");
    PrintLine(80);
    printf("  [+] File created successfully!\n");
    printf("      Records: %d | Blocks: %d | I/O Cost: %d\n", nb_enreg, nb_bloc+1, C2);
    PrintLine(80);
    Pause();
}

void Search_Student()
{
    ShowHeader();
    int cle,num_bloc,pos,trouv;
    LNOF fichier;
    TBloc buf;
    int C33;

    printf("  SEARCH STUDENT BY ID\n");
    PrintLine(80);
    printf("\n  Enter Student ID: ");
    scanf("%d",&cle);

    N_IO=0;
    trouv=SearchIndex(cle,&num_bloc,&pos);
    if(!trouv)
    {
        printf("\n  [!] Student with ID %d not found.\n",cle);
        printf("  I/O Cost: %d\n",N_IO);
        Pause();
        return;
    }

    ouvrir(&fichier,MainFileName,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }
    lire_dire(&fichier,num_bloc,&buf);
    fermer(&fichier);

    C33=N_IO;

    if(buf.t[pos].deleted==0 && buf.t[pos].Student_ID==cle)
    {
        DisplayStudent(&buf.t[pos]);
        printf("  Storage: Block %d, Position %d | I/O Cost: %d\n",num_bloc,pos,C33);
    }
    else
    {
        printf("\n  [!] Student record deleted.\n");
        printf("  I/O Cost: %d\n",C33);
    }
    Pause();
}

void Insert_Student()
{
    ShowHeader();
    LNOF fichier;
    TBloc buf;
    TStudent S;
    int cle,i,j,trouv,pos_lib,num_bloc;
    int C34;
    int spec_choice;

    printf("  INSERT NEW STUDENT\n");
    PrintLine(80);

    memset(UsedIDs,0,sizeof(UsedIDs));
    do {
        cle = 1000 + rand()%8001;
    } while(SearchIndex(cle,&i,&j));

    S.Student_ID = cle;
    printf("\n  Generated ID: %d\n\n", cle);

    printf("  Family Name: ");
    scanf("%s",S.Family_Name);
    printf("  First Name: ");
    scanf(" %[^\n]",S.First_Name);

    printf("  Birth Year (2003-2008): ");
    scanf("%d",&S.Year_Birth);
    printf("  Birth Month (1-12): ");
    scanf("%d",&S.Month_Birth);
    printf("  Birth Day: ");
    scanf("%d",&S.Day_Birth);

    PrintWilayaList();
    printf("  Wilaya (1-58): ");
    scanf("%d",&S.Wilaya_Code);
    if(S.Wilaya_Code<1 || S.Wilaya_Code>58) S.Wilaya_Code=16;
    strcpy(S.Wilaya_Name,Wilaya_List[S.Wilaya_Code]);

    printf("\n  Gender: 1.Male  2.Female\n  Choice: ");
    scanf("%d",&S.Gender);

    printf("\n  Blood Type: 1.O+  2.A+  3.B+  4.O-  5.A-  6.AB+  7.B-  8.AB-\n  Choice: ");
    scanf("%d",&S.Blood_Type_Code);
    MapBloodType(S.Blood_Type_Code,S.Blood_Type);

    printf("\n  Year: 1.1CP  2.2CP  3.1CS  4.2CS  5.3CS\n  Choice: ");
    scanf("%d",&S.Year_Study_Code);

    if(S.Year_Study_Code==1 || S.Year_Study_Code==2)
    {
        strcpy(S.Year_Study, (S.Year_Study_Code==1)?"1CP":"2CP");
        strcpy(S.Speciality,"Integrated Preparatory Classes");
    }
    else if(S.Year_Study_Code==3)
    {
        strcpy(S.Year_Study,"1CS");
        strcpy(S.Speciality,"Common Core");
    }
    else if(S.Year_Study_Code==4 || S.Year_Study_Code==5)
    {
        strcpy(S.Year_Study, (S.Year_Study_Code==4)?"2CS":"3CS");
        printf("\n  Speciality: 1.SIT  2.SIQ  3.SIL  4.SID\n  Choice: ");
        scanf("%d",&spec_choice);
        if(spec_choice>=1 && spec_choice<=4)
            strcpy(S.Speciality,Speciality_2CS_3CS[spec_choice-1]);
        else
            strcpy(S.Speciality,"SIT");
    }

    printf("\n  Resident UC: 1.Yes  0.No\n  Choice: ");
    scanf("%d",&S.Resident_UC);

    S.deleted=0;
    N_IO=0;

    ouvrir(&fichier,MainFileName,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }

    rech_pos_lib(&fichier,&pos_lib,&num_bloc,&trouv);
    if(!trouv)
    {
        TBloc last;
        int nb_bloc=en_tete(&fichier,3);
        if(nb_bloc==-1)
        {
            num_bloc=0;
            af_entete(&fichier,1,0);
            af_entete(&fichier,3,0);
        }
        else
        {
            lire_dire(&fichier,nb_bloc,&last);
            last.suiv=nb_bloc+1;
            ecrire_dire(&fichier,nb_bloc,&last);
            num_bloc=nb_bloc+1;
            af_entete(&fichier,3,num_bloc);
        }
        alloc_bloc(&buf);
        buf.t[0]=S;
        buf.t[0].deleted=0;
        buf.NbArticle=1;
        ecrire_dire(&fichier,num_bloc,&buf);
        pos_lib=0;
    }
    else
    {
        lire_dire(&fichier,num_bloc,&buf);
        buf.t[pos_lib]=S;
        buf.t[pos_lib].deleted=0;
        buf.NbArticle++;
        ecrire_dire(&fichier,num_bloc,&buf);
    }

    af_entete(&fichier,2,en_tete(&fichier,2)+1);
    fermer(&fichier);
    InsertIndexSorted(cle,num_bloc,pos_lib);
    C34=N_IO;

    printf("\n  [+] Student inserted successfully! I/O Cost: %d\n",C34);
    Pause();
}

void Delete_Student()
{
    ShowHeader();
    int cle,num_bloc,pos,trouv;
    LNOF fichier;
    TBloc buf;
    int C35;

    printf("  DELETE STUDENT\n");
    PrintLine(80);
    printf("\n  Enter Student ID: ");
    scanf("%d",&cle);

    N_IO=0;
    trouv=SearchIndex(cle,&num_bloc,&pos);
    if(!trouv)
    {
        printf("\n  [!] Student not found.\n");
        printf("  I/O Cost: %d\n",N_IO);
        Pause();
        return;
    }

    ouvrir(&fichier,MainFileName,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }

    lire_dire(&fichier,num_bloc,&buf);
    if(buf.t[pos].deleted==0 && buf.t[pos].Student_ID==cle)
    {
        buf.t[pos].deleted=1;
        buf.NbArticle--;
        ecrire_dire(&fichier,num_bloc,&buf);
        af_entete(&fichier,2,en_tete(&fichier,2)-1);
        fermer(&fichier);
        DeleteFromIndex(cle);
        C35=N_IO;

        printf("\n  [+] Student deleted successfully! I/O Cost: %d\n",C35);
    }
    else
    {
        fermer(&fichier);
        printf("\n  [!] Student already deleted.\n");
        printf("  I/O Cost: %d\n",N_IO);
    }
    Pause();
}

void Modify_FirstName()
{
    ShowHeader();
    int cle,num_bloc,pos,trouv;
    LNOF fichier;
    TBloc buf;
    int C36;
    char newname[30];

    printf("  MODIFY FIRST NAME\n");
    PrintLine(80);
    printf("\n  Enter Student ID: ");
    scanf("%d",&cle);
    printf("  New First Name: ");
    scanf(" %[^\n]",newname);

    N_IO=0;
    trouv=SearchIndex(cle,&num_bloc,&pos);
    if(!trouv)
    {
        printf("\n  [!] Student not found.\n");
        printf("  I/O Cost: %d\n",N_IO);
        Pause();
        return;
    }

    ouvrir(&fichier,MainFileName,'a');
    lire_dire(&fichier,num_bloc,&buf);
    if(buf.t[pos].deleted==0 && buf.t[pos].Student_ID==cle)
    {
        strcpy(buf.t[pos].First_Name,newname);
        ecrire_dire(&fichier,num_bloc,&buf);

        printf("\n  [+] First name modified successfully!\n");
        printf("  I/O Cost: %d\n",N_IO);
    }
    else
    {
        printf("\n  [!] Student deleted. Cannot modify.\n");
    }
    fermer(&fichier);
    C36=N_IO;
    Pause();
}

/********************** QUERIES *****************************/

void Query_BloodType_Resident()
{
    ShowHeader();
    int code,i,j,C41,count;
    LNOF fichier;
    TBloc buf;
    char bt[4];
    char fullname[90];

    printf("  QUERY: BLOOD TYPE & RESIDENT UC\n");
    PrintLine(80);
    printf("\n  Blood: 1.O+  2.A+  3.B+  4.O-  5.A-  6.AB+  7.B-  8.AB-\n  Choice: ");
    scanf("%d",&code);
    MapBloodType(code,bt);

    N_IO=0;
    count=0;
    ouvrir(&fichier,MainFileName,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }

    printf("\n");
    PrintLine(130);
    printf("%-8s | %-40s | %-10s | %-20s | %-6s | %-4s | %-20s\n",
           "ID","Full Name","Birth","Wilaya","Gender","Year","Speciality");
    PrintLine(130);

    i=en_tete(&fichier,1);
    while(i!=-1)
    {
        lire_dire(&fichier,i,&buf);
        for(j=0;j<MAX;j++)
        {
            if(buf.t[j].deleted==0 &&
               buf.t[j].Blood_Type_Code==code &&
               buf.t[j].Resident_UC==1)
            {
                sprintf(fullname,"%s %s",buf.t[j].Family_Name,buf.t[j].First_Name);
                printf("%-8d | %-40s | %02d/%02d/%d | %-20s | %-6s | %-4s | %-20s\n",
                       buf.t[j].Student_ID,fullname,
                       buf.t[j].Day_Birth,buf.t[j].Month_Birth,buf.t[j].Year_Birth,
                       buf.t[j].Wilaya_Name,
                       (buf.t[j].Gender==2)?"Female":"Male",
                       buf.t[j].Year_Study,
                       buf.t[j].Speciality);
                count++;
            }
        }
        i=buf.suiv;
    }
    fermer(&fichier);

    PrintLine(130);
    printf("  Total: %d students | I/O Cost: %d\n",count,N_IO);
    PrintLine(130);
    Pause();
}

void Query_Speciality()
{
    ShowHeader();
    char spec[40];
    int i,j,C42,count;
    LNOF fichier;
    TBloc buf;
    char fullname[90];

    printf("  QUERY: BY SPECIALITY\n");
    PrintLine(80);
    printf("\n  Enter keyword (Common/SIT/SIQ/SIL/SID/Preparatory): ");
    scanf("%s",spec);

    N_IO=0;
    count=0;
    ouvrir(&fichier,MainFileName,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }

    printf("\n");
    PrintLine(130);
    printf("%-8s | %-40s | %-10s | %-6s | %-4s | %-4s | %-8s | %-30s\n",
           "ID","Full Name","Birth","Gender","Blood","Year","Resident","Speciality");
    PrintLine(130);

    i=en_tete(&fichier,1);
    while(i!=-1)
    {
        lire_dire(&fichier,i,&buf);
        for(j=0;j<MAX;j++)
        {
            if(buf.t[j].deleted==0 && strstr(buf.t[j].Speciality,spec)!=NULL)
            {
                sprintf(fullname,"%s %s",buf.t[j].Family_Name,buf.t[j].First_Name);
                printf("%-8d | %-40s | %02d/%02d/%d | %-6s | %-4s | %-4s | %-8s | %-30s\n",
                       buf.t[j].Student_ID,fullname,
                       buf.t[j].Day_Birth,buf.t[j].Month_Birth,buf.t[j].Year_Birth,
                       (buf.t[j].Gender==2)?"Female":"Male",
                       buf.t[j].Blood_Type,buf.t[j].Year_Study,
                       (buf.t[j].Resident_UC==1)?"Yes":"No",
                       buf.t[j].Speciality);
                count++;
            }
        }
        i=buf.suiv;
    }
    fermer(&fichier);

    PrintLine(130);
    printf("  Total: %d students | I/O Cost: %d\n",count,N_IO);
    PrintLine(130);
    Pause();
}

void Query_Under20_Interval()
{
    ShowHeader();
    int Y1,Y2,i,j,C43,count;
    int currentYear=2025;
    LNOF fichier;
    TBloc buf;
    char fullname[90];

    printf("  QUERY: UNDER 20 YEARS OLD\n");
    PrintLine(80);
    printf("\n  Birth year range [Y1 Y2]: ");
    scanf("%d%d",&Y1,&Y2);

    N_IO=0;
    count=0;
    ouvrir(&fichier,MainFileName,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }

    printf("\n");
    PrintLine(130);
    printf("%-8s | %-40s | %-10s | %-3s | %-6s | %-4s | %-4s | %-20s\n",
           "ID","Full Name","Birth","Age","Gender","Blood","Year","Wilaya");
    PrintLine(130);

    i=en_tete(&fichier,1);
    while(i!=-1)
    {
        lire_dire(&fichier,i,&buf);
        for(j=0;j<MAX;j++)
        {
            if(buf.t[j].deleted==0)
            {
                int age=currentYear-buf.t[j].Year_Birth;
                if(age<20 && buf.t[j].Year_Birth>=Y1 && buf.t[j].Year_Birth<=Y2)
                {
                    sprintf(fullname,"%s %s",buf.t[j].Family_Name,buf.t[j].First_Name);
                    printf("%-8d | %-40s | %02d/%02d/%d | %-3d | %-6s | %-4s | %-4s | %-20s\n",
                           buf.t[j].Student_ID,fullname,
                           buf.t[j].Day_Birth,buf.t[j].Month_Birth,buf.t[j].Year_Birth,
                           age,
                           (buf.t[j].Gender==2)?"Female":"Male",
                           buf.t[j].Blood_Type,buf.t[j].Year_Study,
                           buf.t[j].Wilaya_Name);
                    count++;
                }
            }
        }
        i=buf.suiv;
    }
    fermer(&fichier);

    PrintLine(130);
    printf("  Total: %d students | I/O Cost: %d\n",count,N_IO);
    PrintLine(130);
    Pause();
}

void Query_YearStudy()
{
    ShowHeader();
    int code,i,j,C44,count;
    LNOF fichier;
    TBloc buf;
    char ys[4],spec[40];
    char fullname[90];

    printf("  QUERY: BY YEAR OF STUDY\n");
    PrintLine(80);
    printf("\n  Year: 1.1CP  2.2CP  3.1CS  4.2CS  5.3CS\n  Choice: ");
    scanf("%d",&code);
    MapYearStudy(code,ys,spec);

    N_IO=0;
    count=0;
    ouvrir(&fichier,MainFileName,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }

    printf("\n");
    PrintLine(130);
    printf("%-8s | %-40s | %-10s | %-6s | %-4s | %-8s | %-30s\n",
           "ID","Full Name","Birth","Gender","Blood","Resident","Speciality");
    PrintLine(130);

    i=en_tete(&fichier,1);
    while(i!=-1)
    {
        lire_dire(&fichier,i,&buf);
        for(j=0;j<MAX;j++)
        {
            if(buf.t[j].deleted==0 && buf.t[j].Year_Study_Code==code)
            {
                sprintf(fullname,"%s %s",buf.t[j].Family_Name,buf.t[j].First_Name);
                printf("%-8d | %-40s | %02d/%02d/%d | %-6s | %-4s | %-8s | %-30s\n",
                       buf.t[j].Student_ID,fullname,
                       buf.t[j].Day_Birth,buf.t[j].Month_Birth,buf.t[j].Year_Birth,
                       (buf.t[j].Gender==2)?"Female":"Male",
                       buf.t[j].Blood_Type,
                       (buf.t[j].Resident_UC==1)?"Yes":"No",
                       buf.t[j].Speciality);
                count++;
            }
        }
        i=buf.suiv;
    }
    fermer(&fichier);

    PrintLine(130);
    printf("  Total: %d students | I/O Cost: %d\n",count,N_IO);
    PrintLine(130);
    Pause();
}

void Create_CP_File()
{
    ShowHeader();
    LNOF src,dst;
    TBloc buf_src,buf_dst;
    int i,j,nb_bloc_dst=0,nb_enreg_dst=0;
    float load_factor;
    int C5;

    printf("  CREATE CP FILE (1CP + 2CP)\n");
    PrintLine(80);

    N_IO=0;
    ouvrir(&src,MainFileName,'a');
    if(src.f==NULL)
    {
        free(src.en_tete);
        Pause();
        return;
    }
    ouvrir(&dst,CPFileName,'n');
    if(dst.f==NULL)
    {
        free(dst.en_tete);
        fermer(&src);
        Pause();
        return;
    }

    alloc_bloc(&buf_dst);
    nb_bloc_dst=0;
    af_entete(&dst,1,0);
    af_entete(&dst,3,0);

    printf("\n  [*] Extracting 1CP and 2CP students...\n");

    i=en_tete(&src,1);
    while(i!=-1)
    {
        lire_dire(&src,i,&buf_src);
        for(j=0;j<MAX;j++)
        {
            if(buf_src.t[j].deleted==0 &&
               (buf_src.t[j].Year_Study_Code==1 || buf_src.t[j].Year_Study_Code==2))
            {
                if(buf_dst.NbArticle==MAX)
                {
                    ecrire_dire(&dst,nb_bloc_dst,&buf_dst);
                    nb_bloc_dst++;
                    alloc_bloc(&buf_dst);
                }
                buf_dst.t[buf_dst.NbArticle]=buf_src.t[j];
                buf_dst.NbArticle++;
                nb_enreg_dst++;
            }
        }
        i=buf_src.suiv;
    }

    ecrire_dire(&dst,nb_bloc_dst,&buf_dst);
    af_entete(&dst,2,nb_enreg_dst);
    af_entete(&dst,3,nb_bloc_dst);
    af_entete(&dst,1,0);

    fermer(&src);
    fermer(&dst);

    C5=N_IO;
    load_factor = (float)nb_enreg_dst / ((nb_bloc_dst+1)*MAX) * 100.0f;

    printf("\n  [+] CP file created successfully!\n");
    printf("      Records: %d | Blocks: %d | Load: %.2f%% | I/O: %d\n",
           nb_enreg_dst, nb_bloc_dst+1, load_factor, C5);
    Pause();
}

/********************** DISPLAY *****************************/

void affichage_entete(char nom_fich[30])
{
    ShowHeader();
    LNOF fichier;
    ouvrir(&fichier,nom_fich,'a');
    if(fichier.f!=NULL)
    {
        printf("  FILE HEADER: %s\n",nom_fich);
        PrintLine(80);
        printf("  First Block     : %d\n",en_tete(&fichier,1));
        printf("  Active Records  : %d\n",en_tete(&fichier,2));
        printf("  Last Block      : %d\n",en_tete(&fichier,3));
        PrintLine(80);
    }
    if(fichier.en_tete) fermer(&fichier);
    Pause();
}

void affichage_un_bloc(char nom_fich[30])
{
    ShowHeader();
    LNOF fichier;
    TBloc buf;
    int num,i,count;
    char fullname[90];

    printf("  DISPLAY SPECIFIC BLOCK\n");
    PrintLine(80);
    printf("\n  Block number: ");
    scanf("%d",&num);

    ouvrir(&fichier,nom_fich,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }
    lire_dire(&fichier,num,&buf);

    printf("\n  BLOCK %d (Records: %d, Next: %d)\n",num,buf.NbArticle,buf.suiv);
    PrintLine(80);

    count=0;
    for(i=0;i<MAX;i++)
    {
        if(buf.t[i].deleted==0)
        {
            sprintf(fullname,"%s %s",buf.t[i].Family_Name,buf.t[i].First_Name);
            printf("  [%2d] ID:%-6d | %s\n", i, buf.t[i].Student_ID, fullname);
            count++;
        }
    }

    if(count==0) printf("  (Empty)\n");

    PrintLine(80);
    fermer(&fichier);
    Pause();
}

void affichage_fichier(char nom_fich[30])
{
    ShowHeader();
    LNOF fichier;
    TBloc buf;
    int i,j,count_in_block, total_displayed = 0;
    int total_blocks;
    char input[10];
    char fullname[90];

    ouvrir(&fichier,nom_fich,'a');
    if(fichier.f==NULL)
    {
        free(fichier.en_tete);
        Pause();
        return;
    }

    total_blocks = en_tete(&fichier,3) + 1;
    i=0;

    printf("  ALL RECORDS (Block by Block)\n");
    PrintLine(130);

    while(i <= en_tete(&fichier,3))
    {
        lire_dire(&fichier,i,&buf);

        printf("\n  BLOCK %d (Next: %d)\n",i,buf.suiv);
        PrintLine(130);
        printf("%-8s | %-40s | %-10s | %-20s | %-6s | %-4s | %-4s | %-20s\n",
               "ID","Full Name","Birth","Wilaya","Gender","Blood","Year","Speciality");
        PrintLine(130);

        count_in_block = 0;
        for(j=0;j<MAX;j++)
        {
            if(buf.t[j].deleted==0)
            {
                TStudent *S=&buf.t[j];
                sprintf(fullname,"%s %s",S->Family_Name,S->First_Name);
                printf("%-8d | %-40s | %02d/%02d/%d | %-20s | %-6s | %-4s | %-4s | %-20s\n",
                       S->Student_ID,fullname,
                       S->Day_Birth,S->Month_Birth,S->Year_Birth,
                       S->Wilaya_Name,
                       (S->Gender==2)?"Female":"Male",
                       S->Blood_Type,S->Year_Study,S->Speciality);
                count_in_block++;
                total_displayed++;
            }
        }

        if(count_in_block == 0)
            printf("  (No records)\n");

        PrintLine(130);
        printf("  Block Records: %d | Total: %d\n", count_in_block, total_displayed);

        i++;

        if(i <= en_tete(&fichier,3))
        {
            printf("\n  Press ENTER for next block...");
            getchar();
            fgets(input,sizeof(input),stdin);
        }
    }

    printf("\n  END - Total: %d records in %d blocks\n", total_displayed, total_blocks);
    PrintLine(130);

    fermer(&fichier);
    Pause();
}

/***************************** MENU *********************************/

void Menu()
{
    int choix, C31,C32;
    do
    {
        ShowHeader();
        printf("  ┌───────────────────────────────────────────────────────────────────────┐\n");
        printf("  │                          MAIN MENU                                    │\n");
        printf("  ├───────────────────────────────────────────────────────────────────────┤\n");
        printf("  │  FILE OPERATIONS                                                      │\n");
        printf("  │    1. Create & Initial Load           2. Save Index                   │\n");
        printf("  │    3. Load Index                                                      │\n");
        printf("  ├───────────────────────────────────────────────────────────────────────┤\n");
        printf("  │  STUDENT OPERATIONS                                                   │\n");
        printf("  │    4. Search Student                  5. Insert Student               │\n");
        printf("  │    6. Delete Student                  7. Modify First Name            │\n");
        printf("  ├───────────────────────────────────────────────────────────────────────┤\n");
        printf("  │  QUERIES                                                              │\n");
        printf("  │    8. Blood Type & Resident           9. By Speciality                │\n");
        printf("  │   10. Under 20 Years Old             11. By Year of Study             │\n");
        printf("  ├───────────────────────────────────────────────────────────────────────┤\n");
        printf("  │  ADVANCED                                                             │\n");
        printf("  │   12. Create CP File (1CP+2CP)                                        │\n");
        printf("  ├───────────────────────────────────────────────────────────────────────┤\n");
        printf("  │  DISPLAY                                                              │\n");
        printf("  │   13. File Header                    14. Specific Block               │\n");
        printf("  │   15. All Records (Block by Block)                                    │\n");
        printf("  ├───────────────────────────────────────────────────────────────────────┤\n");
        printf("  │    0. Exit                                                            │\n");
        printf("  └───────────────────────────────────────────────────────────────────────┘\n");
        printf("\n  Enter your choice: ");
        scanf("%d",&choix);

        switch(choix)
        {
            case 1: Create_Initial_Load(); break;
            case 2: ShowHeader(); SaveIndexToFile(&C31); Pause(); break;
            case 3: ShowHeader(); LoadIndexFromFile(&C32); Pause(); break;
            case 4: Search_Student(); break;
            case 5: Insert_Student(); break;
            case 6: Delete_Student(); break;
            case 7: Modify_FirstName(); break;
            case 8: Query_BloodType_Resident(); break;
            case 9: Query_Speciality(); break;
            case 10: Query_Under20_Interval(); break;
            case 11: Query_YearStudy(); break;
            case 12: Create_CP_File(); break;
            case 13: affichage_entete(MainFileName); break;
            case 14: affichage_un_bloc(MainFileName); break;
            case 15: affichage_fichier(MainFileName); break;
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
    }while(choix!=0);
}

/********************************* MAIN ***********************************/

int main()
{
    srand(time(NULL));
    InitIndex();

    ClearScreen();
    PrintLine(90);
    printf("\n");
    printf("                 ███████╗███████╗██╗    ███████╗████████╗██╗   ██╗██████╗ ███████╗███╗   ██╗████████╗███████╗\n");
    printf("                 ██╔════╝██╔════╝██║    ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝████╗  ██║╚══██╔══╝██╔════╝\n");
    printf("                 █████╗  ███████╗██║    ███████╗   ██║   ██║   ██║██║  ██║█████╗  ██╔██╗ ██║   ██║   ███████╗\n");
    printf("                 ██╔══╝  ╚════██║██║    ╚════██║   ██║   ██║   ██║██║  ██║██╔══╝  ██║╚██╗██║   ██║   ╚════██║\n");
    printf("                 ███████╗███████║██║    ███████║   ██║   ╚██████╔╝██████╔╝███████╗██║ ╚████║   ██║   ███████║\n");
    printf("                 ╚══════╝╚══════╝╚═╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝\n");
    printf("\n");
    printf("                              MANAGEMENT SYSTEM - LINKED NON-ORDERED FILE (LNOF)\n");
    printf("\n");
    PrintLine(90);
    printf("\n  Lab: File Structures and Data Systems (FSDS)\n");
    printf("  Presented by: SOLTANI Okba & ZERARI Tarek\n\n");
    PrintLine(90);
    printf("\n  Enter names filename [default: NAMES.txt]: ");

    char tmp[64];
    if (scanf("%63s",tmp)==1 && strlen(tmp)>0 && strcmp(tmp,"default")!=0)
        strcpy(NamesFileName,tmp);

    printf("\n");
    LoadNames();
    printf("\n");
    Pause();

    Menu();
    return 0;
}
