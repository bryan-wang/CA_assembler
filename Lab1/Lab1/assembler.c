//
//  assembler.c
//  
//
//  Created by Bryan Wang on 1/28/16.
//
//
#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255

enum{DONE, OK, EMPTY_LINE};

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
             ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)//Pass by Reference, so it uses char **
{
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE ); //Failed to read a line
	
	for( i = 0; i < strlen( pLine ); i++ )
		pLine[i] = tolower( pLine[i] );
        /* convert entire line to lowercase */
    
	*pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);
        /* ignore the comments */
	lPtr = pLine;
	while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' )
		lPtr++;
    
	*lPtr = '\0';
	if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
		return( EMPTY_LINE );
	if( isOpcode( lPtr ) == 1 && lPtr[0] != '.' ) /* found a label */
		{
			*pLabel = lPtr;
			if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
		}
    
	*pOpcode = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg1 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg2 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg3 = lPtr;
	if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
	*pArg4 = lPtr;
	return( OK );
}



int main(int argc, char* argv[]) {
    char *prgName = NULL;
    char *oFileName = NULL;
    FILE* infile = NULL;
    FILE* outfile = NULL;
    
    prgName = argv[0];
    oFileName = argv[2];
    
    infile = fopen(argv[1], "r");
    printf("program name = '%s'\n", prgName);
    printf("output file name = '%s'\n", oFileName);
    
    fclose(infile);
    fclose(outfile);
}