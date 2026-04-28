# Relatório do Projeto: Parser, Assembler e Executor para a Máquina Neander

## 1. Introdução

Este projeto tem como objetivo implementar uma pequena cadeia de compilação para a Máquina Neander. O sistema desenvolvido recebe um código-fonte com expressões matemáticas simples, realiza a análise léxica e sintática, gera um código assembly intermediário, monta esse assembly para código de máquina e, por fim, executa o programa em um simulador da arquitetura Neander.

A implementação foi dividida em três partes principais:

- Parser da linguagem de expressões matemáticas;
- Assembler para a linguagem assembly da Neander;
- Executor, responsável por simular o funcionamento da Máquina Neander.

## 2. Parser da Linguagem

O parser recebe como entrada uma linguagem simples baseada em declarações de variáveis e expressões matemáticas. A linguagem permite declarar variáveis usando a palavra-chave "var", atribuir valores e calcular expressões com soma, subtração, multiplicação e parênteses.

A gramática utilizada pode ser representada da seguinte forma:

```

"Programa -> Comando* EOF"

"Comando -> Declaracao | Expressao"

"Declaracao -> var IDENTIFIER = Expressao"

"Expressao -> Termo ((+ | -) Termo)*"

"Termo -> Fator (* Fator)*"

"Fator -> NUMERO | IDENTIFIER | ( Expressao )"

```

O parser trabalha de forma recursiva descendente. Cada função representa uma parte da gramática. Por exemplo, existe uma função para expressão, uma para termo e uma para fator. Isso permite respeitar a precedência das operações: primeiro são avaliados os fatores, depois multiplicações e, por fim, somas e subtrações.

Durante a análise, o parser não executa diretamente a expressão. Em vez disso, ele gera um assembly intermediário. Quando encontra constantes numéricas, cria posições de memória para armazená-las. Quando encontra operações, gera instruções equivalentes da Máquina Neander, utilizando temporários para guardar resultados intermediários.

## 3. Estrutura do Assembler

O assembler é responsável por transformar o assembly intermediário em código de máquina compatível com a Máquina Neander. Ele interpreta instruções como LDA, STA, ADD, JMP, JZ, JN e HLT, além de diretivas como DATA, SPACE e ORG.

A tradução dos mnemônicos para opcodes segue a tabela da Máquina Neander:

| Instrução | Opcode hexadecimal | Tipo |
|---|---:|---|
| NOP | 00 | Implícita |
| STA | 10 | Direta |
| LDA | 20 | Direta |
| ADD | 30 | Direta |
| OR | 40 | Direta |
| AND | 50 | Direta |
| NOT | 60 | Implícita |
| JMP | 80 | Direta |
| JN | 90 | Direta |
| JZ | A0 | Direta |
| HLT | F0 | Implícita |

As instruções implícitas ocupam 1 byte, pois não possuem operando. Já as instruções diretas ocupam 2 bytes: o primeiro byte é o opcode e o segundo byte é o endereço do operando na memória.

## 4. Primeira Passagem do Assembler

O assembler utiliza uma estratégia de duas passagens. Na primeira passagem, ele percorre o código assembly sem gerar ainda o código final. O objetivo principal dessa etapa é descobrir os endereços dos rótulos e armazená-los na tabela de símbolos.

Durante essa fase, o assembler:

- Cria uma cópia de cada linha e, durante a análise, descarta comentários iniciados por ; e remove espaços extras no início e no fim da linha.
- Ignora linhas vazias;
- Reconhece diretivas como .CODE, .DATA e ORG;
- Calcula o endereço de cada instrução ou dado usando um contador de localização;
- Identifica rótulos;
- Armazena os rótulos na tabela de símbolos;
- Verifica erros como rótulos duplicados;
- Calcula o tamanho ocupado por cada instrução ou diretiva.

Por exemplo, se o assembler encontra uma linha como:

"A DATA 5"

ele registra o símbolo "A" associado ao endereço atual da memória. Assim, quando uma instrução como "LDA A" aparecer depois, o assembler saberá qual endereço deve ser colocado no código de máquina.

## 5. Tabela de Símbolos

A tabela de símbolos é uma estrutura usada para armazenar a relação entre nomes simbólicos e endereços de memória. Ela é essencial porque permite usar nomes como A, B, ZERO ou LOOP em vez de escrever diretamente os endereços numéricos.

