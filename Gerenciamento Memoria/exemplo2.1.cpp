#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string PATH_LOG = "trace";
string PATH_REF = "./reference_strings/ref";

int main(int argc, char **argv) {
    if (argc != 3 || atoi(argv[1]) < 1 || atoi(argv[1]) > 4 || atoi(argv[2]) < 1 || atoi(argv[2]) > 100) {
        cout << "Forma de executar: criar_reference_string.exe <numero do arquivo de log (entre 1 e 4)> <tamanho da reference string>\n";
        return 0;
    }

    ifstream log(PATH_LOG+argv[1]);
    ofstream ref(PATH_REF+argv[2]+"log"+argv[1]);
    if (!log.is_open() || !ref.is_open()) { 
        cout << "Falha ao abrir os arquivos\n";
        return -1;
    }

    int max_log, logN = atoi(argv[1]), refT = atoi(argv[2]);
    switch (logN) {
        case 1:
        case 2:
        case 3:
            max_log = 2095679;
            break;
        case 4:
            max_log = 6287037;
            break;
        default:
            break;
    }

    refT = (max_log * refT) / 100;
    for (int i = 0; i < refT; i++) {
        string addr;
        getline(log, addr);
        ref << addr << endl;
    }

    log.close();
    ref.close();
    return 0;
}