#ifndef MODEL_HANDLER_H
#define MODEL_HANDLER_H

#include <crow.h>

crow::response getBrandList(const crow::request& req);
crow::response getModelList(const crow::request& req);
crow::response getRecommendedModelList(const crow::request& req);
crow::response getModelDetails(const crow::request& req);
crow::response submitModel(const crow::request& req);
crow::response updateModel(const crow::request& req);
crow::response delModel(const crow::request& req);
crow::response addStar(const crow::request& req);
crow::response delStar(const crow::request& req);
crow::response addRecommend(const crow::request& req);
crow::response delRecommend(const crow::request& req);

#endif  // MODEL_HANDLER_H
