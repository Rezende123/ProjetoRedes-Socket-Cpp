## 🛰️ TP SOCKETS CLOUD

Este projeto implementa uma comunicação simples entre **Cliente** e **Servidor** utilizando **Sockets TCP** em C++ com **Winsock (Windows)**. Ele simula um cliente que coleta nomes de arquivos de um diretório e os envia para um servidor, que cria um arquivo com esses nomes localmente.

![Execução](./exemplo-execucao.png)

---

## 🗂️ Estrutura do Repositório

```
/
├── Client/
│   └── main.cpp
├── Server/
│   └── main.cpp
├── tdir/
│   ├── arquivo1.txt
│   └── imagem.jpg
```

---

## ⚙️ Compilação e Execução

A compilação e execução deve ser feita **em ambiente Windows**, com `g++` (MinGW ou compatível) instalado.

### 🔧 Comando para compilar e rodar (Cliente ou Servidor)

Acesse a pasta (`Client` ou `Server`) e execute:

```bash
g++ -g main.cpp -o main.exe -lws2_32 && main.exe
```

> O parâmetro `-lws2_32` é essencial para linkar a biblioteca Winsock.

---

## 💻 Modo de Uso

### 🖥️ 1. Servidor

* Acesse a pasta `Server` e execute o comando acima.
* O servidor ficará escutando a porta `8080`.
* Ao receber dados, ele criará um arquivo `.txt` contendo os nomes dos arquivos enviados.

### 🧑‍💻 2. Cliente

* Acesse a pasta `Client` e execute o comando acima.
* Quando solicitado, insira os seguintes dados no console:

```bash
<host do servidor> <port do servidor> <caminho do diretório>
```

**Exemplo de uso com o diretório de teste incluso no repositório:**

```
127.0.0.1 8080 ../tdir
```

> Isso vai coletar os nomes de todos os arquivos presentes em `tdir/` e enviá-los ao servidor.

---

## 📥 O que acontece na prática?

### Cliente:

* Lê os arquivos no diretório informado.
* Conecta ao servidor via IP e porta fornecidos.
* Realiza uma “handshake” com a mensagem `READY`.
* Envia os nomes dos arquivos ao servidor.
* Finaliza com a mensagem `bye`.

### Servidor:

* Aguarda a mensagem `READY`.
* Retorna `READY ACK`.
* Recebe o caminho e nomes dos arquivos.
* Cria um arquivo `.txt` com o nome `<host><diretorio>.txt`, contendo os nomes dos arquivos enviados.

---

## 📝 Exemplo de Arquivo Gerado

Se o cliente usar:

```bash
127.0.0.1 8080 ../tdir
```

O servidor criará um arquivo chamado:

```
127001tdir.txt
```

Com conteúdo como:

```
../tdir/arquivo1.txt,../tdir/imagem.jpg,
```

## 🛠️ Compilação e Execução com Makefile (Windows)

Este projeto inclui um `Makefile` que facilita a compilação e execução dos módulos **Cliente** e **Servidor**, desde que você esteja usando **MinGW** no Windows.

### ✅ Requisitos

* [MinGW-w64](https://www.mingw-w64.org/) instalado e adicionado ao `PATH`
* `make` disponível (pode ser o `mingw32-make` renomeado como `make`)

### 📦 Comandos disponíveis

| Comando           | Ação                                         |
| ----------------- | -------------------------------------------- |
| `make`            | Compila o cliente e o servidor               |
| `make run-server` | Compila (se necessário) e executa o servidor |
| `make run-client` | Compila (se necessário) e executa o cliente  |
| `make clean`      | Remove os arquivos `.exe` gerados            |

### ▶️ Exemplo de uso

```bash
# Compilar ambos
make

# Em um terminal, iniciar o servidor:
make run-server

# Em outro terminal, iniciar o cliente:
make run-client
```

> O cliente pedirá: `<host do servidor> <port do servidor> <nome do diretório>`
> Exemplo:
>
> ```
> 127.0.0.1 8080 ../tdir
> ```

## ✅ Requisitos

* Windows com g++ (MinGW-W64)
* Conexão local (127.0.0.1) ou rede habilitada para comunicação entre cliente e servidor
* Permissão de acesso à pasta e arquivos (tdir)
