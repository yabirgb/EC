//  según la versión de gcc y opciones de optimización usadas, tal vez haga falta
//  usar gcc –fno-omit-frame-pointer si gcc quitara el marco pila (%ebp)

#include <stdio.h>	// para printf()
#include <stdlib.h>	// para exit()
#include <sys/time.h>	// para gettimeofday(), struct timeval

#define TEST 1
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

const int mask = 0x1;

int popcount(unsigned* array, int len)
{
  int i, j;
  unsigned contador=0; //Variable donde guardaremos el resultado
  
  for (i=0; i<len; i++){
    unsigned numero = array[i]; //Accedemos a cada elemento
    for(j=0; j < WSIZE; j++){ //para cada bit
      contador += numero & mask; //Comparamos con la mascara
      numero >>= 1; //y desplazamos el número un bit a la derecha
    }

  }
  return contador;
}

int popcount2(unsigned* array, int len)
{

  //Esta función tiene un functionamiento parecido al anterior
  //esta vez en lugar de iterar sobre el número de bits como
  //en cada paso desplazamos el número un bit a la derecha
  //solo tenemos que comprobar que no sea nulo
  int i, j;
  unsigned contador = 0;
  
  for (i=0; i<len; i++){
    unsigned numero = array[i];
    while(numero != 0){
      contador += numero & mask;
      numero >>= 1;
    }

  }
  return contador;
}

int popcount3(unsigned* array, int len)
{
  int i,result=0;
  unsigned x;
  for (i=0; i<len; i++){
    x = array[ i];//para cada numero
    asm("ini3:          \n\t" //realizamos un bucle
	"shr %[x]       \n\t" //desplazamos
	"adc $0, %[r] \n\t" //sumamos cogiendo el accarreo
	"cmp $0, %[x]   \n\t" //comprobamos que no sea nulo
	"jne ini3       \n\t" //volvemos al bucle
	: [r]"+r" (result) //recuperamos los resultados
	: [x]"r"  (x)      );
	
	

  }
  return result;
}

int popcount4(unsigned* array, int len){
  unsigned val = 0;
  int suma = 0;
  int i, j;

   for (j=0; j<len; j++){
    unsigned numero = array[j];
    for (i=0; i < 8; i++){
      val += numero & 0x01010101;
      numero >>= 1;
    }
    
    val += (val >> 16);
    val += (val >> 8);
    suma += val & 0xFF;
    val = 0;
    
   }
   

   return suma;
}

int popcount5(unsigned* array,int len)
{
  unsigned suma = 0;
  int j;
  unsigned numero;

  for (j=0; j<len; j++){
    numero = array[j];
    
    numero = numero - ((numero >> 1) & 0x55555555);
    numero = (numero & 0x33333333) + ((numero >> 2) & 0x33333333);
    suma += (((numero + (numero >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
  }

  return suma;
}


void crono(int (*func)(), char* msg){
  struct timeval tv1,tv2;	// gettimeofday() secs-usecs
  long           tv_usecs;	// y sus cuentas
  unsigned resultado;
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

  crono(popcount,  "popcount (bucle for)");
  crono(popcount2, "popcount (bucle while)");
  crono(popcount3, "popcount (asm)");
  crono(popcount4, "popcount (libro)");
  crono(popcount5, "popcount (google)");
  printf("\n");
  exit(0);
}
