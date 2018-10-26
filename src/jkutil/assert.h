#ifndef JKUTIL_ASSERT_H
#define JKUTIL_ASSERT_H

#include <assert.h>
#include <jkutil\preprocessor.h>

#if JKUTIL_ASSERT_ENABLED
	/*!
		@def JKUTIL_ASSERT(x)
		@brief A standard assert macro.
		@warning Expression \c x is not run at all if asserts are disable i.e. in Release builds. Therefore you should not
		include any state modification inside a \c JKUTIUL_ASSERT.
	*/
	#define JKUTIL_ASSERT(x) (assert((x) && JKUTIL_XSTRING(FAILED in file (at line __LINE__) JKUTIL_FILE_STRING.)))
#else
	#define JKUTIL_ASSERT(x) ((void)0)
#endif

#endif