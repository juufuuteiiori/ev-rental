#include "database/database.h"
#include "server/crow_server.h"

int main(int argc, char* argv[]) {
    initDatabase();

    runCrow();

    return 0;
}