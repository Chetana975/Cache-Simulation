#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

void printInHex(int decimal, int hexSize)
{
    char hex[hexSize];
    sprintf(hex, "%X", decimal);
    printf("0x%s", hex);
}

int hexToBinary(char hex, char *binary)
{
    if (hex == '0')
    {
        strcpy(binary, "0000");
    }
    else if (hex == '1')
    {
        strcpy(binary, "0001");
    }
    else if (hex == '2')
    {
        strcpy(binary, "0010");
    }
    else if (hex == '3')
    {
        strcpy(binary, "0011");
    }
    else if (hex == '4')
    {
        strcpy(binary, "0100");
    }
    else if (hex == '5')
    {
        strcpy(binary, "0101");
    }
    else if (hex == '6')
    {
        strcpy(binary, "0110");
    }
    else if (hex == '7')
    {
        strcpy(binary, "0111");
    }
    else if (hex == '8')
    {
        strcpy(binary, "1000");
    }
    else if (hex == '9')
    {
        strcpy(binary, "1001");
    }
    else if (hex == 'a' || hex == 'A')
    {
        strcpy(binary, "1010");
    }
    else if (hex == 'b' || hex == 'B')
    {
        strcpy(binary, "1011");
    }
    else if (hex == 'c' || hex == 'C')
    {
        strcpy(binary, "1100");
    }
    else if (hex == 'd' || hex == 'D')
    {
        strcpy(binary, "1101");
    }
    else if (hex == 'e' || hex == 'E')
    {
        strcpy(binary, "1110");
    }
    else if (hex == 'f' || hex == 'F')
    {
        strcpy(binary, "1111");
    }
    else
    {
        printf("Invalid hex digit: %c\n", hex);
        return 0; // Return 0 to indicate failure
    }

    return 1; // Return 1 to indicate success
}

int binaryToDecimal(char *binary)
{
    int i = 0, decimal = 0;
    while (binary[i] != '\0')
    {
        decimal = (decimal * 2) + (binary[i] - '0');
        i++;
    }
    return decimal;
}

