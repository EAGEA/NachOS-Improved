// filesys.cc 
//	Routines to manage the overall operation of the file system.
//	Implements routines to map from textual file names to files.
//
//	Each file in the file system has:
//	   A file header, stored in a sector on disk 
//		(the size of the file header data structure is arranged
//		to be precisely the size of 1 disk sector)
//	   A number of data blocks
//	   An entry in the file system directory
//
// 	The file system consists of several data structures:
//	   A bitmap of free disk sectors (cf. bitmap.h)
//	   A directory of file names and file headers
//
//      Both the bitmap and the directory are represented as normal
//	files.  Their file headers are located in specific sectors
//	(sector 0 and sector 1), so that the file system can find them 
//	on bootup.
//
//	The file system assumes that the bitmap and directory files are
//	kept "open" continuously while Nachos is running.
//
//	For those operations (such as Create, Remove) that modify the
//	directory and/or bitmap, if the operation succeeds, the changes
//	are written immediately back to disk (the two files are kept
//	open during all this time).  If the operation fails, and we have
//	modified part of the directory and/or bitmap, we simply discard
//	the changed version, without writing it back to disk.
//
// 	Our implementation at this point has the following restrictions:
//
//	   there is no synchronization for concurrent accesses
//	   files have a fixed size, set when the file is created
//	   files cannot be bigger than about 3KB in size
//	   there is no hierarchical directory structure, and only a limited
//	     number of files can be added to the system
//	   there is no attempt to make the system robust to failures
//	    (if Nachos exits in the middle of an operation that modifies
//	    the file system, it may corrupt the disk)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "disk.h"
#include "bitmap.h"
#include "directory.h"
#include "filehdr.h"
#include "filesys.h"
#include "thread.h"

// Sectors containing the file headers for the bitmap of free sectors,
// and the directory of files.  These file headers are placed in well-known 
// sectors, so that they can be located on boot-up.
#define FreeMapSector 		0
#define DirectorySector 	1

// Initial file sizes for the bitmap and directory; until the file system
// supports extensible files, the directory size sets the maximum number 
// of files that can be loaded onto the disk.
#define FreeMapFileSize 	(NumSectors / BitsInByte)
#define NumDirEntries 		10
#define DirectoryFileSize 	(sizeof(DirectoryEntry) * NumDirEntries)
#define MAX_PATH_LEN        128

// To get the current directory used by the current thread.
extern Thread *currentThread ;

//----------------------------------------------------------------------
// FileSystem::FileSystem
// 	Initialize the file system.  If format = TRUE, the disk has
//	nothing on it, and we need to initialize the disk to contain
//	an empty directory, and a bitmap of free sectors (with almost but
//	not all of the sectors marked as free).  
//
//	If format = FALSE, we just have to open the files
//	representing the bitmap and the directory.
//
//	"format" -- should we initialize the disk?
//----------------------------------------------------------------------

