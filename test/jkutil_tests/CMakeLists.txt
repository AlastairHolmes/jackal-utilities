find_package(Catch2 REQUIRED)

AddTestTarget(JKUtilTests)

target_link_libraries(JKUtilTests PUBLIC Catch2::Catch2)
target_link_libraries(JKUtilTests PUBLIC JKUtil)

AddSourcesToTarget(JKUtilTests
	GROUP "Source Files/Tests"
	SCOPE PRIVATE
	FILES
		"${CMAKE_CURRENT_LIST_DIR}/allocator.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/vector.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/bucket_allocator.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/cache_allocator.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/array_proxy.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/derived.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/zip.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/selective_iterator.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/transform_iterator.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/aligned.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/memory.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/bitwise.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/overloaded.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/scoped_guard.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/type_traits.cpp"
		"${CMAKE_CURRENT_LIST_DIR}/utility.cpp"
)