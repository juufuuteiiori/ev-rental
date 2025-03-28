#ifndef VEHICLE_HANDLER_H
#define VEHICLE_HANDLER_H

#include <crow.h>

crow::response getVehicleList(const crow::request& req);
crow::response submitVehicle(const crow::request& req);
crow::response updateVehicle(const crow::request& req);
crow::response delVehicle(const crow::request& req);

#endif  // VEHICLE_HANDLER_H
