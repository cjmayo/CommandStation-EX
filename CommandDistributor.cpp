#include <Arduino.h>
#include "CommandDistributor.h"
#include "WiThrottle.h"

DCCEXParser * CommandDistributor::parser=0; 

bool  CommandDistributor::parse(byte clientId,byte * buffer, Print * streamer) {
  

   // SIDE EFFECT WARNING:::
  //  We know that parser will read the entire buffer before starting to write to it.
  //  Otherwise we would have to copy the buffer elsewhere and RAM is in short supply.


  bool closeAfter=false;
  // Intercept HTTP requests
  if (isHTTP(buffer)) {
    if (httpCallback) httpCallback(streamer, buffer);
    closeAfter = true;
  }
  else if (buffer[0] == '<')  {
    if (!parser) parser = new DCCEXParser();
    parser->parse(streamer, buffer, true); // tell JMRI parser that ACKS are blocking because we can't handle the async
  }
  else WiThrottle::getThrottle(clientId)->parse(*streamer, buffer);

  return closeAfter;
}

bool CommandDistributor::isHTTP(byte * buffer) {

  // POST GET PUT PATCH DELETE
  // You may think a simple strstr() is better... but not when ram & time is in short supply
  switch (buffer[0]) {
    case 'P':
      if (buffer[1] == 'U' && buffer[2] == 'T' && buffer[3] == ' ' ) return true;
      if (buffer[1] == 'O' && buffer[2] == 'S' && buffer[3] == 'T' && buffer[4] == ' ') return true;
      if (buffer[1] == 'A' && buffer[2] == 'T' && buffer[3] == 'C' && buffer[4] == 'H' && buffer[5] == ' ') return true;
      return false;
    case 'G':
      if (buffer[1] == 'E' && buffer[2] == 'T' && buffer[3] == ' ' ) return true;
      return false;
    case 'D':
      if (buffer[1] == 'E' && buffer[2] == 'L' && buffer[3] == 'E' && buffer[4] == 'T' && buffer[5] == 'E' && buffer[6] == ' ') return true;
      return false;
    default:
      return false;
  }
}

void CommandDistributor::setHTTPCallback(HTTP_CALLBACK callback) {
  httpCallback = callback;
}
HTTP_CALLBACK CommandDistributor::httpCallback=0;
