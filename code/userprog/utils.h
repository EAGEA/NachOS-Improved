#ifndef UTILS_H
#define UTILS_H

/* MIPS -> LINUX string conversion.
 */
void CopyStringFromMachine(int from, char *to, unsigned size) ;

/* LINUX -> MIPS string conversion.
 */
void CopyStringToMachine(char *from, int to, unsigned size) ;

#endif
