// directory.cc 
//	Routines to manage a directory of file names.
//
//	The directory is a table of fixed length entries; each
//	entry represents a single file, and contains the file name,
//	and the location of the file header on disk.  The fixed size
//	of each directory entry means that we have the restriction
//	of a fixed maximum size for file names.
//
//	The constructor initializes an empty directory of a certain size;
//	we use ReadFrom/WriteBack to fetch the contents of the directory
//	from disk, and to write back any modifications back to disk.
//
//	Also, this implementation has the restriction that the size
//	of the directory cannot expand.  In other words, once all the
//	entries in the directory are used, no more files can be created.
//	Fixing this is one of the parts to the assignment.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"
#include "filehdr.h"
#include "directory.h"

//----------------------------------------------------------------------
// Directory::Directory
// 	Initialize a directory; initially, the directory is completely
//	empty.  If the disk is being formatted, an empty directory
//	is all we need, but otherwise, we need to call FetchFrom in order
//	to initialize it from disk.
//
//	"size" is the number of entries in the directory
//----------------------------------------------------------------------

Directory::Directory(int size)
{
    table = new DirectoryEntry[size];
    tableSize = size;
    for (int i = 0; i < tableSize; i++)
	table[i].inUse = FALSE;
}

//----------------------------------------------------------------------
// Directory::~Directory
// 	De-allocate directory data structure.
//----------------------------------------------------------------------

Directory::~Directory()
{ 
    delete [] table;
} 

//----------------------------------------------------------------------
// Directory::FetchFrom
// 	Read the contents of the directory from disk.
//
//	"file" -- file containing the directory contents
//----------------------------------------------------------------------

void
Directory::FetchFrom(OpenFile *file)
{
    (void) file->ReadAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::WriteBack
// 	Write any modifications to the directory back to disk
//
//	"file" -- file to contain the new directory contents
//----------------------------------------------------------------------

void
Directory::WriteBack(OpenFile *file)
{
    (void) file->WriteAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::FindIndex
// 	Look up file name in directory, and return its location in the table of
//	directory entries.  Return -1 if the name isn't in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::FindIndex(const char *name)
{
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen))
	    return i;
    return -1;		// name not in directory
}

//----------------------------------------------------------------------
// Directory::Find
// 	Look up file name in directory, and return the disk sector number
//	where the file's header is stored. Return -1 if the name isn't 
//	in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::Find(const char *name)
{
    int i = FindIndex(name);

    if (i != -1)
	return table[i].sector;
    return -1;
}

//----------------------------------------------------------------------
// Directory::List
// 	List all the file names in the directory. 
//----------------------------------------------------------------------

void
Directory::List()
{
	int i ;
	bool print = false ;
	FileHeader *header = new FileHeader() ;

	printf("Name       | IsDir | Size (bytes)\n") ;
	printf("---------------------------------\n") ;

	for (i = 0 ; i < tableSize ; i ++)
	{
		if (table[i].inUse)
		{
			print = true ;

			header->FetchFrom(table[i].sector) ;
			printf("%-10s | %s | %d\n", table[i].name, 
					table[i].isDirectory ? "true " : "false", 
					header->FileLength() / 8) ;
		}
	}

	if (! print) // Contains "." and "..".
	{
		printf("             EMPTY\n") ;
	}

	printf("---------------------------------\n") ;

	delete header ;
}

//----------------------------------------------------------------------
// Directory::Print
// 	List all the file names in the directory, their FileHeader locations,
//	and the contents of each file.  For debugging.
//----------------------------------------------------------------------

void
Directory::Print()
{ 
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\n");
    for (int i = 0; i < tableSize; i++)
	if (table[i].inUse) {
	    printf("Name: %s, IsDir: %s, Sector: %d\n", table[i].name, 
				table[i].isDirectory ? "true" : "false", table[i].sector);
	    hdr->FetchFrom(table[i].sector);
	    hdr->Print();
	}
    printf("\n");
    delete hdr;
}


///----------------------------------------------------------------------
// Directory::Add*
// 	Add a file/dir into the directory.  Return TRUE if successful;
//	return FALSE if the file/dir name is already in the directory, or if
//	the directory is completely full, and has no more space for
//	additional file/dir.
//----------------------------------------------------------------------


void Directory::Add(const char *name, bool isDirectory, int i, int sector)
{
	table[i].inUse = true ;
	table[i].isDirectory = isDirectory ;
	table[i].sector = sector ;
	strncpy(table[i].name, name, FileNameMaxLen) ;
}

bool Directory::AddFile(const char *name, int sector)
{ 
    if (FindIndex(name) != -1)
	{
		// Already exits.
		return false ;	
	}

	int i ;

    for (i = 0 ; i < tableSize ; i ++)
	{
        if (! table[i].inUse) 
		{
			// Add it.
			Add(name, false, i, sector) ;
			return true ;
		}
	}
	// No place on the system left to add this directory.
	return false ; 
}

bool Directory::AddDir(const char *name, int sector)
{
	if (FindIndex(name) != -1)
	{
		// Already exists.
		return false ;
	}

	int i ;

	for (i = 0 ; i < tableSize ; i ++)
	{
		if (! table[i].inUse) 
		{
			// Add it.
			Add(name, true, i, sector) ;
			return true ;
		}
	}
	// No place on the system left to add this directory.
	return false ; 
}


bool Directory::AddSpecialDir(int sector, int sector_)
{
	// Add "." and ".." on the 1st and 2nd sectors of the table.
	// Check if theses sectors are already used.
    if (table[0].inUse || table[1].inUse)
	{
		return false ;
	}

	Add(".", true, 0, sector) ;
   	Add("..", true, 1, sector_) ;

	return true ;
}

//----------------------------------------------------------------------
// Directory::Remove*
// 	Remove a file/dir from the directory.  Return TRUE if successful;
//	return FALSE if the file/dir isn't in the directory. 
//----------------------------------------------------------------------

bool Directory::Remove(const char *name)
{ 
    int i = FindIndex(name) ;

    if (i == -1)
	{
		// Not in the dir.
		return false ;
	}
	// Remove it.
    table[i].inUse = false ;
    return true ;	
}

bool Directory::RemoveFile(const char *name)
{
	return Remove(name) ;
}

bool Directory::RemoveDir(const char *name)
{
	if (Remove(name))
	{
		// Now remove the file in the directory.
		// TODO
		return true ;
	}

	return false ;
}

//----------------------------------------------------------------------
// Directory::Is*
//	Getters to check the state of a sector/current dir.
//----------------------------------------------------------------------

bool Directory::IsADir(int sector) 
{
	int i ;

    for (i = 0 ; i < tableSize ; i ++)
	{
		if (table[i].sector == sector)
		{
			return table[i].isDirectory ;
		}
	}
	// This sector was not found.
	return false ;
}

bool Directory::IsEmpty()
{
	int i ;

    for (i = 2 ; i < tableSize ; i ++)
	{
		if (table[i].inUse)
		{
			return false ;
		}
	}
	
	return true ;
}

bool Directory::IsFull()
{
	int i ;

    for (i = 2 ; i < tableSize ; i ++)
	{
		if (! table[i].inUse)
		{
			return false ;
		}
	}
	
	return true ;
}
