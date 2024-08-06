#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "strarray.h"
#include "mystr.h"

ssize_t mystr_indexof(const char *str, const char sep, size_t start) {
    ssize_t index = -1;

    ssize_t i = start;
    while(index == -1 && i<(ssize_t)strlen(str)){
        if(str[i]==sep){
            index = i;
        }
        i++;
    }

    return index;
}

strarray_t *mystr_split(const char *str, const char sep) {
    // Create local variables
    size_t firstchar_i = 0;
    size_t lastchar_i = strlen(str);

    size_t maxletters = lastchar_i-firstchar_i;

    size_t numwords = 0;
    size_t currentword = 1;
    size_t letter_i = 0;
    
    char prevchar = '\0';
    char *tempword = malloc(sizeof(char) * maxletters + 1);
    char *str_copy = malloc(sizeof(char) * maxletters + 1);
    strcpy(str_copy, str);

    while (str[firstchar_i] == sep) {
        firstchar_i++;
    }
    
    if (lastchar_i!=0){
        while (str[lastchar_i--]==sep) {}   
    }

    // Loops though input string and counts the number of words using sep as a
    // delimiter. Treats everything else as part of a word and saves the result to
    // numwords.
    for (size_t i = firstchar_i; i <= lastchar_i; i++) {
        if (str_copy[i] == sep && prevchar != sep) {
            numwords++;
        }
        else{
        }
        prevchar = str_copy[i];
    }

    // Handle last letter
    if (str_copy[lastchar_i] != sep || prevchar != sep) {
      numwords++;
    }

    // Creates a strarray struct called split and allocates enough
    // memory for the words needed in the data array.
    if(lastchar_i==0){
        numwords--;
    }
    strarray_t *split = strarray_init(numwords);

    for (size_t i = 0; i < numwords; i++) {
        split->data[i] = malloc(sizeof(char) * (maxletters + 1));
        split->data[i][maxletters] = '\0';
    }

    prevchar = '\0'; // Reset prevchar

    // Loops through strcopy and does nothing else until it reaches the
    // first non sep character.
    for (size_t i = firstchar_i; i <= lastchar_i; i++) {
    // Then, it starts saving characters to both currentchar and lastchar
    // upon every iteration of the loop. If currentchar is not sep, it
    // will be appended to tempword. Else if currentchar is sep, lastchar
    // is not sep, and wordsadded is less than numwords, it adds a string
    // terminator to tempword and appends tempword to the next available
    // index in data using wordsadded to index. After the last word is
    // added, it returns split.
    // This block adds a letter to tempword
        if (str_copy[i] != sep) {
            tempword[letter_i] = str_copy[i];
            letter_i++;
        }
    // This block adds tempwords to words[] when ready
        else if (str_copy[i] == sep && prevchar != sep &&  currentword < numwords) {
            tempword[letter_i] = '\0';
            strcpy(split->data[currentword - 1], tempword);
            currentword++;
            letter_i = 0; // Reset letter index
        }

        prevchar = str_copy[i];
    }

    // Store last word
    if(lastchar_i!=0){
        tempword[letter_i] = '\0';
        currentword--;
        strcpy(split->data[currentword], tempword);
    }

    // Free any malloc’ed space that is NOT the expected return result of the
    // function.
    free(str_copy);
    free(tempword);

    // Return the resulting strarray object.
    return split;
}
