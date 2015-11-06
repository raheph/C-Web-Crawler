#ifndef _UTIL_H_
#define _UTIL_H_

/* Retrieves the MIME type of the document specified by the given url.
   The MIME type is written to the specified char array.  No more than
   bufferSize chars will be written to the array, including the terminating
   '\0'.  If the MIME type is longer than the provided array, it will be
   truncated.

   The function returns 1 if the MIME type was successfully retrieved and
   written to the char array.  The function returns 0 if the MIME type
   could not be retrieved or another error occurred. */

int getMIMEType(const char* url, char* buffer, int bufferSize);

/* Retrieves the content of the document specified by the given url.
   The content is returned as a string in a dynamically allocated array
   that should be free()'d when no longer needed.

   The function returns NULL if the document content could not be
   retrieved or another error occurred. */

char* getContent(const char* url);

#endif
