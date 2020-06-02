//
// Created by Sridhar N on 30/05/20.
//
#include <iostream>
#include <ctime>
#include <tracing.h>

using namespace std;
using namespace sri::tracing;

// TRACE_CONTEXT(name) is needed to enable tracing in that source files ( compilation unit )
// This will be used later to filter traces using environment variable TRACE before running the application.
// The format for TRACE is <name>/<levels>
// example:
// export TRACE="ExampleBasic/1-3"
// This will trace all the TRACE0 to TRACE3 levels in trace context ExampleBasic.
// export TRACE="Example*/0123-57"
// This will trace all the TRACE0, TRACE1, TRACE2, TRACE3, TRACE4, TRACE5 and TRACE7 levels from all the
// trace contexts name starting with 'Example'
// export TRACE="*/*"
// This will trace from all trace contexts and all trace levels.
TRACE_CONTEXT("ExampleBasic")

int main() {
  std::string greeting = "GoodDay";
  auto givemetime = chrono::system_clock::to_time_t(chrono::system_clock::now());
  TRACE0("Hi, " << greeting << ", Current Day, " << ctime(&givemetime));
  // Without any filter options set in TRACE environment variable,
  // Tracing by default will trace TRACE0 (highest level) traces only.
  // So all these below trace levels will be ignored and not printed
  TRACE1("Hi");
  TRACE2("How are you doing?");
  TRACE3("The weather is good.");
  TRACE4("Yes, it is!.");
  TRACE5("What are we doing here?");
  TRACE6("Getting to know tracing basic example.");
  TRACE7("THE END!");

  return 0;
}
