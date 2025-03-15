#ifndef IMAGE_HANDLER_H
#define IMAGE_HANDLER_H

#include <crow.h>

crow::response getImage(const crow::request& req);
crow::response postImage(const crow::request& req);

#endif  // IMAGE_HANDLER_H
