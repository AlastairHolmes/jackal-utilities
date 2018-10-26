/*!
	@file		preprocessor.h
	@author		Alastair Holmes
	@date		26/10/2018
 */


#ifndef JKUTIL_PREPROCESSOR_H
#define JKUTIL_PREPROCESSOR_H

#define JKUTIL_STRING(x) #x
#define JKUTIL_XSTRING(x) JKUTIL_STRING(x)
#define JKUTIL_FILE_STRING JKUTIL_XSTRING(__FILE__)

#endif