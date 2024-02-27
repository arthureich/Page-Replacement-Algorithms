/**
 * @file main.cpp
 * @author Arthur Eich, Luiz Eduardo
 * @brief Algoritmo para determinar o numero de falhas de página
 *        utilizando os algoritmos de substituicao LRU e OPT.
 * @version 0.1
 * @date 2024-02-22
 * @copyright Copyright (c) 2024
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdint> // Define uint_32t
#include <iomanip>
#include <algorithm>

using u32 = uint32_t; // Definindo u32 para enderecos 32 bits.

// Assinatura dos métodos.
std::vector<u32> createReferenceString(std::ifstream &);
int LRU(std::vector<u32> &referenceString, int frameSize);
int findLRUIndex(const std::vector<int> &recents);

int main(int argc, char const *argv[])
{
    // Verifica parametros da Execucao.
    if (argc != 2)
    {
        std::cout << "Execute: main.cpp nome-do-arquivo-trace\n";
        return -1;
    }

    // Caminho dos traces.
    std::string logPath = "./logs/" + std::string(argv[1]);

    // Verifica abertura do log pra leitura.
    std::ifstream trace(logPath);
    if (!trace.is_open())
    {
        return -1;
    }

    // Armazenando tamanho original do log.
    unsigned int originalLogSize = 0;
    if (std::string(argv[1]) == "trace1" || std::string(argv[1]) == "trace2" || std::string(argv[1]) == "trace3")
    {
        originalLogSize = 2095679;
    }
    else
    {
        originalLogSize = 6287038;
    }

    // Cria reference string.
    std::vector<u32> referenceString = createReferenceString(trace);

    // Proporcao de acesso a memoria da reference string em relacao ao log original.
    double proportion = double(referenceString.size()) / originalLogSize;

    std::cout << "Tamanho do referenceString: " << referenceString.size() << "\n";
    std::cout << "Tamanho do Log Original: " << originalLogSize << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Proporcao da referenceString em relacao ao tamanho do log original: "
              << proportion * 100
              << "%\n\n";

    // Obtendo numero de falhas de pagina considerando os seguintes frames livres (4, 8, 16, 32).
    std::cout << "LRU (Last Recently Used)\n";
    for (int framesSize = 4; framesSize <= 32; framesSize *= 2)
    {
        int pageFautls = LRU(referenceString, framesSize);
        std::cout << "Tamanho Frames Livres: " << framesSize
                  << " | Numero de Falhas de Pagina: " << pageFautls << "\n";
    }

    trace.close();

    return 0;
}

// Método que cria e retorna o reference string.
std::vector<u32> createReferenceString(std::ifstream &trace)
{
    // Armazena o reference string.
    std::vector<u32> referenceString;
    // Armazena o endereco 32 bits em hexa.
    u32 Address;
    u32 preAddress = UINT32_MAX;

    // Leitura e tratamendo dos dados.
    while (trace >> std::hex >> Address && !trace.eof())
    {
        // Desloca 12 bits para a direita removendo o deslocamento e extraindo a pagina.
        Address >>= 12;

        // Descarta paginas repetidas.
        if (Address == preAddress)
        {
            continue;
        }

        // Adiciona endereco no vetor.
        referenceString.push_back(Address);

        // Salva o log para comparacao.
        preAddress = Address;
    }

    return referenceString;
}

// Metodo que retorna o numero de falhas de pagina utilizando o algoritmo LRU.
int LRU(std::vector<u32> &referenceString, int frameSize)
{
    std::vector<u32> frames(frameSize, -1); // Paginas por Frames.
    std::vector<int> recents(frameSize, 0); // Tempos Recentes para LRU.
    int pageFaults = 0;                     // Numero de Falhas de Pagina.
    int realTime = 0;                       // Tempo para LRU.

    // Percorre cada endereco de pagina no referencestring.
    for (u32 addr : referenceString)
    {
        // Incrementa o tempo.
        realTime += 1;

        // Se endereco nao estiver presente nos frames.
        if (std::find(frames.begin(), frames.end(), addr) == frames.end())
        {
            // Encontra o indice do elemento LRU.
            int indexLRU = findLRUIndex(recents);
            // Carrega a pagina no indice do elemento LRU.
            frames[indexLRU] = addr;
            // Atualiza o tempo do indice LRU.
            recents[indexLRU] = realTime;
            // Incrementa falha de pagina.
            pageFaults += 1;
        }
        else
        {
            // Encontra a primeira ocorrencia da pagina.
            auto element = std::find(frames.begin(), frames.end(), addr);
            // Calcula seu indice.
            int index = std::distance(frames.begin(), element);
            // Atualiza seu tempo.
            recents[index] = realTime;
        }
    }

    return pageFaults;
}

// Metodo para encontrar o índice menos recentemente usado (LRU).
int findLRUIndex(const std::vector<int> &recents)
{
    // Calcula o menor elemento em recents que representa o frame LRU.
    auto min_it = std::min_element(recents.begin(), recents.end());
    // Retorna o indice do elemento LRU,
    // calculando sua distancia do inicio do array ate ele mesmo.
    return std::distance(recents.begin(), min_it);
}