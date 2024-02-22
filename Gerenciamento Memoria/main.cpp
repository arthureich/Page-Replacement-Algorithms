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
#include <iomanip> // std::stoul

using u32 = uint32_t; // Definindo u32 para enderecos 32 bits.

// Assinatura dos métodos.
std::vector<u32> createReferenceString(std::ifstream &, unsigned int &);

int main(int argc, char const *argv[])
{
    // Verifica parametros da Execucao.
    if (argc != 2)
    {
        std::cout << "Execute: main.cpp nome-do-arquivo-trace\n";
        return -1;
    }

    // Verifica abertura do log pra leitura.
    std::ifstream trace(argv[1]);
    if (!trace.is_open())
    {
        return -1;
    }

    // Tamanho da reference string.
    unsigned int logSize = 0;

    // Cria reference string.
    std::vector<u32> referenceString = createReferenceString(trace, logSize);

    for (int i = 0; i < 1; i++)
    {
        std::cout << referenceString[i] << "\n";
    }

    trace.close();

    return 0;
}

// Método que cria e retorna o reference string.
std::vector<u32> createReferenceString(std::ifstream &trace, unsigned int &logSize)
{
    // Armazena o reference string.
    std::vector<u32> referenceString;
    // Armazena o endereco 32 bits em hexa.
    u32 hexAddress;
    // Armazena o endereco 32 bits em string.
    std::string Address;
    std::string preAddress = "";

    while (trace >> Address)
    {
        // Ajusta enderecos com zeros omitidos a esquerda.
        if (Address.size() < 8)
        {
            // Numero de zeros a esquerda que faltam.
            int zeros = 8 - Address.size();
            // Armazena nova string com os zeros a esquerdas.
            Address = std::string(zeros, '0') + Address;
        }

        // Descarta paginas repetidas.
        if (Address == preAddress)
        {
            continue;
        }

        // Converte string para endereco hexa 32 bits.
        hexAddress = std::stoul(Address, nullptr, 16);

        referenceString.push_back(hexAddress);

        logSize++;

        preAddress = Address;
    }

    return referenceString;
}