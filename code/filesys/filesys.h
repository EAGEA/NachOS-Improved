// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.  This is provided in case the
//	multiprogramming and virtual memory assignments (which make use
//	of the file system) are done before the file system assignment.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "openfile.h"


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
#define PathLenMax          128


#ifdef FILESYS_STUB 		// Temporarily implement file system calls as 
// calls to UNIX, until the real file system
// implementation is available
class FileSystem {
	public:
		FileSystem(bool format) {}

		bool Create(const char *name, int initialSize) { 
			int fileDescriptor = OpenForWrite(name);

			if (fileDescriptor == -1) return FALSE;
			Close(fileDescriptor); 
			return TRUE; 
		}

		OpenFile* Open(char *name) {
			int fileDescriptor = OpenForReadWrite(name, FALSE);

			if (fileDescriptor == -1) return NULL;
			return new OpenFile(fileDescriptor);
		}

		bool Remove(char *name) { return Unlink(name) == 0; }

		OpenFile *GetDirectoryFile() { return NULL ; }
};

#else // FILESYS

#include "openfiletable.h"

class FileSystem 
{
	public :
		// Initialize the file system.
		// Must be called *after* "synchDisk" 
		// has been initialized.
		// If "format", there is nothing on
		// the disk, so initialize the directory
		// and the bitmap of free blocks.
		FileSystem(bool format);

		// List all the files in the file system.
		void List();			
		void Print();		

		// Main operations in the file system (create/remove).
		bool CreateFile(const char* name, int initialSize);
		bool CreateFileInCurrentDirectory(const char *name, int initialSize) ;   	
		bool CreateDir(const char *name) ;
		bool CreateDirInCurrentDirectory(const char *name) ;
		bool RemoveFile(const char *name) ;
		bool RemoveFileInCurrentDirectory(const char *name) ;
		bool RemoveDir(const char *name) ;
		bool RemoveDirInCurrentDirectory(const char *name) ;
		void ChangeCurrentDir(const char *path) ;

		// File operations with the table.
		OpenFileId Open(const char *path, char mode) ;
		OpenFileId OpenInCurrentDirectory(const char *name, char mode) ;
		int Close(OpenFileId i) ;
		int Read(OpenFileId i, char *buf, int nbOctets) ;
		int Write(OpenFileId i, char *buf, int nbOctets) ;
		OpenFile *GetOpenFile(OpenFileId i) ; // Should not be used...

	private :
		// Bit map of free disk blocks, represented as a file.
		OpenFile* freeMapFile;		
		// "Root" directory -- list of file names, represented as a file.
		OpenFile* directoryFile;	
		// Contains all the opened files.
		OpenFileTable *fileTable ;

		// Getters, utilities..
		void SetCurrentDir(const char *dirName) ;
		void SplitPathAndName(const char* path, char* resPath, char* resName) ;
		OpenFile *GetDirectoryFile() ;
} ;

#endif // FILESYS

#endif // FS_H
