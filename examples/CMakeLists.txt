find_package(Tracing)

add_executable(basic_usage 001-basic_usage.cpp)
target_link_libraries(basic_usage PUBLIC Tracing::Tracing)

add_executable(trace_filters_setup 002-trace_filters_setup.cpp)
target_link_libraries(trace_filters_setup PUBLIC Tracing::Tracing)

add_executable(trace_writers_setup 003-trace_writers_setup.cpp)
target_link_libraries(trace_writers_setup PUBLIC Tracing::Tracing)

add_executable(trace_format_setup 004-trace_format_setup.cpp)
target_link_libraries(trace_format_setup PUBLIC Tracing::Tracing)