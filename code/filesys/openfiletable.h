#ifndef OPENFILETABLE_H
#define OPENFILETABLE_H


#include "copyright.h"
#include "openfile.h"
#include "synch.h"


// The maximum of files which can be opened at the same time.
#define NbFileTableEntries			10 


// The following classes define a table of "open file entries" which are representing 
// an open file. This table should be managed by the system.

class OpenFileEntry 
{
	public :

		OpenFileEntry(OpenFile *file, char mode, int sector) ;
		~OpenFileEntry() ;
		// Getters.
		OpenFile *GetOpenFile() ;
		char GetMode() ;
		int GetSector() ;
		
	private :
		
		OpenFile *file ;
		// Read/write.
		char mode ;
		int sector ;
} ;

class OpenFileTable 
{
	public :

		OpenFileTable() ;
		~OpenFileTable() ;

		// Display the table content.
		void Print() ;
		// Open a file and return its index in the table (or -1).
		int Open(const char *name, char mode) ;
		// Close a file and return 0 (or -1). 
		int Close(int i) ;
		// Get the file at the index i.
		OpenFileEntry *Get(int i) ;

	private :

		// Table structure.
		OpenFileEntry *table[NbFileTableEntries] ;
		int nbFiles ;
		// Concurrent accesses protection.
		Lock *lock ;
		// Add an entry in the table and return its index (or -1). 
		int Add(OpenFileEntry *e) ;                  
		// Remove an entry from the table.
		int Remove(int i) ;
		// Return the index of the file in the table (or -1).
		int Find(int sector) ;
		// Return true if contains the file with this sector.
		bool Contains(int sector) ;
} ;


#endif
