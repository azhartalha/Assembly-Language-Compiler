#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include <stdlib.h>

int stringToInt(char *str)
{
    int num = 0;
    for(int i=0; str[i]!='\0'; i++)
    {
        num = str[i]-'0';
    }
    return num;
}
int compareStrings(char *str1, char *str2)
{
    int i;
    for(i=0; str1[i]!='\0'; i++)
    {
        if(str1[i] != str2[i])
            return 0;
    }
    if(str1[i]==str2[i])
        return 1;
    return 0;
}
int stringInRegisters(char * target, char registers[8][3])
{
    for(int i=0; i<8; i++)
        if(compareStrings(target, registers[i]))
            return i;
    return -1;
}
char ** tokenizer(char *line, int& noOfTokens)
{
	char ** tokens = (char **)malloc(10 * sizeof(char *));
	char * token = (char *)malloc(10 * sizeof(char));
	int k=0;
	token[0]='\0';
	int i=0;
	while(line[i]==' ')
        i++;
	for (; line[i] != '\n' && line[i] != '\0'; i++)
	{
		if (line[i] == ' ' || line[i] == ',' || line[i] == '[' || line[i] == ']' || line[i] == '\t')
		{
			if (token[0]!='\0')
			{
				token[k] = '\0';
				tokens[noOfTokens++] = token;
				token = (char *)malloc(10 * sizeof(char));
				token[0]='\0';
				k = 0;
			}
			else
				continue;
		}
		else
        {
            token[k++] = line[i];
        }
	}
	if(token[0]!='\0')
    {
        token[k] = '\0';
        tokens[noOfTokens++] = token;
    }
	return tokens;
}

