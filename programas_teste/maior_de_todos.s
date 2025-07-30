.global _start

_start:

@suponhamos este como o endereço inicial

mov r0, #0x100

@este registrador guarda o valor zero

mov r1, #0x00000000

@r2 será o registrador que irá armazenar o maior número

@vamos inicializá-lo com o primeiro valor e incrementar r0 no processo (pós-indexado)

ldr r2, [r0], #4

@verifica se não é uma sequencia nula

cmp r2, r1

@pula para o final caso seja uma sequencia nula

beq fim

loop:

@busca o próximo valor

ldr r3, [r0], #4

@compara com zero

cmp r3, r1

@vai para o final se for igual

beq fim

@compara o maior valor atual com o valor buscado na memoria

cmp r2, r3

@atualiza o maior valor

movmi r2, r3

@retorna ao loop para buscar o proximo valor

b loop

fim:

b fim
