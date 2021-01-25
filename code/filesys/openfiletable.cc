#include "openfiletable.h"


OpenFileTable::OpenFileTable() 
{
	// Init the file table.
}

OpenFileTable::~OpenFileTable() 
{
	int i ;
	// Close all the files in the table.
	for (i = 0 ; i < NbFileTableEntries ; i ++) 
	{
	}
	// And delete it.
}

void OpenFileTable::Print() 
{
	int i ;

	for (i = 0 ; i < NbFileTableEntries ; i ++) 
	{
	}
}

int OpenFileTable::Add(int id)
{
	// An error occurs.
	return -1 ;
}

int OpenFileTable::Remove(int id)
{
	// An error occurs.
	return -1 ;
}