Cada entrada da tabela possui:

| Campo | Função |
|---|---|
| Nome | Nome do rótulo ou variável |
| Endereço | Posição correspondente na memória da Neander |

Durante a primeira passagem, os símbolos são cadastrados. Durante a segunda passagem, esses símbolos são consultados para resolver operandos.

Exemplo conceitual:

| Símbolo | Endereço |
|---|---:|
| ZERO | 76 |
| A | 79 |
| B | 80 |
| LOOP | 20 |

Se o código possui a instrução:

"LDA A"

o assembler procura "A" na tabela de símbolos e substitui pelo endereço correspondente, gerando algo como:

"LDA 79"

em código de máquina:

"20 4F"

## 6. Segunda Passagem do Assembler

Na segunda passagem, o assembler percorre novamente o arquivo assembly. Dessa vez, ele já possui a tabela de símbolos completa, então consegue traduzir todas as instruções e operandos para bytes de memória.

Durante essa fase, o assembler:

- Traduz mnemônicos para opcodes;
- Resolve operandos usando a tabela de símbolos;
- Escreve os bytes na memória simulada;
- Processa DATA colocando valores diretamente na memória;
- Processa SPACE reservando posições zeradas;
- Respeita ORG para alterar o endereço atual de montagem;
- Gera o arquivo de saída final.

Para instruções diretas, o assembler grava dois bytes: o opcode e o endereço. Para instruções implícitas, grava apenas o opcode.

Exemplo:

"LDA A"

é transformado em:

"20 endereço_de_A"

Já:

"HLT"

é transformado apenas em:

"F0"

## 7. Geração do Arquivo de Saída

Após montar o programa, o assembler gera um arquivo de memória compatível com a Máquina Neander. A memória possui 256 posições, cada uma representando um byte.

No projeto, o assembler também pode gerar um arquivo .mem compatível com o simulador externo da Neander. Esse formato não é apenas uma sequência crua de 256 bytes. Ele utiliza um cabeçalho e armazena cada posição de memória com dois bytes.

A estrutura utilizada é:

"03 4E 44 52"

seguida por 256 posições de memória, cada uma gravada em dois bytes.

Isso permite que o arquivo gerado seja reconhecido pelo simulador da Neander utilizado nos testes.

## 8. Executor da Máquina Neander

O executor simula os principais componentes da arquitetura Neander. Ele possui uma memória de 256 bytes e os principais registradores usados no ciclo de execução.

Os componentes simulados são:

| Componente | Tamanho | Função |
|---|---:|---|
| Memória | 256 bytes | Armazena instruções e dados |
| AC | 8 bits | Acumulador usado em operações aritméticas e lógicas |
| PC | 8 bits | Guarda o endereço da próxima instrução |
| IR | 8 bits | Guarda a instrução atual |
| MAR | 8 bits | Guarda o endereço de memória acessado |
| MDR | 8 bits | Guarda o dado lido ou escrito na memória |
| Flag N | 1 bit | Indica resultado negativo |
| Flag Z | 1 bit | Indica resultado zero |

O executor pode carregar a memória gerada pelo assembler e executar o programa até encontrar a instrução HLT ou até atingir um limite máximo de passos.

## 9. Ciclo de Máquina

O funcionamento do executor segue o ciclo básico de uma CPU:

1. Busca da instrução;
2. Decodificação;
3. Execução.

### 9.1 Busca

Na etapa de busca, o executor usa o PC para acessar a próxima posição de memória. O byte lido é colocado no IR, representando a instrução atual. Depois disso, o PC é incrementado para apontar para a próxima posição.

De forma conceitual:

"MAR recebe PC"

"MDR recebe memoria[MAR]"

"IR recebe MDR"

"PC é incrementado"

### 9.2 Decodificação

Na etapa de decodificação, o executor verifica qual opcode está armazenado no IR. Para isso, utiliza uma estrutura de decisão que compara o valor do IR com os opcodes conhecidos.

Por exemplo:

- Se IR for 20, a instrução é LDA;
- Se IR for 30, a instrução é ADD;
- Se IR for A0, a instrução é JZ;
- Se IR for F0, a execução deve parar.

