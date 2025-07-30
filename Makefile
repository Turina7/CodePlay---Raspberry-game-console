# Compilador e ferramentas para ARM
CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump

# Flags do compilador - otimizado para Raspberry Pi 2
CFLAGS = -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard -nostdlib -nostartfiles -ffreestanding -std=c99 -Wall -Wextra -O2 -g

# Flags do assembler
ASFLAGS = -mcpu=cortex-a7

# Arquivos fonte
SOURCES_C = main.c video.c utils.c
SOURCES_S = boot.s

# Arquivos objeto
OBJECTS = $(SOURCES_S:.s=.o) $(SOURCES_C:.c=.o)

# Arquivo ELF final
KERNEL_ELF = kernel.elf

# Imagem do kernel
KERNEL_IMG = kernel.img

# Regra padrão
all: $(KERNEL_IMG)

# Compilar arquivos C
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compilar arquivos assembly
%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

# Linkar para criar ELF
$(KERNEL_ELF): $(OBJECTS) linker.ld
	$(LD) -T linker.ld $(OBJECTS) -o $@

# Converter ELF para imagem binária
$(KERNEL_IMG): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# Limpar arquivos gerados
clean:
	rm -f $(OBJECTS) $(KERNEL_ELF) $(KERNEL_IMG)

# Instalar no cartão SD (assumindo que está montado em /media/pi)
install: $(KERNEL_IMG)
	@echo "Copiando kernel.img para o cartão SD..."
	@if [ -d "/media/pi" ]; then \
		sudo cp $(KERNEL_IMG) /media/pi/; \
		echo "Kernel instalado com sucesso!"; \
	else \
		echo "Cartão SD não encontrado em /media/pi"; \
		echo "Copie manualmente kernel.img para a raiz do cartão SD"; \
	fi

# Informações sobre o kernel
info: $(KERNEL_ELF)
	$(OBJDUMP) -h $(KERNEL_ELF)
	arm-none-eabi-size $(KERNEL_ELF)

# Debug do kernel
debug: $(KERNEL_ELF)
	$(OBJDUMP) -D $(KERNEL_ELF) > kernel.asm
	$(OBJDUMP) -t $(KERNEL_ELF) > kernel.sym
	@echo "Arquivos de debug criados: kernel.asm, kernel.sym"

# Teste rápido - verifica se compila
test:
	@echo "Testando compilação..."
	@$(MAKE) clean
	@$(MAKE) all
	@echo "Compilação bem-sucedida!"
	@echo "Tamanho do kernel:"
	@ls -lh $(KERNEL_IMG)

.PHONY: all clean install info debug test

# Instruções de debug:
# - Se o LED da placa piscar 5 vezes rapidamente no início: kernel iniciou
# - 1 piscada: inicializando framebuffer
# - 2 piscadas: framebuffer inicializado com sucesso
# - 3 piscadas: começando a pintar tela
# - 4 piscadas: tela pintada, LED fica aceso
# - 6 piscadas: endereço de framebuffer inválido
# - 7 piscadas: framebuffer não foi alocado
# - 8 piscadas: código de resposta do mailbox inválido
# - 9 piscadas: erro ao ler do mailbox
# - 10 piscadas: erro ao escrever no mailbox
# - Piscadas contínuas lentas: erro geral na inicialização do framebuffer