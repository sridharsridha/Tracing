//
// Created by Sridhar N on 30/05/20.
//
#include <iostream>
#include <ctime>
#include <tracing.h>

using namespace std;
using namespace sri::tracing;

TRACE_CONTEXT("ExampleFormatSetup")

int main() {
  // Using environment variable export=TRACE_FORMAT="<format>" we can override the default global trace format.
  // similarly, we can use macro TRACE_FORMAT to override the global trace format in the main function.
  TRACE_FORMAT("Overridden Format: #date #time.#millis #name #level #message");
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