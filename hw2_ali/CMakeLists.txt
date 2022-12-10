set(LLVM_LINK_COMPONENTS support)

add_clang_executable(hw2_ali
	Analyzer.cpp
  )
target_link_libraries(hw2_ali
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
  )


