find_package(Catch2 REQUIRED)

AddTestTarget(JKUtilTests)

target_link_libraries(JKUtilTests PUBLIC Catch2::Catch2)
target_link_libraries(JKUtilTests PUBLIC JKUtil)

AddSourcesToTarget(JKUtilTests
	GROUP "Source Files/Tests"
	SCOPE PRIVATE
	FILES
		"${CMAKE_CURRENT_LIST_DIR}/allocator.cpp"
)