#include "openfiletable.h"
#include "directory.h"
#include "system.h"


/* The table. */


OpenFileTable::OpenFileTable() 
{
	// Init the file table.
	nbFiles = 0 ;
	// And the synch mechanism.
	lock = new Lock("File table lock") ;
}

OpenFileTable::~OpenFileTable() 
{
	int i ;
	// Close all the files in the table.
	for (i = 0 ; i < nbFiles ; i ++) 
	{
		delete table[i] ;
	}
}

void OpenFileTable::Print() 
{
	int i ;

	printf("OpenFileTable\n") ;
	printf("---------------------------------\n") ;

	for (i = 0 ; i < nbFiles ; i ++) 
	{
		printf("Sector: %d, Mode: %c\n", table[i]->GetSector(), 
				table[i]->GetMode()) ;
	}

	printf("---------------------------------\n") ;
}

int OpenFileTable::Open(const char *name, char mode)
{
	int i, res, sector ;

	lock->Acquire() ;

    DEBUG('f', "Opening file %s\n", name) ;

    Directory *directory = new Directory(NumDirEntries);
	directory->FetchFrom(currentThread->getCurrentDirectory()) ;
    sector = directory->Find(name) ;  

    delete directory ;

	if (sector <  0)
	{
		// Name was not found in directory.
		lock->Release() ;
		return -1 ;
	}

	OpenFileEntry *entry = new OpenFileEntry(new OpenFile(sector), mode, sector) ;

	if ((i = Find(entry->GetSector())) != -1)
	{
		// Already contained.
		lock->Release() ;
		return i ; 
	}

	res = Add(entry) ;
	lock->Release() ;

	return res ;
}

int OpenFileTable::Close(int i)
{
	lock->Acquire() ;

	if (! table[i])
	{
		lock->Release() ;
		return -1 ;
	}

	int res = Remove(i) ;

	lock->Release() ;
	return res ;
}

OpenFileEntry *OpenFileTable::Get(int i)
{
	if (i >= NbFileTableEntries || i < 0)
	{
		return NULL ;
	}

	lock->Acquire() ;

	OpenFileEntry *entry = table[i] ;

	lock->Release() ;

	return entry ;
}

int OpenFileTable::Add(OpenFileEntry *e)
{
	if (nbFiles >= NbFileTableEntries)
	{
		// Table is full.
		return -1 ;
	}
	
	int i ;

	for (i = 0 ; i < NbFileTableEntries ; i ++)
	{
		if (! table[i])
		{
			nbFiles ++ ;
			table[i] = e ;
			return i ;
		}
	}
	// Not reachable.
	ASSERT(FALSE) ;
	return - 1 ;
}

int OpenFileTable::Remove(int i)
{
	if (table[i])
	{
		nbFiles -- ;
		delete table[i] ;
		table[i] = NULL ; // Force.
		return 0 ;
	}
	// Wasn't found.
	return -1 ;
}

int OpenFileTable::Find(int sector)
{
	int i ;

	for (i = 0 ; i < NbFileTableEntries ; i ++)
	{
		if (table[i] && table[i]->GetSector() == sector)
		{
			return i ;
		}
	}

	return -1 ;
}

bool OpenFileTable::Contains(int sector)
{
	return Find(sector) != -1 ; 
}


/* The table entries. */


OpenFileEntry::OpenFileEntry(OpenFile *f, char m, int s)
{
	file = f ;
	mode = m ;
	sector = s ;
}

OpenFileEntry::~OpenFileEntry()
{
	delete file ;
}

OpenFile *OpenFileEntry::GetOpenFile()
{
	return file ;
}

char OpenFileEntry::GetMode()
{
	return mode ;
}

int OpenFileEntry::GetSector()
{
	return sector ;
}
