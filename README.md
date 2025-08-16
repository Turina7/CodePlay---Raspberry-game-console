
# CodePlay 🎮

**Console de videogame em Raspberry Pi com interface plug and play para desenvolvedores**

Transforme seu Raspberry Pi em um console de jogos totalmente customizável! O CodePlay permite que qualquer desenvolvedor execute seus próprios jogos escritos em C de maneira prática e rápida, com apenas alguns requisitos básicos de compatibilidade de vídeo e input.

## ✨ Características

- **Bare-metal**: Roda diretamente no hardware, sem sistema operacional
- **Plug and Play**: Adicione novos jogos sem modificar o código principal
- **Multiplayer local**: Suporte a dois jogadores simultâneos
- **Interface simples**: Menu automático e navegação intuitiva
- **Sistema gráfico próprio**: Engine minimalista baseada em matriz de caracteres

## 🎯 Jogos Disponíveis

- **Pong**: Clássico jogo de paddle multiplayer
- **Snake**: Versão multiplayer onde duas cobras competem

## 🚀 Como Usar

### Requisitos
- Raspberry Pi 2/3
- Teclado USB
- Monitor com entrada HDMI
- Cartão SD
- Toolchain ARM: `arm-none-eabi-gcc`

### Compilação
```bash
# Clone o repositório
git clone 
cd codeplay

# Compile o projeto
make

# Instale no cartão SD
sudo cp kernel.img /path/to/sd/card/
```
## 🎮 Como Adicionar um Novo Jogo

1. **Crie uma pasta** em `games/` com o nome do seu jogo:
   ```bash
   mkdir games/meu_jogo
   ```

2. **Crie os arquivos necessários**:
   ```bash
   # Nome que aparece no menu
   echo "Meu Jogo" > games/meu_jogo/game.txt
   
   # Função que será chamada
   echo "game_meu_jogo_run" > games/meu_jogo/runner.sym
   
   # Instruções (opcional)
   echo "Instruções do jogo" > games/meu_jogo/screen.txt
   ```

3. **Implemente o jogo** em `games/meu_jogo/meu_jogo.c`:
   ```c
   #include "video.h"
   #include "teclado.h"

   void game_meu_jogo_run(void) {
       // Seu código aqui
       // Use g_key_pressed_p1, g_last_key_p1 para Player 1
       // Use g_key_pressed_p2, g_last_key_p2 para Player 2
       // Use write_on_screen() para renderizar
   }
   ```

4. **Compile e teste**:
   ```bash
   make
   ```

O jogo aparecerá automaticamente no menu!

## 📄 Licença

Este projeto é desenvolvido para fins educacionais.

---

**CodePlay** - O Console dos
Desenvolvedores 🚀