long hash(char *str)
{
	unsigned long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

int checkIfLable(char * str) //will automatically remove the ':'
{
    int i;
    for(i=0; str[i]!='\0' && str[i]!=':'; i++);
    if(str[i]!=':' || str[i+1]!='\0')
        return 0;
    str[i] = '\0';
    return 1;
}

void codeExecution(int **intertCode, int lp, char *constants[117], int constantTable[117][2], int constantsCount)
{
    void * memory = malloc(1024), *registers = malloc(8);

    for(int i=0; i<constantsCount; i++)
    {
        int index = hash(constants[i])%117;
        *((unsigned char *)(memory+constantTable[index][0])) = constantTable[index][1];
    }
    int i=0;
    while(i<lp)
    {
        if(intertCode[i][0] == 1)
        {
            *((char *)(memory+intertCode[i][1])) = *((char *)(registers+intertCode[i][2]));
        }
        else if(intertCode[i][0] == 2)
        {
            *((  char *)(registers+intertCode[i][2])) = *((  char *)(memory+intertCode[i][1]));
        }
        else if(intertCode[i][0] == 3)
        {
            *((  char *)(registers+intertCode[i][1])) = *((  char *)(registers+intertCode[i][2])) + *((  char *)(registers+intertCode[i][3]));
        }
        else if(intertCode[i][0] == 4)
        {
            *((  char *)(registers+intertCode[i][1])) = *((  char *)(registers+intertCode[i][2])) - *((  char *)(registers+intertCode[i][3]));
        }
        else if(intertCode[i][0] == 5)
        {
            *((  char *)(registers+intertCode[i][1])) = *((  char *)(registers+intertCode[i][2])) * *((  char *)(registers+intertCode[i][3]));
        }
        else if(intertCode[i][0] == 6)
        {
            i = intertCode[i][1];
            continue;
        }
        else if(intertCode[i][0] == 7)
        {
            if(intertCode[i][2] == 8 && !(*((  char *)(registers+intertCode[i][1])) == *((  char *)(registers+intertCode[i][3]))))
            {
                i = intertCode[i][4];
                continue;
            }
            else if(intertCode[i][2] == 9 && !(*((  char *)(registers+intertCode[i][1])) < *((  char *)(registers+intertCode[i][3]))))
            {
                i = intertCode[i][4];
                continue;
            }
            else if(intertCode[i][2] == 10 && !(*((  char *)(registers+intertCode[i][1])) > *((  char *)(registers+intertCode[i][3]))))
            {
                i = intertCode[i][4];
                continue;
            }
            else if(intertCode[i][2] == 11 && !(*((  char *)(registers+intertCode[i][1])) <= *((  char *)(registers+intertCode[i][3]))))
            {
                i = intertCode[i][4];
                continue;
            }
            else if(intertCode[i][2] == 12 && !(*((  char *)(registers+intertCode[i][1])) >= *((  char *)(registers+intertCode[i][3]))))
            {
                i = intertCode[i][4];
                continue;
            }
        }
        else if(intertCode[i][0] == 13)
        {
            printf("%d ", *(  char *)(registers+intertCode[i][1]));
        }
        else if(intertCode[i][0] == 14)
        {
            scanf("%d", (  char *)(registers+intertCode[i][1]));
        }
        i++;
    }
}

int** generateOpcode(FILE* fr, int& lp)
{
	int lableTable[117], lableCount = 0, symbolTable[117][2], symbolCount = 0, constantsTable[117][2] , constantstCount=0;
	char *symbols[117], *constants[117], *lables[117];
	for(int i=0 ;i<117; i++)
    {
        lableTable[i] = -1;
        symbolTable[i][0] = -1;
        constantsTable[i][0] = -1;
    }
	char registers[8][3] = {"AX", "BX", "CX", "DX", "EX", "FX", "GX", "HX"};
	int address = 0;
	while (!feof(fr))
	{
		char line[100];
		fgets(line, 100, fr);
        int noOfTokens = 0;
        char ** tokens = tokenizer(line, noOfTokens);
        if(noOfTokens == 0)
            continue;
        if(compareStrings(tokens[0], "START:"))
            break;
        if(compareStrings(tokens[0], "DATA"))
        {
            int index = hash(tokens[1])%117;
            symbolTable[index][0] = address;
            if(noOfTokens==2)
            {
                symbolTable[index][1] = 1;
                address++;
            }
            else if(noOfTokens == 3)
            {
                int tmp = stringToInt(tokens[2]);
                symbolTable[index][1] = tmp;
                address+= tmp;
            }
            else
            {
                printf("\nInvalid syntax 1\n");
                return NULL;
            }
            symbols[symbolCount++] = tokens[1];
        }
        else if(compareStrings(tokens[0], "CONST"))
        {
            if(noOfTokens == 4)
            {
                int index = hash(tokens[1])%117;
                constantsTable[index][0] = address;
                address++;
                constantsTable[index][1] = stringToInt(tokens[3]);
            }
            else
            {
                printf("\nInvalid syntax 2\n");
                return NULL;
            }
            constants[constantstCount++] = tokens[1];
        }
        else{
            printf("\nInvalid syntax 3\n");
                return NULL;
        }
    }
    printf("\nSymbol table\n");
    for(int i=0; i<symbolCount; i++)
    {
        int index = hash(symbols[i])%117;
        printf("%s\t%d\t%d\n", symbols[i], symbolTable[index][0], symbolTable[index][1]);
    }
    printf("\nConstants table\n");
    for(int i=0; i<constantstCount; i++)
    {
        int index = hash(constants[i])%117;
        printf("%s\t%d\t%d\n", constants[i], constantsTable[index][0], constantsTable[index][1]);
    }

/*------------------------------------------------------------*/
    int **interLang = (int **)malloc(sizeof(int)*100);
    for(int i=0; i<100; i++)
        interLang[i] = (int *)malloc(sizeof(int)*6);
    int ifStack[10], ieTrackStack[10], top = 0;
    for(int i=0; i<100; i++)
    {
        for(int j=0; j<6; j++)
            interLang[i][j] = -1;
    }
    while(!feof(fr))
    {
        char line[50];
		fgets(line, 50, fr);
        int noOfTokens = 0;
        char ** tokens = tokenizer(line, noOfTokens);
        if(noOfTokens == 0)
            continue;
        if(compareStrings(tokens[0], "MOV"))
        {
            if(noOfTokens>4 || noOfTokens<3)
            {
                printf("Invalid syntax 1");
                return NULL;
            }
            int index = hash(tokens[1])%117;
            if(symbolTable[index][0]!=-1)
            {
                if(noOfTokens==4)
                {
                    int RI = stringInRegisters(tokens[3], registers);
                    if(RI == -1)
                    {
                        printf("\nInvalid Synatx\n");
                        return NULL;
                    }
                    interLang[lp][0] = 1;
                    interLang[lp][1] = symbolTable[index][0]+stringToInt(tokens[2]);
                    interLang[lp][2] = RI;
                    lp++;
                }
                else
                {
                    int RI = stringInRegisters(tokens[2], registers);
                    if(RI == -1)
                    {
                        printf("\nInvalid Synatx\n");
                        return NULL;
                    }
                    interLang[lp][0] = 1;
                    interLang[lp][1] = symbolTable[index][0];
                    interLang[lp][2] = RI;
                    lp++;
                }
            }
            else if(constantsTable[index][0]!=-1)
            {
                printf("\nCannot copy into constant\n");
                return NULL;
            }
            else
            {
                int RI = stringInRegisters(tokens[1], registers);
                if(RI == -1)
                {
                    printf("\nInvalid syntax\n");
                    return NULL;
                }
                index = hash(tokens[2])%117;
                if(noOfTokens == 4)
                {
                    if(symbolTable[index][0] == -1)
                    {
                        printf("\nInvalid Syntax\n");
                        return NULL;
                    }
                    interLang[lp][0] = 2;
                    interLang[lp][1] = RI;
                    interLang[lp][2] = symbolTable[index][0]+stringToInt(tokens[3]);
                    lp++;
                }
                else
                {
                    if(symbolTable[index][0] != -1)
                    {
                        interLang[lp][0] = 2;
                        interLang[lp][1] = RI;
                        interLang[lp][2] = symbolTable[index][0];
                        lp++;
                    }
                    else if(constantsTable[index][0] != -1)
                    {
                        interLang[lp][0] = 2;
                        interLang[lp][1] = RI;
                        interLang[lp][2] = constantsTable[index][0];
                        lp++;
                    }
                    else
                    {
                        printf("\nInvalid syntax\n");
                        return NULL;
                    }
                }
            }

        }
        else if(compareStrings(tokens[0], "ADD"))
        {
            if(noOfTokens != 4)
            {
                printf("\nInvalid String\n");
                printf("\nInvalid String\n");
                return NULL;
            }
            int i1 = stringInRegisters(tokens[1], registers), i2 = stringInRegisters(tokens[2], registers), i3 = stringInRegisters(tokens[3], registers);
            if(i1== -1|| i2 == -1 || i3 == -1)
            {
                printf("\nOnly registers can be used in arthematic operations\n");
                return NULL;
            }
            interLang[lp][0] = 3;
            interLang[lp][1] = i1;
            interLang[lp][2] = i2;
            interLang[lp][3] = i3;
            lp++;
        }
        else if(compareStrings(tokens[0], "SUB"))
        {
            if(noOfTokens != 4)
            {
                printf("\nInvalid String x\n");
                return NULL;
            }
            int i1 = stringInRegisters(tokens[1], registers), i2 = stringInRegisters(tokens[2], registers), i3 = stringInRegisters(tokens[3], registers);
            if(i1== -1|| i2 == -1 || i3 == -1)
            {
                printf("\nOnly registers can be used in arthematic operations\n");
                return NULL;
            }
            interLang[lp][0] = 4;
            interLang[lp][1] = i1;
            interLang[lp][2] = i2;
            interLang[lp][3] = i3;
            lp++;
        }
        else if(compareStrings(tokens[0], "MUL"))
        {
            if(noOfTokens != 4)
            {
                printf("\nInvalid String\n");
                return NULL;
            }
            int i1 = stringInRegisters(tokens[1], registers), i2 = stringInRegisters(tokens[2], registers), i3 = stringInRegisters(tokens[3], registers);
            if(i1== -1|| i2 == -1 || i3 == -1)
            {
                printf("\nOnly registers can be used in arthematic operations\n");
                return NULL;
            }
            interLang[lp][0] = 5;
            interLang[lp][1] = i1;
            interLang[lp][2] = i2;
            interLang[lp][3] = i3;
            lp++;
        }
        else if(compareStrings(tokens[0], "READ"))
        {
            if(noOfTokens!= 2)
            {
                printf("\nInvalid syntax\n");
                return NULL;
            }
            int ind = stringInRegisters(tokens[1], registers);
            if(ind == -1)
            {
                printf("\nInvalid syntax\n");
                return NULL;
            }
            interLang[lp][0] = 14;
            interLang[lp][1] = ind;
            lp++;
        }
        else if(compareStrings(tokens[0], "PRINT"))
        {
            if(noOfTokens!= 2)
            {
                printf("\nInvalid syntax\n");
                return NULL;
            }
            int ind = stringInRegisters(tokens[1], registers);
            if(ind == -1)
            {
                printf("\nInvalid syntax\n");
                return NULL;
            }
            interLang[lp][0] = 13;
            interLang[lp][1] = ind;
            lp++;
        }
        else if(compareStrings(tokens[0], "JUMP"))
        {
            if(noOfTokens != 2)
            {
                printf("\nInvalid Syntax\n");
                return NULL;
            }
            int index = hash(tokens[1])%117;
            if(lableTable[index]==-1)
            {
                printf("\nInvalid Syntax\n");
                return NULL;
            }
            interLang[lp][0] = 6;
            interLang[lp][1] = lableTable[index];
            lp++;
        }
        else if(compareStrings(tokens[0], "IF"))
        {
            if(noOfTokens != 5)
            {
                printf("\nInvalid syntax at if\n");
                return NULL;
            }
            int i1 = stringInRegisters(tokens[1], registers), i2, i3 = stringInRegisters(tokens[3], registers);
            if(compareStrings(tokens[2], "EQ"))
            {
                i2 = 8;
            }
            else if(compareStrings(tokens[2], "LT"))
            {
                i2 = 9;
            }
            else if(compareStrings(tokens[2], "GT"))
            {
                i2 = 10;
            }
            else if(compareStrings(tokens[2], "LTEQ"))
            {
                i2 = 11;
            }
            else if(compareStrings(tokens[2], "GTEQ"))
            {
                i2 = 12;
            }
            else
            {
                printf("\nInvalid syntax 1\n");
                return NULL;
            }
            if(i3 == -1 || i1 == -1 || !compareStrings(tokens[4], "THEN"))
            {
                printf("\n%d %d %s\n", i1, i3, tokens[3]);
                printf("\nInvalid syntax 2\n");
                return NULL;
            }
            interLang[lp][0] = 7;
            interLang[lp][1] = i1;
            interLang[lp][2] = i2;
            interLang[lp][3] = i3;
            ifStack[top] = lp;
            ieTrackStack[top] = 1;
            top++;
            lp++;
        }
        else if(compareStrings(tokens[0], "ELSE"))
        {
            if(top > 0 && ieTrackStack[top-1]!=1)
            {
                printf("\nInvalid invalid if else\n");
                return NULL;
            }
            interLang[ifStack[top-1]][4] = lp + 1;
            top--;
            interLang[lp][0] = 6;
            ifStack[top] = lp;
            ieTrackStack[top] = 0;
            top++;
            lp++;
        }
        else if(compareStrings(tokens[0], "ENDIF"))
        {
            if(top > 0 && ieTrackStack[top-1] == 1)
                interLang[ifStack[top-1]][4] = lp;
            else
                interLang[ifStack[top-1]][1] = lp;
            top--;
        }
        else if(noOfTokens == 1 && checkIfLable(tokens[0]))
        {
            lables[lableCount++] = tokens[0];
            int index = hash(tokens[0])%117;
            lableTable[index] = lp;
        }
        else
        {
            for(int a0 = 0; a0<noOfTokens; a0++)
                printf("%s\t", tokens[a0], noOfTokens);
            printf("\n");
        }
    }
    printf("\n\n_______________INTERMEDIATE CODE_______________\n\n");
    for(int i=0; i<lp; i++)
    {
        for(int j=0; j<6; j++)
        {
            if(interLang[i][j]!=-1)
                printf("%d\t", interLang[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
    fclose(fr);
    FILE * fp = fopen("interCode.txt", "w"); // Storing the constants and the intermediate code in a file -1 denotes there is no value there
    fprintf(fp, "%d\n", constantstCount);
    for(int i=0; i<constantstCount; i++)
    {
        int index = hash(constants[i])%117;
        fprintf(fp, "%s %d %d\n",constants[i], constantsTable[index][0], constantsTable[index][1]);
    }
    fprintf(fp, "%d 6\n", lp);
    for(int i=0; i<lp; i++)
    {
        for(int j=0; j<6; j++)
        {
            fprintf(fp, "%d ", interLang[i][j]);
        }
        fprintf(fp, "\n");
    }
    codeExecution(interLang, lp, constants, constantsTable, constantstCount);
}

void readFromFile(FILE * fr)
{
    int constantstCount = 0, lp = 0, row =0;
    fscanf(fr, "%d", &constantstCount);
    int constantsTable[117][2];
    int **interLang = (int **)malloc(sizeof(int)*100);
    for(int i=0; i<100; i++)
        interLang[i] = (int *)malloc(sizeof(int)*6);
    char *constants[117];
    for(int i=0; i<constantstCount; i++)
    {
        constants[i] = (char *)malloc(30 * sizeof(char));
        fscanf(fr, "%s", constants[i]);
        int index = hash(constants[i])%117;
        fscanf(fr, "%d%d", &constantsTable[index][0], &constantsTable[index][1]);
    }
    fscanf(fr, "%d%d", &lp, &row);

    for(int i=0; i<lp; i++)
    {
        for(int j=0; j<row; j++)
        {
            if(interLang[i][j]!= -1)
                fscanf(fr, "%d", &interLang[i][j]);
        }
    }
    printf("\nConstants table\n");
    for(int i=0; i<constantstCount; i++)
    {
        int index = hash(constants[i])%117;
        printf("%s\t%d\t%d\n", constants[i], constantsTable[index][0], constantsTable[index][1]);
    }
    printf("\n\n_______________INTERMEDIATE CODE_______________\n\n");
    for(int i=0; i<lp; i++)
    {
        for(int j=0; j<6; j++)
        {
            if(interLang[i][j]!= -1)
            printf("%d\t", interLang[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
    codeExecution(interLang, lp, constants, constantsTable, constantstCount);
}

int main()
{

    FILE * fr = fopen("interCode.txt", "r");
    int op = 0;
    if(fr!= NULL)
    {
	    printf("Intermediate code available do you want to execute program from it (0/1)? ");
    	    scanf("%d", &op);
    }
    if(op == 1)
    {
        readFromFile(fr);
        return 0;
    }

	char filePath[30];
	printf("Enter the file path:\n");
	scanf("%s", filePath);
	fr = fopen(filePath, "r");
	if (fr == NULL)
	{
		printf("File not found\n");
		_getch();
		return 0;
	}
	int lp = 0;
	int ** intermediateCode = generateOpcode(fr, lp);
    return 0;
}
