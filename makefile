GCC=gcc
PFLAGS=-pthread
EXE_1=client
EXE_2=server

all:
	${GCC} -o ${EXE_1}.out ${EXE_1}.c ${PFLAGS}
	${GCC} -o ${EXE_2}.out ${EXE_2}.c ${PFLAGS} 
	./${EXE_2}.out ${PFLAGS}



