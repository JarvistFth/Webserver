//
// Created by jarvist on 5/8/20.
//

#include "HttpContext.h"

const char HttpContext::kCRLF[] = "\r\n";

bool HttpContext::processLine(const char *begin, const char *end) {
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start,end,' ');
    if(space != end && request.setMethod(start,space))
    {
        start = space +1;
        space = std::find(start,end,' ');
        if(space != end){
            const char* question = std::find(start,space,'?');
            if(question != space){
                request.setPath(start,question);
                request.setQuery(question,space);
            }else{
                request.setPath(start,space);
            }
            start = space + 1;
            succeed = end - start == 8 && std::equal(start,end-1,"HTTP/1.");
            if(succeed){
                if(*(end -1 ) == '0'){
                    request.setVersion(HttpRequest::HTTP_10);
                }else if (*(end -1 ) == '1'){
                    request.setVersion(HttpRequest::HTTP_11);
                } else{
                    succeed = false;
                }
            }
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(Buffer *input, TimeStamp receivedTime) {
    bool ok = true;
    bool hasMore = true;
    while (hasMore){
        if(state == EXPECT_METHOD){
            const char* crlf = input->findCRLF();
            if(crlf){
                ok = processLine(input->peek(),crlf);
                if(ok){
                    request.setReceivedTime(receivedTime);
                    input->retrieveUntil(crlf+2);
                    state = EXPECT_HEADERS;
                }else{
                    hasMore = false;
                }
            }else{
                hasMore = false;
            }
        }
        else if(state == EXPECT_HEADERS){
            const char* crlf = input->findCRLF();
            if(crlf){
                const char* colon = std::find(input->peek(),crlf,':');
                if(colon != crlf){
                    request.addHeader(input->peek(),colon,crlf);
                }else{
                    state = GETALL;
                    hasMore = false;
                }
                input->retrieveUntil(crlf+2);
            }else{
                hasMore = false;
            }
        }
        else if(state == EXPECT_BODY){
            // todo: parse body here
        }

    }
    return ok;
}
