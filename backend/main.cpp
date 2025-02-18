#include <crow.h>

#include "database/db_init.h"

int main(int argc, char* argv[]) {
    initDatabase();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]() { return "EV Rental System Backend"; });

    app.port(8081).multithreaded().run();
}