int main()
{
    int sets, totalsize, blocksize, assosiativity, tagbits, offset, indexbits, address_size, tagvalue, index, hit = -1;
    char rep_policy[7], write_policy[3];
    FILE *file1 = fopen("cache.config", "r");
    char line1[100], temp[100];
    int k = 0;
    while ((fgets(line1, sizeof(line1), file1) != NULL) && (k < 5))
    {
        line1[strcspn(line1, "\n")] = '\0';
        printf("Hello\n");
        strcpy(temp, line1);
        char *temporary = strtok(temp, " ");
        // printf("%s\n", temporary);
        if (k == 0)
        {
            totalsize = atoi(temporary);
        }
        else if (k == 1)
        {
            blocksize = atoi(temporary);
            printf("%d blocksize\n", blocksize);
        }
        else if (k == 2)
        {
            assosiativity = atoi(temporary);
            printf("%d assosiativity\n", assosiativity);
        }
        else if (k == 3)
        {
            strncpy(rep_policy, line1, 6);
            rep_policy[6] = '\0';
        }
        else if (k == 4)
        {
            strncpy(write_policy, line1, 2);
            write_policy[2] = '\0';
        }
        k++;
    }
    // input read file
    // indexbits=   ;
    // tagbits=addressbits-indexbits-offset;
    if (assosiativity == 0)
    {
        k = 0;
        indexbits = 0;
        assosiativity = (totalsize) / (blocksize);
        while (pow(2, k) != blocksize)
        {
            k++;
        }
        offset = k;
        tagbits = 32 - indexbits - offset;
        int cache[assosiativity];
        FILE *file2 = fopen("cache.access", "r");
        char line2[100], templine[100];
        for (int i = 0; i < assosiativity; i++)
        {
            cache[i] = -1;
        }
        while (fgets(line2, sizeof(line2), file2) != NULL)
        {   
            hit=-1;
            strcpy(templine, line2);
            char address[33];
            int index;
            char *token = strtok(templine, " ");
            token = strtok(NULL, " ");
            char hexString[9];
            //printf("Address: %s, ", token);
            for (int i = 2; i < 10; i++)
            {
                hexString[i - 2] = token[i];
            }
            // char address[33];
            printf("Address: 0x%s, ", hexString);
            for (int i = 0; i < 8; i++)
            {
                if (hexToBinary(hexString[i], address + i * 4) == 0)
                {
                    return 1; // Exit with an error code if an invalid hex digit is encountered
                }
            }
            address[32] = '\0';
            char tag[tagbits + 1];
            for (int i = 0; i < tagbits; i++)
            {
                tag[i] = address[i];
            }
            tag[tagbits] = '\0';
            tagvalue = binaryToDecimal(tag);
            printf("Set: 0x0, ");
            if (strncmp(rep_policy, "LRU", 3) == 0)
            {
                if (((line2[0] == 'R') || (line2[0] == 'r')) || (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WB", 2) == 0)))
                {
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        // hit
                        printf(" Hit");
                        int hittag = cache[hit];
                        while (cache[hit + 1] != -1)
                        {
                            cache[hit] = cache[hit + 1];
                            hit++;
                        }
                        cache[hit] = hittag;
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                        int i = 0;
                        while ((cache[i] != -1) && (i < assosiativity))
                        {
                            i++;
                        }
                        if (i == assosiativity)
                        {
                            // cache full,replace LRU and insert new one
                            for (int j = 1; j < assosiativity; j++)
                            {
                                cache[j - 1] = cache[j];
                            }
                            cache[assosiativity - 1] = tagvalue;
                        }
                        else if (cache[i] == -1)
                        {
                            // we have space to enter tag
                            cache[i] = tagvalue;
                        }
                    }
                }
                else if (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WT", 2) == 0))
                {
                    // WT
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        // hit
                        printf(" Hit");
                        int hittag = cache[hit];
                        while (cache[hit + 1] != -1)
                        {
                            cache[hit] = cache[hit + 1];
                            hit++;
                        }
                        cache[hit] = hittag;
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                    }
                }
            }
            if (strncmp(rep_policy, "FIFO", 4) == 0)
            {
                if (((line2[0] == 'R') || (line2[0] == 'r')) || (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WB", 2) == 0)))
                {
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        printf(" Hit");
                        // hit
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                        int i = 0;
                        while ((cache[i] != -1) && (i < assosiativity))
                        {
                            i++;
                        }
                        if (i == assosiativity)
                        {
                            // cache full,replace FI and insert new one
                            for (int j = 1; j < assosiativity; j++)
                            {
                                cache[j - 1] = cache[j];
                            }
                            cache[assosiativity - 1] = tagvalue;
                        }
                        else if (cache[i] == -1)
                        {
                            // we have space to enter tag
                            cache[i] = tagvalue;
                        }
                    }
                }
                else if (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WT", 2) == 0))
                {
                    // WT
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        // hit
                        printf(" Hit");
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                    }
                }
            }
            if (strncmp(rep_policy, "RANDOM", 6) == 0)
            {
                if (((line2[0] == 'R') || (line2[0] == 'r')) || (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WB", 2) == 0)))
                {
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        // hit
                        printf(" Hit");
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                        int i = 0;
                        while ((cache[i] != -1) && (i < assosiativity))
                        {
                            i++;
                        }
                        if (i == assosiativity)
                        {
                            srand(time(NULL));
                            int randomNumber = rand() % assosiativity;
                            // cache full,replace random and insert new one
                            cache[randomNumber] = tagvalue;
                        }
                        else if (cache[i] == -1)
                        {
                            // we have space to enter tag
                            cache[i] = tagvalue;
                        }
                    }
                }
                else if (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WT", 2) == 0))
                {
                    // WT
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        // hit
                        printf(" Hit");
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                    }
                }
            }
            k = tagbits;
        if (k % 4 != 0)
        {
            k = (k / 4) + 1;
        }
        else
        {
            k = k / 4;
        }
        printf(", Tag: ");
        printInHex(tagvalue, k);
        printf("\n");
        }
        
    }

    else
    {

        k = 0;
        sets = (totalsize) / (blocksize * assosiativity);
        while (pow(2, k) != sets)
        {
            k++;
        }
        indexbits = k;

        k = 0;

        while (pow(2, k) != blocksize)
        {
            k++;
        }
        offset = k;
        tagbits = 32 - indexbits - offset;
        int cache[sets][assosiativity];
        FILE *file2 = fopen("cache.access", "r");
        char line2[100], templine[100];
        for (int i = 0; i < sets; i++)
        {
            for (int j = 0; j < assosiativity; j++)
            {
                cache[i][j] = -1;
            }
        }
        while (fgets(line2, sizeof(line2), file2) != NULL)
        {
            hit=-1;
            strcpy(templine, line2);
            char address[33];
            int index;
            char *token = strtok(templine, " ");
            token = strtok(NULL, " ");
            char hexString[9];

            for (int i = 2; i < 10; i++)
            {
                hexString[i - 2] = token[i];
            }
            hexString[8] = '\0';
            printf("Address: 0x%s, ", hexString);
            // char address[33];
            for (int i = 0; i < 8; i++)
            {
                if (hexToBinary(hexString[i], address + i * 4) == 0)
                {
                    return 1; // Exit with an error code if an invalid hex digit is encountered
                }
            }
            address[32] = '\0';
            char tag[tagbits + 1];
            char set[indexbits + 1];
            for (int i = 0; i < tagbits; i++)
            {
                tag[i] = address[i];
            }
            tag[tagbits] = '\0';
            for (int i = tagbits; i < (tagbits + indexbits); i++)
            {
                set[i - tagbits] = address[i];
            }
            set[indexbits] = '\0';
            tagvalue = binaryToDecimal(tag);
            index = binaryToDecimal(set);
            k = indexbits;
            if (k % 4 != 0)
            {
                k = (k / 4) + 1;
            }
            else
            {
                k = k / 4;
            }
            printf("Set: ");
            printInHex(index, k);
            printf(", ");

            if (strncmp(rep_policy, "LRU", 3) == 0)
            {
                if (((line2[0] == 'R') || (line2[0] == 'r')) || (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WB", 2) == 0)))
                {
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[index][i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        // hit
                        printf(" Hit");
                        int hittag = cache[index][hit];
                        while (cache[index][hit + 1] != -1)
                        {
                            cache[index][hit] = cache[index][hit + 1];
                            hit++;
                        }
                        cache[index][hit] = hittag;
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                        int i = 0;
                        while ((cache[index][i] != -1) && (i < assosiativity))
                        {
                            i++;
                        }
                        if (i == assosiativity)
                        {
                            // cache full,replace LRU and insert new one
                            for (int j = 1; j < assosiativity; j++)
                            {
                                cache[index][j - 1] = cache[index][j];
                            }
                            cache[index][assosiativity - 1] = tagvalue;
                        }
                        else if (cache[index][i] == -1)
                        {
                            // we have space to enter tag
                            cache[index][i] = tagvalue;
                        }
                    }
                }
                else if (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WT", 2) == 0))
                {
                    // WT
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[index][i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        // hit
                        printf(" Hit");
                        int hittag = cache[index][hit];
                        while (cache[index][hit + 1] != -1)
                        {
                            cache[index][hit] = cache[index][hit + 1];
                            hit++;
                        }
                        cache[index][hit] = hittag;
                    }
                    else if (hit == -1)
                    {
                        printf(" Miss");
                        // miss
                    }
                }
            }
            if (strncmp(rep_policy, "FIFO", 4) == 0)
            {
                if (((line2[0] == 'R') || (line2[0] == 'r')) || (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WB", 2) == 0)))
                {
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[index][i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        printf(" Hit");
                        // hit
                    }
                    else if (hit == -1)
                    {
                        printf(" Miss");
                        // miss
                        int i = 0;
                        while ((cache[index][i] != -1) && (i < assosiativity))
                        {
                            i++;
                        }
                        if (i == assosiativity)
                        {
                            // cache full,replace FI and insert new one
                            for (int j = 1; j < assosiativity; j++)
                            {
                                cache[index][j - 1] = cache[index][j];
                            }
                            cache[index][assosiativity - 1] = tagvalue;
                        }
                        else if (cache[index][i] == -1)
                        {
                            // we have space to enter tag
                            cache[index][i] = tagvalue;
                        }
                    }
                }
                else if (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WT", 2) == 0))
                {
                    // WT
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[index][i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        printf(" Hit");
                        // hit
                    }
                    else if (hit == -1)
                    {
                        printf(" Miss");
                        // miss
                    }
                }
            }
            if (strncmp(rep_policy, "RANDOM", 6) == 0)
            {
                if (((line2[0] == 'R') || (line2[0] == 'r')) || (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WB", 2) == 0)))
                {
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[index][i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        printf(" Hit");
                        // hit
                    }
                    else if (hit == -1)
                    {
                        // miss
                        printf(" Miss");
                        int i = 0;
                        while ((cache[index][i] != -1) && (i < assosiativity))
                        {
                            i++;
                        }
                        if (i == assosiativity)
                        {
                            srand(time(NULL));
                            int randomNumber = rand() % assosiativity;
                            // cache full,replace random and insert new one
                            cache[index][randomNumber] = tagvalue;
                        }
                        else if (cache[index][i] == -1)
                        {
                            // we have space to enter tag
                            cache[index][i] = tagvalue;
                        }
                    }
                }
                else if (((line2[0] == 'W') || (line2[0] == 'w')) && (strncmp(write_policy, "WT", 2) == 0))
                {
                    // WT
                    for (int i = 0; i < assosiativity; i++)
                    {
                        if (cache[index][i] == tagvalue)
                        {
                            hit = i;
                            break;
                            
                        }
                    }
                    if (hit >= 0)
                    {
                        printf(" Hit");
                        // hit
                    }
                    else if (hit == -1)
                    {
                        printf(" Miss");
                        // miss
                    }
                }
            }
            k = tagbits;
            if (k % 4 != 0)
            {
                k = (k / 4) + 1;
            }
            else
            {
                k = k / 4;
            }
            printf(", Tag: ");
            printInHex(tagvalue, k);
            printf("\n");
        }
    }
}
