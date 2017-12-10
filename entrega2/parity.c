//  según la versión de gcc y opciones de optimización usadas, tal vez haga falta
//  usar gcc -fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval


#define TEST 0
#define WSIZE 8*sizeof(int)

#if ! TEST

    #define SIZE (1<<16)	// tamaño suficiente para tiempo apreciable
    unsigned lista[SIZE];
#else

    #define SIZE 8
    unsigned lista[SIZE] = {0x80000000, 0x00100000, 0x00000800, 0x00000001};
    //unsigned lista[SIZE] = {0x7fffffff, 0xffefffff, 0xfffff7ff, 0xfffffffe,
    //			0x01000024, 0x00356700, 0x8900ac00, 0x00bd00ef};
    //unsigned lista[SIZE] = {0x0, 0x10204080, 0x3590ac06, 0x70b0d0e0,
    //			0xffffffff, 0x12345678, 0x9abcdef0, 0xcafebeef};


#endif

const int mask = 0x1; //Declaramos la máscara con la que vamos a comparar

int parity(unsigned* array, int len)
{
  unsigned i, j,ret=0; //Declaramos los indices sobre los que vamos a operar
  unsigned val = 0; //El valor que obtenemos para cada elemento de la lista

  for (i=0; i<len; i++){
    unsigned numero = array[i]; //Seleccionamos cada elemento de la lista
    for(j=0; j < WSIZE; j++){ //Iteramos sobre el número de bits
      val ^= numero & mask; //Obtenemos el último bit y realizamos una operación xor
      numero >>= 1; //Desplazamos un bit a la derecha
    }

    ret += val; //Guardamos el resultado obtenido
    val = 0; //Reiniciamos para la siguiente iteración
  }
  return ret;
}

int parity2(unsigned* array, int len){
  unsigned i,ret=0;
  unsigned val = 0;

  for (i=0; i<len; i++){
    unsigned numero = array[i];
    //En lugar de iterar sobre el número de bits
    //lo hacemos mientras el número no sea 0
    //ya que en cada paso desplazamos el número
    while(numero != 0){
      val ^= numero & mask;
      numero >>= 1;
    }

    ret += val;
    val = 0;

  }
  return ret;
}

int parity3(unsigned* array, int len){
  int i, j,ret=0;
  int val = 0;

  for (i=0; i<len; i++){
    unsigned numero = array[i];
    val = 0;
    while(numero){
      val ^= numero;
      numero >>= 1;
    }

    //En lugar de comparar en cada paso lo hacemos una
    //única vez al final
    ret += val & 0x1;

  }
  return ret;
}


int parity4(unsigned* array, int len)
{
  int i;
  unsigned numero, val, ret = 0;
  for (i=0; i<len; i++){
    numero = array[i];
    //val = 0;
    asm(
	"mov $0, %[val]       \n"
	"ini3:                \n\t"
	"xor %[numero],%[val] \n\t"
	"shr %[numero]        \n\t"
	"cmp $0, %[numero]    \n\t"
	"jne ini3             \n\t"
	"and $1, %[val]       \n\t"
	:[val]"+r" (val)
	:[numero]"r"  (numero)
	);

    ret += val;

  }
  return ret;
}

int parity5(unsigned* array, int len){
  unsigned numero;
  unsigned ret = 0;
  int i, j;

   for (i=0; i<len; i++){
    numero = array[i];
    for(j = 16; j >= 1; j /= 2) //Hacemos desplazamiento de 16 8 4 2
      numero ^= numero>>j;
     
    ret += numero & 0x1;
    
   }

   return ret;
}

int parity6(unsigned* array, int len){
  unsigned numero;
  unsigned ret = 0;
  int i;

   for (i=0; i<len; i++){
    numero = array[i];
    asm(
	"mov %[numero], %%edx   \n\t"   //movemos el número al registro
	"shr $16,%[numero]      \n\t"   //Desplazamos 16 posiciones a la derecha (Estamos en 32)
	"xor %[numero] , %%edx  \n\t"   //Hacemos un xor
	"xor %%dh , %%dl       \n\t"    //Usamos las subpartes en la que se dividen los registros
	"setpo %%dl             \n\t"
	"movzx %%dl, %[numero]  \n\t"    //devolver en 32 bits
	:[numero]"+r" (numero)
	:
	:"edx"	
	);

    ret += numero;
    
   }

   return ret;
}


void crono(int (*func)(), char* msg){
  struct timeval tv1,tv2;	// gettimeofday() secs-usecs
  long           tv_usecs;	// y sus cuentas
  unsigned resultado=0;

  gettimeofday(&tv1,NULL);
  resultado = func(lista, SIZE);
  gettimeofday(&tv2,NULL);

  tv_usecs=(tv2.tv_sec -tv1.tv_sec )*1E6+
    (tv2.tv_usec-tv1.tv_usec);
  #if ! TEST
  printf("%lu\t", tv_usecs);
  #else
  printf("resultado = %d\t", resultado);
  printf("%s:%9ld us\n", msg, tv_usecs);
  #endif
}


int main()
{
#if ! TEST
	//Inicializar array
	int i;
	for (i=0; i<SIZE; i++) lista[i]=i;
#endif

  crono(parity, "parity (bucle for)");
  crono(parity2, "parity (bucle while)");
  crono(parity3, "parity (libro)");
  crono(parity4, "parity (asm)");
  crono(parity5, "parity (desplazamiento lateral)");
  crono(parity6, "parity (desplazamiento lateral asm)");
  printf("\n");
  exit(0);
}
