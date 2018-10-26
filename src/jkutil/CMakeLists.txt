# Create a library
AddLibrary(JKUtil STATIC)
AddTargetToGroup(JKUtil JKUtil)

install(TARGETS JKUtil EXPORT JKUtil
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include)
	
AddIncludeDirectoryToTarget(JKUtil "${CMAKE_CURRENT_SOURCE_DIR}/.." INSTALL_INTERFACE "include")

target_compile_features(JKUtil PUBLIC
	cxx_std_17)

# Header Files

	AddSourcesToTarget(JKUtil
		GROUP "Header Files\\Utility"
		SCOPE PRIVATE
		INSTALL_DESTINATION "include/jkutil"
		FILES
			"${CMAKE_CURRENT_LIST_DIR}/preprocessor.h"
			"${CMAKE_CURRENT_LIST_DIR}/assert.h"
			"${CMAKE_CURRENT_LIST_DIR}/aligned.h"
			"${CMAKE_CURRENT_LIST_DIR}/scoped_guard.h"
			"${CMAKE_CURRENT_LIST_DIR}/overloaded.h"
			"${CMAKE_CURRENT_LIST_DIR}/type_traits.h"
			"${CMAKE_CURRENT_LIST_DIR}/array_proxy.h"
			"${CMAKE_CURRENT_LIST_DIR}/allocator.h"
			"${CMAKE_CURRENT_LIST_DIR}/object_container.h"
			"${CMAKE_CURRENT_LIST_DIR}/memory.h"
			"${CMAKE_CURRENT_LIST_DIR}/bitwise.h"
			"${CMAKE_CURRENT_LIST_DIR}/allocator_pointer.h"
			"${CMAKE_CURRENT_LIST_DIR}/allocator_reference.h"
			"${CMAKE_CURRENT_LIST_DIR}/any_derived.h"
			"${CMAKE_CURRENT_LIST_DIR}/test_allocator.h")

# Source Files

	AddSourcesToTarget(JKUtil
		GROUP "Source Files\\Utility"
		SCOPE PRIVATE
		FILES
			"${CMAKE_CURRENT_LIST_DIR}/allocator.cpp"
			"${CMAKE_CURRENT_LIST_DIR}/memory.cpp")
