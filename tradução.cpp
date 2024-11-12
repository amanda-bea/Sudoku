#include <iostream>
#include <fstream>
#include <iomanip>

void traduzir_binario_para_texto(const char* arquivo_binario, const char* arquivo_texto) {
    // Abrir o arquivo binário para leitura
    std::ifstream bin_file(arquivo_binario, std::ios::binary);
    if (!bin_file) {
        std::cerr << "Erro ao abrir o arquivo binário." << std::endl;
        return;
    }

    // Abrir o arquivo de texto para escrita
    std::ofstream txt_file(arquivo_texto);
    if (!txt_file) {
        std::cerr << "Erro ao abrir o arquivo de texto." << std::endl;
        return;
    }

    // Ler o conteúdo do arquivo binário e escrever no arquivo de texto em formato hexadecimal
    unsigned char ch;
    while (bin_file.read(reinterpret_cast<char*>(&ch), sizeof(ch))) {
        txt_file << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ch) << " ";
    }

    // Fechar os arquivos
    bin_file.close();
    txt_file.close();

    std::cout << "Tradução concluída com sucesso." << std::endl;
}

int main() {
    const char* arquivo_binario = "TYgLh3f6qZ.bin";
    const char* arquivo_texto = "saida.txt";

    traduzir_binario_para_texto(arquivo_binario, arquivo_texto);

    return 0;
}