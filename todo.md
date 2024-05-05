- [ ] Logs minimos y obligatorios creeria que solo faltan del kernel

## Todo Checkpoint 3 previo al parcial


### Kernel
- [ ] Virutal round robin (REVISAR LO ULTIMO)
- [x] Manejo de recursos (va al mismo tiempo que la cpu con las instrucciones wait y singal)
- [x] Planificador largo plazo, cola de new con grado de multiprogramacion fijo del config
- [x] Planificador largo plazo, permite modificar por consola el grado de multiprogramacion
- [x] Se permite pausar la planificacion por consola (consultar issue o soporte)
- [ ] Finalizar proceso por consola (volver a consultar con persona adecuada en soporte)
### Cpu 
- [x] instrucciones WAIT y SIGNAL, mandar mensaje al kernel para que reste sume la instancia del recurso y bloquee/desbloquee al proceso si corresponde
- [x] Instrucciones MOV_IN MOV OUT RESIZE COPY_STRING IO_STDIN_READ y IO_SDOUT_WRITE (Por ahora no se puede hacer mucho pero por lo menos podemos establecer las conexiones en el caso de las 4 primeras creo que es con memoria derecho y en las de IO hay que mandarle la peticion al kernel y el kernel se comunicara con la interfaz y podemos hacer que la memoria le mande un string o reciba un string hardcodeado y lo imprima para ver si le llego bien el mensaje al menos)

### Memoria
- [x] establecer los mensajes de las instrucciones de cpu y mensajes de las interfaces

### I/O
- [x] Interfaz STDIN, que te deje escribir algo (fgets) y que se lo mande a memoria y que lo loguee
- [x] Interfaz STDOUT, que le llege de la memoria un string hardcodeado y lo loguee
