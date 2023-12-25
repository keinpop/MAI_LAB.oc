#include "./src/parserIni.h"
#include "./src/dag.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Error! You have entered few arguments\n\t Example:\n" <<
            argv[0] << " <MUTEX_NAME_TO_LOCKED>" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string mtxLocked = argv[1];

    DAG_J graph;
    graph.createDAG("test2.ini");

    graph.runCompile(mtxLocked);

    return 0;
}