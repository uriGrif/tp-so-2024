
### I/O
- [x] DIALFS arrancar file system solo si corresponde, crear archivo de bloques y archivo de bitmap en el pathbase. (mmap(), msync(), bitarray)
- [x] DIALFS Permite crear un archivo, le crea el archivo de metadata, le asigna el primer bloque libre, arranca tamaño en 0??
- [x] DIALFS Permite eliminar un archivo (asumo liberar todos los bloques y remove(fd_metada)?????
- [ ] DIALFS Permite ampliar o reducir el tamaño de un archivo a gusto. En el caso de que no halla espacio contiguo pero si libre total, hay que hacer la compactacion con su debido retraso y log
- [ ] DIALFS permite leer y escribir un archivo y escribir/leer en memoria (cuidado concurrencia!!)



      