FileSystem::FileSystem(bool format)
{ 
	DEBUG('f', "Initializing the file system.\n");

	if (format) 
	{
		BitMap *freeMap = new BitMap(NumSectors);
		Directory *directory = new Directory(NumDirEntries);
		FileHeader *mapHdr = new FileHeader;
		FileHeader *dirHdr = new FileHeader;
		// Add sepcial directories "." and ".." to the main one.
		directory->AddSpecialDir(DirectorySector, DirectorySector) ;
				
		DEBUG('f', "Formatting the file system.\n");
		// First, allocate space for FileHeaders for the directory and bitmap
		// (make sure no one else grabs these!)
		freeMap->Mark(FreeMapSector);	    
		freeMap->Mark(DirectorySector);
		// Second, allocate space for the data blocks containing the contents
		// of the directory and bitmap files.  There better be enough space!
		ASSERT(mapHdr->Allocate(freeMap, FreeMapFileSize));
		ASSERT(dirHdr->Allocate(freeMap, DirectoryFileSize));
		// Flush the bitmap and directory FileHeaders back to disk
		// We need to do this before we can "Open" the file, since open
		// reads the file header off of disk (and currently the disk has garbage
		// on it!).
		DEBUG('f', "Writing headers back to disk.\n");
		mapHdr->WriteBack(FreeMapSector);    
		dirHdr->WriteBack(DirectorySector);
		// OK to open the bitmap and directory files now
		// The file system operations assume these two files are left open
		// while Nachos is running.
		freeMapFile = new OpenFile(FreeMapSector);
		directoryFile = new OpenFile(DirectorySector);
		// Once we have the files "open", we can write the initial version
		// of each file back to disk.  The directory at this point is completely
		// empty; but the bitmap has been changed to reflect the fact that
		// sectors on the disk have been allocated for the file headers and
		// to hold the file data for the directory and bitmap.
		DEBUG('f', "Writing bitmap and directory back to disk.\n");
		freeMap->WriteBack(freeMapFile);	 // flush changes to disk
		directory->WriteBack(directoryFile);

		if (DebugIsEnabled('f')) 
		{
			freeMap->Print();
			directory->Print();

			delete freeMap; 
			delete directory; 
			delete mapHdr; 
			delete dirHdr;
		}
	} 
	else 
	{
		// if we are not formatting the disk, just open the files representing
		// the bitmap and directory; these are left open while Nachos is running
        freeMapFile = new OpenFile(FreeMapSector);
        directoryFile = new OpenFile(DirectorySector);
    }
	// Save the current working directory.
	currentThread->setCurrentDirectory(directoryFile) ; 
}

//----------------------------------------------------------------------
// FileSystem::Open
// 	Open a file for reading and writing.  
//	To open a file:
//	  Find the location of the file's header, using the directory 
//	  Bring the header into memory
//
//	"name" -- the text name of the file to be opened
//----------------------------------------------------------------------

OpenFile *
FileSystem::Open(const char *name)
{ 
    Directory *directory = new Directory(NumDirEntries);
    OpenFile *openFile = NULL;
    int sector;

    DEBUG('f', "Opening file %s\n", name);

    sector = directory->Find(name); 
    if (sector >= 0) 		
	openFile = new OpenFile(sector);	// name was found in directory 
    delete directory;
    return openFile;				// return NULL if not found
}

//----------------------------------------------------------------------
// FileSystem::List
// 	List all the files in the file system directory.
//----------------------------------------------------------------------

void
FileSystem::List()
{
    Directory *directory = new Directory(NumDirEntries);
    directory->FetchFrom(currentThread->getCurrentDirectory()) ;
    directory->List();

    delete directory;
}

//----------------------------------------------------------------------
// FileSystem::Print
// 	Print everything about the file system:
//	  the contents of the bitmap
//	  the contents of the directory
//	  for each file in the directory,
//	      the contents of the file header
//	      the data in the file
//----------------------------------------------------------------------

void
FileSystem::Print()
{
    FileHeader *bitHdr = new FileHeader;
    FileHeader *dirHdr = new FileHeader;
    BitMap *freeMap = new BitMap(NumSectors);
    Directory *directory = new Directory(NumDirEntries);

    printf("Bit map file header:\n");
    bitHdr->FetchFrom(FreeMapSector);
    bitHdr->Print();

    printf("Directory file header:\n");
    dirHdr->FetchFrom(DirectorySector);
    dirHdr->Print();

    freeMap->FetchFrom(freeMapFile);
    freeMap->Print();

    directory->FetchFrom(currentThread->getCurrentDirectory()) ;
    directory->Print();

    delete bitHdr;
    delete dirHdr;
    delete freeMap;
    delete directory;
} 

//----------------------------------------------------------------------
// FileSystem::CreateFile
// 	Create a file in the Nachos file system (similar to UNIX create).
//	Since we can't increase the size of files dynamically, we have
//	to give Create the initial size of the file.
//
//	The steps to create a file are:
//	  Make sure the file doesn't already exist
//        Allocate a sector for the file header
// 	  Allocate space on disk for the data blocks for the file
//	  Add the name to the directory
//	  Store the new file header on disk 
//	  Flush the changes to the bitmap and the directory back to disk
//
//	Return TRUE if everything goes ok, otherwise, return FALSE.
//
// 	Create fails if:
//   		file is already in directory
//	 	no free space for file header
//	 	no free entry for file in directory
//	 	no free space for data blocks for the file 
//
// 	Note that this implementation assumes there is no concurrent access
//	to the file system!
//
//	"name" -- name of file to be created
//	"initialSize" -- size of file to be created
//----------------------------------------------------------------------

