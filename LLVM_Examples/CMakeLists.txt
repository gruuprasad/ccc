llvm_map_components_to_libnames(llvm_libs support core irreader)

add_executable(example_llvm_fac
              fac.cpp
               )
target_link_libraries(example_llvm_fac ${llvm_libs})
add_dependencies(check example_llvm_fac)

add_executable(example_llvm_main
              main.cpp
               )
target_link_libraries(example_llvm_main ${llvm_libs})
add_dependencies(check example_llvm_main)

add_executable(example_llvm_max
              max.cpp
               )
target_link_libraries(example_llvm_max ${llvm_libs})
add_dependencies(check example_llvm_max)

add_executable(example_llvm_struct
              struct.cpp
               )
target_link_libraries(example_llvm_struct ${llvm_libs})
add_dependencies(check example_llvm_struct)
