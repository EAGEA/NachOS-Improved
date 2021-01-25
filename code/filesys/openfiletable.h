#ifdef FILESYS
#ifndef OPENFILETABLE_H
#define OPENFILETABLE_H


#include "copyright.h"
#include "openfile.h"


// The maximum of files which can be opened at the same time.
#define NbFileTableEntries			10 

// The following class defines a talbe of "open file entries" which are representing 
// an open file. This table should be managed by the system.

class OpenFileTable 
{
	public :

		OpenFileTable() ;
		~OpenFileTable() ;

		// Display the table content.
		void Print() ;
		// Add an entry in the table.
		int Add(int ID) ;                  
		// Remove an entry from the table.
		int Remove(int ID) ;                 

	private :
} ;


#endif
#endif
