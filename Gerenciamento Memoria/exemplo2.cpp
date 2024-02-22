#include <iostream>
#include <fstream>
#include <string>

using namespace std;

string PATH_REF = "../reference_strings/";

int OPTPontos(unsigned int end, int posI, unsigned int *referenceString, int refStrT)
{
    int pontos = 1;

    for (int i = posI; i < refStrT; i++)
    {
        if (referenceString[i] == end)
            break;
        pontos++;
    }

    return pontos;
}

unsigned int OPT(unsigned int *frames, int framesT, unsigned int *referenceString, int refStrT)
{
    unsigned int pageFaults = 0;
    int *framesPontos = new int[framesT];
    for (int i = 0; i < framesT; i++)
        framesPontos[i] = 0;

    /* Para cada endereço na referenceString:
        Verificar se o frame está em frames
            se encontrar NULL:
                adicionar endereço em frames[i]
                calcular framesPontos[endereço pos]
                incrementar pageFaults
            se não estiver:
                substituir frames[indice de maior valor em framesPontos] por endereço
                calcular framesPontos[indice de maior valor em framesPontos]
                incrementar pageFaults
    */

    int maiorPontos = 0x80000000;
    int frame = -1;
    for (int e = 0; e < refStrT; e++)
    {
        bool miss = true;
        for (int f = 0; f < framesT; f++)
        {
            if (frames[f] == 0)
            {
                frame = f;
                break;
            }
            else if (frames[f] == referenceString[e])
            {
                miss = false;
                break;
            }

            if (maiorPontos < framesPontos[f])
            {
                maiorPontos = framesPontos[f];
                frame = f;
            }
        }

        if (miss)
        {
            frames[frame] = referenceString[e];
            framesPontos[frame] = OPTPontos(referenceString[e], e + 1, referenceString, refStrT);
            pageFaults++;
        }
    }

    return pageFaults;
}

unsigned int LRU(unsigned int *frames, int framesT, unsigned int *referenceString, int refStrT)
{
    cout << "LRU!\n";
    unsigned int pageFaults = 0;

    return pageFaults;
}

int main(int argc, char **argv)
{
    if (argc != 4 || (atoi(argv[2]) != 4 && atoi(argv[2]) != 8 && atoi(argv[2]) != 16 && atoi(argv[2]) != 32) || (atoi(argv[3]) != 0 && atoi(argv[3]) != 1))
    {
        cout << "Forma de executar: main.exe <nome da reference string> <numero de frames livres (4, 8, 16, 32)> <algoritmo de substituicao (0 = OPT, 1 = LRU)\n";
        return 0;
    }

    unsigned int *frames = new unsigned int[atoi(argv[2])];
    unsigned int *referenceString = new unsigned int[6287037];
    string hexStr;
    ifstream log(PATH_REF + argv[1]);
    if (!log.is_open())
    {
        cout << "Falha ao abrir o arquivo\n";
        return -1;
    }

    int refStrT = 0;
    while (getline(log, hexStr))
    {
        unsigned int addr = stoul(hexStr, nullptr, 16);
        unsigned int pagina = addr & 0xFFFFF000;
        referenceString[refStrT++] = pagina;
    }
    log.close();

    unsigned int resultado;
    switch (atoi(argv[3]))
    {
    case 0:
        resultado = OPT(frames, atoi(argv[2]), referenceString, refStrT);
        break;
    case 1:
        resultado = LRU(frames, atoi(argv[2]), referenceString, refStrT);
        break;
    default:
        break;
    }

    cout << "# de Page Faults: " << (int)resultado << endl;
    return 0;
}