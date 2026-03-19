add_library(ADCFilter adc_filter.cpp)
target_link_libraries(ADCFilter PUBLIC pico_stdlib hardware_adc)
target_include_directories(ADCFilter INTERFACE .)
target_include_directories(ADCFilter PUBLIC . pico_stdlib)
