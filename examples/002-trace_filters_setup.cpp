//
// Created by Sridhar N on 30/05/20.
//
#include <iostream>
#include <ctime>
#include <tracing.h>

using namespace std;
using namespace sri::tracing;

TRACE_CONTEXT("ExampleBasicUsage")

int main() {
  // Exactly the same code 001-basic_usage.cpp,
  // but with this example we will see how to set the filter similar to (export TRACE="<name>/<levels>")
  // inside the main function using the provided macro TRACE_FILTER
  TRACE_FILTER("Example*/023-6");
  // With the filter, we will print all traces context names which starts with Example and in those files
  // the trace levels 0,2,3,4,5,6 will be printed, in this case we will not print TRACE1 and TRACE7
  std::string greeting = "GoodDay";
  auto givemetime = chrono::system_clock::to_time_t(chrono::system_clock::now());
  TRACE0("Hi, " << greeting << ", Current Day, " << ctime(&givemetime));
  TRACE1("Hi,");
  TRACE2("How are you doing?");
  TRACE3("The weather is good.");
  TRACE4("Yes, it is!.");
  TRACE5("What are we doing here?");
  TRACE6("Getting to know tracing basic example.");
  TRACE7("THE END!");

  return 0;
}