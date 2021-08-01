
/*
	system.c

	System commands

*/

#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>
#include "inc/bob.h"

// Ensures a file exists
BOOL FileExists(LPCSTR szPath)
{
    struct stat st;
    int n = stat(szPath, &st);
    if (n)
    {
        return FALSE;
    }
    return (st.st_mode & __S_IFREG);
}

// Ensures a folder exists
BOOL FolderExists(LPCSTR szPath)
{
    struct stat st;
    int n = stat(szPath, &st);
    if (n)
    {
        return FALSE;
    }
    return (st.st_mode & __S_IFDIR);
}

// Invokes the command shell
void Shell(LPCSTR command)
{
    Print("");
    if ((!command) || (!command[0]))
    {
        system("start");
    }
    else
    {
        system(command);
    }
    Print("");
}

// Displays a file directory
// NOTE: updated from Win to Linux
void Dir(LPCSTR path)
{
    if (path)
    {
        char temp[10 + MAX_PATH];
        sprintf(temp, "ls %s", path);
        Shell(temp);
    }
    else
    {
        Shell("ls");
    }
}

// Clear the screen
// NOTE: updated from Win to Linux
void Cls()
{
    system("clear");
}

// Pauses for a keypress
// NOTE: updated from Win to Linux
void Pause()
{
    // NOTE: Need to rewrite
    Shell("read -n1 -r -p \"Press any key to continue...\" key");
    // Shell("pause");
}

// Lists a text file
void ListFile(LPCSTR path)
{
    if (path)
    {
        char temp[10 + MAX_PATH];
        sprintf(temp, "type %s", path);
        Shell(temp);
    }
}

// Changes the console window title
// Deprecated (Windows only)
void Title(LPCSTR title)
{
    /*
	if ((!title) || (!title[0])) title = "BOB";
	Print("");
	Print(title);
	char temp[10 + MAX_PATH];
	sprintf(temp, "title %s", title);
	Shell(temp);
    */
}

// Displays an ordinary string
void Print(LPCSTR msg)
{
    fprintf(stderr, "%s\n", msg);
}

// Displays a boolean flag
void PrintFlag(LPCSTR msg, BOOL flag)
{
    fprintf(stderr, "%s = %s\n", msg, flag ? "TRUE" : "FALSE");
}

// Displays a text message
void PrintText(LPCSTR msg, LPCSTR txt)
{
    fprintf(stderr, "%s = %s\n", msg, txt);
}

// Displays a text array
// (final array element MUST be NULL!!!)
void PrintTextArray(LPCSTR msg, LPCSTR *pa)
{
    fprintf(stderr, "%s\n", msg);
    while (*pa)
    {
        Print(*pa);
        ++pa;
    }
}

// Displays an integer
void PrintInteger(LPCSTR msg, int num)
{
    fprintf(stderr, "%s = %i\n", msg, num);
}

// Displays a floating point number
void PrintFloat(LPCSTR msg, double num)
{
    fprintf(stderr, "%s = %g\n", msg, num);
}

// Displays a hexadecimal number
void PrintHex(LPCSTR msg, DWORD num)
{
    fprintf(stderr, "%s = 0x%08Xi\n", msg, num);
}

// Displays a memory address
void PrintAddress(LPCSTR msg, const void *p)
{
    if (p)
    {
        QWORD hi = (QWORD)p >> 16;
        QWORD lo = (QWORD)p & 0xFFFFFFFF;
        fprintf(stderr, "%s = %08X:%08X\n", msg, (DWORD)hi, (DWORD)lo);
    }
    else
    {
        fprintf(stderr, "%s = NULL\n", msg);
    }
}

// Displays
void PrintVector(LPCSTR msg, Vec v)
{
    fprintf(stderr, "%s = %g %g %g\n", msg, v[0], v[1], v[2]);
}

// Displays a 4x4 matrix
void PrintMatrix(LPCSTR msg, Matrix m)
{
    fprintf(stderr, "%s = \n", msg);
    fprintf(stderr, "%g %g %g %g\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    fprintf(stderr, "%g %g %g %g\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    fprintf(stderr, "%g %g %g %g\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    fprintf(stderr, "%g %g %g %g\n", m[3][0], m[3][1], m[3][2], m[3][3]);
}

/*
	OpenHelpFile() -- Opens the help file for a topic

	Topics are symbolic names only. They contain no
	path or filename extension. However, a complete
	pathname is composed based on the topic name.

	The format for help files is:

	[path]bob-[topic].html

	The file must be located in one of the BOB search
	directories.

	Also, the operating system must have a file association
	for opening HTML files. This is typically the user's
	default web browser.
*/

// TODO: Rewrite for Linux

void OpenHelpFile(LPCSTR topic)
{
    PSTRING s1 = CreateString("bob-");
    PSTRING s2 = AppendString(s1, topic);
    DestroyString(s1);
    s1 = AppendString(s2, ".html");
    DestroyString(s2);
    s2 = CompletePath(s1->data);
    DestroyString(s1);
    if (!s2)
    {
        Print("No help file was located for this topic.");
        return;
    }
    Shell(s2->data);
    DestroyString(s2);
}
