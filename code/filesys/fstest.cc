// fstest.cc 
//	Simple test routines for the file system.  
//
//	We implement:
//	   Copy -- copy a file from UNIX to Nachos
//	   Print -- cat the contents of a Nachos file 
//	   Perftest -- a stress test for the Nachos file system
//		read and write a really large file in tiny chunks
//		(won't work on baseline system!)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "utility.h"
#include "filesys.h"
#include "system.h"
#include "thread.h"
#include "disk.h"
#include "stats.h"

#define TransferSize 	10 	// make it small, just to be difficult

//----------------------------------------------------------------------
// Copy
// 	Copy the contents of the UNIX file "from" to the Nachos file "to"
//----------------------------------------------------------------------

void Copy(const char *from, const char *to)
{
	FILE *fp;
	int openFile;
	int amountRead, fileLength;
	char *buffer;

	DEBUG('f', "Copying file \"%s\" to file \"%s\"\n", from, to);

	// Open UNIX file.
	if ((fp = fopen(from, "r")) == NULL) 
	{	
		DEBUG('f', "Can't open input file \"%s\"\n", from) ; 
		return;
	}
	// Figure out length of UNIX file.
	fseek(fp, 0, 2);		
	fileLength = ftell(fp);
	fseek(fp, 0, 0);
	DEBUG('f', "Copy of size %d\n", fileLength) ;

	// Create a Nachos file of the same length
	if (! fileSystem->CreateFile(to, fileLength)) 
	{	 
		// Create Nachos file.
		DEBUG('f', "Can't create output file \"%s\"\n", to) ; 
		fclose(fp);
		return;
	}
	// Copy the data in TransferSize chunks.
	openFile = fileSystem->Open(to, 'w') ;
	ASSERT(openFile != -1) ;
	buffer = new char[TransferSize];

	while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0)
	{
		fileSystem->Write(openFile, buffer, amountRead);	
	}

	delete [] buffer;
	fileSystem->Close(openFile) ;

	DEBUG('f', "Copy of \"%s\" to \"%s\" done\n", from, to) ; 
    fclose(fp);
}

//----------------------------------------------------------------------
// Print
// 	Print the contents of the Nachos file "name".
//----------------------------------------------------------------------

void Print(char *name)
{
    int openFile;    
    int i, amountRead;
    char *buffer;

    if ((openFile = fileSystem->Open(name, 'r')) == -1) 
	{
		DEBUG('f', "Can't open \"%s\" to print\n", name) ;
		return ;
    }
    
    buffer = new char[TransferSize];

    while ((amountRead = fileSystem->Read(openFile, buffer, TransferSize)) > 0)
	{
		for (i = 0; i < amountRead; i++)
		{
			printf("%c", buffer[i]);
		}
	}

    delete [] buffer;
	fileSystem->Close(openFile) ;

    return;
}

//----------------------------------------------------------------------
// PerformanceTest
// 	Stress the Nachos file system by creating a large file, writing
//	it out a bit at a time, reading it back a bit at a time, and then
//	deleting the file.
//
//	Implemented as three separate routines:
//	  FileWrite -- write the file
//	  FileRead -- read the file
//	  PerformanceTest -- overall control, and print out performance #'s
//----------------------------------------------------------------------

#define FileName 	"TestFile"
#define Contents 	"1234567890"
#define ContentSize 	strlen(Contents)
#define FileSize 	((int)(ContentSize * 5000))

static void 
FileWrite()
{
    int openFile;    
    int i, numBytes;

    printf("Sequential write of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);
    if (! fileSystem->CreateFile(FileName, 0)) {
      printf("Perf test: can't create %s\n", FileName);
      return;
    }
    openFile = fileSystem->Open(FileName, 'w');
    if (openFile == -1) {
	printf("Perf test: unable to open %s\n", FileName);
	return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = fileSystem->Write(openFile, (char *)Contents, ContentSize);
	if (numBytes < 10) {
		printf("Perf test: unable to write %s\n", FileName);
		fileSystem->Close(openFile) ;
	    return;
	}
    }

	fileSystem->Close(openFile) ;
}

static void 
FileRead()
{
    int openFile;    
    char *buffer = new char[ContentSize];
    int i, numBytes;

    printf("Sequential read of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);

    if ((openFile = fileSystem->Open(FileName, 'r')) == -1) {
	printf("Perf test: unable to open file %s\n", FileName);
	delete [] buffer;
	return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = fileSystem->Read(openFile, buffer, ContentSize);
	if ((numBytes < 10) || strncmp(buffer, Contents, ContentSize)) {
	    printf("Perf test: unable to read %s\n", FileName);
		fileSystem->Close(openFile) ;
	    delete [] buffer;
	    return;
	}
    }
    delete [] buffer;

	fileSystem->Close(openFile) ;
}

void
PerformanceTest()
{
    printf("Starting file system performance test:\n");
    stats->Print();
    FileWrite();
    FileRead();
    if (! fileSystem->RemoveFile(FileName)) {
      printf("Perf test: unable to remove %s\n", FileName);
      return;
    }
    stats->Print();
}

