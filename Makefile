# Nome base do programa (sem extensão)
PROG = program

# Ferramentas da toolchain ARM
AS = arm-none-eabi-as
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy

# Endereço de carga na memória do simulador (deve bater com 0x8000 do main.cpp)
LDFLAGS = -Ttext=0x8000

# Executável do simulador
SIMULADOR = ./build/arm_simulator

# Regra padrão: gera o arquivo .img
all: $(PROG).img

# Monta o arquivo .s em .o
$(PROG).o: $(PROG).s
	$(AS) -o $@ $<

# Linka para gerar .elf com base no endereço 0x8000
$(PROG).elf: $(PROG).o
	$(LD) $(LDFLAGS) -o $@ $<

# Converte para .img (binário cru, sem cabeçalhos)
$(PROG).img: $(PROG).elf
	$(OBJCOPY) -O binary $< $@

# Executa o simulador com a imagem
run: $(PROG).img
	$(SIMULADOR) $(PROG).img

# Limpa arquivos gerados
clean:
	rm -f *.o *.elf *.img

.PHONY: all run clean