bool FileSystem::CreateFile(const char *path, int initialSize)
{
	char tmpPath[140] ;
	char name[35] ;

	SplitPathAndName(path, name, tmpPath) ;
	OpenFile *tmpDir = currentThread->getCurrentDirectory() ;

	if (tmpPath[0] != '\0')
	{
		ChangeCurrentDir(tmpPath) ;
	}

	bool success = CreateFileInCurrentDirectory((const char *) name, initialSize) ;
	currentThread->setCurrentDirectory(tmpDir) ;

	return success ;
}

bool FileSystem::CreateFileInCurrentDirectory(const char *name, int initialSize)
{
    Directory *directory = new Directory(NumDirEntries);
    BitMap *freeMap = new BitMap(NumSectors);
    FileHeader *hdr = new FileHeader() ;
    int sector;
    bool success ;

    DEBUG('f', "Creating file \"%s\", size %d...\n", name, initialSize);

	directory->FetchFrom(currentThread->getCurrentDirectory()) ;

    if (directory->Find(name) != -1)
	{
		DEBUG('f', "Can't be created: file already existing in this directory\n") ;
		success = false ;
	}
    else 
	{	
		// Find a sector to hold the file header.
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	

    	if (sector == -1) 		
		{
			DEBUG('f', "Can't be created: no free block available for file header\n") ;
			success = false ;
		}
        else if (! directory->AddFile(name, sector))
		{
			DEBUG('f', "Can't be created: no space in this directory for this file\n") ;
			success = false ;
		}
		else if (! hdr->Allocate(freeMap, initialSize))
		{
			DEBUG('f', "Can't be created: no space on disk for this file\n") ;
			success = false ;
		}
		else 
		{	
			// Everthing worked, flush all changes back to disk.
			hdr->WriteBack(sector); 		
			directory->WriteBack(currentThread->getCurrentDirectory());
			freeMap->WriteBack(freeMapFile);

			DEBUG('f', "File \"%s\" created\n", name) ;
			success = true ;
		}
	}

	delete directory;
	delete freeMap;
	delete hdr;

	return success;
}

//----------------------------------------------------------------------
// FileSystem::CreateDir
// 	Create a directory in the Nachos file system. 
//----------------------------------------------------------------------

bool FileSystem::CreateDir(const char *path)
{
	char tmpPath[140] ;
	char name[35] ;

	SplitPathAndName(path, name, tmpPath) ;
	OpenFile *tmpDir = currentThread->getCurrentDirectory() ;

	if (tmpPath[0] != '\0')
	{
		ChangeCurrentDir(tmpPath) ;
	}

	bool success = CreateDirInCurrentDirectory((const char *) name) ;
	currentThread->setCurrentDirectory(tmpDir) ;

	return success ;
}

bool FileSystem::CreateDirInCurrentDirectory(const char *name)
{
	Directory *directory = new Directory(NumDirEntries) ;
	Directory *newDir = new Directory(DirectoryFileSize) ;
	BitMap *freeMap = new BitMap(NumSectors) ;
	FileHeader *header = new FileHeader() ;
    int sector;
	bool success ;

	DEBUG('f', "Creating directory \"%s\"...\n", name) ;

	directory->FetchFrom(currentThread->getCurrentDirectory()) ;

    if (directory->Find(name) != -1)
	{
		DEBUG('f', "Can't be created: directory already existing in this directory\n") ;
		success = false ;
	}
	else 
	{
		// Find a sector to hold the directory header.
        freeMap->FetchFrom(freeMapFile);
        sector = freeMap->Find();	

    	if (sector == -1) 		
		{
			DEBUG('f', "Can't be created: no free block available for directory header\n") ;
			success = false ;
		}
        else if (! directory->AddDir(name, sector))
		{
			DEBUG('f', "Can't be created: no space in this directory for this directory\n") ;
			success = false ;
		}
		else if (! header->Allocate(freeMap, DirectoryFileSize * sizeof(DirectoryEntry)))
		{
			DEBUG('f', "Can't be created: no space on the disk for this directory\n") ;
			success = false ;
		}
		else 
		{
			if (! newDir->AddSpecialDir(sector, directory->Find(".")))
			{
				DEBUG('f', "Can't be created: special directories are not added\n") ;
				success = false ;
			}
			else
			{
				// Everthing worked, flush all changes back to disk.
				OpenFile *fileDir = new OpenFile(sector) ;
				newDir->WriteBack(fileDir) ;
				header->WriteBack(sector) ;
				directory->WriteBack(currentThread->getCurrentDirectory()) ;
				freeMap->WriteBack(freeMapFile) ;

				DEBUG('f', "Directory \"%s\" created\n", name) ;
				success = true ;

				delete fileDir ;
			}
		}
	}

	delete directory ;
	delete newDir ;
	delete freeMap ;
	delete header ;

	return success;
}

