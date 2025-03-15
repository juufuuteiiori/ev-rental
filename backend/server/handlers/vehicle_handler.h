#ifndef VEHICLE_HANDLER_H
#define VEHICLE_HANDLER_H

#include <crow.h>

crow::response getBrandList();
crow::response getVehicleList();
crow::response getRecommendedVehicleList();
crow::response getVehicleDetails(const crow::request& req);
crow::response submitModel(const crow::request& req);
crow::response updateModel(const crow::request& req);

#endif  // VEHICLE_HANDLER_H
