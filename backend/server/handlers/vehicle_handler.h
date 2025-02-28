#ifndef VEHICLE_HANDLER_H
#define VEHICLE_HANDLER_H

#include <crow.h>

crow::response getBrandList();
crow::response getVehicleList();
crow::response getRecommendedVehicleList();

#endif  // VEHICLE_HANDLER_H