//----------------------------------------------------------------------
// FileSystem::RemoveFile
// 	Delete a file from the file system.  This requires:
//	    Remove it from the directory
//	    Delete the space for its header
//	    Delete the space for its data blocks
//	    Write changes to directory, bitmap back to disk
//
//	Return TRUE if the file was deleted, FALSE if the file wasn't
//	in the file system.
//
//	"name" -- the text name of the file to be removed
//----------------------------------------------------------------------

bool FileSystem::RemoveFile(const char *path)
{ 
	char tmpPath[140] ;
	char name[35] ;

	SplitPathAndName(path, name, tmpPath) ;
	OpenFile *tmpDir = currentThread->getCurrentDirectory() ;

	//If path is not empty
	if (tmpPath[0] != '\0')
	{
		ChangeCurrentDir(tmpPath) ;
	}

	bool success = RemoveFileInCurrentDirectory((const char*) name) ;
	currentThread->setCurrentDirectory(tmpDir) ;

	return success ;
}

bool FileSystem::RemoveFileInCurrentDirectory(const char *name)
{ 
    Directory *directory;
    BitMap *freeMap;
    FileHeader *fileHdr;
    int sector;

    DEBUG('f', "Removing file \"%s\"...\n", name) ;
    
    directory = new Directory(NumDirEntries);
	directory->FetchFrom(currentThread->getCurrentDirectory()) ;

    sector = directory->Find(name);

	if (sector == -1) 
	{
		DEBUG('f', "Can't be removed: file not found\n") ;
		delete directory;
		return false ;
	}

	fileHdr = new FileHeader;
	fileHdr->FetchFrom(sector);

	freeMap = new BitMap(NumSectors);
	freeMap->FetchFrom(freeMapFile);

	fileHdr->Deallocate(freeMap);  		// remove data blocks
	freeMap->Clear(sector);			// remove header block
	directory->Remove(name);

	freeMap->WriteBack(freeMapFile);		// flush to disk
    directory->WriteBack(currentThread->getCurrentDirectory());        // flush to disk

	DEBUG('f', "File \"%s\" removed\n") ;

    delete fileHdr;
    delete directory;
    delete freeMap;

    return true ;
} 

//----------------------------------------------------------------------
// FileSystem::RemoveDir
// 	Delete a directory from the file system. 
//----------------------------------------------------------------------

bool FileSystem::RemoveDir(const char *path)
{ 
	char tmpPath[140] ;
	char name[35] ;

	SplitPathAndName(path, name, tmpPath) ;
	OpenFile *tmpDir = currentThread->getCurrentDirectory() ;
	//If path is not empty.
	if (tmpPath[0] != '\0')
	{
		ChangeCurrentDir(tmpPath) ;
	}

	bool success = RemoveDirInCurrentDirectory((const char*) name) ;
	currentThread->setCurrentDirectory(tmpDir) ;

	return success ;
}

