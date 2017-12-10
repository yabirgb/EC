.section .data
lista:		.int -1,-1,-1,-193,-1,-1,-45,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
longlista:	.int (.-lista)/4
resultado:	.quad -1

.section .text
_start:	.global _start

  mov   $lista, %ebx
  mov   $0, %edx
  mov   longlista, %ecx
  call  suma

  mov   %eax, resultado
  mov   %edx, resultado+4

  mov   $1, %eax
  mov   $0, %ebx
  int   $0x80

suma:
  push %esi
  mov $0, %eax
  mov $0, %esi

bucle:
  mov (%ebx,%esi,4), %ebp
  test %ebp, %ebp
  js negativo #Comprobamos el estado del flag de signo

#Distinguimos entre si es positivo o negativo el número que sumamos
positivo:
  add %ebp, %eax
  adc $0, %edx
  jmp siguiente

negativo:
  neg %ebp
  sub %ebp, %eax
  sbb $0, %edx

siguiente:
  inc %esi
  cmp %esi, %ecx
  jne bucle

  pop   %esi
  ret
