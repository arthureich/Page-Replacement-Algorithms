/**
 * @file main.cpp
 * @author Arthur Eich, Luiz Eduardo, Pedro Berti
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
#include <chrono>
#include <algorithm>

using u32 = uint32_t; // Definindo u32 para enderecos 32 bits.

// Assinatura dos métodos.
std::vector<u32> createReferenceString(std::ifstream &trace, double proportion, unsigned int originalLogSize);
int LRU(std::vector<u32> &referenceString, int frameSize);
int findLRUIndex(const std::vector<int> &recents);
int OPT(std::vector<u32> &referenceString, int frameSize);
int findOPTIndex(std::vector<u32> &frames, std::vector<u32> &referenceString, int start);

int main(int argc, char const *argv[])
{
    // Verifica parametros da Execucao.
    if (argc != 3)
    {
        std::cout << "Execute: main.cpp nome-do-arquivo-trace proporcao-reference-string\n";
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
    // Lendo a proporção da reference string em relação ao log original.
    double proportion = std::stod(argv[2]);
    // Cria reference string.
    std::vector<u32> referenceString = createReferenceString(trace, proportion, originalLogSize);

     // Proporcao de acesso a memoria da reference string em relacao ao log original.
    proportion = double(referenceString.size()) / originalLogSize;

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
    std::cout << "OPT (Optimal Page Replacement)\n";
    for (int framesSize = 4; framesSize <= 32; framesSize *= 2)
    {
        int pageFaults = OPT(referenceString, framesSize);
        std::cout << "Frame Size: " << framesSize
                  << " | Number of Page Faults: " << pageFaults << "\n";
    }

    trace.close();
    return 0;
}

// Método que cria e retorna o reference string.
std::vector<u32> createReferenceString(std::ifstream &trace, double proportion, unsigned int originalLogSize)
{
    // Armazena o reference string.
    std::vector<u32> referenceString;
    // Armazena o endereco 32 bits em hexa.
    u32 Address;
    u32 preAddress = UINT32_MAX;

    // Calcula quantos elementos devem ser lidos com base na proporção.
    int elementsToRead = static_cast<int>(originalLogSize * proportion);

    // Leitura e tratamendo dos dados.
    for (int i = 0; i < elementsToRead; ++i)
    {
        // Leitura do endereço hexadecimal.
        if (!(trace >> std::hex >> Address))
        {
            break; // Se a leitura falhar ou chegar ao final do arquivo, sai do loop.
        }

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
    auto start = std::chrono::high_resolution_clock::now();
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
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "Tempo Gasto Algoritmo LRU = " << duration << " ms\n";
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

// Função para encontrar o índice da página no quadro que será utilizada mais adiante no futuro.
int findOPTIndex(std::vector<u32> &frames, std::vector<u32> &referenceString, int start) {
  // Vetor para armazenar a primeira ocorrência de cada página na string de referência após o índice atual.
  std::vector<int> indices(frames.size(), INT32_MAX);

  // Loop por cada quadro.
  for (int i = 0; i < frames.size(); i++) {
    // Loop pela string de referência começando a partir do elemento após a posição atual.
    for (int j = start; j < referenceString.size(); j++) {
      // Se a página no quadro for encontrada na string de referência, atualize seu índice no array 'indices'.
      if (frames[i] == referenceString[j]) {
        indices[i] = j;
        break; // Sair do loop interno quando a página for encontrada.
      }
    }
  }

  // Encontre o índice da página no quadro que será utilizada mais adiante no futuro (com o índice mais alto).
  auto max_it = std::max_element(indices.begin(), indices.end());

  // Retorne a posição relativa do elemento mais distante no array 'indices'.
  return std::distance(indices.begin(), max_it);
}

// Função para calcular o número de falhas de página usando o algoritmo Otimo (OPT).
int OPT(std::vector<u32> &referenceString, int frameSize) {
  // Iniciar a medição do tempo.
  auto start = std::chrono::high_resolution_clock::now();

  // Inicializar o array de quadros com slots vazios (-1).
  std::vector<u32> frames(frameSize, -1);
  
  // Contador de falhas de página.
  int pageFaults = 0;

  // Variável para acompanhar o progresso (impresso a cada 10000 iterações).
  int progress = 0;
  int total = referenceString.size();

  // Loop por cada elemento na string de referência.
  for (int i = 0; i < referenceString.size(); i++) {
    // Verifique se a página não está presente em nenhum quadro.
	if (find(frames.begin(), frames.end(), referenceString[i]) == frames.end()) {
		// Verifica se há frame livre
        if (find(frames.begin(), frames.end(), -1) != frames.end()) {
			// Encontra frame livre e carrega a página
            auto it = find(frames.begin(), frames.end(), -1);
            *it = referenceString[i];
            } else {
				// Encontra frame para substituir a página
                int indexOPT = findOPTIndex(frames, referenceString, i + 1);
                frames[indexOPT] = referenceString[i];
            }
            pageFaults++;
        }

    // Imprimir o progresso a cada 10000 iterações.
    if (i % 10000 == 0) {
      progress = (i * 100) / total;
      std::cout << "Progresso: " << progress << "%\n";
    }
  }

  // Imprimir o progresso final (100%).
  std::cout << "Progresso: 100%\n";

  // Finalizar a medição do tempo e calcular o tempo de execução em milissegundos.
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  std::cout << "Tempo Gasto Algoritmo OPT = " << duration << " ms\n";

  // Retornar o número de falhas de página.
  return pageFaults;
}
