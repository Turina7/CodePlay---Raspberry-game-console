# LabProc

Este repositório apresenta o trabalho desenvolvido para a disciplina **PCS3732 - Laboratório de Processadores**.

## Integrantes do Grupo

- **Guilherme Turina** — 13679523  
- **Lucas Papoti** — 12554626  
- **Tiago Marinho** — 12554550

## Visão Geral do Projeto

O projeto tem como objetivo aplicar conceitos estudados em aula, além de explorar novas possibilidades relacionadas ao desenvolvimento embarcado.

Trabalharemos com a placa **Raspberry Pi**, utilizando-a como uma **central de processamento de jogos**. Importante destacar que **não** pretendemos desenvolver um emulador. Em vez disso, os jogos serão implementados diretamente na placa, sem o uso de imagens ou ROMs pré-existentes.

Dado o escopo ambicioso do projeto, estabelecemos marcos de **provas de conceito** que podem ser entregues caso o desenvolvimento completo não seja possível dentro do prazo.

## Entrada dos Controles

Como o objetivo é permitir jogos competitivos locais (**1 contra 1**), será necessário implementar a leitura de entradas via **teclado USB** conectado à Raspberry Pi. Os jogadores utilizarão as teclas **WASD** e **setas direcionais**, além de outras teclas conforme a necessidade de cada jogo.

Entretanto, essa funcionalidade foi destacada pela orientação como um possível ponto crítico. Como alternativa para a prova de conceito, podemos utilizar as **entradas GPIO** da placa para simular controles, caso não seja viável o uso do USB.

## Saída de Vídeo

Não há intenção de utilizar um computador auxiliar para exibir imagens ou rodar qualquer programa de suporte. Portanto, a **saída de vídeo deve ser via HDMI nativa da Raspberry Pi**. Neste caso, há poucas alternativas, e essa abordagem é considerada fixa no projeto.

## Mecânica do Projeto

Pretendemos desenvolver um **menu inicial** onde o usuário poderá selecionar diferentes jogos, como:

- Tetris  
- TRON  
- Pong  
- Outros

Como **prova de conceito**, implementaremos pelo menos **1 ou 2 jogos**, com uma estrutura que permita a fácil adição de novos títulos posteriormente.
