int main(){
	__asm__ (
	/* 1 is the syscall number for write on 64bit */
    "movl $1, %eax;\n"  
	/* 1 is stdout and is the first argument */
    "movl $1, %ebx;\n"  
	/* load the address of string into the second argument*/
    "movl $message, %esi;\n" 
	/* third argument is the length of the string to print*/
    "movl $10, %edx;\n"  
	"syscall;\n"
	"movl $60,%eax;\n" // call exit
	"syscall;\n"
	 // Store the Hello World inside the main function
	"message: .ascii \"holi boli\\n\";"
	);
	return 0;
}

