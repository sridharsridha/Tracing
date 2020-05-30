//
// Created by Sridhar N on 30/05/20.
//
#include <iostream>
#include <ctime>
#include <tracing.h>

using namespace std;
using namespace sri::tracing;

TRACE_CONTEXT("ExampleWritersSetup")

int main() {
  // We we setup multiple TraceWriter, all trace writers will log the enabled traces.
  // TraceCoutWriter writes the trace to std::cout
  // TraceCerrWriter writes tha trace to std::cerr
  // TraceFileWriter writes the trace to a file (given file_name as argument)
  // Notice for each TraceWriter we can specify the TraceFormat string where #<tag> will be replaced with the corresponding values
  // By Default:
  // if TRACE_WRITERS is not setup, then traces will all go to std::cout ( TraceCoutWriter ).
  // Using environment variable export TRACE_FILE="file_name.log" one can add tracing be added to a trace file also.
  TRACE_WRITERS({make_shared<TraceCoutWriter>(),
                  make_shared<TraceCerrWriter>("Cerr: [#date #time.#millis] [#file:#line] [#name] [#level] [#func] #message"),
                  make_shared<TraceFileWriter>("example1.log", "example1.log [#date #time.#millis] #name #level #func #message")});
  TRACE_FILTER("*/*");
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