Se a instrução precisar de operando, o executor busca o próximo byte da memória, que representa o endereço utilizado pela instrução.

### 9.3 Execução

Na etapa de execução, o comportamento depende da instrução encontrada.

Exemplos:

| Instrução | Funcionamento |
|---|---|
| LDA end | Carrega no AC o valor armazenado em memoria[end] |
| STA end | Armazena o valor do AC em memoria[end] |
| ADD end | Soma memoria[end] ao AC |
| OR end | Aplica OR lógico entre AC e memoria[end] |
| AND end | Aplica AND lógico entre AC e memoria[end] |
| NOT | Inverte todos os bits do AC |
| JMP end | Altera o PC para end |
| JN end | Altera o PC para end se a flag N estiver ativada |
| JZ end | Altera o PC para end se a flag Z estiver ativada |
| HLT | Encerra a execução |

Como o acumulador tem 8 bits, os resultados são mantidos dentro do intervalo de 0 a 255. Caso uma soma ultrapasse esse valor, ocorre o comportamento natural de estouro de 8 bits, mantendo apenas os 8 bits menos significativos.

## 10. Manipulação das Flags

O executor utiliza duas flags principais: Z e N.

A flag Z indica se o resultado no acumulador é zero. Ela recebe 1 quando AC é igual a 0 e recebe 0 caso contrário.

A flag N indica se o resultado é negativo na representação de 8 bits com sinal. Ela recebe 1 quando o bit mais significativo do acumulador está ligado. Como o acumulador tem 8 bits, esse bit é o bit 7, correspondente ao valor 0x80.

Assim:

| Condição | Flag |
|---|---|
| AC == 0 | Z = 1 |
| AC != 0 | Z = 0 |
| AC com bit 7 ligado | N = 1 |
| AC com bit 7 desligado | N = 0 |

As flags são atualizadas após instruções que alteram o conteúdo do acumulador, como LDA, ADD, OR, AND e NOT. Elas são importantes para as instruções condicionais JZ e JN.

A instrução JZ usa a flag Z para decidir se deve saltar para outro endereço. A instrução JN usa a flag N para decidir se o acumulador representa um valor negativo.

## 11. Funcionamento Geral do Projeto

O fluxo completo do projeto é:

1. O código-fonte da linguagem simples é enviado ao parser;
2. O lexer transforma o texto em tokens;
3. O parser verifica a estrutura sintática do programa;
4. O gerador de assembly cria instruções compatíveis com a Máquina Neander;
5. O assembler lê o assembly gerado;
6. A primeira passagem monta a tabela de símbolos;
7. A segunda passagem gera o código de máquina;
8. O executor carrega a memória gerada;
9. O executor roda o ciclo de busca, decodificação e execução;
10. Ao final, são exibidos os registradores e a memória relevante.

## 12. Programas de Teste

Um programa de teste usado foi uma expressão com multiplicação:

"var A = 2 * 3"

"var B = 12"

"var C = A * B"

Esse programa testa a criação de variáveis, constantes, temporários, operações de multiplicação por laço, armazenamento em memória e uso das instruções LDA, ADD, STA, JMP e JZ.

A multiplicação é implementada por somas sucessivas. O multiplicador é decrementado até chegar a zero. Quando a flag Z é ativada, o programa sai do laço usando JZ.

Esse teste é importante porque valida tanto operações aritméticas quanto saltos condicionais.

## 13. Conclusão

O projeto implementa uma cadeia completa para a Máquina Neander, incluindo análise de uma linguagem simples, geração de assembly, montagem para código de máquina e execução em um simulador próprio.

O assembler foi estruturado em duas passagens, permitindo resolver corretamente rótulos e endereços por meio da tabela de símbolos. A primeira passagem identifica os símbolos e calcula endereços, enquanto a segunda passagem gera os bytes finais da memória.

O executor simula o ciclo básico de uma CPU, realizando busca, decodificação e execução das instruções. Ele também mantém os registradores principais da arquitetura Neander e atualiza corretamente as flags N e Z, permitindo o funcionamento de instruções condicionais como JN e JZ.

Com isso, o projeto atende ao objetivo de demonstrar o funcionamento interno de um assembler e de um executor para uma arquitetura didática de 8 bits.
