function(simple_exe file)
  add_executable(${file} ${file}.cpp)
endfunction()

simple_exe(write-file)
