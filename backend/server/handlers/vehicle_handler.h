#ifndef VEHICLE_HANDLER_H
#define VEHICLE_HANDLER_H

#include <crow.h>

crow::response getBrandList(const crow::request& req);
crow::response getVehicleList(const crow::request& req);
crow::response getRecommendedVehicleList(const crow::request& req);
crow::response getVehicleDetails(const crow::request& req);
crow::response submitModel(const crow::request& req);
crow::response updateModel(const crow::request& req);
crow::response delModel(const crow::request& req);
crow::response addStar(const crow::request& req);
crow::response delStar(const crow::request& req);
crow::response addRecommend(const crow::request& req);
crow::response delRecommend(const crow::request& req);

#endif  // VEHICLE_HANDLER_H
