#include "database/database.h"
#include "server/crow_server.h"

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-i") initDatabase();

    runCrow();

    return 0;
}