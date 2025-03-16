#ifndef COMMENT_HANDLER_H
#define COMMENT_HANDLER_H

#include <crow.h>

crow::response getComments(const crow::request& req);
crow::response getCommentsById(const crow::request& req);
crow::response addComment(const crow::request& req);
crow::response delComment(const crow::request& req);

#endif  // COMMENT_HANDLER_H