bool FileSystem::RemoveDirInCurrentDirectory(const char *name)
{ 
    Directory *directory, *rmDir ;
    BitMap *freeMap;
    FileHeader *header;
	OpenFile *fileDir ;
    int sector;

    DEBUG('f', "Removing directory \"%s\"...\n", name) ;
    
    directory = new Directory(NumDirEntries);
    rmDir = new Directory(NumDirEntries);

    directory->FetchFrom(currentThread->getCurrentDirectory()) ;

    sector = directory->Find(name);

    if (sector == -1) 
	{
		DEBUG('f', "Can't be removed: directory not found\n") ;
		delete directory ;
		return false ; 
    }

    header = new FileHeader() ;
    header->FetchFrom(sector) ;

	fileDir = new OpenFile(sector) ;
	rmDir->FetchFrom(fileDir) ;

	if (! rmDir->IsEmpty()) 
	{ 
		DEBUG('f', "Can't be removed: directory not empty\n") ;
		delete directory ;
		delete header ;
		delete fileDir ;
		return false ;
	}

	freeMap = new BitMap(NumSectors);
	freeMap->FetchFrom(freeMapFile);

	header->Deallocate(freeMap);  		// remove data blocks
	freeMap->Clear(sector);			// remove header block
	directory->Remove(name);

    freeMap->WriteBack(freeMapFile);		// flush to disk
    directory->WriteBack(currentThread->getCurrentDirectory());        // flush to disk

	DEBUG('f', "Directory \"%s\" removed\n") ;

    delete directory;
    delete header ;
    delete fileDir ;
    delete freeMap;
    return true ;
} 

//----------------------------------------------------------------------
// FileSystem::ChangeCurrentDir
//	Change the current workspace.	
//----------------------------------------------------------------------

void FileSystem::ChangeCurrentDir(const char *path)
{
	char dirName[FileNameMaxLen] ;
	int len = strlen(path) ;
	int i, j = 0 ;

	DEBUG('f', "Changing current directory by \"%s\"\n", path) ;

	for (i = 0 ; i < len + 1 ; i ++)
	{
		if (path[i] == '/' || path[i] == '\0')
		{
			if (dirName[j - 1] != '\0')
			{
				dirName[j] = '\0' ;
				j = 0 ;
				// Go to the directory just readed in the path.
				SetCurrentDir(dirName) ; 
			}
		}
		else
		{
			dirName[j ++] = path[i] ;
		}
	}
}

void FileSystem::SetCurrentDir(const char *dirName)
{
	// Get a current directory instance.
	Directory *dir = new Directory(NumDirEntries) ;
	dir->FetchFrom(currentThread->getCurrentDirectory()) ;
	// Get the sector of the directory wanted.
	int sector = dir->Find(dirName) ;

	if (sector == -1)
	{
		DEBUG('f', "Can't go to the directory \"%s\" : it doesn't exist\n", dirName) ;
	}

	// Set it as the new directory.
	OpenFile* dirFile = new OpenFile(sector) ;
	currentThread->setCurrentDirectory(dirFile) ;
}

//----------------------------------------------------------------------
// FileSystem::Get*
//	Getters.
//----------------------------------------------------------------------

void FileSystem::SplitPathAndName(const char *path, char *resName, char *resPath)
{
	int lastSlash=-1;
	int i;

	if(path[0]=='/') ASSERT(false); // Can't start a path with '/' (only relative pathes are allowed).
	for(i=0;path[i]!='\0';i++){
		if(path[i]=='/') lastSlash=i;
		resPath[i]=path[i];
	}
	resPath[i]='\0';
	//If only the name of a file/directory was given
	if(lastSlash==-1){
		strcpy(resName,path);//name is the whole path
		resPath[0]='\0';//path is empty
		return;
	}
	//If the name of a directory is given with a / ending the path
	if(lastSlash==i-1){
		resPath[lastSlash]='\0';
		while(i>-1 && resPath[i]!='/'){
			i--;
		}
		// Name of a directory with the form: "dir/"
		if(i==-1){
			strcpy(resName,resPath); //We write "dir" in resName
			resPath[0]='\0'; //resPath is kept empty
			
			return;
		}
		//else, the path has the form "dir1/dir2/.../dirn/"
		resPath[i]='\0';
		strcpy(resName,&resPath[i+1]);
		
		return;
	}

	strcpy(resName,&resPath[lastSlash+1]);
	resPath[lastSlash]='\0';
	
	return;
}

OpenFile *FileSystem::GetDirectoryFile()
{
	return directoryFile ;
}
